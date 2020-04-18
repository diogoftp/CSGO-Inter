#pragma once
#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx9.h"
#include "imgui/imgui_impl_win32.h"
#include "vars.hpp"
#include <d3d9.h>

#define DIRECTINPUT_VERSION 0x0800

extern LPDIRECT3D9              g_pD3D;
extern LPDIRECT3DDEVICE9        g_pd3dDevice;
extern D3DPRESENT_PARAMETERS    g_d3dpp;

extern Globals::myGlobals Vars;

namespace GUI {
	struct GUIStruct {
		HWND hwnd = nullptr;
		WNDCLASSEX wc{ 0 };
		int left = 0;
		int right = 0;
		int bottom = 0;
		int top = 0;
		MSG msg{ 0 };
	};

	// Forward declarations of helper functions
	HWND Setup(GUIStruct* GUIProps);
	void Shutdown(HWND hwnd, WNDCLASSEX wc);
	bool CreateDeviceD3D(HWND hWnd);
	void CleanupDeviceD3D();
	void ResetDevice();
	LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	void windowLoop(Globals::myGlobals* Vars, GUIStruct* GUIProps);
}