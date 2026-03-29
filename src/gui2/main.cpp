// Dear ImGui: standalone example application for DirectX 11

// Learn about Dear ImGui:
// - FAQ                  https://dearimgui.com/faq
// - Getting Started      https://dearimgui.com/getting-started
// - Documentation        https://dearimgui.com/docs (same as your local docs/ folder).
// - Introduction, links and more at the top of imgui.cpp


#include "imgui_internal.h"
#include "imgui.h"
#include "backends/imgui_impl_win32.h"
#include "backends/imgui_impl_dx11.h"
#include <d3d11.h>
#include <tchar.h>
#include "main_wnd.h"
#include "utils/WinTimer.h"
#include "TrayIcon.h"

// Data
ID3D11Device* g_pd3dDevice = nullptr;
static ID3D11DeviceContext* g_pd3dDeviceContext = nullptr;
static IDXGISwapChain* g_pSwapChain = nullptr;
static UINT                     g_ResizeWidth = 0, g_ResizeHeight = 0;
static ID3D11RenderTargetView* g_mainRenderTargetView = nullptr;

// Forward declarations of helper functions
bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void CreateRenderTarget();
void CleanupRenderTarget();
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

static bool ImWaitNewFrame(float minDelay = 0.012f) { // Real FPS would be 64 (1/0.015ms)
	using namespace std::chrono;

	// 1. SLEEP PHASE: Determine how long to wait before the next frame
	float toWaitSeconds = std::max(_g_wantFrameDelay, minDelay);

	do {
		auto start = steady_clock::now();

		// Wait for OS events or timeout (MsgWaitForMultipleObjectsEx uses milliseconds)
		DWORD res = ::MsgWaitForMultipleObjectsEx(
			0, NULL,
			static_cast<DWORD>(toWaitSeconds * 1000.0f),
			QS_ALLINPUT,
			MWMO_INPUTAVAILABLE | MWMO_ALERTABLE
		);

		if (res == WAIT_TIMEOUT) {
			break; // Wake up: requested delay has passed
		}

		// 2. MESSAGE PROCESSING: Handle Windows events
		MSG msg;
		while (::PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE)) {
			if (msg.message == WM_QUIT) return false;
			::TranslateMessage(&msg);
			::DispatchMessage(&msg); // Might trigger UI changes and update _g_wantFrameDelay
		}

		// Adjust remaining wait time based on time spent processing messages
		// If DispatchMessage set a shorter delay, respect it
		duration<float> elapsed = steady_clock::now() - start;
		toWaitSeconds -= elapsed.count();
		minDelay -= elapsed.count();
		if (toWaitSeconds > _g_wantFrameDelay) {
			// Try to reduce wait time, but not below the remaining minDelay threshol
			toWaitSeconds = std::max(minDelay, _g_wantFrameDelay);
		}

	} while (toWaitSeconds > 0.001f);

	// 3. BURST PHASE: Ensure a sequence of frames for UI consistency (inertia)
	{
		static int framesRemaining = 0;

		// Reset burst counter if we are starting a new interaction cycle
		if (framesRemaining <= 0) {
			framesRemaining = 3;
		}

		framesRemaining--;

		// If burst is active, force immediate next frame (0s delay).
		// Once finished, go into deep sleep (100s delay) until next event.
		_g_wantFrameDelay = (framesRemaining > 0) ? 0.0f : 100.0f;
	}

	return true;
}

