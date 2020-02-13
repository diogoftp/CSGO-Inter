#include <iostream>
#include <atlbase.h>
	
#include <time.h>
//#include <dwmapi.h>
//#pragma comment(lib, "dwmapi.lib")

#include "SDK/SDK.hpp"
#include "xor.hpp"
#include "interfaces.hpp"
#include "netvars.hpp"

#include "gui.hpp"
#include "ESP.hpp"
#include "radar.hpp"
#include "aimbot.hpp"

//#define DEBUG1 1

// Data
LPDIRECT3D9              g_pD3D = NULL;
LPDIRECT3DDEVICE9        g_pd3dDevice = NULL;
D3DPRESENT_PARAMETERS    g_d3dpp = {};

DWORD WINAPI LoopThread(HMODULE hModule) {
	//GUI Setup
	WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC | CS_NOCLOSE | CS_HREDRAW | CS_VREDRAW, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, (HBRUSH)RGB(0, 0, 0), NULL, ("HehexD"), NULL };
	RegisterClassEx(&wc);
	HWND hwnd = CreateWindow(wc.lpszClassName, ("HehexD"), WS_OVERLAPPED, 100, 100, 400, 400, NULL, NULL, wc.hInstance, NULL);

	MSG msg;
	ZeroMemory(&msg, sizeof(msg));
	int teste = guiSetup(hwnd, wc);
	if (teste != 0) return 1;

	float aimbotFOV = 3.0f;
	float aimbotSmooth = 4.0f;
	bool bRadar = false, bESP = false, bMenu = true;
	//End GUI Setup

	USES_CONVERSION;
	//uintptr_t dwClient = (uintptr_t)GetModuleHandleW(L"client_panorama.dll");
	//uintptr_t dwEngine = (uintptr_t)GetModuleHandleW(L"engine.dll");
	
	uintptr_t dwClient = (uintptr_t)GetModuleHandleW(CA2W(Decrypt("*% ,'=9('&;($(g-%%").c_str())); //client_panorama.dll
	uintptr_t dwEngine = (uintptr_t)GetModuleHandleW(CA2W(Decrypt(",'. ',g-%%").c_str())); //engine.dll

	uintptr_t pLocalPlayer = dwClient + offsets::signatures::dwLocalPlayer;
	uintptr_t pGlowObjectManager = dwClient + offsets::signatures::dwGlowObjectManager;
	uintptr_t pEntityList = dwClient + offsets::signatures::dwEntityList;
	uintptr_t pClientState = dwEngine + offsets::signatures::dwClientState;
	uintptr_t pGameState = pClientState + offsets::signatures::dwClientState_State;

	uintptr_t m_LoopDist = 0x10;

	srand((unsigned int)time(0));

	//PRINTS
	#ifdef DEBUG1
	AllocConsole();
	FILE* f;
	freopen_s(&f, "CONOUT$", "w", stdout);
	#endif // DEBUG1


	EntList* entityList = (EntList*)pEntityList;

	Interfaces::Initialize();
	Offsets2::Initialize();

	while (!GetAsyncKeyState(VK_INSERT)) {
		uintptr_t EntityList = *(uintptr_t*)pEntityList;
		uintptr_t LocalPlayer = *(uintptr_t*)pLocalPlayer;
		uintptr_t GlowObjectManager = *(uintptr_t*)pGlowObjectManager;
		uintptr_t ClientState = *(uintptr_t*)pClientState;
		int GameState = *(uintptr_t*)(ClientState + offsets::signatures::dwClientState_State);

		Vec3* viewAngles = (Vec3*)(ClientState + offsets::signatures::dwClientState_ViewAngles);

		Entity* localPlayer = (Entity*)(*(uintptr_t*)pLocalPlayer);

		//PRINTS
		#ifdef DEBUG1
		system("cls");;
		//printashex((char*)Decrypt("Aimbot Smooth").c_str());
		//char tee[] = { 0x8, 0x20, 0x24, 0x2B, 0x26, 0x3D, 0x69, 0x1A, 0x24, 0x26, 0x26, 0x3D, 0x21 };
		//std::cout << tee << std::endl;
		std::cout << "Dormant: " << offsets::signatures::dwLocalPlayer << std::endl;
		std::cout << "Dormant2: " << Offsets2::m_bDormant << std::endl;
		std::cout << "FOV: " << aimbotFOV << std::endl << "Smooth: " << aimbotSmooth << std::endl;
		std::cout << "[HOME]bRadar: " << bRadar << std::endl;
		std::cout << "[DELETE]bESP: " << bESP << std::endl;
		std::cout << "[INSERT]Desligar" << std::endl;
		std::cout << "GameState: " << GameState << std::endl;
		if (localPlayer && GameState == 6) std::cout << "LocalPlayer: ID: " << localPlayer->clientId() << " Health: " << localPlayer->health() << std::endl;
		for (int i = 0; i < 32; i++) {
			if (entityList->entityListObjs[i].entity != NULL) {
				std::cout << "ID: " << entityList->entityListObjs[i].entity->clientId() << " Health: " << entityList->entityListObjs[i].entity->health() << " Index: " << i << std::endl;
			}
		}
		/*if (g_EntityList->GetClientEntity(g_EngineClient->GetLocalPlayer()) != NULL && GameState == 6) std::cout << "LocalPlayer: ID: " << g_EntityList->GetClientEntity(g_EngineClient->GetLocalPlayer())->clientId() << " Health: " << g_EntityList->GetClientEntity(g_EngineClient->GetLocalPlayer())->health() << std::endl;
		for (int i = 1; i < 32; i++) {
			if (g_EntityList->GetClientEntity(i) != NULL) {
				std::cout << "ID: " << g_EntityList->GetClientEntity(i)->clientId() << " Health: " << g_EntityList->GetClientEntity(i)->health() << std::endl;
			}
		}*/
		#endif // DEBUG1

		//Draw GUI
		if (bMenu) {
			ShowWindow(hwnd, SW_SHOW);
			windowLoop(hwnd, msg, &bESP, &bRadar, &aimbotFOV, &aimbotSmooth);
			UpdateWindow(hwnd);
		}
		else {
			ShowWindow(hwnd, SW_HIDE);
		}
		//End Draw GUI

		if (GetAsyncKeyState(VK_DELETE) & 1) {
			bMenu = !bMenu;
		}

		if (GetAsyncKeyState(VK_XBUTTON1) && GameState == 6) {
		//if (GetAsyncKeyState(VK_SHIFT) && GameState == 6) {
			aimbotbyFOV(localPlayer, entityList, viewAngles, aimbotFOV, aimbotSmooth);
		}

		if (GetAsyncKeyState(VK_CAPITAL) && GameState == 6) {
			RCS(localPlayer, entityList, viewAngles);
		}

		if ((bRadar || bESP) && GameState == 6) {
			for (unsigned short int i = 0; i < 32; i++) {
				if (entityList->entityListObjs[i].entity != NULL) {
					if (entityList->entityListObjs[i].entity->dormant()) continue;
					if (bRadar) Radar(entityList->entityListObjs[i].entity);
					if (bESP) ESP(localPlayer, entityList->entityListObjs[i].entity, dwClient, GlowObjectManager);
				}
			}
		}
		Sleep(5);
	}

	//PRINTS
	#ifdef DEBUG1
	if(f != NULL) fclose(f);
	FreeConsole();
	#endif // DEBUG1

	guiEnd(hwnd, wc);

	FreeLibraryAndExitThread(hModule, 0);
	return 0;

}

BOOL APIENTRY DllMain(HMODULE hModule, uintptr_t ul_reason_for_call, LPVOID lpReserved) {
	HWND hwndGoto = NULL;
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		DisableThreadLibraryCalls(hModule);
		//CloseHandle(CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)LoopThread, hModule, 0, nullptr));
		if (auto handle = CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)LoopThread, hModule, 0, nullptr)) {
			CloseHandle(handle);
		}
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}