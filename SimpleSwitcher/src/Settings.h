#pragma once

#include "Dispatcher.h"
#include "loader_api.h"



enum HotKeyType : TUInt32
{
    hk_NULL,

    hk_RevertLastWord,
    hk_RevertCycle,
    hk_RevertSel,
    hk_CapsGenerate,
    hk_CycleCustomLang,
    hk_CycleLang_win_hotkey,
    hk_ScrollGenerate,
    hk_toUpperSelected,

    hk_SetLayout_flag = 0b100000000000,
};

inline const char* HotKeyTypeName(HotKeyType hk_type)
{
	switch (hk_type)
	{
	case hk_RevertLastWord:	return "hk_RevertLastWord";
	case hk_RevertCycle: return "hk_RevertSeveralWords";
	case hk_RevertSel: return "hk_RevertSelelected";
	case hk_CapsGenerate:return "hk_EmulateCapsLock";
	case hk_CycleCustomLang:return "hk_CycleSwitchLayout";
	case hk_CycleLang_win_hotkey:return "hk_CycleLang_win_hotkey";
	case hk_ScrollGenerate:return "hk_EmulateScrollLock";
    case hk_toUpperSelected:    return "hk_toUpperSelected";
	default: return "hk_Unknown";
	}
}

struct CHotKeyList {
    std::vector<CHotKey> keys{ 1 };
    bool HasKey(CHotKey ktest, CHotKey::TCompareFlags flags) const {
        for (const auto& k : keys) {
            if (ktest.Compare(k, flags))
                return true;
        }
        return false;
    }

    bool Empty() {
        for (const auto& k : keys) {
            if (!k.IsEmpty())
                return false;
        }
        return true;
    }

    bool HasKey_skipkeyup(CHotKey ktest, CHotKey::TCompareFlags flags) const {
        for (const auto& k : keys) {
            if (!k.GetKeyup() && ktest.Compare(k, flags))
                return true;
        }
        return false;
    }

    CHotKey& key() {
        return keys[0];
    }

    const CHotKey& key() const {
        return keys[0];
    }
};

struct CHotKeySet
{
    std::vector<CHotKey> def_list;
	bool fNeedSavedWord = false;
	bool fUseDef = false;
    HotKeyType hkId = hk_NULL;

    CHotKeyList keys;

    const TChar* gui_text = _(L"unknown").wc_str();

};

inline TStatus PostMsgSettingChanges()
{
    HWND hwnd = FindWindow(c_sClassNameServer2, 0);
	if (hwnd != NULL)
	{
		PostMessage(hwnd, c_MSG_SettingsChanges, 0, 0);
	}
	RETURN_SUCCESS;
}
inline TStatus GetCurLayRequest() {
    HWND hwnd = FindWindow(c_sClassNameServer2, 0);
    if (hwnd != NULL) {
        PostMessage(hwnd, WM_GetCurLay, 0, 0);
    }
    RETURN_SUCCESS;
}
using TStrList = std::vector<std::wstring>;


struct LayoutInfo {
    HKL layout = 0;
    bool enabled = true;
    bool fix_ralt = false;
    CHotKey WinHotKey;
    CHotKeyList hotkey;
};



class SettingsGui {
public:
    SettingsGui() {

        GenerateListHK();

        fDbgMode =
#ifdef _DEBUG
            true;
#else
            false;
#endif
    }

    TInt64 time_debug_log_last_enabled = 0;

    std::vector<LayoutInfo> layouts_info;
    bool AllLayoutEnabled() const {
        for (const auto& it : layouts_info) {
            if (!it.enabled) return false;
        }
        return true;
    }
    bool HasLayout(HKL lay) const {
        for (const auto& it : layouts_info) {
            if (it.layout == lay) return true;
        }
        return false;
    }
    const LayoutInfo* GetLayoutInfo(HKL lay) const {
        for (const auto& it : layouts_info) {
            if (it.layout == lay) 
                return &it;
        }
        return nullptr;
    }

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

    bool fixAltCtrl = false;
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

    auto& GetHk(HotKeyType type) {
        for (auto& it : hotkeysList) {
            if (it.hkId == type) return it;
        }
        LOG_INFO_1(L"CRITICAL ERR");
        abort();
    }
    const auto& GetHk(HotKeyType type) const {
        for (auto& it : hotkeysList) {
            if (it.hkId == type) return it;
        }
        LOG_INFO_1(L"CRITICAL ERR");
        abort();
    }

    std::generator < std::tuple<HotKeyType, const CHotKeyList&, bool>> All_hot_keys() const {
        for (const auto& it : hotkeysList) {
            if (it.hkId == hk_CycleLang_win_hotkey) continue;
            co_yield { it.hkId, it.keys, it.fNeedSavedWord };
        }
        int i = -1;
        for (const auto& it : layouts_info) {
            i++;
            co_yield{ (HotKeyType)(hk_SetLayout_flag | i), it.hotkey, false };
        }
    }

    HKL GetLayToFix() const {
        for (const auto& it : layouts_info) {
            if (it.fix_ralt) return it.layout;
        }
        return 0;
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

//extern void Rereg_all();