static std::function wnd_handler = [](HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {return false; };

// Main code
int StartGui(bool show, bool err_conf) {
	// Make process DPI aware and obtain main monitor scale
	ImGui_ImplWin32_EnableDpiAwareness();
	float main_scale = ImGui_ImplWin32_GetDpiScaleForMonitor(::MonitorFromPoint(POINT{ 0, 0 }, MONITOR_DEFAULTTOPRIMARY));
	WNDCLASSEXW wc = { sizeof(wc), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(nullptr), nullptr, nullptr, nullptr, nullptr, L"ImGui Example", nullptr };
	::RegisterClassExW(&wc);
	HWND hwnd = ::CreateWindowW(wc.lpszClassName, L"Dear ImGui DirectX11 Example", WS_OVERLAPPEDWINDOW, 100, 100, 1280, 800, nullptr, nullptr, wc.hInstance, nullptr);
	if (hwnd == 0) return 1;
	g_guiHandle = hwnd;

	// Initialize Direct3D
	if (!CreateDeviceD3D(hwnd)) {
		CleanupDeviceD3D();
		::UnregisterClassW(wc.lpszClassName, wc.hInstance);
		return 1;
	}

	// Show the window
	::ShowWindow(hwnd, SW_HIDE);
	::UpdateWindow(hwnd);

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	auto ctx = ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigInputTextCursorBlink = false;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	//io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows
	io.ConfigViewportsNoAutoMerge = true;
	//io.ConfigViewportsNoTaskBarIcon = true;
	io.ConfigViewportsNoDefaultParent = true;
	//io.ConfigDockingAlwaysTabBar = true;
	//io.ConfigDockingTransparentPayload = true;

	// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
	SetStyle();

	// Setup scaling
	ImGuiStyle& style = ImGui::GetStyle();
	style.ScaleAllSizes(main_scale);        // Bake a fixed style scale. (until we have a solution for dynamic style scaling, changing this requires resetting Style + calling this again)
	style.FontScaleDpi = main_scale;        // Set initial font scale. (using io.ConfigDpiScaleFonts=true makes this unnecessary. We leave both here for documentation purpose)
	io.ConfigDpiScaleFonts = true;          // [Experimental] Automatically overwrite style.FontScaleDpi in Begin() when Monitor DPI changes. This will scale fonts but _NOT_ scale sizes/padding for now.
	io.ConfigDpiScaleViewports = true;      // [Experimental] Scale Dear ImGui and Platform Windows when Monitor DPI changes.
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
		style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;
	}

	// Setup Platform/Renderer backends
	ImGui_ImplWin32_Init(hwnd);
	ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);

	{
		wchar_t winPath[1000]; winPath[0] = 0;
		UINT result = GetWindowsDirectoryW(winPath, std::size(winPath));
		string path;
		if (result > 0 && result < std::size(winPath)) {
			path = StrUtils::Convert(winPath);
			if (path.back() != '\\') path += '\\';
			path += "Fonts\\"; // Склеиваем всё в Wide-строке
		}
		ImFont* font = nullptr;

		if (!font) {
			font = io.Fonts->AddFontFromFileTTF((path + "segoeui.ttf").c_str(), 24);
		}
		if (!font) {
			font = io.Fonts->AddFontFromFileTTF((path + "tahoma.ttf").c_str(), 20);
		}
		//if (!font) {
		//	auto data = WinUtils::GetResource(L"font2");
		//	if (!data.empty()) {
		//		ImFontConfig cfg{};
		//		cfg.FontDataOwnedByAtlas = false;
		//		io.Fonts->AddFontFromMemoryTTF(data.data(), data.size(), std::floorf(17), &cfg);
		//	}
		//}
		// fallback
		if (!font) {
			font = io.Fonts->AddFontDefault();
		}
	}

	MainWindow mainWindow(show, err_conf);
	Notific::Notificator notif;
	Notific::g_notif = &notif;
	WinTimer timer;
	TrayIcon trayIcon;
	timer.CycleTimer([&]() {
		if (notif.Process()) {
			ImWantFrameWithDelay(0);
		}
		}, 2000);

	wnd_handler = [&](HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {

		if (msg == WM_ShowWindow) {
			int mode = wParam;
			if (mode) notif.ShowHide();
			else mainWindow.ShowHide();
			ImWantFrameWithDelay(0);
			return true;
		}

		if (msg == WM_LayNotif) {
			trayIcon.Update((HKL)wParam);
			return true;
		}

		return false;
		};

	while (ImWaitNewFrame()) {
		// Start the Dear ImGui frame
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		// UI Logic
		mainWindow.DrawFrame();
		notif.Draw();

		// Timer update if needed
		if (notif.IsVisible()) {
			ImWantFrameWithDelay(0.5f);
		}

		// Rendering
		ImGui::Render();

		// Standard DX11 Render calls here...
		// g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, nullptr);
		// ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

		// Viewports support
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
		}

		// Present (no vsync needed as ImWaitNewFrame controls the pace)
		g_pSwapChain->Present(0, 0);
	}

	// Cleanup
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	CleanupDeviceD3D();
	::DestroyWindow(hwnd);
	::UnregisterClassW(wc.lpszClassName, wc.hInstance);

	return 0;
}

// Helper functions
bool CreateDeviceD3D(HWND hWnd) {
	// Setup swap chain
	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.BufferCount = 2;
	sd.BufferDesc.Width = 0;
	sd.BufferDesc.Height = 0;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = hWnd;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = TRUE;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	UINT createDeviceFlags = 0;
	//createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
	D3D_FEATURE_LEVEL featureLevel;
	const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };
	HRESULT res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext);
	if (res == DXGI_ERROR_UNSUPPORTED) // Try high-performance WARP software driver if hardware is not available.
		res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_WARP, nullptr, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext);
	if (res != S_OK)
		return false;

	CreateRenderTarget();
	return true;
}

void CleanupDeviceD3D() {
	CleanupRenderTarget();
	if (g_pSwapChain) { g_pSwapChain->Release(); g_pSwapChain = nullptr; }
	if (g_pd3dDeviceContext) { g_pd3dDeviceContext->Release(); g_pd3dDeviceContext = nullptr; }
	if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = nullptr; }
}

void CreateRenderTarget() {
	ID3D11Texture2D* pBackBuffer;
	g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
	g_pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &g_mainRenderTargetView);
	pBackBuffer->Release();
}

void CleanupRenderTarget() {
	if (g_mainRenderTargetView) { g_mainRenderTargetView->Release(); g_mainRenderTargetView = nullptr; }
}

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Win32 message handler
// You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
// - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
// - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
// Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	if (wnd_handler(hWnd, msg, wParam, lParam))
		return true;

	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
		return true;

	switch (msg) {
	case WM_SIZE:
		if (wParam == SIZE_MINIMIZED)
			return 0;
		g_ResizeWidth = (UINT)LOWORD(lParam); // Queue resize
		g_ResizeHeight = (UINT)HIWORD(lParam);
		return 0;
	case WM_SYSCOMMAND:
		if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
			return 0;
		break;
	case WM_DESTROY:
		::PostQuitMessage(0);
		return 0;
	}
	return ::DefWindowProcW(hWnd, msg, wParam, lParam);
}
