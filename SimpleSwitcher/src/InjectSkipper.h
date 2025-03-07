
// многопоточный класс для реализации настройки Allow Remote Keys

class InjectSkipper {
    struct SkipInjectEntry {
        ULONGLONG actualUNTIL;
        int skipCnt;
    };
    std::deque<SkipInjectEntry> skipdata;
	std::mutex m_mtx;
public:
    void AddOur(int cnt) {
		if (conf_get()->AllowRemoteKeys) {
			std::unique_lock<std::mutex> _lock(m_mtx);
			skipdata.emplace_back(GetTickCount64() + 500, cnt);
			if (skipdata.size() >= 10000) {
				skipdata.clear();
			}
		}
    }
	static InjectSkipper& Inst() {
		static InjectSkipper inst;
		return inst;
	}
    bool IsAllowInject() {

		if (conf_get()->AllowRemoteKeys) {
			std::unique_lock<std::mutex> _lock(m_mtx);
			while (1) {
				if (skipdata.empty()) {
					break; // нечего пропускать.
				}
				if (GetTickCount64() > skipdata.front().actualUNTIL || skipdata.front().skipCnt <= 0) {
					skipdata.pop_front(); // истек срок годности данный ноды...
					continue;
				}
				// все хорошо, нода актуальна, пропускаем inject
				skipdata.front().skipCnt--;
				LOG_INFO_3(L"skip enjected by evristics");
				return false;
			}
			LOG_INFO_3(L"allow injected because of setting");
			return true;
		}
		LOG_INFO_3(L"skip inject");
		return false;
    }
};