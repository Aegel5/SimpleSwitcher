#pragma once

#include "Dispatcher.h"
#include "loader_api.h"

#include "ConfigData.h"

inline TStatus GetCurLayRequest() {
    HWND hwnd = FindWindow(c_sClassNameServer2, 0);
    if (hwnd != NULL) {
        PostMessage(hwnd, WM_GetCurLay, 0, 0);
    }
    RETURN_SUCCESS;
}


class SettingsGui {
public:
    SettingsGui() {
        GenerateListHK();
    }

    TInt64 time_debug_log_last_enabled = 0;

    std::set<std::string> disableInPrograms;
    std::set<std::wstring> __disableInPrograms; // TODO use wxString instead!!!

    TLogLevel logLevel = LOG_LEVEL_3;

    bool IsSkipProgram(const std::wstring& sExeName) const {
        if (sExeName.empty()) return false;
        auto has = __disableInPrograms.find(sExeName) != __disableInPrograms.end();
        if (has) {
            LOG_INFO_1(L"Skip process %s because of disableInProcess", sExeName.c_str());
            return true;
        }
        return false;
    }

    enum class UiLang {
        rus,
        eng,
    };

    int config_version = 0;

    UiLang uiLang = UiLang::rus;

    bool fixRAlt = false;
    HKL fixRAlt_lay = 0;
    bool isMonitorAdmin = false;
    bool isTryOEM2 = true;
    bool fDbgMode              = false;
    bool fClipboardClearFormat = false;
    bool fEnableKeyLoggerDefence = false;
    bool disableAccessebility    = false;
    bool injectDll = false;
    bool showFlags = IsWindows10OrGreater();
    bool AllowRemoteKeys = false;
    bool AlternativeLayoutChange = false;

    std::vector< CHotKeySet> hotkeysList;

    static bool IsNeedSavedWords(HotKeyType hk_type) {
        if (Utils::is_in(hk_type, hk_RevertLastWord, hk_RevertCycle)) return true;
        return false;
    }

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
            if (it.hkId == hk_CycleLang_win_hotkey) continue;
            for (const auto& key : it.keys.keys) {
                co_yield{ it.hkId, key };
            }

        }
        int i = -1;
        for (const auto& it : layouts_info.info) {
            i++;
            for (const auto& key : it.hotkey.keys) {
                co_yield{ (HotKeyType)(hk_SetLayout_flag | i), key };
            }
        }
    }

    void GenerateListHK();

};

/*
Многопоточный конфиг

Чтение: 
    функция conf_get - держит текущий конфиг. Если нужно обращаться к нескольким переменным, объявляем переменную-держателя для атомарности чтения.
Запись: 1) делаем копию conf_copy 
    2) меняем новый на старый через conf_set 
    3) после set не имеем права больше писать в конфиг.

*/


using ConfPtr = std::shared_ptr<SettingsGui>;
inline ConfPtr __g_config(new SettingsGui()); // создаем как можно раньше.
inline auto conf_get() { 
    auto res = std::const_pointer_cast<const SettingsGui>(__g_config);
    return res; 
}
inline ConfPtr conf_copy() { return ConfPtr(new SettingsGui(*conf_get())); }


inline int g_hotkeyWndOpened = 0;

TStatus LoadConfig(SettingsGui& sets);
TStatus Save2(const SettingsGui& gui);
inline TStatus Save() { return Save2(*conf_get());}

inline void conf_set(ConfPtr& conf) {
    __g_config.swap(conf);
    conf.reset();
    IFS_LOG(Save()); // сразу сохраняем в файл.
}




