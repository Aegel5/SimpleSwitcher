
#pragma once
// многопоточный класс для реализации настройки Allow Remote Keys

class InjectSkipper {

	std::atomic_bool is_our_send = false;
public:
	struct LocSkipper {
		LocSkipper() { 
			InjectSkipper::Inst().is_our_send = true; 
		}
		~LocSkipper() { 
			LOG_ANY(L"end our inject");
			InjectSkipper::Inst().is_our_send = false; 
		}
	};
	bool IsSkipInject() {
		if (conf_get_unsafe()->SkipAllInjectKeys) {
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

	static InjectSkipper& Inst() {
		static InjectSkipper inst;
		return inst;
	}

};
