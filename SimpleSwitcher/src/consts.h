#pragma once

// #define SW_INT_CHECK


static const UINT c_MSG_TypeHotKey = 0xBFFF - 37;
static const UINT c_MSG_Quit = 0xBFFF - 35;
static const UINT WM_LayNotif = 0xBFFF - 29; 
static const UINT c_timerKeyloggerDefence = 12;
static const TChar c_sArgAutostart[] = L"/autostart";

enum 
{
	SW_CLIENT_PUTTEXT =      1<<0,
	SW_CLIENT_BACKSPACE   =  1<<1,
	SW_CLIENT_SetLang =      1<<2,
	SW_CLIENT_CTRLC =        1<<3,
	SW_CLIENT_CTRLV =        1<<4,
	SW_CLIENT_NO_WAIT_LANG = 1<<5,
};


static const int c_nCommonWaitProcess = 5000;
static const int c_nCommonWaitMtx = 30000;

static const LPCWSTR c_wszTaskName = L"SimpleSwitcherTask";
const static TChar c_sRegRunValue[] = L"SimpleSwitcher";

enum HotKeyType : TUInt32
{
    hk_NULL,

    hk_RevertLastWord,
    hk_RevertCycle,
    hk_RevertAllRecentText,
    hk_RevertSel,
    hk_CapsGenerate,
    hk_CycleCustomLang,
    hk_CycleLang_win_hotkey,
    hk_ScrollGenerate,
    hk_toUpperSelected,
    hk_toggleEnabled,

    hk_SetLayout_flag  = 1<<30,
    hk_RunProgram_flag = 1<<29,
};

inline bool IsNeedSavedWords(HotKeyType hk_type) {
    if (Utils::is_in(hk_type, hk_RevertLastWord, hk_RevertCycle, hk_RevertAllRecentText)) return true;
    return false;
}


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
    case hk_RevertAllRecentText: return "hk_RevertAllRecentText";
    case hk_toggleEnabled: return "hk_ToggleEnabled";
    default: return "hk_Unknown";
    }
}