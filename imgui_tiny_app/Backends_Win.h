#pragma once

// Абстракция над родными бакендами для Windows

#include <d3d11.h>
#include "backends/imgui_impl_win32.h"
#include "backends/imgui_impl_dx11.h"

#pragma comment(lib, "d3d11.lib")

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace ImBackends {

	inline ID3D11Device* g_pd3dDevice = nullptr;
	inline ID3D11DeviceContext* g_pd3dDeviceContext = nullptr;
	inline float main_scale = 0;
	inline HWND hwnd_host = 0;

    namespace details {
        inline uint64_t lastTimerId = 8;
        inline std::vector<std::function<void()>> timerCallbacks;
        inline std::function<bool(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)> customH;
    }

    inline void CreateTimer(auto&& callback, int msDelay) {
        using namespace details;
        timerCallbacks.push_back(std::forward<decltype(callback)>(callback));
        SetTimer(hwnd_host, lastTimerId++, msDelay, NULL);
    }

    inline void SetCustomHandler(auto&& callback) {
        details::customH = std::forward<decltype(callback)>(callback);
    }

	namespace details {

		inline IDXGISwapChain* g_pSwapChain = nullptr;
		inline UINT                     g_ResizeWidth = 0, g_ResizeHeight = 0;
		inline ID3D11RenderTargetView* g_mainRenderTargetView = nullptr;

		inline void CreateRenderTarget() {
			ID3D11Texture2D* pBackBuffer;
			g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
			g_pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &g_mainRenderTargetView);
			pBackBuffer->Release();
		}

		inline void CleanupRenderTarget() {
			if (g_mainRenderTargetView) { g_mainRenderTargetView->Release(); g_mainRenderTargetView = nullptr; }
		}

		inline bool CreateDeviceD3D(HWND hWnd) {
			// Setup swap chain
			// This is a basic setup. Optimally could use e.g. DXGI_SWAP_EFFECT_FLIP_DISCARD and handle fullscreen mode differently. See #8979 for suggestions.
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

			// Disable DXGI's default Alt+Enter fullscreen behavior.
			// - You are free to leave this enabled, but it will not work properly with multiple viewports.
			// - This must be done for all windows associated to the device. Our DX11 backend does this automatically for secondary viewports that it creates.
			IDXGIFactory* pSwapChainFactory;
			if (SUCCEEDED(g_pSwapChain->GetParent(IID_PPV_ARGS(&pSwapChainFactory)))) {
				pSwapChainFactory->MakeWindowAssociation(hWnd, DXGI_MWA_NO_ALT_ENTER);
				pSwapChainFactory->Release();
			}

			CreateRenderTarget();
			return true;
		}

		

        inline LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {

            if (customH && customH(hWnd, msg, wParam, lParam) == 0) {
                return 0;
            }

            if (msg == WM_TIMER) {
                UINT_PTR timerId = wParam - 8;
                if (timerId < timerCallbacks.size() && timerId >= 0) {
                    timerCallbacks[timerId]();
                    return 0;
                }
            }

            if (!g_pSwapChain) return ::DefWindowProcW(hWnd, msg, wParam, lParam);

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

		inline void CleanupDeviceD3D() {
			CleanupRenderTarget();
			if (g_pSwapChain) { g_pSwapChain->Release(); g_pSwapChain = nullptr; }
			if (g_pd3dDeviceContext) { g_pd3dDeviceContext->Release(); g_pd3dDeviceContext = nullptr; }
			if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = nullptr; }
		}

	}

	inline bool Init(const wchar_t* title, int width, int height, bool hideMode = false) {

		ImGui_ImplWin32_EnableDpiAwareness();
		main_scale = ImGui_ImplWin32_GetDpiScaleForMonitor(::MonitorFromPoint(POINT{ 0, 0 }, MONITOR_DEFAULTTOPRIMARY));

		{
			WNDCLASSEXW wc = { sizeof(wc), CS_CLASSDC, details::WndProc, 0L, 0L, GetModuleHandle(nullptr), nullptr, nullptr, nullptr, nullptr, L"ImGui Example 2", nullptr };
			::RegisterClassExW(&wc);
			hwnd_host = ::CreateWindowW(wc.lpszClassName, title, WS_OVERLAPPEDWINDOW, 100, 100, width, height, nullptr, nullptr, wc.hInstance, nullptr);
			if (hwnd_host == 0)
				return false;
		}

		if (hideMode) {

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
		}
		else {
			using namespace details;
			if (!CreateDeviceD3D(hwnd_host)) {
				CleanupDeviceD3D();
				return false;
			}
		}

		return true;
	}

	inline bool InitDisableMainViewport() {
		return Init(L"ImGui App", 1, 1, true);
	}

	inline void InitRenders() {
		ImGui_ImplWin32_Init(hwnd_host);
		ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);
	}

	inline void Cleanup() {
		ImGui_ImplDX11_Shutdown();
		ImGui_ImplWin32_Shutdown();
		details::CleanupDeviceD3D();
	}

	inline void NewFrame() {

		{
			using namespace details;
			if (g_ResizeWidth != 0 && g_ResizeHeight != 0) {
				CleanupRenderTarget();
				g_pSwapChain->ResizeBuffers(0, g_ResizeWidth, g_ResizeHeight, DXGI_FORMAT_UNKNOWN, 0);
				g_ResizeWidth = g_ResizeHeight = 0;
				CreateRenderTarget();
			}
		}

		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
	}

	inline void RenderVSync() {

		ImGui::Render();

		using namespace details;
		if (g_mainRenderTargetView) {
			constexpr ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
			const float clear_color_with_alpha[4] = { clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w };
			g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, nullptr);
			g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, clear_color_with_alpha);
			ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
		}

		if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
		}

		if(g_pSwapChain)
			g_pSwapChain->Present(1, 0);
	}

	inline ImTextureID LoadTexture_RGBA8(unsigned char* image_data, int image_width, int image_height) {

		// Create texture
		D3D11_TEXTURE2D_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.Width = image_width;
		desc.Height = image_height;
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.SampleDesc.Count = 1;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		desc.CPUAccessFlags = 0;

		ID3D11Texture2D* pTexture = NULL;
		D3D11_SUBRESOURCE_DATA subResource;
		subResource.pSysMem = image_data;
		subResource.SysMemPitch = desc.Width * 4;
		subResource.SysMemSlicePitch = 0;
		g_pd3dDevice->CreateTexture2D(&desc, &subResource, &pTexture);

		if (!pTexture) 
			return 0;

		// Create texture view
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
		ZeroMemory(&srvDesc, sizeof(srvDesc));
		srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = desc.MipLevels;
		srvDesc.Texture2D.MostDetailedMip = 0;
		ID3D11ShaderResourceView* res = 0;
		g_pd3dDevice->CreateShaderResourceView(pTexture, &srvDesc, &res);
		pTexture->Release();

		return (ImTextureID)(void*)res;
	}

	inline void CleanupTexture(ImTextureID t) {
		((ID3D11ShaderResourceView*)(void*)t)->Release();
	}

	inline bool WaitNewFrame() {
		using namespace std::chrono;

		if (_g_wantFrameDelay <= 0) {
			// simple check messages
			MSG msg;
			while (::PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE)) {
				if (msg.message == WM_QUIT) return false;
				::TranslateMessage(&msg);
				::DispatchMessage(&msg); // Might trigger UI changes and update _g_wantFrameDelay
			}
		}
		else {
			float toWaitSeconds = _g_wantFrameDelay;
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
				if (toWaitSeconds > _g_wantFrameDelay) {
					// Try to reduce wait time, but not below the remaining minDelay threshol
					toWaitSeconds = _g_wantFrameDelay;
				}

			} while (toWaitSeconds > 0.001f);
		}

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
			_g_wantFrameDelay = (framesRemaining > 0) ? 0.0f : 200.0f;
		}

		return true;
	}

}
