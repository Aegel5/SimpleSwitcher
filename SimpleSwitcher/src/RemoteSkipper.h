
// многопоточный класс для реализации настройки Allow Remote Keys

class RemoteSkipper {
    struct SkipInjectEntry {
        ULONGLONG actualUNTIL;
        int skipCnt;
    };
    std::deque<SkipInjectEntry> skipdata;
public:
    void AddOur() {
		//if (conf_get()->AllowRemoteKeys) {
		//	g_hooker->skipdata.emplace_back(GetTickCount64() + 500, list.size());
		//	if (g_hooker->skipdata.size() >= 10000) {
		//		g_hooker->skipdata.clear();
		//	}
		//}
    }
    void IsAllowInject() {

		//if (conf_get()->AllowRemoteKeys) {
		//	while (1) {
		//		if (skipdata.empty()) {
		//			LOG_INFO_3(L"allow injected because of setting");
		//			break; // нечего пропускать.
		//		}
		//		if (GetTickCount64() > skipdata.front().actualUNTIL || skipdata.front().skipCnt <= 0) {
		//			skipdata.pop_front(); // истек срок годности данный ноды...
		//			continue;
		//		}
		//		// все хорошо, нода актуальна, пропускаем inject
		//		skipdata.front().skipCnt--;
		//		LOG_INFO_3(L"skip enjected by evristics");
		//		RETURN_SUCCESS;
		//	}
		//}

    }
};