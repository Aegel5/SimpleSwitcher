#pragma once

#include "imgui.h"
#include "imgui_sugar.hpp"
#include "imgui_stdlib.h"

#include "SwAutostart.h"
#include "SetHotKeyCombo.h"

class MainWindow {
	bool check_add_to_auto = false;
	bool check_altmode = false;
	bool show_demo_window = false;
	UStr show_message = 0;
	std::vector<SetHotKeyCombo> hotbox;
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
		if (ImGui::BeginPopupModal(LOC("Message"), 0, ImGuiWindowFlags_AlwaysAutoResize)) {
			ImGui::Text(show_message);
			//ImGui::Separator();
			if (ImGui::Button("OK", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
			ImGui::EndPopup();
		}
	}

public:
	MainWindow()  {
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
	}
	void DrawFrame() {

		if (!g_show_gui) return;

		if (show_demo_window)
			ImGui::ShowDemoWindow(&show_demo_window);

		GETCONF;

		{
			bool val = g_show_gui;
			ImGui::Begin("SimpleSwitcher", &val, ImGuiWindowFlags_NoCollapse);
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

				if (ImGui::BeginCombo(LOC("Set of flags"), 0, 0)) {
					//for (int n = 0; n < IM_ARRAYSIZE(items); n++) {
					//	const bool is_selected = (item_selected_idx == n);
					//	if (ImGui::Selectable(items[n], is_selected))
					//		item_selected_idx = n;

					//	// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
					//	if (is_selected)
					//		ImGui::SetItemDefaultFocus();
					//}
					ImGui::EndCombo();
				}

			}

			with_TabItem(LOC("Hotkeys")) {

				for (auto& it : hotbox) {
					it.Draw();
				}

			}

			with_TabItem(LOC("Experimental")) {

				if (ImGui::Button("Show demo"))
					show_demo_window = true;

			}


			ImGui::EndTabBar();
		}

		ImGui::End();

	}
};
