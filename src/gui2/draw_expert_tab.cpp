#include "main_wnd.h"

void MainWindow::Draw_expert_tab() {

	with_TabItem(LOC("Expert")) {

		//{
		//	ImGui::SameLine();
		//	if (ImGui::Button(LOC("Styles..."))) {
		//		ShowStyleEditor ^= 1;
		//	}
		//}

		{
			if (ImGui::Checkbox(LOC("Skip low level inject keys"), &conf_gui()->SkipLowLevelInjectKeys)) {
				SaveApplyGuiConfig();
			}
			ImGui::SetItemTooltip(LOC("Disable interception of keys sent to remote computer for RDP connection"));
		}

		{
			if (ImGui::Checkbox(LOC("Disable Ctrl + LAlt as RAlt on extended layouts"), &conf_gui()->fixRAlt)) {
				SaveApplyGuiConfig();
			}
			ImGui::SetItemTooltip("%s %s", LOC("This is done by temporary switching to layout"), "[config/fixRAlt_lay_]");
		}

		{
			if (ImGui::Checkbox(LOC("Use the British flag for the English language"), &conf_gui()->useBritishFlag)) {
				SaveApplyGuiConfig();
				update_flags();
				new_layout_request();
			}
		}

		{
			if (ImGui::Checkbox(LOC("Enable V-Sync"), &conf_gui()->vsync)) {
				SaveApplyGuiConfig();
			}
			ImGui::SetItemTooltip(LOC("Improves UI smoothness, but may require more GPU resources"));
		}

		{
			int val = conf_gui()->quick_press_ms;
			if (ImGui::InputInt(LOC("Double tap interval ms"), &val)) {
				val = std::clamp(val, 0, 1000);
				conf_gui()->quick_press_ms = val;
				SaveApplyGuiConfig();
			}

		}





		//{
		//	ImGui::SameLine();
		//	int val = 70;
		//	//float width = ImGui::CalcTextSize("000").x
		//	//	+ ImGui::GetFrameHeight() * 2.0f // Место под кнопки + и -
		//	//	+ ImGui::GetStyle().FramePadding.x * 2.0f
		//	//	+ ImGui::GetStyle().ItemInnerSpacing.x * 2.0f;
		//	ImGui::SetNextItemWidth(ImGui::GetFrameHeight() * 3.5f);
		//	if (ImGui::InputInt("##ALPHA_ID", &val)) {
		//	}
		//}



		if (ImGui::CollapsingHeader("Scancode remap")) {

			if (!remap_open) {
				remap_open = true;
				IFS_LOG(remap.FromRegistry());
			}

			auto edit = [&](auto vk, auto vk2, UStr title) {
				TKeyCode remapped;
				remap.GetRemapedKey(vk, remapped);
				bool val = remapped == vk2;
				if (ImGui::Checkbox(title, &val)) {
					IFS_LOG(remap.FromRegistry());
					if (val) {
						remap.PutRemapKey(vk, vk2);
					}
					else {
						remap.DelRemapKey(vk);
					}
					IFS_LOG(remap.ToRegistry());
					IFS_LOG(remap.FromRegistry());
				}
				};

			edit(VK_CAPITAL, VK_F24, LOC("Remap CapsLock as F24"));
			edit(VK_SCROLL, VK_F23, LOC("Remap ScrollLock as F23"));

		}
		else {
			remap_open = false;
		}

		if (ImGui::CollapsingHeader(LOC("Tray Menu"))) {
			if (ImGui::Checkbox(LOC("Show languages"), &conf_gui()->ShowLangsInTrayMenu)) {
				SaveApplyGuiConfig();
			}
			if (ImGui::Checkbox(LOC("Show Reminder entries"), &conf_gui()->ShowReminderInTrayMenu)) {
				SaveApplyGuiConfig();
			}
		}


	}
}
