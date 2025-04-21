					//i++;
					//with_ID(next_id++) {
					//	bool val = cfg->hotkeysList[i].keys.key().IsEnabled();
					//	if (ImGui::Checkbox("", &val)) {
					//		SaveConfigWith([&](auto p) {p->hotkeysList[i].keys.key().SetEnabled(val); });
					//	}
					//}
					//ImGui::SameLine();



        std::regex regex("\\[(\\ |\\n|\\r|\\t)*");
        json = std::regex_replace(json, regex, "[ ");
        std::regex regex2("(\\ |\\n|\\r|\\t)*\\]");
        json = std::regex_replace(json, regex2, " ]");


				//if (ImGui::BeginTable("table1", 3, 0)) {
				//	// Submit columns name with TableSetupColumn() and call TableHeadersRow() to create a row with a header in each column.
				//	// (Later we will show how TableSetupColumn() has other uses, optional flags, sizing weight etc.)
				//	ImGui::TableSetupColumn("##layout");
				//	ImGui::TableSetupColumn("Two");
				//	ImGui::TableSetupColumn("Three");

				//	ImGui::EndTable();
				//}



		std::stringstream o;
		// std::ofstream o(res);
		o << std::setw(4) << data << std::endl;

		auto json = o.str();



		ImGuiWindowClass windowClass;
		windowClass.TabItemFlagsOverrideSet = ImGuiViewportFlags_TopMost;
		ImGui::SetNextWindowClass(&windowClass);
		ImGui::SetNextWindowPos({ (float)cursorPos.x, (float)cursorPos.y });



		class PushIdLoc {
			UStr name;
		public:
			PushIdLoc(UStr name) :name(name) {
				ImGui::PushID(name);
			}
			UStr Loc() { return LOC(name); }
			~PushIdLoc() {
				ImGui::PopID();
			}
		};
