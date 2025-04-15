#pragma once

#include "imgui.h"
#include "imgui_sugar.hpp"
#include "imgui_stdlib.h"

#include "SetHotKeyCombo.h"

class MainWindow {
	bool check_enabled = false;
	bool check_add_to_auto = false;
	bool check_admin = false;
	bool check_altmode = false;
	bool show_demo_window = false;
	std::vector<SetHotKeyCombo> hotbox;
public:
	MainWindow()  {
		GETCONF
		for (const auto& it : cfg->hotkeysList) {
			hotbox.emplace_back(GetGuiTextForHk(it.hkId), it.keys.key(), 1);
		}
	}
	void DrawFrame() {

		if (show_demo_window)
			ImGui::ShowDemoWindow(&show_demo_window);

		if (!g_show_gui) return;

		GETCONF;

		{
			bool show = g_show_gui;
			ImGui::Begin("SimpleSwitcher", &show, ImGuiWindowFlags_NoCollapse);
			g_show_gui = show;
		}

		ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
		if (ImGui::BeginTabBar("MyTabBar", tab_bar_flags)) {
			if (ImGui::BeginTabItem("Settings")) {
				ImGui::Checkbox("Enable", &check_enabled);
				ImGui::Checkbox("Add to autostart", &check_add_to_auto);      // Edit bools storing our window open/close state
				ImGui::Checkbox("Work in programs running by admin", &check_admin);      // Edit bools storing our window open/close state
				ImGui::Checkbox("Alternative mode layout change (emulate LAlt + Shift)", &check_altmode);      // Edit bools storing our window open/close state

				//const char* combo_preview_value = items[item_selected_idx];
				const char* items[] = { "AAAA", "BBBB", "CCCC", "DDDD", "EEEE", "FFFF", "GGGG", "HHHH", "IIII", "JJJJ", "KKKK", "LLLLLLL", "MMMM", "OOOOOOO" };
				static int item_selected_idx = 0; // Here we store our selection data as an index.

				// Pass in the preview value visible before opening the combo (it could technically be different contents or not pulled from items[])
				const char* combo_preview_value = items[item_selected_idx];
				if (ImGui::BeginCombo("Set of flags", 0, 0)) {
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

				ImGui::EndTabItem();
			}
			with_TabItem("Hotkeys") {

				for (auto& it : hotbox) {
					it.Draw();
				}

			}

			with_TabItem("Experimental") {

				if (ImGui::Button("Show demo"))
					show_demo_window = true;

			}


			ImGui::EndTabBar();
		}

		ImGui::End();

	}
};
