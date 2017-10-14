#pragma once

#include "CHotKey.h"
#include "SimpleSwitcher.h"

struct CHotKeySet
{
	CHotKey key;
	CHotKey def;
	bool fReserveHotKey = true;
	bool fFillFromDef = true;
};

inline TStatus PostMsgSettingChanges()
{
	HWND hwnd = FindWindow(c_sClassName32, 0);
	if (hwnd != NULL)
	{
		PostMessage(hwnd, c_MSG_SettingsChanges, 0, 0);
	}
	SW_RETURN_SUCCESS;
}

class SettingsGui
{
public:
	SettingsGui();
	~SettingsGui();

	bool isEnabled;
	bool isEnabledSaved;
	bool isAddToAutoStart;
	bool isMonitorAdmin;
	bool isAddToTray;
	bool isTryOEM2;
	bool isDashSeparate;
	bool fHookDll;
	//bool isAltCapsGen;

	//CHotKey capsRemap;
	TKeyCode capsRemapApply;
	TUInt64 bootTime;

	enum 
	{
		SW_HKL_1 = 0,
		SW_HKL_2,
		SW_HKL_3,
	};
	HKL hkl_lay[3];
	

	CHotKeySet hotKeys[HotKey_SIZE];


	void Load();
	void Save();
	void SaveAndPostMsg()
	{
		Save();
		PostMsgSettingChanges();
	}

	static SettingsGui& Global()
	{
		static SettingsGui settings;
		return settings;
	}
private:
	std::wstring m_sIniPath;
	
};

inline SettingsGui& SettingsGlobal() { return SettingsGui::Global(); }

