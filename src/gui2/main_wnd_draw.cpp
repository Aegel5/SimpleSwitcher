#include "main_wnd.h"

void MainWindow::DrawFrameActual() {

	ImGuiUtils::ToCenter();
	ImGui::SetNextWindowSize(startsize, ImGuiCond_FirstUseEver);
	ImGui::Begin(title.c_str(), &show_wnd, ImGuiWindowFlags_NoCollapse);

	process_helper();

	if (ImGui::BeginTabBar("MyTabBar", ImGuiTabBarFlags_None)) {

		ImGuiTabItemFlags flags = 0;
		if (loc_details::do_reinit) {
			loc_details::do_reinit = false;
			flags |= ImGuiTabItemFlags_SetSelected;
		}
		with_TabItem(LOC("Settings"), 0, flags) {

			{
				bool val = g_enabled.IsEnabled();
				if (ImGui::Checkbox(LOC("Enable"), &val)) {
					if (val && !IsAdminOk()) {
						val = false;
						ShowMessageAdmin();
					}
					if (g_enabled.TryEnable(val)) {
						new_layout_request();
					}
				}
			}

			if (ImGui::Checkbox(LOC("Add to autostart"), &check_add_to_auto)) {
				if (!autostart_set(check_add_to_auto)) {
					ShowMessageAdmin();
				}
				check_add_to_auto = autostart_get();
			}

			DrawMessage();

			if (ImGui::Checkbox(LOC("Work in programs running by admin"), &conf_gui()->isMonitorAdmin)) {
				SaveApplyGuiConfig();
				if (!IsAdminOk()) g_enabled.TryEnable(false);
				check_add_to_auto = autostart_get();
			}

			if (ImGui::Checkbox(LOC("Alternative mode layout change"), &conf_gui()->AlternativeLayoutChange)) {
				SaveApplyGuiConfig();
			}
			ImGui::SetItemTooltip("%s %s\n%s", LOC("Emulate Windows hotkey"), "'Alt + Shift' [config / win_hotkey_cycle_lang]", LOC("Use this mode if the main one does not work correctly"));

			{
				ImGuiUtils::Combo(LOC("Set of flags"), conf_gui()->flagsSet,
					[this]()-> std::generator<UStr> {
						co_yield ProgramConfig::showFlags_Nothing;
						co_yield ProgramConfig::showFlags_AppIcon;
						for (const auto& it : flagsSets) co_yield it.c_str();
					},
					[] {
						SaveApplyGuiConfig();
						new_layout_request();
					}
				);
				ImGui::SameLine();
				if (ImGui::Button(LOC("Update"))) {
					update_flags();
				}
			}

			{
				if (ImGui::Checkbox(LOC("Use the British flag for the English language"), &conf_gui()->useBritishFlag)) {
					SaveApplyGuiConfig();
					update_flags();
					new_layout_request();
				}
			}

			{
				UStr items[] = { LOC("Disabled"), LOC("Only for several words correction"), LOC("Always") };
				Clamp(conf_gui()->separate_ext_mode, 0, std::ssize(items)-1);
				if (ImGui::Combo(LOC("Extended word separation"), &conf_gui()->separate_ext_mode, items, std::size(items))) {
					SaveApplyGuiConfig();
				}
				ImGui::SetItemTooltip((UStr)u8"%s\n%s: ']}' - 'ъЪ'", 
					LOC("Separate words by keys that can produce both letters and other symbols"),
					LOC("Example"));
			}

			if (ImGui::Checkbox(LOC("Disable the accessibility functions"), &conf_gui()->disableAccessebility)) {
				SaveApplyGuiConfig();
				ApplyAcessebil();
			}
			ImGui::SetItemTooltip(LOC("Disable sticking after 5 Shift and others"));

			if (ImGui::Checkbox(LOC("Clear text format on Ctrl-C"), &conf_gui()->fClipboardClearFormat)) {
				SaveApplyGuiConfig();
			}


			{
				bool val = GetLogLevel() > LOG_LEVEL_DISABLE;
				if (ImGui::Checkbox(LOC("Enable debug log"), &val)) {
					SetLogLevel_print_info(val ? conf_gui()->logLevel : LOG_LEVEL_DISABLE);
				}
			}

			{
				ImGuiUtils::Combo(LOC("UI Color"), conf_gui()->theme,
					std::array {
					"Dark", "Light", "Classic", "Ocean", "Dark Relax"
				},
					[]() {
						SaveApplyGuiConfig();
						SetStyle();
					}
				);

			}

			{
				ImGuiUtils::Combo("UI Language", conf_gui()->gui_lang,
					std::array {"English", "Russian", "Hebrew"},
					[this]() {
						SaveApplyGuiConfig();
						ApplyLocalization();
						ReinitHk();
						loc_details::do_reinit = true;
					}
				);
			}

			{
				ImGui::AlignTextToFramePadding();
				ImGui::TextLinkOpenURL("SimpleSwitcher.json", config_path.c_str());
				ImGui::SameLine();

				if (ImGui::Button(LOC("Reload"))) {
					Reinit(!cfg_details::ReloadGuiConfig());
				}
			}

			{
				if (ImGui::Button(LOC("Reminder"))) {
					show_main_wind(1);
				}
			}

		}

		with_TabItem(LOC("Hotkeys")) {

			for (int i = -1; auto & it : hotbox) {
				it.Draw();
			}
			ImGui::SeparatorText(LOC("Language layouts"));
			if (ImGui::BeginPopupContextItem("lay_upd")) {
				if (ImGui::MenuItem(LOC("Update"))) {
					SyncLays();
				}
				ImGui::EndPopup();
			}
			for (int i = -1; auto & it : conf_gui()->layouts_info.info) {
				i++;
				with_ID(i+20) {
					if (ImGui::Checkbox("", &it.enabled)) {
						SaveApplyGuiConfig();
					}
				}
				ImGui::SameLine();
				layout_hotkeys[i].Draw();
			}

		}

		with_TabItem(LOC("Expert")) {

			if (Utils::IsDebug()) {
				if (ImGui::Button("Show demo"))
					show_demo_window = true;
			}



			{
				ImGuiUtils::Combo(LOC("UI Skin"), conf_gui()->ui_skin,
					[this]()-> std::generator<UStr> {
						co_yield "None";
						for (const auto& it : backgrounds) co_yield it.c_str();
					},
					[this] {
						SaveApplyGuiConfig();
						apply_background();
					},
					ImGuiComboFlags_HeightLargest
				);

				ImGui::SameLine();
				with_ID("UPD_SKIN") {
					if (ImGui::Button(LOC("Update"))) {
						update_backg();
					}
				}
			}

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

		with_TabItem(LOC("About")) {
			ImGui::TextLinkOpenURL("Github", "https://github.com/Aegel5/SimpleSwitcher");
			//ImGui::TextLinkOpenURL("Telegram", "https://t.me/simple_switcher");
		}

		ImGui::EndTabBar();


	}

	{
		auto text = LOC("Close to tray");
		ImGuiUtils::SetCursorToRightForButton(text);
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + ImGui::GetContentRegionAvail().y - ImGui::GetFrameHeightWithSpacing()); // todo use child_wnd
		if (ImGui::Button(text)) { show_wnd = false; }
	}

	DrawSkin();

	ImGui::End();
}
