#pragma once

#include "Entry.h"

namespace Notific {

	struct Settings {
		std::vector<Entry> list;
	};

	class Notificator : public ImGuiUtils::WindowHelper {
		Settings settings;
		void Save();
		void Load();
		bool need_show = false;
		HWND last_hwnd = 0;
		std::vector<Entry>& entries;
	public:
		Notificator() : entries(settings.list) {
			Load();
		}

		bool IsVisible() { 
			return need_show || show_wnd;
		}

		bool Process() {
			need_show = false;
			for (auto& it : entries) {
				if (it.IsTrigger()) {
					it.AdjastActiveString();
					need_show = true;
					break;
				}
			}
			return need_show;
		}

		void DrawNotify() {
			if (!need_show) return;
			for (auto& it : entries) {
				if (it.IsTrigger()) {
					bool val = true;
					ImGuiUtils::ToCenter(true);
					ImGui::Begin("Notification", &val, ImGuiWindowFlags_NoSavedSettings);
					{
						HWND hwnd = (HWND)ImGui::GetWindowViewport()->PlatformHandle;
						if (hwnd != 0 && last_hwnd != hwnd) {
							last_hwnd = hwnd;
							IFW_LOG(SetWindowPos(last_hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE));
							//IFW_LOG(SetForegroundWindow(last_hwnd));
						}
					}

					ImGui::Text("Event:");
					ImGui::SameLine();
					ImGui::Text(it.name.c_str());
					if (ImGui::Button("Got it!")) {
						it.SetupNextActivate();
						Save();
					}
					ImGui::SameLine();
					if (ImGui::Button("Remain me tomorow...")) {
						auto now = Convert(Now());
						now.AddDay();
						it.SetActivate(Convert(now));
						Save();
					}

					ImGui::End();
					return;
				}
			}
			need_show = false;
		}
		void Draw() {
			DrawSettings();
			DrawNotify();

		}
		void DrawSettings() {

			if (!show_wnd) return;

			ImGuiUtils::ToCenter();
			ImGui::SetNextWindowSize({800, 600}, ImGuiCond_FirstUseEver);
			ImGui::Begin("Notifications", &show_wnd);

			process_helper();

			int id = 1;
			for (int i = 0; i < entries.size(); i++) {
				auto cur = entries[i].Draw(id);
				if (cur == 10) {
					Utils::RemoveAt(entries, i);
					i--;
				}
				if (cur) {
					Save();
				}
			}

			if (ImGui::Button("+", { ImGui::GetFrameHeight(),0 })) {
				entries.emplace_back();
				entries.back().SetPoint(Now());
				Save();
			}

			ImGui::End();
		}
	};
}
