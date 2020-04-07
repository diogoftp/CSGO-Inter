#pragma once
#include <Windows.h>
#include <d3d9.h>
#include <d3dx9.h>

#include "SDK/SDK.hpp"

namespace Hooks {
	void Setup();
	void Shutdown();

	/*typedef struct {
		float matrix[16];
	} ViewMatrix_t;

	bool WorldToScreen2(ViewMatrix_t matrix, Vec3 pos, float screen[]);*/

	bool Detour32(char* src, char* dst, const intptr_t len);
	char* TrampHook32(char* src, char* dst, const intptr_t len);
	bool GetD3D9Device(void** pTable, size_t Size);
}