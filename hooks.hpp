#pragma once
#include <Windows.h>
#include <d3d9.h>
#include <d3dx9.h>
#include "vars.hpp"
#include "interfaces.hpp"

extern Globals::myGlobals Vars;

namespace Hooks {
	void Setup();
	void Shutdown();

	bool Detour32(char* src, char* dst, const intptr_t len);
	char* TrampHook32(char* src, char* dst, const intptr_t len);
	bool GetD3D9Device(void** pTable, size_t Size);
}