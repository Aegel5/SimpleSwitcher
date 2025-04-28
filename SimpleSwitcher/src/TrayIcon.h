#pragma once

#include "utils/WinTray.h"
#include "IconManager.h"
class TrayIcon {
	HKL curlay = 0;
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
public:
	WinTray& TrayHandler() {
		return tray;
	}
	TrayIcon() {
		app_icon = LoadIcon(GetModuleHandle(0), MAKEINTRESOURCE(101));
		Update();
		tray.OnDouble([this]() { show_main_wind(); });
		tray.OnCreateMenu([this]() {
			std::vector<WinTray::TrayItem> res;
			for (const auto& it : conf_get_unsafe()->layouts_info.info) {
				auto lay = it.layout;
				res.push_back({ .name = StrUtils::Convert(Utils::GetNameForHKL(lay)), .callback = [lay]() {Worker()->PostMsg([lay](auto w) {w->SetNewLay(lay); }); } });
			}
			res.push_back({ .is_separator = true });
			res.push_back({ .name = LOC("Show"), .callback = [this]() { show_main_wind(); } });
			res.push_back({ .name = LOC("Enable"), .callback = [this]() { try_toggle_enable(); }, .is_checkbox = true, .edit_val = g_enabled.IsEnabled() });
			res.push_back({ .name = LOC("Exit"), .callback = []() { PostQuitMessage(0); } });
			return res;
			});
	}
	void Update(HKL lay = 0) {

		if (lay != 0) {
			curlay = lay;
		}
		if (conf_get_unsafe()->flagsSet == ProgramConfig::showFlags_Nothing) {
			tray.DeleteIcon();
			return;
		}
		if (curlay == 0) {
			curlay = Utils::GetFocusedWndInfo().lay;
		}
		wstring id;
		{
			WORD langid = LOWORD(curlay);

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
		auto icon = IconMgr::Inst().GetIcon(id.c_str(), GetSize(), !g_enabled.IsEnabled());
		if (!icon->IsOk()) {
			tray.SetIcon(app_icon);
			return;
		}

		tray.SetIcon(icon->hicon);
	}
};
