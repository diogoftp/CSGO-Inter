#include <iostream>
	
#include <time.h>
//#include <dwmapi.h>
//#pragma comment(lib, "dwmapi.lib")

#include "SDK/SDK.hpp"
#include "xor.hpp"
#include "interfaces.hpp"
#include "offsets.hpp"

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
	HWND hwnd = CreateWindow(wc.lpszClassName, "HehexD", WS_OVERLAPPED, 100, 100, 400, 400, NULL, NULL, wc.hInstance, NULL);

	MSG msg;
	ZeroMemory(&msg, sizeof(msg));
	int teste = guiSetup(hwnd, wc);
	if (teste != 0) return 1;
	//End GUI Setup

	//Vars
	float aimbotFOV = 3.0f;
	float aimbotSmooth = 4.0f;
	bool bRadar = false, bESP = false, bMenu = true, bRCSAimbot = true;

	//Flags
	bool aimKeyState = false;
	bool aimKeyPrevState = false;
	bool targetLock = false;
	bool clearTarget = true;

	//Interfaces::Initialize();
	offsets::Initialize();

	uintptr_t pLocalPlayer = offsets::dwClient + offsets::dwLocalPlayer;
	uintptr_t pGlowObjectManager = offsets::dwClient + offsets::dwGlowObjectManager;
	uintptr_t pEntityList = offsets::dwClient + offsets::dwEntityList;
	uintptr_t pClientState = offsets::dwEngine + offsets::dwClientState;

	srand((unsigned int)time(0));

	//PRINTS
	#ifdef DEBUG1
	AllocConsole();
	FILE* f;
	freopen_s(&f, "CONOUT$", "w", stdout);
	#endif // DEBUG1

	EntList* entityList = (EntList*)pEntityList;
	while (!GetAsyncKeyState(VK_INSERT)) {
		uintptr_t GlowObjectManager = *(uintptr_t*)pGlowObjectManager;
		uintptr_t ClientState = *(uintptr_t*)pClientState;

		int GameState = *(uintptr_t*)(ClientState + offsets::dwClientState_State);

		Vec3* viewAngles = (Vec3*)(ClientState + offsets::dwClientState_ViewAngles);

		Entity* localPlayer = (Entity*)(*(uintptr_t*)pLocalPlayer);

		//PRINTS
		#ifdef DEBUG1
		system("cls");;
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
			UpdateWindow(hwnd);
			ShowWindow(hwnd, SW_SHOW);
			windowLoop(hwnd, msg, &bESP, &bRadar, &aimbotFOV, &aimbotSmooth, &bRCSAimbot);
		}
		else {
			ShowWindow(hwnd, SW_HIDE);
		}
		//End Draw GUI

		//Menu toggle
		if (GetAsyncKeyState(VK_DELETE) & 1) {
			bMenu = !bMenu;
		}

		//Check if needs to change target
		aimKeyState = (GetKeyState(VK_XBUTTON1) & 0x100) != 0;
		if (aimKeyPrevState != aimKeyState) {
			if (aimKeyState) {
				targetLock = true;
			}
			else {
				targetLock = false;
				clearTarget = true;
			}
			aimKeyPrevState = aimKeyState;
		}

		//Aimbot
		if (GetAsyncKeyState(VK_XBUTTON1) && GameState == 6) {
			aimbotbyFOV(localPlayer, entityList, viewAngles, aimbotFOV, aimbotSmooth, clearTarget, bRCSAimbot);
			if (clearTarget == true) clearTarget = false;
		}

		//Standalone RCS
		if (GetAsyncKeyState(VK_CAPITAL) && GameState == 6) {
			RCS(localPlayer, entityList, viewAngles);
		}

		//Radar and ESP
		if ((bRadar || bESP) && GameState == 6) {
			for (unsigned short int i = 0; i < 32; i++) {
				if (entityList->entityListObjs[i].entity != NULL) {
					if (entityList->entityListObjs[i].entity->dormant()) continue;
					if (bRadar) Radar(entityList->entityListObjs[i].entity);
					if (bESP) ESP(localPlayer, entityList->entityListObjs[i].entity, offsets::dwClient, GlowObjectManager);
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