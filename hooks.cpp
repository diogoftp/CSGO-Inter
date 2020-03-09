#include "hooks.hpp"
#include <iostream>
#include <ctime>
#include "aimbot.hpp"

typedef HRESULT(APIENTRY* tEndScene)(LPDIRECT3DDEVICE9 pDevice);
tEndScene oEndScene = nullptr;
BYTE originalBytes[7] = {};
void* d3d9Device[119];

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
	std::cout << "[*] Trying to get DirectXDevice by new_creation method..\n";

	if (!pTable) {
		std::cout << "[-] pTable not set.\n";
		return false;
	}


	std::cout << "[*] Right before Direct3DCreate9.\n";
	IDirect3D9* pD3D = Direct3DCreate9(D3D_SDK_VERSION);
	if (!pD3D) {
		std::cout << "[-] Direct3DCreate9 failed.\n";
		return false;
	}
	std::cout << "[+] Direct3DCreate9 successfull.\n";

	D3DPRESENT_PARAMETERS d3dpp = { 0 };
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.hDeviceWindow = GetForegroundWindow();
	d3dpp.Windowed = ((GetWindowLong(d3dpp.hDeviceWindow, GWL_STYLE) & WS_POPUP) != 0) ? FALSE : TRUE;;

	std::cout << "[*] Got here..\n";

	IDirect3DDevice9* pDummyDevice = nullptr;
	HRESULT create_device_ret = pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, d3dpp.hDeviceWindow, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &pDummyDevice);
	std::cout << "[*] Return of CreateDevice:\n";
	std::cout << create_device_ret;
	std::cout << "\n";
	if (!pDummyDevice || FAILED(create_device_ret)) {
		std::cout << "[-] CreateDevice failed\n";
		pD3D->Release();
		return false;
	}
	std::cout << "[+] CreateDevice successfull.\n";

	memcpy(pTable, *reinterpret_cast<void***>(pDummyDevice), Size);

	pDummyDevice->Release();
	pD3D->Release();

	std::cout << "[+] Success!\n";
	return true;
}

HRESULT APIENTRY hkEndScene(LPDIRECT3DDEVICE9 pDevice) {
	//do stuff in here
	D3DRECT BarRect = { 100, 100, 200, 200 };
	pDevice->Clear(1, &BarRect, D3DCLEAR_TARGET, D3DCOLOR_ARGB(1, 1, 1, 1), 0.0f, 0);
	return oEndScene(pDevice);
}

int startHook() {
    if (GetD3D9Device(d3d9Device, sizeof(d3d9Device))) {
		ReadProcessMemory(GetCurrentProcess(), (LPVOID)(char*)d3d9Device[42], originalBytes, 6, 0);
        oEndScene = (tEndScene)TrampHook32((char*)d3d9Device[42], (char*)hkEndScene, 7);
        return 0;
    }
    else return 1;
}

int endHook() {
	if (originalBytes && d3d9Device) {
		DWORD curProtection;
		VirtualProtect((char*)d3d9Device[42], 7, PAGE_EXECUTE_READWRITE, &curProtection);
		memcpy((char*)d3d9Device[42], originalBytes, 7);
		VirtualProtect((char*)d3d9Device[42], 7, curProtection, &curProtection);
		return 0;
	}
	return 1;
}