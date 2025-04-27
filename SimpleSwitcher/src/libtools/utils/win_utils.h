#pragma once

#include "WinApiInt.h"

namespace WinUtils {

	inline Vec2 GetDpiMainMonScale() {

		if (!WinApiInt::GetDpiForMonitor) {
			return { 1.0f,1.0f };
		}

		HMONITOR hMonitor = MonitorFromWindow(NULL, MONITOR_DEFAULTTOPRIMARY);
		UINT dpiX = 96, dpiY = 96;
		WinApiInt::GetDpiForMonitor(hMonitor, MDT_EFFECTIVE_DPI, &dpiX, &dpiY);

		// Корректируем размеры с учетом DPI
		return{ dpiX / 96.0f,  dpiY / 96.0f };
	}
	inline float GetDpiMainMonScale2() {
		auto res = GetDpiMainMonScale();
		return std::max(res.x, res.y);
	}
	inline HWND CreateMsgWin(TStr name, WNDPROC proc = 0) {
		WNDCLASS wc = {};
		wc.lpfnWndProc = proc != 0?proc: DefWindowProc;
		wc.hInstance = GetModuleHandle(NULL);
		wc.lpszClassName = name;
		RegisterClass(&wc);
		return CreateWindowW(name, name, 0, 0, 0, 0, 0, HWND_MESSAGE, NULL, wc.hInstance, NULL);
	}
	inline void PostMsg(HWND hwnd, UINT msg, WPARAM wparm = 0) {
		if(hwnd != 0) PostMessage(hwnd, msg, wparm, 0);
	}
}


