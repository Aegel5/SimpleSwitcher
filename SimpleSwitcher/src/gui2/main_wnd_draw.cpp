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
				UStr items[] = { LOC("Disabled"), LOC("Only for several words correction"), LOC("Always") };
				Clamp(conf_gui()->separate_ext_mode, 0, std::ssize(items)-1);
				if (ImGui::Combo(LOC("Extended word seperation"), &conf_gui()->separate_ext_mode, items, std::size(items))) {
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
			ImGui::SetItemTooltip(LOC("Disable sticking after 5 shift and others"));

			if (ImGui::Checkbox(LOC("Clear text format on Ctrl-C"), &conf_gui()->fClipboardClearFormat)) {
				SaveApplyGuiConfig();
			}


			{
				bool val = GetLogLevel() > LOG_LEVEL_DISABLE;
				if (ImGui::Checkbox(LOC("Enable debug log"), &val)) {
					SetLogLevel_info(val ? conf_gui()->logLevel : LOG_LEVEL_DISABLE);
				}
			}

			{
				ImGuiUtils::Combo(LOC("Theme"), conf_gui()->theme,
					std::array {"Dark", "Light", "Classic", "Ocean", "Dark Relax"},
					[]() {
						SaveApplyGuiConfig();
						SetStyle();
					}
				);

				//ImGui::SameLine();
				//if (ImGui::Button(LOC("Styles..."))) {
				//	ShowStyleEditor ^= 1;
				//}
			}

			{
				ImGuiUtils::Combo("GUI language", conf_gui()->gui_lang,
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

				if (ImGui::Button("Reload")) {
					Reinit(!cfg_details::ReloadGuiConfig());
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

			//{
			//	if (ImGui::Checkbox(LOC("Optimize GUI"), &optmz)) {
			//		SaveConfigWith([this](auto p) {p->optimize_gui = optmz; });
			//	}
			//	ImGui::SetItemTooltip(LOC("Unload gui completely when closed to tray (actual for old PC)"));
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
				ImGuiUtils::Combo(LOC("Background"), conf_gui()->background,
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
				if (ImGui::Button(LOC("Update"))) {
					update_backg();
				}
			}

			{
				if (ImGui::Button(LOC("Remainder"))) {
					show_main_wind(1);
				}
			}

		}

		with_TabItem(LOC("About")) {
			ImGui::TextLinkOpenURL("Github", "https://github.com/Aegel5/SimpleSwitcher");
			ImGui::TextLinkOpenURL("Telegram", "https://t.me/simple_switcher");
		}


		ImGui::EndTabBar();
	}

	{
		auto text = LOC("Close to tray");
		float w = ImGui::CalcTextSize(text).x + ImGui::GetStyle().FramePadding.x*2.f + ImGui::GetStyle().ItemSpacing.x;
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetContentRegionAvail().x - w);
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + ImGui::GetContentRegionAvail().y - ImGui::GetFrameHeightWithSpacing()); // todo use child_wnd
		if (ImGui::Button(text)) { show_wnd = false; }
	}

	if (background->IsOk()) {


		//auto w = background->img.width;
		//auto h = background->img.height;
		auto size = ImGui::GetWindowSize();
		auto h = size.y;
		auto w = background->img.width * size.y / background->img.height;
		auto x = size.x - w;
		//ImGui::SetCursorPos({ x,0 });
		//ImGui::Image((ImTextureID)(intptr_t)background->pTexture, ImVec2(background->img.width, background->img.height));
		ImVec2 window_pos = ImGui::GetWindowPos();
		ImVec2 min = window_pos;
		min.x += x;
		ImVec2 max = min;
		max.x += w;
		max.y += h;
		ImGui::GetForegroundDrawList()->AddImage((ImTextureID)(intptr_t)background->pTexture, min, max);
	}

	ImGui::End();
}
