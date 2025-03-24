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
    hk_RevertSeveralWords,
    hk_RevertAllRecentText,
    hk_RevertSelelected,
    hk_EmulateCapsLock,
    hk_CycleSwitchLayout,
    hk_CycleLang_win_hotkey,
    hk_EmulateScrollLock,
    hk_toUpperSelected,
    hk_ToggleEnabled,

    hk_hotkeys_end,

    hk_SetLayout_flag  = 1<<30,
    hk_RunProgram_flag = 1<<29,
};


consteval auto adl_enum_bounds(HotKeyType) -> simple_enum::adl_info<HotKeyType> {
    return { HotKeyType::hk_NULL, HotKeyType::hk_hotkeys_end }; // Assumes my_enum satisfies enum_concept
}

inline bool IsNeedSavedWords(HotKeyType hk_type) {
    return (Utils::is_in(hk_type, hk_RevertLastWord, hk_RevertSeveralWords, hk_RevertAllRecentText));
}

inline std::string_view HotKeyTypeName(HotKeyType hk_type) { return simple_enum::enum_name(hk_type); }