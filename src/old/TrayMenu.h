#pragma once

class TrayMenu {
	POINT cursorPos{ -1,-1 };
public:
	void DrawTrayMenu() {
		if (cursorPos.x != -1) {
			ImGui::SetNextWindowPos({ (float)cursorPos.x, (float)cursorPos.y }, ImGuiCond_Always);
			ImGui::OpenPopup("tray menu", 0);
			cursorPos.x = -1;
		}
		if (ImGui::BeginPopup("tray menu", ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings)) {


			if (ImGui::IsWindowAppearing()) {
				menu_lays.clear();
				for (const auto& it : conf_get_unsafe()->layouts_info.info) {
					menu_lays.emplace_back(StrUtils::Convert(Utils::GetNameForHKL(it.layout)), it.layout);
				}
			}

			HWND hwnd = (HWND)ImGui::GetWindowViewport()->PlatformHandle;
			if (ImGui::GetWindowViewport()->PlatformWindowCreated) {
				static HWND last_hwnd = 0;
				if (last_hwnd != hwnd) {
					last_hwnd = hwnd;
					SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
					SetForegroundWindow(hwnd);
					ImGui::GetPlatformIO().Platform_SetWindowFocus(ImGui::GetWindowViewport());
				}
				if (!ImGui::GetPlatformIO().Platform_GetWindowFocus(ImGui::GetWindowViewport())) {
					ImGui::CloseCurrentPopup();
				}
			}

			for (const auto& it : menu_lays) {
				if (ImGui::Selectable(it.first.c_str(), false)) {
					auto lay = it.second;
					Worker()->PostMsg([lay](auto w) {w->SetNewLay(lay); });
				}
			}

			ImGui::Separator();

			{
				bool val = g_enabled.IsEnabled();
				if (ImGui::Checkbox(LOC("Enable"), &val)) {
					g_enabled.TryEnable(val);
					ImGui::CloseCurrentPopup();
				}
			}

			if (ImGui::Selectable(LOC("Show"), false)) {
				Show();
			}

			if (ImGui::Selectable(LOC("Exit"), false)) {
				PostQuitMessage(0);
			}
			ImGui::EndPopup();
		}
	}
};
