#include "main_wnd.h"

void MainWindow::Draw_run_tab() {

	with_TabItem(LOC("Commands")) {

		if (run_programs_hks.empty()) {
			std::vector<CHotKey> def_list = {};
			for (int i = -1; auto& it : conf_gui()->run_programs) {
				run_programs_hks.emplace_back("##HK", def_list, &it.hotkey);
			}
		}

		float _label_width = ImGui::CalcTextSize("snippet").x + ImGui::GetStyle().ItemSpacing.x;
		float _label_width2 = ImGui::CalcTextSize("path").x + ImGui::GetStyle().ItemSpacing.x;

		int to_del = -1;
		bool changes = false;
		for (int i = -1; auto& it : conf_gui()->run_programs) {
			i++;
			with_ID(i + 12344) {
				auto ctx_menu_last_item = [&](bool wnd = false) {
					if (wnd ? ImGui::BeginPopupContextWindow("ctx_menu"): ImGui::BeginPopupContextItem("ctx_menu")) {
						if (ImGui::Selectable(LOC("Delete"))) {
							to_del = i;
						}
						ImGui::EndPopup();
					}
					};
				bool open = ImGui::TreeNodeEx("##header", ImGuiTreeNodeFlags_CollapsingHeader, "%s", it.cmd.c_str());
				ctx_menu_last_item();
				if(open){
					ImGui::BeginChild("", {}, ImGuiChildFlags_Borders | ImGuiChildFlags_AutoResizeY);
					ctx_menu_last_item(true);
					float label_width = (it.type == CommandType::Run ? _label_width2 : _label_width) + ImGui::GetCursorPosX();

					// enabled
					{
						//ImGui::AlignTextToFramePadding();
						//ImGui::Text("enabled");
						//ImGui::SameLine(label_width);
						//ImGui::SetNextItemWidth(-1.0f);
						if (ImGui::Checkbox(LOC("Enabled"), &it.enabled)) {
							changes = true;
						}
					}



					// type
					{
						ImGui::SameLine();
						auto text = [](CommandType x) { 
							if (x == CommandType::Run) return LOC("Run process"); 
							if (x == CommandType::Snippet) return LOC("Snippet"); 
							return "Unknown";
							};
						if (ImGui::BeginCombo(LOC("Command type"), text(it.type), 0)) {
							for (auto type : { CommandType::Run, CommandType::Snippet }) {
								if (ImGui::Selectable(text(type), it.type == type)) {
									if (it.type != type) {
										it.type = type;
										changes = true;
									}
								}
							}
							ImGui::EndCombo();
						}
					}

					if (it.type == CommandType::Run) {

						ImGui::AlignTextToFramePadding();
						ImGui::TextUnformatted("path");
						ImGui::SameLine(label_width);
						ImGui::SetNextItemWidth(-1.0f);
						if (ImGui::InputText("##path", &it.cmd)) {
							changes = true;
						}

						ImGui::AlignTextToFramePadding();
						ImGui::TextUnformatted("args   ");
						ImGui::SameLine(label_width);
						ImGui::SetNextItemWidth(-1.0f);
						if (ImGui::InputText("##args", &it.args)) {
							changes = true;
						}
					}

					if (it.type == CommandType::Snippet) {
						ImGui::AlignTextToFramePadding();
						ImGui::TextUnformatted("snippet");
						ImGuiUtils::SetItemTooltip(LOC("By default, it just enters text; use @@() for hotkeys. Example: @@(Ctrl + V, Enter)MyText"));
						ImGui::SameLine(label_width);
						ImGui::SetNextItemWidth(-1.0f);
						if (ImGui::InputText("##snippet", &it.cmd)) {
							changes = true;
						}
					}

					// hkey
					{
						ImGui::AlignTextToFramePadding();
						ImGui::TextUnformatted("hkey ");
						ImGui::SameLine(label_width);
						run_programs_hks[i].Draw();
					}

					if (it.type == CommandType::Run) {

						// run now
						{
							ImGui::SameLine();
							if (ImGui::Button(LOC("Run now"))) {
								auto hk = (HotKeyType)(hk_RunProgram_flag | i);
								Worker()->PostMsg([hk](auto p) {p->RunProcess(hk); });
							}
						}

						// delay
						{
							ImGui::SameLine();
							ImGui::AlignTextToFramePadding();
							ImGui::Text(LOC("delay"));
							ImGui::SameLine();
							ImGui::SetNextItemWidth(-1.0f);
							if (ImGui::InputInt("##delay", &it.delay)) {
								it.delay = std::clamp(it.delay, 0, 10000);
								changes = true;
							}
						}
					}

					ImGui::EndChild();

				}
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
