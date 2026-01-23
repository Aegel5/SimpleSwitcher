#pragma once

#include "utils/WinTray.h"
#include "IconManager.h"
#include "gui2/Notificator/Notificator.h"

class TrayIcon {
	HKL curlay = 0;
	uint64_t last_lay_cnt = 0;
	WinTray tray;
	HICON app_icon = 0;
	Vec_i2 GetSize() {

		UINT dpi = GetDpiForSystem();

		// Получаем ширину и высоту малой иконки (для трея) с учетом DPI
		int iconWidth = GetSystemMetricsForDpi(SM_CXSMICON, dpi);
		int iconHeight = GetSystemMetricsForDpi(SM_CYSMICON, dpi);

		return { iconWidth, iconHeight };

		//int iconWidth = GetSystemMetrics(SM_CXSMICON);
		//int iconHeight = GetSystemMetrics(SM_CYSMICON);
		//int bigIconWidth = GetSystemMetrics(SM_CXICON);
		//int bigIconHeight = GetSystemMetrics(SM_CYICON);
		//auto scale = WinUtils::GetDpiMainMonScale();
		//return{ RoundToInt(iconWidth * scale.x), RoundToInt(iconHeight * scale.y) };
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

			// notif
			if (conf_get_unsafe()->ShowReminderInTrayMenu) {
				bool has_notif = false;
				for (const auto& it : Notific::g_notif->SordedEntries()) {
					has_notif = true;
					res.push_back({ .name = it, .callback = []() { show_main_wind(1); } });
				}
				if (has_notif) {
					res.push_back({ .is_separator = true });
				}
			}

			// layouts
			if (conf_get_unsafe()->ShowLangsInTrayMenu) {
				for (const auto& it : conf_get_unsafe()->layouts_info.info) {
					auto lay = it.layout;
					res.push_back({ .name = StrUtils::Convert(Utils::GetNameForHKL(lay)), .callback = [lay]() {Worker()->PostMsg([lay](auto w) {w->SetNewLay(lay); }); } });
				}
				res.push_back({ .is_separator = true });
			}

			// menu
			res.push_back({ .name = LOC("Show"), .callback = []() { show_main_wind(); } });
			res.push_back({ .name = LOC("Enable"), .callback = []() { try_toggle_enable(); }, .is_checkbox = true, .edit_val = g_enabled.IsEnabled() });
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

		auto id  = Utils::GetNameForHKL_simple(curlay);
		LOG_ANY(L"mainguid new layout: {}, name={}", (void*)lay, id);

		if (!id.empty()) {
			auto icon = IconMgr::Inst().GetIcon(id.c_str(), GetSize(), !g_enabled.IsEnabled());
			if (icon->IsOk()) {
				tray.SetIcon(icon->hicon);
				return;
			}
		}

		// fallback
		tray.SetIcon(app_icon);
	}
};
