#pragma once

#include "Dispatcher.h"
#include "loader_api.h"



enum HotKeyType : TUInt32
{
    hk_NULL = 0b100000000000000000000,

    hk_RevertLastWord = 0,
    hk_RevertCycle = 1,
    hk_RevertSel = 2,

    //hk_RevertAdv = 3,
    //hk_RevertCycleAdv = 4,

    //hk_ChangeLayoutCycle = 5,

    //hk_ChangeSetLayout_1 = 6,
    //hk_ChangeSetLayout_2 = 7,
    //hk_ChangeSetLayout_3 = 8,

    hk_CapsGenerate = 9,

    hk_CycleCustomLang = 10,

    //hk_ChangeCase = 11,

    hk_ScrollGenerate = 12,

    //hk_EmulCopyNoFormat = 13,
    //hk_EmulCopyWithFormat = 14,

    //hk_RevertRecentTyped = 15,

    //hk_ChangeTextCase = 16,

    //hk_RevertLastWord_CustomLang = 17,
    //hk_RevertCycle_CustomLang = 18,

    hk_toUpperSelected = 19,

    hk_SetLayout_flag = 0b10000000000,

	hk_MAX,

	

	//hk_RevertFromClip,
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
	//tstring name = L"unknown";
	//CHotKey key;
    std::vector<CHotKey> def_list;
	//CHotKey def;
	//CHotKey def2;
	//bool fReserveHotKey = true;
	bool fNeedSavedWord = false;
	bool fUseDef = false;
	//bool fGui = true;
	//bool fDisabled = false;
    HotKeyType hkId = hk_NULL;

    CHotKeyList keys;

    const TChar* gui_text = _(L"unknown");

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


//struct UserConf
//{
//
//
//    TStatus Load2();
//};

struct LayoutInfo {
    HKL layout = 0;
    bool enabled = true;
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

    CHotKey SystemLayoutChange { VK_LMENU, VK_LSHIFT };

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

    void Update_hk_from_layouts() {

        for (auto i = std::ssize(hotkeysList)-1; i >= 0; i--) {
            auto it = hotkeysList[i];
            if (TestFlag(it.hkId, hk_SetLayout_flag)) {
                Utils::RemoveAt(hotkeysList, i);
            }
        }

        for (int i = 0; i < layouts_info.size(); i++) {
            auto& it = layouts_info[i];
            if (!it.hotkey.Empty()) {
                auto hk = (HotKeyType)(hk_SetLayout_flag | i);
                CHotKeySet data;
                data.hkId = hk;
                data.keys = it.hotkey;
                hotkeysList.push_back(data);
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

//extern void Rereg_all();



