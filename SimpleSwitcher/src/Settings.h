#pragma once

#include "ConfigData.h"
#include "ConfigData_hk.h"

class ProgramConfig {
	struct CHotKeySet {
		HotKeyType hkId = hk_NULL;
		CHotKeyList keys;
	};
public:
	static auto GetPath_Conf() { return PathUtils::GetPath_folder_noLower2() / L"SimpleSwitcher.json"; }

    ProgramConfig() {

		auto add = [&](HotKeyType type, bool usedef = false) {
			hotkeysList.emplace_back(type);
			if (usedef)
				hotkeysList.back().keys.key() = *(GetHk_Defaults(type).begin());
			};

		add(hk_RevertLastWord,true);
		add(hk_RevertSeveralWords,true);
		add(hk_RevertAllRecentText,true);
		add(hk_RevertSelelected,true);
		add(hk_CycleSwitchLayout);
		add(hk_EmulateCapsLock);
		add(hk_toUpperSelected);
		add(hk_ToggleEnabled,true);
		add(hk_ShowMainWindow,true);
		add(hk_ShowRemainderWnd);
		add(hk_InsertWithoutFormat);
    }

    std::set <std::wstring> disableInPrograms;

    void NormalizePaths() {
		std::set <std::wstring> res;
        for (const auto& it : disableInPrograms) {
			auto cur = it;
			StrUtils::ToLower(cur);
            PathUtils::NormalizeDelims(cur);
            res.insert(std::move(cur)); // todo cast
        }
		disableInPrograms = std::move(res);
    }
    bool IsSkipProgramTop() const {

        const auto& col = disableInPrograms;

        if (col.empty()) return false;

        auto info = Utils::GetFocusedWndInfo();

        std::wstring path;
        std::wstring name;
        IFS_LOG(Utils::GetProcLowerNameByPid(info.pid_top, path, name));
        if (name.empty()) {
            LOG_ANY(L"can't find name. pid={}", info.pid_top);
            return false;
        }

        if (col.contains(name)) {
            LOG_ANY(L"Skip process by name {} because of disableInProcess", name);
            return true;
        }

        if (col.contains(path)) {
            LOG_ANY(L"Skip process by path {} because of disableInProcess", path);
            return true;
        }

        return false;
    }

    TLogLevel logLevel = LOG_LEVEL_3;

    string config_version;

    bool fixRAlt = false;
    HKL fixRAlt_lay_ = (HKL)0x4090409;
    bool isMonitorAdmin = false;
    bool force_DbgMode              = false;
    bool fClipboardClearFormat = false;
    bool disableAccessebility    = false;
    static constexpr UStr showFlags_OriginalFlags = "Original Flags";
    static constexpr UStr showFlags_AppIcon = "Application Icon";
    static constexpr UStr showFlags_Nothing = "Nothing";
    string flagsSet = "Square";
    bool SkipAllInjectKeys = false;
    bool SkipLowLevelInjectKeys = false;
    bool AlternativeLayoutChange = false;
    TUInt32 quick_press_ms = 280;
    int separate_ext_mode = 0; // 1 - several words, 2 - several + last
    CHotKey win_hotkey_cycle_lang { VK_LMENU, VK_SHIFT };
	std::string theme = "Light";
	string background = "";
	string gui_lang = "Russian";

    std::vector< CHotKeySet> hotkeysList;
    std::vector< RunProgramInfo> run_programs = { {.path = L"example: calc.exe"} };

    LayoutInfoList layouts_info;

    const auto& GetHk(HotKeyType type) const {
        for (auto& it : hotkeysList) {
            if (it.hkId == type) return it;
        }
        LOG_ANY(L"CRITICAL ERR");
        std::terminate();
    }

    std::generator < std::tuple<HotKeyType, const CHotKey&>> All_hot_keys() const {
        for (const auto& it : hotkeysList) {
            for (const auto& key : it.keys.keys) {
                co_yield{ it.hkId, key };
            }

        }
        for (int i = -1; const auto& it : layouts_info.info) {
            i++;
            for (const auto& key : it.hotkey.keys) {
                co_yield{ (HotKeyType)(hk_SetLayout_flag | i), key };
            }
        }
        for (int i = -1; const auto & it : run_programs) {
            i++;
            for (const auto& key : it.hotkey.keys) {
                co_yield{ (HotKeyType)(hk_RunProgram_flag | i), key };
            }
        }
    }

};


namespace cfg_details {

	using ConfPtr = std::shared_ptr<ProgramConfig>;

	inline constinit ConfPtr g_config;
	inline constinit std::unique_ptr<ProgramConfig> g_guiCfg;

	inline auto conf_gui() { return g_guiCfg.get(); }

	TStatus LoadConfig(ProgramConfig& cfg);
	TStatus Save_conf(const ProgramConfig& gui);

	inline void ApplyGuiConfig() {
		ConfPtr ptr = MAKE_SHARED(ptr);
		*ptr = *conf_gui();
		g_config.swap(ptr);
	}

	inline void SaveGuiConfig() {
		IFS_LOG(Save_conf(*conf_gui()));
	}

	inline bool ReloadGuiConfig() {
		bool res = true;
		if (!g_guiCfg) {
			g_guiCfg.reset(new ProgramConfig());
		}
		auto errLoadConf = LoadConfig(*conf_gui());
		if (errLoadConf != TStatus::SW_ERR_SUCCESS) {
			IFS_LOG(errLoadConf);
			res = false;
		}
		else {
			if (conf_gui()->config_version != SW_VERSION) {
				conf_gui()->config_version = SW_VERSION;
				SaveGuiConfig();
			}
		}
		ApplyGuiConfig();
		return res;
	}
}

/*
Многопоточный конфиг
	conf_gui() - read/write from gui thread
	conf_get_unsafe, GETCONF - read from any threads
*/

inline auto conf_get_unsafe() { // Проблемы синтаксиса conf_get_unsafe()->...  1) std::generator не держит temporary 2) множественном вызов даст другую версию.
    auto res = std::const_pointer_cast<const ProgramConfig>(cfg_details::g_config);
    return res; 
}

inline auto conf_gui() { return cfg_details::conf_gui();}

#define GETCONF auto cfg = conf_get_unsafe();


inline void SaveApplyGuiConfig() {
	cfg_details::ApplyGuiConfig();
	cfg_details::SaveGuiConfig();
}

inline void ApplyLocalization() {
	loc_details::g_loc.Reinit(conf_gui()->gui_lang.c_str());
}

inline void SetLogLevel_info(TLogLevel logLevel) {
	SetLogLevel(logLevel);
	LOG_ANY("Log level now {}. ver={}", (int)logLevel, SW_VERSION);
	for (const auto& it : conf_get_unsafe()->layouts_info.info) { // todo print config
		LOG_ANY("have lay {}. enabled={}", (void*)it.layout, it.enabled);
	}
}






