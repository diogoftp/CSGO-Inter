#include "hooks.hpp"
#include "vars.hpp"
#include "aimbot.hpp"
#include <vector>

extern Globals::myGlobals Vars;

namespace Hooks {
	void Circle(LPDIRECT3DDEVICE9 pDevice, int x, int y, int radius, int points, D3DCOLOR colour) {
		SD3DVertex* pVertex = new SD3DVertex[points + 1];
		for (int i = 0; i <= points; i++) pVertex[i] = { x + radius * cos(D3DX_PI * (i / (points / 2.0f))), y - radius * sin(D3DX_PI * (i / (points / 2.0f))), 0.0f, 1.0f, colour };
		pDevice->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);
		pDevice->DrawPrimitiveUP(D3DPT_LINESTRIP, points, pVertex, sizeof(SD3DVertex));
		delete[] pVertex;
	}

	bool WorldToScreen2(ViewMatrix_t matrix, Vec3 pos, float screen[]) {
		float clipCoords[4];
		clipCoords[0] = pos.x * matrix.matrix[0] + pos.y * matrix.matrix[1] + pos.z * matrix.matrix[2] + matrix.matrix[3];
		clipCoords[1] = pos.x * matrix.matrix[4] + pos.y * matrix.matrix[5] + pos.z * matrix.matrix[6] + matrix.matrix[7];
		clipCoords[2] = pos.x * matrix.matrix[8] + pos.y * matrix.matrix[9] + pos.z * matrix.matrix[10] + matrix.matrix[11];
		clipCoords[3] = pos.x * matrix.matrix[12] + pos.y * matrix.matrix[13] + pos.z * matrix.matrix[14] + matrix.matrix[15];

		if (clipCoords[3] < 0.1f) return false;

		Vec3 NDC;
		NDC.x = clipCoords[0] / clipCoords[3];
		NDC.y = clipCoords[1] / clipCoords[3];
		NDC.z = clipCoords[2] / clipCoords[3];

		screen[0] = (1080 / 2 * NDC.x) + (NDC.x + 1080 / 2);
		screen[1] = -(1080 / 2 * NDC.y) + (NDC.y + 1080 / 2);
		return true;
	}

	typedef HRESULT(APIENTRY* tEndScene)(LPDIRECT3DDEVICE9 pDevice);
	tEndScene oEndScene = nullptr;

	BYTE originalBytes[7] = {};
	void* d3d9Device[119];

	HRESULT APIENTRY hkEndScene(LPDIRECT3DDEVICE9 pDevice) {
		EntList* entityList = (EntList*)(Offsets::dwClient + Offsets::dwEntityList);
		uintptr_t ClientState = *(uintptr_t*)(Offsets::dwEngine + Offsets::dwClientState);
		int GameState = *(uintptr_t*)(ClientState + Offsets::dwClientState_State);
		Vec3* viewAngles = (Vec3*)(ClientState + Offsets::dwClientState_ViewAngles);
		Entity* localPlayer = (Entity*)(*(uintptr_t*)(Offsets::dwClient + Offsets::dwLocalPlayer));
		ViewMatrix_t ViewMatrix = *(ViewMatrix_t*)(Offsets::dwClient + Offsets::dwViewMatrix);
		if (localPlayer && GameState == 6 && viewAngles && entityList) {
			for (unsigned short int i = 0; i < 32; i++) {
				if (entityList->entityListObjs[i].entity == NULL) continue;
				Entity* target = entityList->entityListObjs[i].entity;
				if (target->dormant() || target->lifeState() != 0 || target->health() < 1 || target == localPlayer) continue;
				float EnemyXY[2] = { 0 };
				float neck[2] = { 0 };
				float chest[2] = { 0 };
				WorldToScreen2(ViewMatrix, target->getBonePos(7), neck);
				WorldToScreen2(ViewMatrix, target->getBonePos(6), chest);
				float dif = abs(neck[0] - chest[0] + neck[1] - chest[1]) * 0.8f;
				if (WorldToScreen2(ViewMatrix, target->getBonePos(Vars.boneIndex), EnemyXY)) {
					Circle(pDevice, (int)EnemyXY[0], (int)EnemyXY[1], (int)dif, 20, D3DCOLOR_RGBA(255, 0, 0, 255));
					//D3DRECT BarRect = { (int)EnemyXY[0] - dif, (int)EnemyXY[1] - dif, (int)EnemyXY[0] + dif, (int)EnemyXY[1] + dif };
					//pDevice->Clear(1, &BarRect, D3DCLEAR_TARGET, D3DCOLOR_ARGB(1, 1, 1, 1), 0.0f, 0);
				}
			}	
		}
		//D3DRECT BarRect = { 300, 300, 600, 600 };
		//pDevice->Clear(1, &BarRect, D3DCLEAR_TARGET, D3DCOLOR_ARGB(1, 1, 1, 1), 0.0f, 0);
		return oEndScene(pDevice);
	}

	void Setup() {
		if (GetD3D9Device(d3d9Device, sizeof(d3d9Device))) {
			ReadProcessMemory(GetCurrentProcess(), (LPVOID)(char*)d3d9Device[42], originalBytes, 6, 0);
			oEndScene = (tEndScene)TrampHook32((char*)d3d9Device[42], (char*)hkEndScene, 7);
		}
	}

	void Shutdown() {
		if (originalBytes && d3d9Device) {
			DWORD curProtection;
			VirtualProtect((char*)d3d9Device[42], 7, PAGE_EXECUTE_READWRITE, &curProtection);
			memcpy((char*)d3d9Device[42], originalBytes, 7);
			VirtualProtect((char*)d3d9Device[42], 7, curProtection, &curProtection);
		}
	}

	bool Detour32(char* src, char* dst, const intptr_t len) {
		if (len < 5) return false;

		DWORD  curProtection;
		VirtualProtect(src, len, PAGE_EXECUTE_READWRITE, &curProtection);

		intptr_t  relativeAddress = (intptr_t)(dst - (intptr_t)src) - 5;

		memcpy(originalBytes, src, 7);
		*src = (char)'\xE9';
		*(intptr_t*)((intptr_t)src + 1) = relativeAddress;

		VirtualProtect(src, len, curProtection, &curProtection);
		return true;
	}

	char* TrampHook32(char* src, char* dst, const intptr_t len) {
		// Make sure the length is greater than 5
		if (len < 5) return 0;

		// Create the gateway (len + 5 for the overwritten bytes + the jmp)
		void* gateway = VirtualAlloc(0, len + 5, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);

		//Write the stolen bytes into the gateway
		memcpy(gateway, src, len);

		// Get the gateway to destination addy
		intptr_t  gatewayRelativeAddr = ((intptr_t)src - (intptr_t)gateway) - 5;

		// Add the jmp opcode to the end of the gateway
		*(char*)((intptr_t)gateway + len) = 0xE9;

		// Add the address to the jmp
		*(intptr_t*)((intptr_t)gateway + len + 1) = gatewayRelativeAddr;

		// Perform the detour
		Detour32(src, dst, len);

		return (char*)gateway;
	}

	bool GetD3D9Device(void** pTable, size_t Size) {
		//std::cout << "[*] Trying to get DirectXDevice by new_creation method..\n";

		if (!pTable) {
			//std::cout << "[-] pTable not set.\n";
			return false;
		}

		//std::cout << "[*] Right before Direct3DCreate9.\n";
		IDirect3D9* pD3D = Direct3DCreate9(D3D_SDK_VERSION);
		if (!pD3D) {
			//std::cout << "[-] Direct3DCreate9 failed.\n";
			return false;
		}
		//std::cout << "[+] Direct3DCreate9 successfull.\n";

		D3DPRESENT_PARAMETERS d3dpp = { 0 };
		d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
		d3dpp.hDeviceWindow = GetForegroundWindow();
		d3dpp.Windowed = ((GetWindowLong(d3dpp.hDeviceWindow, GWL_STYLE) & WS_POPUP) != 0) ? FALSE : TRUE;;

		//std::cout << "[*] Got here..\n";

		IDirect3DDevice9* pDummyDevice = nullptr;
		HRESULT create_device_ret = pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, d3dpp.hDeviceWindow, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &pDummyDevice);
		//std::cout << "[*] Return of CreateDevice:\n";
		//std::cout << create_device_ret;
		//std::cout << "\n";
		if (!pDummyDevice || FAILED(create_device_ret)) {
			//std::cout << "[-] CreateDevice failed\n";
			pD3D->Release();
			return false;
		}
		//std::cout << "[+] CreateDevice successfull.\n";

		memcpy(pTable, *reinterpret_cast<void***>(pDummyDevice), Size);

		pDummyDevice->Release();
		pD3D->Release();

		//std::cout << "[+] Success!\n";
		return true;
	}
}