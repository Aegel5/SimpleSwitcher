#include "main_wnd.h"

void MainWindow::Draw_run_tab() {

	with_TabItem(LOC("Run")) {

		if (run_programs_hks.empty()) {
			std::vector<CHotKey> def_list = {};
			for (int i = -1; auto& it : conf_gui()->run_programs) {
				run_programs_hks.emplace_back("##HK", def_list, &it.hotkey);
			}
		}

		float _label_width = ImGui::CalcTextSize("snippet").x + ImGui::GetStyle().ItemSpacing.x;

		int to_del = -1;
		bool changes = false;
		for (int i = -1; auto& it : conf_gui()->run_programs) {
			i++;
			with_ID(i + 12344) {
				ImGui::BeginChild("", {}, ImGuiChildFlags_AlwaysAutoResize | ImGuiChildFlags_Borders | ImGuiChildFlags_AutoResizeY);
				float label_width = _label_width + ImGui::GetCursorPosX();
				if (ImGui::BeginPopupContextWindow("ctx_menu")) {
					if (ImGui::Selectable(LOC("Delete"))) {
						to_del = i;
					}
					ImGui::EndPopup();
				}

				// enabled
				{
					//ImGui::AlignTextToFramePadding();
					//ImGui::Text("enabled");
					//ImGui::SameLine(label_width);
					//ImGui::SetNextItemWidth(-1.0f);
					if (ImGui::Checkbox("enabled", &it.enabled)) {
						changes = true;
					}
				}

				// run now
				{
					ImGui::SameLine();
					if (ImGui::Button(LOC("Run now"))) {
						auto hk = (HotKeyType)(hk_RunProgram_flag | i);
						Worker()->PostMsg([hk](auto p) {p->RunProcess(hk); });
					}
				}


				ImGui::AlignTextToFramePadding();
				ImGui::Text("path");
				ImGui::SameLine(label_width);
				ImGui::SetNextItemWidth(-1.0f);
				if (ImGui::InputText("##path", &it.path)) {
					changes = true;
				}

				ImGui::AlignTextToFramePadding();
				ImGui::Text("args   ");
				ImGui::SameLine(label_width);
				ImGui::SetNextItemWidth(-1.0f);
				if (ImGui::InputText("##args", &it.args)) {
					changes = true;
				}

				ImGui::AlignTextToFramePadding();
				ImGui::Text("snippet");
				ImGui::SameLine(label_width);
				ImGui::SetNextItemWidth(-1.0f);
				if (ImGui::InputText("##snippet", &it.snippet)) {
					changes = true;
				}

				// hkey
				{
					ImGui::AlignTextToFramePadding();
					ImGui::Text("hkey ");
					ImGui::SameLine(label_width);
					run_programs_hks[i].Draw();
				}

				// delay
				{
					ImGui::SameLine();
					ImGui::AlignTextToFramePadding();
					ImGui::Text("delay");
					ImGui::SameLine();
					ImGui::SetNextItemWidth(-1.0f);
					if (ImGui::InputInt("##delay", &it.delay)) {
						it.delay = std::clamp(it.delay, 0, 10000);
						changes = true;
					}
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
