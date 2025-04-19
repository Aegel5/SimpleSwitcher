#pragma once

#include "utils/WinTray.h"

class TrayIcon {
	WinTray tray;
	HICON app_icon = 0;
	ImVec2 GetSize() {
		int iconWidth = GetSystemMetrics(SM_CXSMICON);
		int iconHeight = GetSystemMetrics(SM_CYSMICON);
		int bigIconWidth = GetSystemMetrics(SM_CXICON);
		int bigIconHeight = GetSystemMetrics(SM_CYICON);
		auto scale = WinUtils::GetDpiMainMonScale();
		return{ iconWidth * scale.first, iconHeight * scale.second };
	}
public:
	WinTray& TrayHandler() {
		return tray;
	}
	void Update(bool is_gray) {
		auto cur = Utils::GetFocusedWndInfo();
		auto lay = cur.lay;
		wstring id;
		{
			WORD langid = LOWORD(lay);

			TCHAR buf[512];
			buf[0] = 0;

			int flag = LOCALE_SNAME;
			int len = GetLocaleInfo(MAKELCID(langid, SORT_DEFAULT), flag, buf, std::ssize(buf));
			IFW_LOG(len != 0);

			auto len_str = wcslen(buf);
			if (len_str >= 2) {
				TStr name = buf + len_str - 2;
				id = name;
				StrUtils::ToUpper(id);
				LOG_ANY(L"mainguid new layout: {}, name={}", (void*)lay, id);
			}
		}
		if (id.empty()) {
			tray.SetIcon(app_icon);
			return;
		}
		auto icon = Test::Inst().GetIcon(StrUtils::Convert(id).c_str(), GetSize(), is_gray);
		if (!icon->IsOk()) {
			tray.SetIcon(app_icon);
			return;
		}

		tray.SetIcon(icon->hicon);
	}
};
