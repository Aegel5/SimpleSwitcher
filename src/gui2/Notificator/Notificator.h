#pragma once

#include "Entry.h"

namespace Notific {

	struct Folder {
		string name = LOC("Folder");
		std::vector<Entry> list;
	};

	struct Settings {
		std::vector<Folder> folders;
		int cur_folder_index = 0;
	};

	class Notificator : public ImGuiUtils::WindowHelper {
		Settings settings;
		void SaveRequest() {
			need_save = true;
		}
		void SaveCheck() {
			if (need_save) {
				need_save = false;
				Save();
			}
		}
		bool need_save = false;
		void Load();
		void Save();
		bool need_show = false;
		HWND last_hwnd = 0;
		std::generator<Entry&> all_entries() {
			for (auto& folder : settings.folders) {
				for (auto& it : folder.list) {
					co_yield it;
				}
			}
		}
	public:
		Notificator() {
			Load();
		}
		~Notificator() {
			SaveCheck();
		}

		bool IsVisible() { 
			return need_show || show_wnd;
		}

		bool Process() {
			need_show = false;
			for (auto& it : all_entries()) {
				if (it.IsTrigger()) {
					it.AdjastActiveString();
					need_show = true;
					break;
				}
			}
			SaveCheck();
			return need_show;
		}

		void DrawNotify() {
			if (!need_show) return;


			for (auto& it : all_entries()) {
				if (it.IsTrigger()) {
					if (it.play_sound) {
						if (it.lastStartPlaySound != it.nextActivate) {
							it.lastStartPlaySound = it.nextActivate;
							SoundManager::Inst().PlayRandom();
						}
					}
					bool val = true;
					ImGuiUtils::ToCenter(true);
					ImGui::Begin(LOC("Event!"), &val, ImGuiWindowFlags_NoSavedSettings| ImGuiWindowFlags_AlwaysAutoResize);
					{
						HWND hwnd = (HWND)ImGui::GetWindowViewport()->PlatformHandle;
						if (hwnd != 0 && last_hwnd != hwnd) {
							last_hwnd = hwnd;
							if (it.wnd_top) {
								IFW_LOG(SetWindowPos(last_hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE));
							}
							//IFW_LOG(SetForegroundWindow(last_hwnd));
						}
					}

					ImGui::TextWrapped(it.name.c_str());
					if (ImGui::Button(LOC("Got it!"))) {
						SoundManager::Inst().StopAllSound();
						it.SetupNextActivate();
						SaveRequest();
					}
					ImGui::SameLine();
					if (ImGui::Button(LOC("Remain me tomorow..."))) {
						it.SetActivate(Now() + days(1));
						SaveRequest();
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
			ImGui::Begin(LOC("Reminder"), &show_wnd);

			process_helper();

			int id = 1;

			// left
			{
				auto w = ImGui::GetFontSize() * 15;
				ImGui::BeginChild("left pane", ImVec2(w, 0), ImGuiChildFlags_Borders | ImGuiChildFlags_ResizeX);

				for (int i = 0; i < settings.folders.size(); i++) {
					auto& folder = settings.folders[i];
					{
						set_ID(id++);
						if (ImGui::Selectable(folder.name.c_str(), i == settings.cur_folder_index)) {
							settings.cur_folder_index = i;
						}
					}
					with_ID(id++) {
						if (ImGui::BeginPopupContextItem("ctx1")) {
							if (ImGui::InputText("##edit_f", &folder.name, ImGuiInputTextFlags_None)) {
								SaveRequest();
							}
							if (ImGui::Selectable(LOC("Delete"))) {
								Utils::RemoveAt(settings.folders, i);
								i--;
								SaveRequest();
							}
							ImGui::EndPopup();
						}
					}
				}


				if (ImGui::Button("+", { ImGui::GetFrameHeight(),0 })) {
					settings.folders.emplace_back();
					SaveRequest();
				}

				ImGui::EndChild();
			}

			ImGui::SameLine();
			id += 1000;

			// right
			{

				ImGui::BeginGroup();

				for (int iFolder = -1; auto & folder : settings.folders) {
					iFolder++;
					if (iFolder == settings.cur_folder_index) {
						for (int i = 0; i < folder.list.size(); i++) {
							auto& entr = folder.list[i];
							auto cur = entr.Draw(id);
							if (cur == 10) {
								Utils::RemoveAt(folder.list, i);
								i--;
							}
							if (cur) {
								SaveRequest();
							}
						}
						if (ImGui::Button("+", { ImGui::GetFrameHeight(),0 })) {
							auto& res = folder.list.emplace_back();
							res.SetPoint(Now());
							SaveRequest();
						}
					}
				}

				ImGui::EndGroup();
			}

			ImGui::End();
		}
	};
}
