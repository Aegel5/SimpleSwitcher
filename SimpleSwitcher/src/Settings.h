#pragma once

#include "ConfigData.h"

inline TStatus GetPath_Conf(std::wstring& path) {
    IFS_RET(Utils::GetPath_folder_noLower(path));
    path += L"SimpleSwitcher.json";
    RETURN_SUCCESS;
}

inline std::wstring GetPath_Conf() {
    std::wstring path;
    IFS_LOG(GetPath_Conf(path));
    return path;
}

class ProgramConfig {
public:
    ProgramConfig() {
        GenerateListHK();
    }

    std::set <std::wstring> disableInPrograms;
    std::set <std::wstring> disableInPrograms_normalized;

    void NormalizePaths() {
        for (const auto& it : disableInPrograms) {
			auto cur = it;
			StrUtils::ToLower(cur);
            Utils::NormalizeDelims(cur);
            disableInPrograms_normalized.insert(cur);
        }
    }
    bool IsSkipProgramTop() const {

        const auto& col = disableInPrograms_normalized;

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

    TUInt32 uiLang_ = wxLANGUAGE_RUSSIAN;

    bool fixRAlt = false;
    HKL fixRAlt_lay_ = (HKL)0x4090409;
    bool isMonitorAdmin = false;
    bool force_DbgMode              = false;
    bool fClipboardClearFormat = false;
    bool EnableKeyLoggerDefence = false;
    bool disableAccessebility    = false;
    static constexpr TStr showOriginalFlags = L"Original Flags";
    static constexpr TStr showAppIcon = L"Application Icon";
    wstring flagsSet = L"Square";
    string flagsSet2 = "Square";
    bool AlwaysSkipInject = false;
    bool AlternativeLayoutChange = false;
    TUInt32 quick_press_ms = 280;
    bool separate_ext_last_word = false;
    bool separate_ext_several_words = false;
    CHotKey win_hotkey_cycle_lang { VK_LMENU, VK_SHIFT };
	std::string theme = "Light";

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

    void GenerateListHK();

};


using ConfPtr = std::shared_ptr<ProgramConfig>;
inline constinit ConfPtr __g_config; 
inline auto conf_get_unsafe() { // Проблемы синтаксиса conf_get_unsafe()->...  1) std::generator не держит temporary 2) множественном вызов даст другую версию.
	assert(__g_config.get() != 0);
    auto res = std::const_pointer_cast<const ProgramConfig>(__g_config);
    return res; 
}

#define GETCONF auto cfg = conf_get_unsafe();


TStatus LoadConfig(ProgramConfig& sets);
TStatus _Save_conf(const ProgramConfig& gui);

inline void _conf_set(ConfPtr& conf) {
    __g_config.swap(conf);
    conf.reset();
    IFS_LOG(_Save_conf(*__g_config)); // сразу сохраняем в файл.
}

inline void SaveConfigWith(auto change) {
    ConfPtr conf{ new ProgramConfig(*conf_get_unsafe()) };
    change(conf.get());
    _conf_set(conf);
}

/*
Многопоточный конфиг

Чтение:
    GETCONF, далее используем cfg.
Запись:
    SaveConfigWith()

*/



