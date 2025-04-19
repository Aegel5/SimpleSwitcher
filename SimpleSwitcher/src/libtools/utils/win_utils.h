#pragma once

#include "WinApiInt.h"

namespace WinUtils {
	inline std::pair<float,float> GetDpiMainMonScale() {

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
		return std::max(res.first, res.second);
	}
}
