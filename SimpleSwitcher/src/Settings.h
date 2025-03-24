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

class SettingsGui {
public:
    SettingsGui() {
        GenerateListHK();
    }

    std::set <wxString> disableInPrograms;
    std::set <wxString> disableInPrograms_normalized;

    void NormalizePaths() {
        for (const auto& it : disableInPrograms) {
            auto cur = it.Lower();
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

    enum class UiLang {
        rus,
        eng,
    };

    wxString config_version;

    UiLang uiLang = UiLang::rus;

    bool fixRAlt = false;
    HKL fixRAlt_lay_ = (HKL)0x4090409;
    bool isMonitorAdmin = false;
    bool force_DbgMode              = false;
    bool IsNeedDebug() const { return Utils::IsDebug() || force_DbgMode; }
    bool fClipboardClearFormat = false;
    bool EnableKeyLoggerDefence = false;
    bool disableAccessebility    = false;
    bool showFlags = IsWindows10OrGreater();
    bool AllowRemoteKeys_ = true;
    bool AlternativeLayoutChange = false;

    bool separate_ext_last_word = false;
    bool separate_ext_several_words = false;

    std::vector< CHotKeySet> hotkeysList;
    std::vector< RunProgramInfo> run_programs = { {.path = L"example: calc.exe"} };

    LayoutInfoList layouts_info;

    const auto& GetHk(HotKeyType type) const {
        for (auto& it : hotkeysList) {
            if (it.hkId == type) return it;
        }
        LOG_INFO_1(L"CRITICAL ERR");
        abort();
    }

    std::generator < std::tuple<HotKeyType, const CHotKey&>> All_hot_keys() const {
        for (const auto& it : hotkeysList) {
            if (Utils::is_in(it.hkId, hk_CycleLang_win_hotkey, hk_ToggleEnabled)) continue;
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

/*
Многопоточный конфиг

Чтение: 
    GETCONF, далее используем cfg.
Запись: 
    1) делаем копию conf_copy 
    2) меняем новый на старый через conf_set 
    3) после set не имеем права больше писать в конфиг.

*/


using ConfPtr = std::shared_ptr<SettingsGui>;
inline ConfPtr __g_config(new SettingsGui()); // создаем как можно раньше.
inline auto conf_get_unsafe() { // Проблемы синтаксиса conf_get_unsafe()->...  1) std::generator не держит temporary 2) множественном вызов даст другую версию.
    auto res = std::const_pointer_cast<const SettingsGui>(__g_config);
    return res; 
}
inline ConfPtr conf_copy() { return ConfPtr(new SettingsGui(*conf_get_unsafe())); }
#define GETCONF auto cfg = conf_get_unsafe();




TStatus LoadConfig(SettingsGui& sets);
TStatus Save2(const SettingsGui& gui);
inline TStatus Save() { return Save2(*conf_get_unsafe());}

inline void conf_set(ConfPtr& conf) {
    __g_config.swap(conf);
    conf.reset();
    IFS_LOG(Save()); // сразу сохраняем в файл.
}

inline void SaveConfigWith(auto set) {
    auto conf = conf_copy();
    set(conf.get());
    conf_set(conf);
}



