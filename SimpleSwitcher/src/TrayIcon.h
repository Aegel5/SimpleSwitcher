#pragma once

#include "utils/WinTray.h"
#include "IconManager.h"
extern int StartGui();
class TrayIcon {
	
	uint64_t last_lay_cnt = 0;
	WinTray tray;
	HICON app_icon = 0;
	Vec2 GetSize() {
		int iconWidth = GetSystemMetrics(SM_CXSMICON);
		int iconHeight = GetSystemMetrics(SM_CYSMICON);
		int bigIconWidth = GetSystemMetrics(SM_CXICON);
		int bigIconHeight = GetSystemMetrics(SM_CYICON);
		auto scale = WinUtils::GetDpiMainMonScale();
		return{ iconWidth * scale.x, iconHeight * scale.y };
	}
	void ShowGui() {

	}
public:
	WinTray& TrayHandler() {
		return tray;
	}
	TrayIcon() {
		tray.OnDouble([this]() { ShowGui(); });
		tray.OnCreateMenu([this]() {
			std::vector<WinTray::TrayItem> res;
			res.push_back({ .name = LOC("Show"), .callback = [this]() { ShowGui(); } });
			res.push_back({ .name = LOC("Exit"), .callback = []() { PostQuitMessage(0); } });
			return res;
			});
	}
public:void UpdateCheck() {
		if (last_lay_cnt != g_layout_change_cnt) {
			last_lay_cnt = g_layout_change_cnt;
			Update(!g_enabled.IsEnabled());
		}
	}
void Update(bool is_gray=false) {
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
		auto icon = IconMgr::Inst().GetIcon(StrUtils::Convert(id).c_str(), GetSize(), is_gray);
		if (!icon->IsOk()) {
			tray.SetIcon(app_icon);
			return;
		}

		tray.SetIcon(icon->hicon);
	}
};
