#include "main_wnd.h"

void MainWindow::DrawFrameActual() {
	GETCONF;

	ImGui::SetNextWindowSize(startsize, ImGuiCond_FirstUseEver);
	ImGui::Begin(title.c_str(), &show_main, ImGuiWindowFlags_NoCollapse);

	if (g_bring_gui_to_top && ImGui::GetWindowViewport()->PlatformWindowCreated) {
		g_bring_gui_to_top = false;
		auto hwnd = (HWND)ImGui::GetWindowViewport()->PlatformHandle;
		SetForegroundWindow(hwnd);
	}

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

			{
				if (ImGui::BeginCombo(LOC("Set of flags"), cfg->flagsSet.c_str(), 0)) {
					auto add = [&cfg](UStr s) {
						if (ImGui::Selectable(s, cfg->flagsSet == s)) {
							SaveConfigWith([&](auto p) {p->flagsSet = s; });
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
			}

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
				if (ImGui::BeginCombo("Theme", cfg->theme.c_str(), 0)) {
					for (const char* it : { "Dark","Light","Classic" }) {
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

			for (int i = -1; auto & it : hotbox) {
				it.Draw();
			}
			ImGui::SeparatorText("Language layouts");
			for (int i = -1; const auto & it : cfg->layouts_info.info) {
				i++;
				with_ID(i+20) {
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

			{
				bool val = cfg->optimize_gui;
				if (ImGui::Checkbox(LOC("Optimize GUI"), &val)) {
					SaveConfigWith([val](auto p) {p->optimize_gui = val; });
				}
			}

			{
				bool val = cfg->SkipLowLevelInjectKeys;
				if (ImGui::Checkbox("SkipLowLevelInjectKeys", &val)) {
					SaveConfigWith([val](auto p) {p->SkipLowLevelInjectKeys = val; });
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
		if (ImGui::Button(text)) { show_main = false; }
	}

	ImGui::End();
}
