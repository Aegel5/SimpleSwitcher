#include "main_wnd.h"

void MainWindow::Draw_run_tab() {

	with_TabItem(LOC("Run")) {

		if (run_programs_hks.empty()) {
			std::vector<CHotKey> def_list = {};
			for (int i = -1; auto& it : conf_gui()->run_programs) {
				run_programs_hks.emplace_back("##HK", def_list, &it.hotkey);
			}
		}

		int to_del = -1;
		bool changes = false;
		for (int i = -1; auto& it : conf_gui()->run_programs) {
			i++;
			with_ID(i + 12344) {
				ImGui::BeginChild("", {}, ImGuiChildFlags_AlwaysAutoResize | ImGuiChildFlags_Borders | ImGuiChildFlags_AutoResizeY);
				if (ImGui::BeginPopupContextWindow("ctx_menu")) {
					if (ImGui::Selectable(LOC("Delete"))) {
						to_del = i;
					}
					ImGui::EndPopup();
				}
				ImGui::AlignTextToFramePadding();
				ImGui::Text("path ");
				ImGui::SameLine();
				ImGui::SetNextItemWidth(-1.0f);
				if (ImGui::InputText(LOC("##path"), &it.path)) {
					changes = true;
				}

				ImGui::AlignTextToFramePadding();
				ImGui::Text("args ");
				ImGui::SameLine();
				ImGui::SetNextItemWidth(-1.0f);
				if (ImGui::InputText(LOC("##args"), &it.args)) {
					changes = true;
				}

				ImGui::AlignTextToFramePadding();
				ImGui::Text("delay");
				ImGui::SameLine();
				ImGui::SetNextItemWidth(-1.0f);
				if (ImGui::InputInt("##delay", &it.delay)) {
					it.delay = std::clamp(it.delay, 0, 10000);
					changes = true;
				}

				ImGui::AlignTextToFramePadding();
				ImGui::Text("hkey ");
				ImGui::SameLine();
				run_programs_hks[i].Draw();
				ImGui::SameLine();
				if (ImGui::Button(LOC("Run now"))) {
					auto hk = (HotKeyType)(hk_RunProgram_flag | i);
					Worker()->PostMsg([hk](auto p) {p->RunProcess(hk); });
				}

				ImGui::EndChild();
			}

		}
		if (to_del != -1) {
			Utils::RemoveAt(conf_gui()->run_programs, to_del);
			run_programs_hks.clear();
			changes = true;
		}
		if (ImGuiUtils::PlusButton()) {
			conf_gui()->run_programs.emplace_back();
			run_programs_hks.clear();
			changes = true;
		}
		if (changes) {
			SaveApplyGuiConfig();
		}
	}
}
