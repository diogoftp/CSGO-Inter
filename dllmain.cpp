#include <iostream>
	
#include <time.h>
#include <thread>
#include <chrono>

#include "SDK/SDK.hpp"
#include "xor.hpp"
#include "interfaces.hpp"
#include "offsets.hpp"
#include "hooks.hpp"
#include "vars.hpp"

#include "gui.hpp"
#include "ESP.hpp"
#include "radar.hpp"
#include "aimbot.hpp"

//#define DEBUG1 1

//Data
LPDIRECT3D9              g_pD3D = NULL;
LPDIRECT3DDEVICE9        g_pd3dDevice = NULL;
D3DPRESENT_PARAMETERS    g_d3dpp = {};

//Globals
Globals::myGlobals Vars;

DWORD WINAPI LoopThread(HMODULE hModule) {
	//PRINTS (Console)
	#ifdef DEBUG1
	AllocConsole();
	FILE* f;
	freopen_s(&f, "CONOUT$", "w", stdout);
	#endif // DEBUG1

	srand((unsigned int)time(0));

	//Flags
	bool setForeground = true;
	bool aimKeyState = false;
	bool aimKeyPrevState = true;
	bool targetLock = false;
	bool clearTarget = true;

	//GUI Setup
	GUI::GUIStruct GUIProps;
	if (!GUI::Setup(&GUIProps)) return 1;

	//Utilities Setup
	Interfaces::Setup();
	Offsets::Setup();
	//Hooks::Setup();

	aimbot *Aimbot = new aimbot;

	uintptr_t pLocalPlayer = Offsets::dwClient + Offsets::dwLocalPlayer;
	uintptr_t pGlowObjectManager = Offsets::dwClient + Offsets::dwGlowObjectManager;
	uintptr_t pEntityList = Offsets::dwClient + Offsets::dwEntityList;
	uintptr_t pClientState = Offsets::dwEngine + Offsets::dwClientState;
	//CGlobalVarsBase* globals = (CGlobalVarsBase*)(Offsets::dwEngine + Offsets::dwGlobalVars);
	EntList* entityList = (EntList*)pEntityList;

	while (!GetAsyncKeyState(VK_INSERT)) {
		uintptr_t GlowObjectManager = *(uintptr_t*)pGlowObjectManager;
		uintptr_t ClientState = *(uintptr_t*)pClientState;
		int GameState = *(uintptr_t*)(ClientState + Offsets::dwClientState_State);
		Vec3* viewAngles = (Vec3*)(ClientState + Offsets::dwClientState_ViewAngles);
		Entity* localPlayer = (Entity*)(*(uintptr_t*)pLocalPlayer);

		//PRINTS
		#ifdef DEBUG1
		system("cls");
		//std::cout << "FPS: " << 1.0f / globals->frametime << std::endl;
		std::cout << "FOV: " << Vars.aimbotFOV << std::endl;
		std::cout << "Smooth: " << Vars.aimbotSmooth << std::endl;
		std::cout << "Radar: " << Vars.bRadar << std::endl;
		std::cout << "ESP: " << Vars.bESP << std::endl;
		std::cout << "[DELETE]Open GUI" << std::endl;
		std::cout << "[INSERT]Desligar" << std::endl;
		std::cout << "GameState: " << GameState << std::endl;
		//std::cout << "TL: " << targetLock << std::endl << "CT: " << clearTarget << std::endl;
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
		if (Vars.bMenu) {
			UpdateWindow(GUIProps.hwnd);
			ShowWindow(GUIProps.hwnd, SW_RESTORE);
			if (setForeground) {
				SetForegroundWindow(GUIProps.hwnd);
				setForeground = false;
			}
			GUI::windowLoop(GUIProps.hwnd, GUIProps.msg, GUIProps.hsize, GUIProps.vsize, &Vars.bESP, &Vars.bRadar, &Vars.aimbotFOV, &Vars.aimbotSmooth, &Vars.bRCSAimbot);
		}
		else {
			ShowWindow(GUIProps.hwnd, SW_HIDE);
			setForeground = true;
		}
		//End Draw GUI

		//Menu toggle
		if (GetAsyncKeyState(VK_DELETE) & 1) {
			Vars.bMenu = !Vars.bMenu;
		}

		//Check if needs to change aimbot target
		aimKeyState = (GetAsyncKeyState(VK_XBUTTON1)) != 0;
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
			if (Vars.aimbotSmooth == 0) {
				Aimbot->rageAimbot(localPlayer, entityList, viewAngles, Vars.aimbotFOV, Vars.bRCSAimbot);
			}
			else {
				Aimbot->aimbotbyFOV(localPlayer, entityList, viewAngles, Vars.aimbotFOV, Vars.aimbotSmooth, clearTarget, Vars.bRCSAimbot);
			}
			if (clearTarget == true) clearTarget = false;
		}

		//Standalone RCS
		if (GetAsyncKeyState(VK_CAPITAL) && GameState == 6) {
			Aimbot->RCS(localPlayer, viewAngles);
		}

		//TRay
		if (GetAsyncKeyState(VK_END) && GameState == 6) {
			std::cout << "Inter: " << Interfaces::g_EngineTrace << std::endl;
			for (unsigned short int i = 0; i < 32; i++) {
				if (entityList->entityListObjs[i].entity != NULL) {
					if (entityList->entityListObjs[i].entity->clientId() != localPlayer->clientId() && entityList->entityListObjs[i].entity->dormant() != TRUE) {
						if (Aimbot->isVisible(localPlayer, entityList->entityListObjs[i].entity)) {
							std::cout << "Visible: " << entityList->entityListObjs[i].entity->clientId() << std::endl;
						}
					}
				}
			}
		}

		//Radar and ESP
		if ((Vars.bRadar || Vars.bESP) && GameState == 6) {
			for (unsigned short int i = 0; i < 32; i++) {
				if (entityList->entityListObjs[i].entity != NULL) {
					if (entityList->entityListObjs[i].entity->dormant()) continue;
					if (Vars.bRadar) Radar(entityList->entityListObjs[i].entity);
					if (Vars.bESP) ESP(localPlayer, entityList->entityListObjs[i].entity, Offsets::dwClient, GlowObjectManager);
				}
			}
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(5));
	}

	//Hooks::Shutdown();
	GUI::Shutdown(GUIProps.hwnd, GUIProps.wc);

	//PRINTS
	#ifdef DEBUG1
	if(f != NULL) fclose(f);
	FreeConsole();
	#endif // DEBUG1

	FreeLibraryAndExitThread(hModule, 0);
	return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule, uintptr_t ul_reason_for_call, LPVOID lpReserved) {
	switch (ul_reason_for_call)	{
	case DLL_PROCESS_ATTACH:
		DisableThreadLibraryCalls(hModule);
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