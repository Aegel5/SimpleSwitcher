// Dear ImGui: standalone example application for Windows API + DirectX 11

// Learn about Dear ImGui:
// - FAQ                  https://dearimgui.com/faq
// - Getting Started      https://dearimgui.com/getting-started
// - Documentation        https://dearimgui.com/docs (same as your local docs/ folder).
// - Introduction, links and more at the top of imgui.cpp


#include "imgui_internal.h"
#include "imgui.h"
#include "backends/imgui_impl_win32.h"
#include "backends/imgui_impl_dx11.h"
#include "misc/freetype/imgui_freetype.h"
#include <d3d11.h>
#include <tchar.h>
#include "main_wnd.h"
#include "utils/WinTimer.h"
#include "TrayIcon.h"
#include "ImWaitNewFrame.h"
#include "LoadFonts.h"

// Data
ID3D11Device*            g_pd3dDevice = nullptr; // no static
static ID3D11DeviceContext*     g_pd3dDeviceContext = nullptr;

bool CreateDeviceD3D() {

	UINT createDeviceFlags = 0;
	// createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;

	D3D_FEATURE_LEVEL featureLevel;
	const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0 };

	// 1. Пытаемся создать аппаратное устройство
	HRESULT res = D3D11CreateDevice(
		nullptr,                    // Видеоадаптер (nullptr — по умолчанию)
		D3D_DRIVER_TYPE_HARDWARE,   // Тип драйвера
		nullptr,                    // Дескриптор программного драйвера
		createDeviceFlags,          // Флаги (например, DEBUG)
		featureLevelArray, 2,       // Массив поддерживаемых версий
		D3D11_SDK_VERSION,          // Версия SDK
		&g_pd3dDevice,              // Результат: устройство
		&featureLevel,              // Результат: выбранный уровень функций
		&g_pd3dDeviceContext        // Результат: контекст
	);

	// 2. Если железо не тянет, пробуем WARP (программный рендеринг)
	if (res == DXGI_ERROR_UNSUPPORTED) {
		res = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_WARP, nullptr, createDeviceFlags,
			featureLevelArray, 2, D3D11_SDK_VERSION,
			&g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext);
	}

	if (FAILED(res))
		return false;

	return true;
}

void CleanupDeviceD3D() {
	if (g_pd3dDeviceContext) { g_pd3dDeviceContext->Release(); g_pd3dDeviceContext = nullptr; }
	if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = nullptr; }
}

// Main code
int StartGui(bool show, bool err_conf) {
	// Make process DPI aware and obtain main monitor scale
	ImGui_ImplWin32_EnableDpiAwareness();
	float main_scale = ImGui_ImplWin32_GetDpiScaleForMonitor(::MonitorFromPoint(POINT{ 0, 0 }, MONITOR_DEFAULTTOPRIMARY));
	WNDCLASSEXW wc = { sizeof(wc), CS_CLASSDC, DefWindowProcW, 0L, 0L, GetModuleHandle(nullptr), nullptr, nullptr, nullptr, nullptr, L"ImGui Example", nullptr };
	::RegisterClassExW(&wc);
	HWND hwnd = ::CreateWindowW(wc.lpszClassName, L"Dear ImGui DirectX11 Example", WS_OVERLAPPEDWINDOW, 100, 100, 1, 1, nullptr, nullptr, wc.hInstance, nullptr);
	if (hwnd == 0) return 1;

	// Initialize Direct3D
	if (!CreateDeviceD3D()) {
		CleanupDeviceD3D();
		return 1;
	}


	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
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
    style.FontScaleDpi = main_scale;        // Set initial font scale. (in docking branch: using io.ConfigDpiScaleFonts=true automatically overrides this for every window depending on the current monitor)
	io.ConfigDpiScaleFonts = true;          // [Experimental] Automatically overwrite style.FontScaleDpi in Begin() when Monitor DPI changes. This will scale fonts but _NOT_ scale sizes/padding for now.
	io.ConfigDpiScaleViewports = true;      // [Experimental] Scale Dear ImGui and Platform Windows when Monitor DPI changes.

    // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
		style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;
	}

	// Setup Platform/Renderer backends
	ImGui_ImplWin32_Init(hwnd);
	ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);

	LoadFonts();


	MainWindow mainWindow(show, err_conf);
	Notific::Notificator notif;
	Notific::g_notif = &notif;
	WinTimer timer;
	g_guiHandle = timer.GetHandler();
	TrayIcon trayIcon;
	timer.CycleTimer([&]() {
		if (notif.Process()) {
			ImWantFrameWithDelay(0);
		}
		}, 2000);

	timer.CustomHandler( [&](HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
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
		});

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

		// Viewports support
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
#ifdef SS_WIN_7_COMPAT
		Sleep(1);
#endif
	}

	// Cleanup
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	CleanupDeviceD3D();

	return 0;
}

