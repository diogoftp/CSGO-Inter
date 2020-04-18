#pragma once
#include "SDK/SDK.hpp"
#include <Windows.h>
#include <d3d9.h>
#include <d3dx9.h>

namespace Hooks {
	struct SD3DVertex {
		float x, y, z, rhw;
		DWORD colour;
	};

	struct ViewMatrix_t {
		float matrix[16];
	};

	void Setup();
	void Shutdown();

	bool WorldToScreen2(ViewMatrix_t matrix, Vec3 pos, float screen[]);

	bool Detour32(char* src, char* dst, const intptr_t len);
	char* TrampHook32(char* src, char* dst, const intptr_t len);
	bool GetD3D9Device(void** pTable, size_t Size);
}