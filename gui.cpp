#include "gui.hpp"
#include "xor.hpp"

#define hSize 300
#define vSize 470

#define HehexDSTR Decrypt({ 0x1, 0x2c, 0x21, 0x2c, 0x31, 0xd }).c_str()

#define ResetDefaultsSTR Decrypt({ 0x1b, 0x2c, 0x3a, 0x2c, 0x3d, 0x69, 0xd, 0x2c, 0x2f, 0x28, 0x3c, 0x25, 0x3d, 0x3a }).c_str()
#define ESPSTR Decrypt({ 0xc, 0x1a, 0x19 }).c_str()
#define RadarSTR Decrypt({ 0x1b, 0x28, 0x2d, 0x28, 0x3b }).c_str()
#define ScoreboardSTR Decrypt({ 0x1a, 0x2a, 0x26, 0x3b, 0x2c, 0x2b, 0x26, 0x28, 0x3b, 0x2d }).c_str()
#define AimbotRCSSTR Decrypt({ 0x8, 0x20, 0x24, 0x2b, 0x26, 0x3d, 0x69, 0x62, 0x69, 0x1b, 0xa, 0x1a }).c_str()
#define AimbotFOVSTR Decrypt({ 0x8, 0x20, 0x24, 0x2b, 0x26, 0x3d, 0x69, 0xf, 0x6, 0x1f }).c_str()
#define aAimbotFOVSTR Decrypt({ 0x6a, 0x6a, 0x8, 0x20, 0x24, 0x2b, 0x26, 0x3d, 0xf, 0x6, 0x1f }).c_str()
#define AimbotSmoothSTR Decrypt({ 0x8, 0x20, 0x24, 0x2b, 0x26, 0x3d, 0x69, 0x1a, 0x24, 0x26, 0x26, 0x3d, 0x21 }).c_str()
#define aAimbotSmoothSTR Decrypt({ 0x6a, 0x6a, 0x8, 0x20, 0x24, 0x2b, 0x26, 0x3d, 0x1a, 0x24, 0x26, 0x26, 0x3d, 0x21 }).c_str()
#define AimbotSmoothRandomSTR Decrypt({ 0x8, 0x20, 0x24, 0x2b, 0x26, 0x3d, 0x69, 0x1a, 0x24, 0x26, 0x26, 0x3d, 0x21, 0x69, 0x1b, 0x28, 0x27, 0x2d, 0x26, 0x24 }).c_str()
#define aAimbotSmoothRandSTR Decrypt({ 0x6a, 0x6a, 0x8, 0x20, 0x24, 0x2b, 0x26, 0x3d, 0x1a, 0x24, 0x26, 0x26, 0x3d, 0x21, 0x1b, 0x28, 0x27, 0x2d }).c_str()
#define AimbotOveraimSTR Decrypt({ 0x8, 0x20, 0x24, 0x2b, 0x26, 0x3d, 0x69, 0x6, 0x3f, 0x2c, 0x3b, 0x28, 0x20, 0x24 }).c_str()
#define aAimbotOverSTR Decrypt({ 0x6a, 0x6a, 0x8, 0x20, 0x24, 0x2b, 0x26, 0x3d, 0x6, 0x3f, 0x2c, 0x3b }).c_str()
#define RCSSmoothRandomSTR Decrypt({ 0x1b, 0xa, 0x1a, 0x69, 0x1a, 0x24, 0x26, 0x26, 0x3d, 0x21, 0x69, 0x1b, 0x28, 0x27, 0x2d, 0x26, 0x24 }).c_str()
#define aRCSSmoothRandSTR Decrypt({ 0x6a, 0x6a, 0x1b, 0xa, 0x1a, 0x1a, 0x24, 0x26, 0x26, 0x3d, 0x21, 0x1b, 0x28, 0x27, 0x2d }).c_str()
#define BoneSTR Decrypt({ 0xb, 0x26, 0x27, 0x2c }).c_str()
#define HeadSTR Decrypt({ 0x1, 0x2c, 0x28, 0x2d }).c_str()
#define NeckSTR Decrypt({ 0x7, 0x2c, 0x2a, 0x22 }).c_str()
#define ChestSTR Decrypt({ 0xa, 0x21, 0x2c, 0x3a, 0x3d }).c_str()
#define ResolutionSTR Decrypt({ 0x1b, 0x2c, 0x3a, 0x26, 0x25, 0x3c, 0x3d, 0x20, 0x26, 0x27 }).c_str()
#define aResolutionSTR Decrypt({ 0x6a, 0x6a, 0x1b, 0x2c, 0x3a, 0x26, 0x25, 0x3c, 0x3d, 0x20, 0x26, 0x27 }).c_str()

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace GUI {
	// Helper functions
	bool CreateDeviceD3D(HWND hWnd) {
		if ((g_pD3D = Direct3DCreate9(D3D_SDK_VERSION)) == NULL) {
			return false;
		}

		// Create the D3DDevice
		ZeroMemory(&g_d3dpp, sizeof(g_d3dpp));
		g_d3dpp.Windowed = TRUE;
		g_d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
		g_d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
		g_d3dpp.EnableAutoDepthStencil = TRUE;
		g_d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
		g_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;           // Present with vsync
		//g_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;   // Present without vsync, maximum unthrottled framerate
		if (g_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &g_d3dpp, &g_pd3dDevice) < 0) {
			return false;
		}

		return true;
	}

	void CleanupDeviceD3D() {
		if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = NULL; }
		if (g_pD3D) { g_pD3D->Release(); g_pD3D = NULL; }
	}

	void ResetDevice() {
		ImGui_ImplDX9_InvalidateDeviceObjects();
		HRESULT hr = g_pd3dDevice->Reset(&g_d3dpp);
		if (hr == D3DERR_INVALIDCALL) {
			IM_ASSERT(0);
		}
		ImGui_ImplDX9_CreateDeviceObjects();
	}

	// Win32 message handler
	LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam)) {
			return true;
		}

		switch (msg) {
		case WM_SIZE:
			if (g_pd3dDevice != NULL && wParam != SIZE_MINIMIZED) {
				g_d3dpp.BackBufferWidth = LOWORD(lParam);
				g_d3dpp.BackBufferHeight = HIWORD(lParam);
				ResetDevice();
			}
			return 0;
		case WM_SYSCOMMAND:
			if ((wParam & 0xfff0) == SC_KEYMENU) { // Disable ALT application menu
				return 0;
			}
			break;
		case WM_DESTROY:
			::PostQuitMessage(0);
			return 0;
		}
		return ::DefWindowProc(hWnd, msg, wParam, lParam);
	}

	HWND Setup(GUIStruct* GUIProps) {
		// Create main window
		RECT desktop;
		const HWND hDesktop = GetDesktopWindow();
		GetWindowRect(hDesktop, &desktop);
		GUIProps->left = desktop.left;
		GUIProps->right = desktop.right;
		GUIProps->bottom = desktop.bottom;
		GUIProps->top = desktop.top;
		GUIProps->wc = { sizeof(WNDCLASSEX), NULL, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, (HBRUSH)RGB(0, 0, 0), NULL, "HehexDD", NULL };
		RegisterClassEx(&GUIProps->wc);
		GUIProps->hwnd = CreateWindowEx(WS_EX_LAYERED, GUIProps->wc.lpszClassName, GUIProps->wc.lpszClassName, WS_POPUP, 0, 0, GUIProps->right, GUIProps->bottom, NULL, NULL, GUIProps->wc.hInstance, NULL);
		SetLayeredWindowAttributes(GUIProps->hwnd, RGB(0, 0, 0), 255, LWA_ALPHA | LWA_COLORKEY);
		GUIProps->msg;
		ZeroMemory(&GUIProps->msg, sizeof(GUIProps->msg));

		// Initialize Direct3D
		if (!CreateDeviceD3D(GUIProps->hwnd)) {
			CleanupDeviceD3D();
			::UnregisterClass(GUIProps->wc.lpszClassName, GUIProps->wc.hInstance);
			return nullptr;
		}

		// Show the window
		::ShowWindow(GUIProps->hwnd, SW_SHOWDEFAULT);
		::UpdateWindow(GUIProps->hwnd);

		// Setup Dear ImGui context
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

		// Setup Dear ImGui style
		ImGui::StyleColorsDark();

		// Setup Platform/Renderer bindings
		ImGui_ImplWin32_Init(GUIProps->hwnd);
		ImGui_ImplDX9_Init(g_pd3dDevice);

		return GUIProps->hwnd;
	}

	void windowLoop(Globals::myGlobals* Vars, GUIStruct* GUIProps) {
		// Poll and handle messages (inputs, window resize, etc.)
	// You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
	// - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
	// - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
	// Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
		if (::PeekMessage(&GUIProps->msg, NULL, 0U, 0U, PM_REMOVE)) {
			::TranslateMessage(&GUIProps->msg);
			::DispatchMessage(&GUIProps->msg);
			return;
		}

		// Start the Dear ImGui frame
		ImGui_ImplDX9_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize;
		//window_flags |= ImGuiWindowFlags_NoBackground;
		//window_flags |= ImGuiWindowFlags_NoTitleBar;
		//window_flags |= ImGuiWindowFlags_NoDecoration;

		{
			//ImGui::SetNextWindowBgAlpha(255.0f);
			ImGui::SetNextWindowSize(ImVec2(hSize, vSize));
			ImGui::SetNextWindowPos(ImVec2(float((GUIProps->right - hSize) / 2), float((GUIProps->bottom - vSize) / 2)));
			ImGui::Begin(HehexDSTR, NULL, window_flags);

			ImGui::Indent(90);
			if (ImGui::Button(ResetDefaultsSTR)) Vars->restoreValues(); //Reset Defaults
			ImGui::Spacing();
			ImGui::Separator();
			ImGui::Spacing();
			ImGui::Indent(-90);

			ImGui::Checkbox(ESPSTR, &Vars->bESP); //ESP

			ImGui::Checkbox(RadarSTR, &Vars->bRadar); //Radar

			ImGui::Checkbox(ScoreboardSTR, &Vars->bScoreWall); //Scoreboard Wall

			ImGui::Checkbox(AimbotRCSSTR, &Vars->bRCSAimbot); //RCSAimbot

			ImGui::Text(AimbotFOVSTR);
			ImGui::SliderFloat(aAimbotFOVSTR, &Vars->aimbotFOV, 1.0f, 360.0f, "%.1f"); //AimbotFOV
			if (Vars->aimbotFOV > 360.0f) Vars->aimbotFOV = 360.0f;
			else if (Vars->aimbotFOV < 1.0f) Vars->aimbotFOV = 1.0f;

			ImGui::Spacing();
			ImGui::Text(AimbotSmoothSTR);
			ImGui::SliderFloat(aAimbotSmoothSTR, &Vars->aimbotSmooth, 0.0f, 20.0f, "%.1f"); //AimbotSmooth
			if (Vars->aimbotSmooth > 20.0f) Vars->aimbotSmooth = 20.0f;
			else if (Vars->aimbotSmooth < 0.0f || (Vars->aimbotSmooth > 0.0f && Vars->aimbotSmooth < 1.0f)) Vars->aimbotSmooth = 1.0f;

			ImGui::Spacing();
			ImGui::Text(AimbotSmoothRandomSTR);
			ImGui::SliderFloat(aAimbotSmoothRandSTR, &Vars->aimbotSmoothRand, 0.0f, 5.0f, "%.2f"); //AimbotSmoothRand
			if (Vars->aimbotSmoothRand < 0.0f) Vars->aimbotSmoothRand = 0.0f;
			else if (Vars->aimbotSmoothRand > 5.0f) Vars->aimbotSmoothRand = 5.0f;

			ImGui::Spacing();
			ImGui::Text(AimbotOveraimSTR);
			ImGui::SliderFloat(aAimbotOverSTR, &Vars->aimbotOver, 0.0f, 5.0f, "%.2f"); //AimbotOver
			if (Vars->aimbotOver < 0.0f) Vars->aimbotOver = 0.0f;
			else if (Vars->aimbotOver > 5.0f) Vars->aimbotOver = 5.0f;

			ImGui::Spacing();
			ImGui::Text(RCSSmoothRandomSTR);
			ImGui::SliderFloat(aRCSSmoothRandSTR, &Vars->RCSSmoothRand, 0.0f, 2.0f, "%.1f"); //RCSSmoothRand
			if (Vars->RCSSmoothRand < 0.0f) Vars->RCSSmoothRand = 0.0f;
			else if (Vars->RCSSmoothRand > 2.0f) Vars->RCSSmoothRand = 2.0f;

			ImGui::Spacing();
			ImGui::Text(BoneSTR); //Bone
			ImGui::RadioButton(HeadSTR, &Vars->boneIndex, 8); ImGui::SameLine(); //Head
			ImGui::RadioButton(NeckSTR, &Vars->boneIndex, 7); ImGui::SameLine(); //Neck
			ImGui::RadioButton(ChestSTR, &Vars->boneIndex, 6); //Chest

			ImGui::Spacing();
			static const char* resolutions[]{"1920x1080", "1080x1080"};
			static int selectedResolution = 0;
			ImGui::Text(ResolutionSTR); //Resolution
			ImGui::Combo(aResolutionSTR, &selectedResolution, resolutions, IM_ARRAYSIZE(resolutions));
			switch (selectedResolution) {
			case 0:
				GUIProps->right = 1920;
				GUIProps->bottom = 1080;
				break;

			case 1:
				GUIProps->right = 1080;
				GUIProps->bottom = 1080;
				break;
			default:
				GUIProps->right = 1920;
				GUIProps->bottom = 1080;
				break;
			}

			ImGui::End();
		}

		// Rendering
		ImGui::EndFrame();
		g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, false);
		g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
		g_pd3dDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, false);
		D3DCOLOR clear_col_dx = D3DCOLOR_RGBA(0, 0, 0, 0);
		g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, clear_col_dx, 1.0f, 0);
		if (g_pd3dDevice->BeginScene() >= 0) {
			ImGui::Render();
			ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
			g_pd3dDevice->EndScene();
		}
		HRESULT result = g_pd3dDevice->Present(NULL, NULL, NULL, NULL);

		// Handle loss of D3D9 device
		if (result == D3DERR_DEVICELOST && g_pd3dDevice->TestCooperativeLevel() == D3DERR_DEVICENOTRESET) {
			ResetDevice();
		}
	}

	void Shutdown(HWND hwnd, WNDCLASSEX wc) {
		ImGui_ImplDX9_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();
		CleanupDeviceD3D();
		::DestroyWindow(hwnd);
		::UnregisterClass(wc.lpszClassName, wc.hInstance);
	}
}