#pragma once

// Абстракция над родными бакендами для Windows

#include <d3d11.h>
#include "backends/imgui_impl_win32.h"
#include "backends/imgui_impl_dx11.h"

namespace ImBackends {

	inline ID3D11Device* g_pd3dDevice = nullptr;
	inline ID3D11DeviceContext* g_pd3dDeviceContext = nullptr;
	inline float main_scale = 0;
	inline HWND hwnd_host = 0;

	inline bool Init() {

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

		ImGui_ImplWin32_EnableDpiAwareness();
		main_scale = ImGui_ImplWin32_GetDpiScaleForMonitor(::MonitorFromPoint(POINT{ 0, 0 }, MONITOR_DEFAULTTOPRIMARY));
		WNDCLASSEXW wc = { sizeof(wc), CS_CLASSDC, DefWindowProcW, 0L, 0L, GetModuleHandle(nullptr), nullptr, nullptr, nullptr, nullptr, L"ImGui Example", nullptr };
		::RegisterClassExW(&wc);
		hwnd_host = ::CreateWindowW(wc.lpszClassName, L"Dear ImGui DirectX11 Example", WS_OVERLAPPEDWINDOW, 100, 100, 1, 1, nullptr, nullptr, wc.hInstance, nullptr);
		if (hwnd_host == 0) 
			return false;

		return true;
	}

	inline void InitRenders() {
		ImGui_ImplWin32_Init(hwnd_host);
		ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);
	}


	inline void Cleanup() {
		ImGui_ImplDX11_Shutdown();
		ImGui_ImplWin32_Shutdown();
		if (g_pd3dDeviceContext) { g_pd3dDeviceContext->Release(); g_pd3dDeviceContext = nullptr; }
		if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = nullptr; }
	}

	inline void NewFrame() {
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
	}

	inline void RenderVSync() {
		ImGui::Render();
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
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
