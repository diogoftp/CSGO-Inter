#include "gui.hpp"
#include "ESP.hpp"
#include "radar.hpp"
#include "aimbot.hpp"
#include "scorewall.hpp"

#include "SDK/SDK.hpp"
#include "xor.hpp"
#include "interfaces.hpp"
#include "offsets.hpp"
//#include "hooks.hpp"
#include "vars.hpp"

#include <time.h>
#include <thread>
#include <chrono>

//#define DEBUG1 1

//PRINTS (Console)
#ifdef DEBUG1
#include <iostream>
#endif // DEBUG1

#define CONOUT$STR Decrypt({ 0xa, 0x6, 0x7, 0x6, 0x1c, 0x1d, 0x6d }).c_str()
#define wSTR Decrypt({ 0x3e }).c_str()
#define tier0STR Decrypt({ 0x3d, 0x20, 0x2c, 0x3b, 0x79, 0x67, 0x2d, 0x25, 0x25 }).c_str()
#define AllocateThreadIDSTR Decrypt({ 0x8, 0x25, 0x25, 0x26, 0x2a, 0x28, 0x3d, 0x2c, 0x1d, 0x21, 0x3b, 0x2c, 0x28, 0x2d, 0x0, 0xd }).c_str()
#define FreeThreadIDSTR Decrypt({ 0xf, 0x3b, 0x2c, 0x2c, 0x1d, 0x21, 0x3b, 0x2c, 0x28, 0x2d, 0x0, 0xd }).c_str()

//Data
LPDIRECT3D9              g_pD3D = NULL;
LPDIRECT3DDEVICE9        g_pd3dDevice = NULL;
D3DPRESENT_PARAMETERS    g_d3dpp = {};

//Globals
Globals::myGlobals Vars;

DWORD WINAPI LoopThread(HMODULE hModule) {
	//Multi-thread bug fix for raytrace
	typedef void(__cdecl* _allocateThread)();
	_allocateThread FixRay = (_allocateThread)GetProcAddress(GetModuleHandleA(tier0STR), AllocateThreadIDSTR);
	FixRay();

	//PRINTS (Console)
	#ifdef DEBUG1
	AllocConsole();
	FILE* f;
	freopen_s(&f, CONOUT$STR, wSTR, stdout);
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

	//CGlobalVarsBase* globals = (CGlobalVarsBase*)(Offsets::dwEngine + Offsets::dwGlobalVars);
	EntList* entityList = (EntList*)(Offsets::dwClient + Offsets::dwEntityList);

	while (!GetAsyncKeyState(VK_INSERT)) {
		uintptr_t ClientState = *(uintptr_t*)(Offsets::dwEngine + Offsets::dwClientState);
		int GameState = *(uintptr_t*)(ClientState + Offsets::dwClientState_State);
		Vec3* viewAngles = (Vec3*)(ClientState + Offsets::dwClientState_ViewAngles);
		Entity* localPlayer = (Entity*)(*(uintptr_t*)(Offsets::dwClient + Offsets::dwLocalPlayer));
		uintptr_t GlowObjectManager = *(uintptr_t*)(Offsets::dwClient + Offsets::dwGlowObjectManager);

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
		if (localPlayer && GameState == 6) std::cout << "LocalPlayer: ID: " << localPlayer->clientId() << " Health: " << localPlayer->health() << std::endl;
		for (int i = 0; i < 32; i++) {
			if (entityList->entityListObjs[i].entity != NULL) {
				Entity* ent = entityList->entityListObjs[i].entity;
				std::cout << "ID: " << ent->clientId() << " Index: " << i << " Health: " << ent->health() << std::endl;
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
			GUI::windowLoop(&Vars, &GUIProps);
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
		if (GetAsyncKeyState(VK_XBUTTON1) && GameState == 6 && localPlayer && entityList && viewAngles) {
			aimbot::aimbotFOV(localPlayer, entityList, viewAngles, clearTarget, GUIProps);
			if (clearTarget == true) clearTarget = false;
		}

		//Standalone RCS
		/*if (GetAsyncKeyState(VK_CAPITAL) && GameState == 6 && localPlayer && viewAngles) {
			aimbot::RCS(localPlayer, viewAngles);
		}*/

		//Scoreboard Wallhack
		if (GetAsyncKeyState(VK_TAB) && Vars.bScoreWall && localPlayer && entityList && GameState == 6) scoreWall(GUIProps, localPlayer, viewAngles, entityList);

		//Radar and ESP
		if ((Vars.bRadar || Vars.bESP) && GameState == 6 && localPlayer && entityList) {
			for (unsigned short int i = 0; i < 32; i++) {
				if (!entityList->entityListObjs[i].entity) continue;
				Entity* entity = entityList->entityListObjs[i].entity;
				if (entity == localPlayer || entity->dormant() || entity->lifeState() != 0 || entity->health() < 1) continue;
				if (Vars.bRadar && entity->team() != localPlayer->team()) Radar(entity);
				if (Vars.bESP && GlowObjectManager) ESP(localPlayer, entity, GlowObjectManager);
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

	//Multi-thread bug fix for raytrace
	typedef void(__cdecl* _freeThread)();
	_freeThread freeFixRay = (_freeThread)GetProcAddress(GetModuleHandleA(tier0STR), FreeThreadIDSTR);
	freeFixRay();

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