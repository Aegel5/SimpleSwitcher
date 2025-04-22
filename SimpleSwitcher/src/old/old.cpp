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

		//   struct SkipInjectEntry {
//       ULONGLONG actualUNTIL;
//       int skipCnt;
//   };
//   std::deque<SkipInjectEntry> skipdata;
   //std::mutex m_mtx;

		  //  void AddOur(int cnt) {
		//if (conf_get_unsafe()->AlwaysSkipInject) {
		//	std::unique_lock<std::mutex> _lock(m_mtx);
		//	skipdata.emplace_back(GetTickCount64() + 500, cnt);
		//	if (skipdata.size() >= 10000) {
		//		skipdata.clear();
		//	}
		//}
  //  }

		  //  bool IsAllowInject() {

		//if (conf_get_unsafe()->AlwaysSkipInject) {
		//	std::unique_lock<std::mutex> _lock(m_mtx);
		//	while (1) {
		//		if (skipdata.empty()) {
		//			break; // нечего пропускать.
		//		}
		//		if (GetTickCount64() > skipdata.front().actualUNTIL || skipdata.front().skipCnt <= 0) {
		//			skipdata.pop_front(); // истек срок годности данный ноды...
		//			continue;
		//		}
		//		// все хорошо, нода актуальна, пропускаем inject
		//		skipdata.front().skipCnt--;
		//		LOG_INFO_3(L"skip enjected by evristics");
		//		return false;
		//	}
		//	LOG_INFO_3(L"allow injected because of setting");
		//	return true;
		//}
		//LOG_INFO_3(L"skip inject");
		//return false;
  //  }
