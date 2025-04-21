
#pragma once
// многопоточный класс для реализации настройки Allow Remote Keys

class InjectSkipper {
 //   struct SkipInjectEntry {
 //       ULONGLONG actualUNTIL;
 //       int skipCnt;
 //   };
 //   std::deque<SkipInjectEntry> skipdata;
	//std::mutex m_mtx;
	std::atomic_bool is_our_send = false;
public:
	struct LocSkipper {
		LocSkipper() { 
			LOG_ANY(L"start our inject");
			InjectSkipper::Inst().is_our_send = true; 
		}
		~LocSkipper() { 
			LOG_ANY(L"end our inject");
			InjectSkipper::Inst().is_our_send = false; 
		}
	};
	bool IsInjectOur() {
		if (conf_get_unsafe()->AlwaysSkipInject) {
			LOG_ANY(L"skip inject because AlwaysSkipInject");
			return true;
		}
		if (is_our_send) {
			LOG_ANY(L"skip our inject by flag");
			return true;
		}
		LOG_ANY(L"allow inject");
		return false;
	}
  //  void AddOur(int cnt) {
		//if (conf_get_unsafe()->AlwaysSkipInject) {
		//	std::unique_lock<std::mutex> _lock(m_mtx);
		//	skipdata.emplace_back(GetTickCount64() + 500, cnt);
		//	if (skipdata.size() >= 10000) {
		//		skipdata.clear();
		//	}
		//}
  //  }
	static InjectSkipper& Inst() {
		static InjectSkipper inst;
		return inst;
	}
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
};
