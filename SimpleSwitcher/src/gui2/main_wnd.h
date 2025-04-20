#pragma once

#include "imgui.h"
#include "imgui_sugar.hpp"
#include "imgui_stdlib.h"

#include "gui_utils.h"
#include "SwAutostart.h"
#include "SetHotKeyCombo.h"
#include "IconManager.h"
#include "TrayIcon.h"

class MainWindow {
	string config_path;
	std::string title;
	bool check_add_to_auto = false;
	bool check_altmode = false;
	bool show_demo_window = false;
	bool show_main = true;
	UStr show_message = 0;
	std::vector<SetHotKeyCombo> hotbox;
	std::vector<SetHotKeyCombo> layout_hotkeys;
	std::vector<SetHotKeyCombo> layout_win_hotkeys;
	COM::CAutoCOMInitialize autoCom;
	std::vector<string> flagsSets;
	TrayIcon tray;
	uint64_t last_lay_cnt = 0;
	HWND hwnd = 0;
	POINT cursorPos{-1,-1};
	std::vector<std::pair<string, HKL>> menu_lays;
	CoreWorker coreWork;
	ImVec2 startsize{ 544.0, 544.0/1.12 };
private:
	void update_flags() { flagsSets = { std::from_range, IconMgr::Inst().ScanFlags() }; }
	bool IsAdminOk() { return Utils::IsSelfElevated() || !conf_get_unsafe()->isMonitorAdmin; }
	void ShowMessage(UStr msg) {
		show_message = msg;
		ImGui::OpenPopup(LOC("Message"));
	}
	void ShowMessageAdmin() { ShowMessage(LOC("Need admin right")); }
	void DrawMessage() {
		ImVec2 center = ImGui::GetWindowViewport()->GetCenter();
		ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
		if (ImGui::BeginPopupModal(LOC("Message"), 0, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings)) {
			ImGui::Text(show_message);
			//ImGui::Separator();
			if (ImGui::Button("OK", ImVec2(ImGui::CalcTextSize("OK").x * 4, 0))) { ImGui::CloseCurrentPopup(); }
			ImGui::EndPopup();
		}
	}
	void SyncLays() {
		SyncLayouts();
		GETCONF;
		std::vector<CHotKey> def_list = {
			CHotKey(VK_LCONTROL).SetKeyup(),
			CHotKey(VK_RCONTROL).SetKeyup(),
			CHotKey(VK_LSHIFT).SetDouble(),
			CHotKey(VK_RSHIFT).SetDouble()
		};
		layout_hotkeys.clear();
		layout_win_hotkeys.clear();
		for (int i = -1; const auto & it : cfg->layouts_info.info) {
			i++;
			layout_hotkeys.emplace_back( StrUtils::Convert(Utils::GetNameForHKL(it.layout)), it.hotkey.key(), def_list,
				[i](auto key) {
				SaveConfigWith([&](auto conf) {
					if (i >= conf->layouts_info.info.size()) return;
					auto& rec = conf->layouts_info.info[i];
					rec.hotkey.key() = key;
					});
				}
			);

		}
	}
	void DrawFrameActual();
public:
	MainWindow()  {
		SetLogLevel(Utils::IsDebug() ? LOG_LEVEL_2 : LOG_LEVEL_DISABLE);
		__g_config.reset(new ProgramConfig());
		auto errLoadConf = LoadConfig(*__g_config);
		if (errLoadConf != TStatus::SW_ERR_SUCCESS) {
			IFS_LOG(errLoadConf);
			ShowMessage("Error load config"); // todo check
		}
		else {
			if (__g_config->config_version != SW_VERSION) {
				__g_config->config_version = SW_VERSION;
				SaveConfigWith([](auto cfg) {}); // пересохраним конфиг, чтобы туда добавились все последние настройки, которые заполнены по-умолчанию.
			}
		}
		IFS_LOG(autoCom.Init());
		setlocale(LC_ALL, "en_US.utf8");
		IFS_LOG(update_cur_dir());
		LOG_ANY("Start program {}", SW_VERSION);
		InitImGui();
		title = std::format(
			"SimpleSwitcher {}{}{}###main_wnd", SW_VERSION,
			Utils::IsSelfElevated() ? " Administrator" : "",
			Utils::IsDebug() ? " DEBUG" : "");
		GETCONF;
		for (int i = -1; const auto & it : cfg->hotkeysList) {
			i++;
			hotbox.emplace_back(GetGuiTextForHk(it.hkId), it.keys.key(), it.def_list, 
				[i](auto k) {
				SaveConfigWith([i,k](auto p) {p->hotkeysList[i].keys.key() = k; });
				});
		}
		if (IsAdminOk()) {
			if (Utils::IsDebug()) {
				if (!g_enabled.TryEnable()) {
					auto hk = conf_get_unsafe()->GetHk(hk_ToggleEnabled).keys.key();
					for (auto& it : hk) if (it == VKE_WIN) it = VK_LWIN;
					InputSender::SendHotKey(hk);
					Sleep(50);
				}
			}
			g_enabled.TryEnable();
		}
		check_add_to_auto = autostart_get();
		SyncLays();
		ApplyAcessebil();
		config_path = StrUtils::Convert(std::format(L"file://{}", GetPath_Conf()));
		show_main = !g_autostart;
		update_flags();
		tray.TrayHandler().OnDouble([this]() { Show(); });
		tray.TrayHandler().OnRight([this]() { GetCursorPos(&cursorPos);});
		coreWork.Start();
		auto scal = WinUtils::GetDpiMainMonScale();
		startsize.x *= scal.first;
		startsize.y *= scal.second;
	}
	void Show() {
		show_main = true;
		if(hwnd != 0)
			SetForegroundWindow(hwnd);
	}
	void SafeUpdate() {
	}
	void DrawTrayMenu() {
		if (cursorPos.x != -1) {
			ImGui::SetNextWindowPos({ (float)cursorPos.x, (float)cursorPos.y }, ImGuiCond_Always);
			ImGui::OpenPopup("tray menu", 0);
			cursorPos.x = -1;
		}
		if (ImGui::BeginPopup("tray menu", ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings)) {


			if (ImGui::IsWindowAppearing()) {
				menu_lays.clear();
				for (const auto & it : conf_get_unsafe()->layouts_info.info) {
					menu_lays.emplace_back(StrUtils::Convert(Utils::GetNameForHKL(it.layout)), it.layout);
				}
			}

			HWND hwnd = (HWND)ImGui::GetWindowViewport()->PlatformHandle;
			if (ImGui::GetWindowViewport()->PlatformWindowCreated) {
				static HWND last_hwnd = 0;
				if (last_hwnd != hwnd) {
					last_hwnd = hwnd;
					SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
					SetForegroundWindow(hwnd);
					ImGui::GetPlatformIO().Platform_SetWindowFocus(ImGui::GetWindowViewport());
				}
				if (!ImGui::GetPlatformIO().Platform_GetWindowFocus(ImGui::GetWindowViewport())) {
					ImGui::CloseCurrentPopup();
				}
			}

			for (const auto& it : menu_lays) {
				if (ImGui::Selectable(it.first.c_str(), false)) {
					auto lay = it.second;
					Worker()->PostMsg([lay](auto w) {w->SetNewLay(lay); });
				}
			}

			ImGui::Separator();

			{
				bool val = g_enabled.IsEnabled();
				if (ImGui::Checkbox(LOC("Enable"), &val)) {
					g_enabled.TryEnable(val);
					ImGui::CloseCurrentPopup();
				}
			}

			if (ImGui::Selectable(LOC("Show"), false)) {
				Show();
			}

			if (ImGui::Selectable(LOC("Exit"), false)) {
				PostQuitMessage(0);
			}
			ImGui::EndPopup();
		}
	}
public: void DrawFrame() {

	DrawTrayMenu();

	if (show_main) {

		DrawFrameActual();

		if (show_demo_window)
			ImGui::ShowDemoWindow(&show_demo_window);
	}

	if (last_lay_cnt != g_layout_change_cnt) {
		last_lay_cnt = g_layout_change_cnt;
		tray.Update(!g_enabled.IsEnabled());
	}

}
};
