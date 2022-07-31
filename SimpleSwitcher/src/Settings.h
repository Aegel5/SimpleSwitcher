#pragma once

#include "Dispatcher.h"
#include "loader_api.h"


enum HotKeyType : TUInt32
{
    hk_NULL = 0xFFFFFF,

	hk_RevertLastWord = 0,
	hk_RevertCycle = 1,
	hk_RevertSel = 2,

	//hk_RevertAdv = 3,
	//hk_RevertCycleAdv = 4,

	//hk_ChangeLayoutCycle = 5,

	hk_ChangeSetLayout_1 = 6,
	hk_ChangeSetLayout_2 = 7,
	hk_ChangeSetLayout_3 = 8,

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
	case hk_ChangeSetLayout_1:return "hk_SetLayout_1";
	case hk_ChangeSetLayout_2:return "hk_SetLayout_2";
	case hk_ChangeSetLayout_3:return "hk_SetLayout_3";
	case hk_CapsGenerate:return "hk_EmulateCapsLock";
	case hk_CycleCustomLang:return "hk_CycleSwitchLayout";
	case hk_ScrollGenerate:return "hk_EmulateScrollLock";
    case hk_toUpperSelected:    return "hk_toUpperSelected";
	case hk_MAX:return "hk_MAX";
	default: return "hk_Unknown";
	}
}

struct CHotKeySet
{
	//tstring name = L"unknown";
	//CHotKey key;
	CHotKey def;
	CHotKey def2;
	//bool fReserveHotKey = true;
	bool fNeedSavedWord = false;
	bool fUseDef = false;
	//bool fGui = true;
	//bool fDisabled = false;
    HotKeyType hkId = hk_NULL;

    std::vector<CHotKey> keys;

    bool HasKey(CHotKey ktest, CHotKey::TCompareFlags flags) {
        for (auto& k : keys) {
            if (ktest.Compare(k, flags))
                return true;
        }
        return false;
    }

    CHotKeySet() {
        keys.resize(1);
    }

    CHotKey& key() {
        return keys[0];
    }
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
        hkl_lay.resize(3);
    }

    std::set<std::string> disableInPrograms;
    std::set<std::wstring> __disableInPrograms; // TODO use wxString instead!!!

    TLogLevel logLevel = LOG_LEVEL_3;

    bool IsSkipProgram(std::wstring sExeName) {
        auto has = __disableInPrograms.find(sExeName) != __disableInPrograms.end();
        if (has) {
            LOG_INFO_1(L"Skip process %s because of disableInProcess", sExeName.c_str());
            return true;
        }
        return false;
    }



    bool isMonitorAdmin = false;
    bool isTryOEM2 = true;
    bool fDbgMode              = false;
    bool fClipboardClearFormat = false;
    bool fEnableKeyLoggerDefence = false;
    bool disableAccessebility    = false;
    bool injectDll = false;
    bool showFlags = IsWindows10OrGreater();

    std::vector<HKL> customLangList;
    std::vector<HKL> hkl_lay;


    typedef std::map<HotKeyType, CHotKeySet> THotKeyMap;
    THotKeyMap hotkeysList;

    CHotKeySet& GetHk(HotKeyType type) {
        return hotkeysList[type];
    }

    enum {
        SW_HKL_1 = 0,
        SW_HKL_2,
        SW_HKL_3,
    };

    void GenerateListHK();

};

//inline UserConf u_conf;

inline SettingsGui settings_thread;
inline SettingsGui setsgui;

inline int g_hotkeyWndOpened = 0;

TStatus Load(SettingsGui& sets, bool* notExists = nullptr);
TStatus Save2(SettingsGui& gui);
inline TStatus Save() {
    return Save2(setsgui);
}

inline void SaveAndPostMsg() {
    IFS_LOG(Save());
    PostMsgSettingChanges();
}



