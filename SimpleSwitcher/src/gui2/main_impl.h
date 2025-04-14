#pragma once

#include "imgui.h"

class MainImpl {
	bool check_enabled = false;
	bool check_add_to_auto = false;
	bool check_admin = false;
	bool check_altmode = false;
	bool exit;
public:
	void DrawFrame() {

		if (!ImGui::Begin("SimpleSwitcher", &exit, ImGuiWindowFlags_NoCollapse)) {
			int k = 0;
		}

		ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
		if (ImGui::BeginTabBar("MyTabBar", tab_bar_flags)) {
			if (ImGui::BeginTabItem("Settings")) {
				ImGui::Text((const char*)u8"Привет");
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
			if (ImGui::BeginTabItem("Hotkeys")) {



				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("Experimental")) {
				ImGui::EndTabItem();
			}
			ImGui::EndTabBar();
		}

		ImGui::End();


	}
};
