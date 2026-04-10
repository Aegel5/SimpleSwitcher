#include "main_wnd.h"

void MainWindow::DrawFrameActual() {

	ImGuiUtils::ToCenter();
	ImGui::SetNextWindowSize(startsize, ImGuiCond_FirstUseEver);
	ImGui::Begin(title.c_str(), &show_wnd, ImGuiWindowFlags_NoCollapse);


	process_helper();

	//ImGuiIO& io = ImGui::GetIO();
	//// Display FPS in your UI
	//ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
	//	1000.0f / io.Framerate, io.Framerate);

	if (ImGui::BeginTabBar("MyTabBar", ImGuiTabBarFlags_None)) {

		//ImGuiTabItemFlags flags = 0;
		//if (loc_details::do_reinit) {
		//	loc_details::do_reinit = false;
		//	flags |= ImGuiTabItemFlags_SetSelected;
		//}
		with_TabItem(LOC("Settings"), 0, 0) {

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
				auto text = [](SeparateExtMode x) {
					if (x == SeparateExtMode::Disabled) return LOC("Base (only by spaces)");
					if (x == SeparateExtMode::PossibleSymb_Always) return LOC("Separate non-letters and potential non-letters (always)");
					if (x == SeparateExtMode::PossibleSymb_SeveralW) return LOC("Separate non-letters and potential non-letters (several words correction)");
					if (x == SeparateExtMode::Symbol) return LOC("Separate non-letters");
					return "Unknown";
					};
				auto tips = [](SeparateExtMode x) -> UStr {
					//if (x == SeparateExtMode::Disabled) return LOC("Only separate by spaces");
					return nullptr;
					};
				if (ImGui::BeginCombo(LOC("Word separation"), text(conf_gui()->separate_ext_mode))) {
					for (auto type : { SeparateExtMode::Disabled, SeparateExtMode::Symbol,  SeparateExtMode::PossibleSymb_SeveralW, SeparateExtMode::PossibleSymb_Always }) {
						if (ImGui::Selectable(text(type), conf_gui()->separate_ext_mode == type)) {
							if (conf_gui()->separate_ext_mode != type) {
								conf_gui()->separate_ext_mode = type;
								SaveApplyGuiConfig();
							}
						}
						auto tip = tips(type);
						if(tip)
							ImGui::SetItemTooltip(tip);
					}
					ImGui::EndCombo();
				}
				if (ImGui::InputText(LOC("Treat as letters"), &conf_gui()->treat_as_letters)) {
					SaveApplyGuiConfig();
				}
				ImGui::SetItemTooltip(LOC("Which symbols should be treated as letters for word separation"));
				
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
					std::array {"English", "Russian"},
					[this]() {
						SaveApplyGuiConfig();
						ApplyLocalization();
						ReinitHk();
						}
				);
			}

			if (g_isAura) {
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

			ImGui::BeginChild("Bottom Panel", ImVec2(0, 0));

			if (ImGui::BeginPopupContextWindow("lay_upd")) {
				if (ImGui::MenuItem(LOC("Update"))) {
					SyncLays();
				}
				ImGui::EndPopup();
			}

			ImGui::SeparatorText(LOC("Language layouts"));
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

			ImGui::EndChild();

		}


		Draw_expert_tab();
		Draw_run_tab();
		Draw_about_tab();



		ImGui::EndTabBar();

	}

	// глючит
	//{
	//	auto text = LOC("Close to tray");
	//	ImGuiUtils::SetCursorToRightForButton(text);
	//	ImGui::SetCursorPosY(ImGui::GetCursorPosY() + ImGui::GetContentRegionAvail().y - ImGui::GetFrameHeightWithSpacing()); // todo use child_wnd
	//	if (ImGui::Button(text)) { show_wnd = false; }
	//}

	DrawSkin();

	ImGui::End();
}
