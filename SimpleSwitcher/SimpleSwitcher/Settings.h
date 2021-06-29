#pragma once

#include "SimpleSwitcher.h"


enum HotKeyType
{
	hk_RevertLastWord = 0,
	hk_RevertCycle = 1,
	hk_RevertSel = 2,

	//hk_RevertAdv = 3,
	//hk_RevertCycleAdv = 4,

	hk_ChangeLayoutCycle = 5,

	hk_ChangeSetLayout_1 = 6,
	hk_ChangeSetLayout_2 = 7,
	hk_ChangeSetLayout_3 = 8,

	hk_CapsGenerate = 9,

	hk_CycleCustomLang = 10,

	//hk_ChangeCase = 11,

	hk_ScrollGenerate = 12,

	hk_EmulCopyNoFormat = 13,
	hk_EmulCopyWithFormat = 14,

	hk_RevertRecentTyped = 15,

	//hk_ChangeTextCase = 16,

	hk_RevertLastWord_CustomLang = 17,
	hk_RevertCycle_CustomLang = 18,

	hk_MAX,

	//hk_RevertFromClip,
};

inline const wchar_t* HotKeyTypeName(HotKeyType hk_type)
{
	switch (hk_type)
	{
	case hk_RevertLastWord:	return L"hk_RevertLastWord";
	case hk_RevertCycle: return L"hk_RevertCycle";
	case hk_RevertSel: return L"hk_RevertSel";
	//case hk_RevertAdv: return "hk_RevertAdv";
	//case hk_RevertCycleAdv:return "hk_RevertCycleAdv";
	case hk_ChangeLayoutCycle:return L"hk_ChangeLayoutCycle";
	case hk_ChangeSetLayout_1:return L"hk_ChangeSetLayout_1";
	case hk_ChangeSetLayout_2:return L"hk_ChangeSetLayout_2";
	case hk_ChangeSetLayout_3:return L"hk_ChangeSetLayout_3";
	case hk_CapsGenerate:return L"hk_CapsGenerate";
	case hk_CycleCustomLang:return L"hk_CycleCustomLang";
	//case hk_ChangeCase:return L"hk_ChangeCase";
	case hk_ScrollGenerate:return L"hk_ScrollGenerate";
	case hk_EmulCopyNoFormat:return L"hk_EmulCopyNoFormat";
	case hk_EmulCopyWithFormat:return L"hk_EmulCopyWithFormat";
	case hk_RevertRecentTyped:return L"hk_RevertRecentTyped";
	//case hk_ChangeTextCase:return L"hk_ChangeTextCase";
	case hk_MAX:return L"hk_MAX";
	default: return L"Unknown";
	}
}

struct CHotKeySet
{
	tstring name = L"unknown";
	CHotKey key;
	CHotKey def;
	CHotKey def2;
	//bool fReserveHotKey = true;
	bool fNeedSavedWord = false;
	bool fUseDef = false;
	bool fGui = true;
	//bool fDisabled = false;
	TUInt32 hkId = -1;
};

inline TStatus PostMsgSettingChanges()
{
	HWND hwnd = FindWindow(c_sClassName32, 0);
	if (hwnd != NULL)
	{
		PostMessage(hwnd, c_MSG_SettingsChanges, 0, 0);
	}
	RETURN_SUCCESS;
}
using TStrList = std::vector<std::wstring>;
struct UserConf
{
	TStrList disableInProcess;
	int ll = 0;
	std::wstring s_hk_ChangeTextCase;
	bool fUseAltMode = false;
	TStrList altModePrg;
	int msDelayAfterCtrlC = 25;
};


class SettingsGui
{
public:
	SettingsGui()
	{
		GenerateListHK();
		ResetToDef();
	}
	~SettingsGui()
	{

	}

	// Runtime data
	// -----------------------------

	bool isEnabled = false;
	bool isAddToAutoStart = false;

	// Saved data
	// -------------------------

	bool isEnabledSaved;
	bool isMonitorAdmin;
	bool isAddToTray;
	bool isTryOEM2;
	//bool isDashSeparate;
	bool fDbgMode;
	bool fClipboardClearFormat;
	bool fCloseByEsc;

	//bool fHookDll;

	std::vector<HKL> customLangList;
	std::vector<HKL> revert_customLangList; 
	HKL hkl_lay[3] = { 0 };

	SwLang idLang = SLANG_UNKNOWN;

	
	typedef std::map<HotKeyType, CHotKeySet> THotKeyMap;
	THotKeyMap hotkeysList;

	// --------------------------------
	// Functions

	CHotKeySet& GetHk(HotKeyType type)
	{
		return hotkeysList[type];
	}

	void SetLogLevelBySettings()
	{
		if (fDbgMode)
		{
			SetLogLevel(LOG_LEVEL_1);
		}
		else
		{
			SetLogLevel(LOG_LEVEL_0);
		}
	}

	enum
	{
		SW_HKL_1 = 0,
		SW_HKL_2,
		SW_HKL_3,
	};

	void ResetToDef();
	bool IsAddToTray() { return isAddToTray; }

	void GenerateListHK();

	TStatus Load();
	TStatus LoadAutoSettings();

	void Save();
	void SaveAndPostMsg()
	{
		Save();
		PostMsgSettingChanges();
	}

	bool IsSkipProgram(std::wstring sExeName)
	{
		for (auto& sProc : u_conf.disableInProcess)
		{
			if (sExeName == sProc)
			{
				return true;
				LOG_INFO_1(L"Skip process %s because of disableInProcess", sProc.c_str());
				RETURN_SUCCESS;
			}
		}
		return false;
	}

	static SettingsGui& Global()
	{
		static SettingsGui settings;
		return settings;
	}
	UserConf u_conf;
private:

	std::wstring GetPathIni()
	{
		std::wstring sCurFolder;
		GetPath(sCurFolder, PATH_TYPE_SELF_FOLDER, GetSelfBit());

		std::wstring res = sCurFolder + L"settings.ini";

		return res;
	}
	
};

inline SettingsGui& SettingsGlobal() { return SettingsGui::Global(); }
//inline LuaConfig& LuaGlobal() { return SettingsGlobal().luaCfg; }



