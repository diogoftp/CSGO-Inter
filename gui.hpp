#include <Windows.h>
#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx9.h"
#include "imgui/imgui_impl_win32.h"
#include <d3d9.h>
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <tchar.h>
#include "xor.hpp"

extern LPDIRECT3D9              g_pD3D;
extern LPDIRECT3DDEVICE9        g_pd3dDevice;
extern D3DPRESENT_PARAMETERS    g_d3dpp;

// Forward declarations of helper functions
bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void ResetDevice();
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
int guiSetup(HWND hwnd, WNDCLASSEX wc);
void windowLoop(HWND hwnd, MSG msg, int hsize, int vsize, bool* bESP, bool* bRadar, float* aimbotFOV, float* aimbotSmooth, bool* bRCSAimbot);
void guiEnd(HWND hwnd, WNDCLASSEX wc);