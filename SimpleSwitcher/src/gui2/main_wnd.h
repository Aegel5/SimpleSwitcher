#pragma once

#include "imgui.h"
#include "imgui_sugar.hpp"
#include "imgui_stdlib.h"

#include "gui_utils.h"
#include "SwAutostart.h"
#include "SetHotKeyCombo.h"

class MainWindow {
	string config_path;
	std::string title;
	bool check_add_to_auto = false;
	bool check_altmode = false;
	bool show_demo_window = false;
	UStr show_message = 0;
	std::vector<SetHotKeyCombo> hotbox;
	std::vector<SetHotKeyCombo> layout_hotkeys;
	std::vector<SetHotKeyCombo> layout_win_hotkeys;
private:
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
			if (ImGui::Button("OK", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
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

public:
	MainWindow()  {
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
	}
	void SafeUpdate() {
	}
	void DrawFrame() {

		if (!g_show_gui) return;

		if (show_demo_window)
			ImGui::ShowDemoWindow(&show_demo_window);

		int next_id = 5;

		GETCONF;

		ImGui::SetNextWindowSize({514,455}, ImGuiCond_FirstUseEver);

		{
			bool val = g_show_gui;
			ImGui::Begin(title.c_str(), &val, ImGuiWindowFlags_NoCollapse);
			g_show_gui = val;
		}

		ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
		if (ImGui::BeginTabBar("MyTabBar", tab_bar_flags)) {

			with_TabItem(LOC("Settings")) {

				{
					bool val = g_enabled.IsEnabled();
					if (ImGui::Checkbox("Enable", &val)) {
						if (val && !IsAdminOk()) {
							val = false;
							ShowMessageAdmin();
						}
						g_enabled.TryEnable(val);
					}
				}

				if (ImGui::Checkbox(LOC("Add to autostart"), &check_add_to_auto)) {
					if (!autostart_set(check_add_to_auto)) {
						ShowMessageAdmin();
					}
					check_add_to_auto = autostart_get();
				}

				DrawMessage();

				{
					bool val = cfg->isMonitorAdmin;
					if (ImGui::Checkbox(LOC("Work in programs running by admin"), &val)) {
						SaveConfigWith([val](auto p) {p->isMonitorAdmin = val; });
						if (!IsAdminOk()) g_enabled.TryEnable(false);
						check_add_to_auto = autostart_get();
					}
				}

				{
					bool val = cfg->AlternativeLayoutChange;
					if (ImGui::Checkbox(LOC("Alternative mode layout change (emulate LAlt + Shift)"), &val)) {
						SaveConfigWith([val](auto p) {p->AlternativeLayoutChange = val; });
					}
				}

				//if (ImGui::BeginCombo(LOC("Set of flags"), 0, 0)) {
				//	ImGui::EndCombo();
				//}

				{
					bool val = cfg->disableAccessebility;
					if (ImGui::Checkbox(LOC("Disable the accessibility shortcut keys (5 SHIFT and others)"), &val)) {
						SaveConfigWith([val](auto p) {p->disableAccessebility = val; });
						ApplyAcessebil();
					}
				}

				{
					bool val = cfg->fClipboardClearFormat;
					if (ImGui::Checkbox(LOC("Clear text format on Ctrl-C"), &val)) {
						SaveConfigWith([val](auto p) {p->fClipboardClearFormat = val; });
					}
				}

				{
					bool val = GetLogLevel() > LOG_LEVEL_DISABLE;
					if (ImGui::Checkbox(LOC("Enable debug log"), &val)) {
						SetLogLevel_info(val ? conf_get_unsafe()->logLevel : LOG_LEVEL_DISABLE);
					}
				}

				{
					if (ImGui::BeginCombo("Theme", cfg->theme.c_str(), 0)){
						for (const char* it : {"Dark","Light","Classic"} ) {
							if (ImGui::Selectable(it, cfg->theme == it)) {
								SaveConfigWith([&](auto p) {p->theme = it; });
								SetStyle();
							}
						}
						ImGui::EndCombo();
					}
				}

				ImGui::AlignTextToFramePadding();
				ImGui::TextLinkOpenURL("SimpleSwitcher.json", config_path.c_str());
				ImGui::SameLine();
				if (ImGui::Button("Reload")) {
					auto conf = ConfPtr(new ProgramConfig());
					auto errLoadConf = LoadConfig(*conf);
					IFS_LOG(errLoadConf);
					if (errLoadConf != SW_ERR_SUCCESS) {
						ShowMessage(LOC("Error reading config"));
					}
					else {
						_conf_set(conf);
					}
				}

			}

			with_TabItem(LOC("Hotkeys")) {

				for (int i = -1;  auto & it : hotbox) {
					it.Draw();
				}
				ImGui::SeparatorText("Language layouts");
				for (int i = -1; const auto & it : cfg->layouts_info.info) {
					i++;
					with_ID(next_id++) {
						bool val = it.enabled;
						if (ImGui::Checkbox("", &val)) {
							SaveConfigWith([&](auto p) {p->layouts_info.info[i].enabled ^= 1; });
						}
					}
					ImGui::SameLine();
					layout_hotkeys[i].Draw();
				}

			}

			with_TabItem(LOC("Experimental")) {

				if (Utils::IsDebug()) {
					if (ImGui::Button("Show demo"))
						show_demo_window = true;
				}

			}

			with_TabItem(LOC("About")) {
				ImGui::TextLinkOpenURL("Github", "https://github.com/Aegel5/SimpleSwitcher");
				ImGui::TextLinkOpenURL("Telegram", "https://t.me/simple_switcher");
			}


			ImGui::EndTabBar();
		}

		{
			float widthNeeded = 100 + ImGui::GetStyle().ItemSpacing.x;
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetContentRegionAvail().x - widthNeeded);
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + ImGui::GetContentRegionAvail().y - ImGui::GetFrameHeightWithSpacing()); // todo use child_wnd
			if (ImGui::Button("Close to tray", { 100,0 })) { g_show_gui = false; }
		}

		ImGui::End();

	}
};
