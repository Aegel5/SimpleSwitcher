#include "main_wnd.h"

void MainWindow::DrawFrameActual() {

	ImGuiUtils::ToCenter();
	ImGui::SetNextWindowSize(startsize, ImGuiCond_FirstUseEver);
	ImGui::Begin(title.c_str(), &show_wnd, ImGuiWindowFlags_NoCollapse);

	process_helper();

	if (ImGui::BeginTabBar("MyTabBar", ImGuiTabBarFlags_None)) {

		with_TabItem(LOC("Settings")) {

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

			if (ImGui::Checkbox(LOC("Alternative mode layout change (emulate LAlt + Shift)"), &conf_gui()->AlternativeLayoutChange)) {
				SaveApplyGuiConfig();
			}

			{
				if (ImGui::BeginCombo(LOC("Set of flags"), conf_gui()->flagsSet.c_str(), 0)) {
					auto add = [](UStr s) {
						if (ImGui::Selectable(s, conf_gui()->flagsSet == s)) {
							conf_gui()->flagsSet = s;
							SaveApplyGuiConfig();
							new_layout_request();
						}
						};
					add(ProgramConfig::showFlags_Nothing);
					add(ProgramConfig::showFlags_AppIcon);
					for (const auto& it : flagsSets) {
						add(it.c_str());
					}
					ImGui::EndCombo();
				}
				ImGui::SameLine();
				if (ImGui::Button(LOC("Update"))) {
					update_flags();
				}
			}

			if (ImGui::Checkbox(LOC("Disable the accessibility shortcut keys (5 SHIFT and others)"), &conf_gui()->disableAccessebility)) {
				SaveApplyGuiConfig();
				ApplyAcessebil();
			}

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
				if (ImGui::BeginCombo(LOC("Theme"), conf_gui()->theme.c_str(), 0)) {
					for (const char* it : { "Dark","Light","Classic", "Ocean","Dark Relax" }) {
						if (ImGui::Selectable(it, conf_gui()->theme == it)) {
							conf_gui()->theme = it;
							SaveApplyGuiConfig();
							SetStyle();
						}
					}
					ImGui::EndCombo();
				}
				//ImGui::SameLine();
				//if (ImGui::Button(LOC("Styles..."))) {
				//	ShowStyleEditor ^= 1;
				//}
			}

			ImGui::AlignTextToFramePadding();
			ImGui::TextLinkOpenURL("SimpleSwitcher.json", config_path.c_str());
			ImGui::SameLine();

			if (ImGui::Button("Reload")) {
				Reinit(!cfg_details::ReloadGuiConfig());
			}

		}

		with_TabItem(LOC("Hotkeys")) {

			for (int i = -1; auto & it : hotbox) {
				it.Draw();
			}
			ImGui::SeparatorText("Language layouts");
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
				if (ImGui::Checkbox("SkipLowLevelInjectKeys", &conf_gui()->SkipLowLevelInjectKeys)) {
					SaveApplyGuiConfig();
				}
				ImGui::SetItemTooltip(LOC("Disable interception of keys sent to remote computer for RDP connection"));
			}

			{
				if (ImGui::BeginCombo(LOC("Background"), conf_gui()->background.c_str(), 0)) {
					auto add = [this](UStr s) {
						if (ImGui::Selectable(s, conf_gui()->background == s)) {
							conf_gui()->background = s;
							SaveApplyGuiConfig();
							apply_background();
						}
						};
					add("None");
					for (const auto& it : backgrounds) {
						add(it.c_str());
					}
					ImGui::EndCombo();
				}
				ImGui::SameLine();
				if (ImGui::Button(LOC("Update"))) {
					update_backg();
				}
			}

			{
				if (ImGui::Button("Notifications")) {
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
