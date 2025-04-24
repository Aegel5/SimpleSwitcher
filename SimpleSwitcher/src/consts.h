#pragma once

// #define SW_INT_CHECK


static const UINT c_MSG_TypeHotKey = 0xBFFF - 37;
//static const UINT c_MSG_Quit = 0xBFFF - 35;
static const UINT WM_LayNotif = 0xBFFF - 29; 
static const UINT WM_ShowWindow = 0xBFFF - 30; 

static const UINT c_timerKeyloggerDefence = 12;
static const TChar c_sArgAutostart[] = L"/autostart";




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
    hk_EmulateScrollLock,
    hk_toUpperSelected,
    hk_ToggleEnabled,
    hk_ShowMainWindow,
    hk_ShowRemainderWnd,
	hk_InsertWithoutFormat,

    hk_hotkeys_end,

    hk_SetLayout_flag  = 1<<30,
    hk_RunProgram_flag = 1<<29,
};

inline const char* LOC(const char* s) { return s; }

inline const char* GetGuiTextForHk(HotKeyType hk) {
    switch (hk) {
    case hk_RevertLastWord: return LOC("Change layout for last word");
    case hk_RevertSeveralWords: return LOC("Change layout for last several words");
    case hk_RevertAllRecentText: return LOC("Change layout for all recent text");
    case hk_RevertSelelected: return LOC("Change layout for selected text");
    case hk_CycleSwitchLayout: return  LOC("Cyclic layout change");
    case hk_EmulateCapsLock: return LOC("Generate CapsLock");
    case hk_toUpperSelected: return LOC("Selected text to UPPER/lower");
    case hk_ToggleEnabled: return LOC("Enable/Disable");
    case hk_ShowMainWindow: return LOC("Show/hide main window");
    case hk_ShowRemainderWnd: return LOC("Show/hide remainder");
	case hk_InsertWithoutFormat: return LOC("Paste text without formatting");
    }
    return "Error";
}


consteval auto adl_enum_bounds(HotKeyType) -> simple_enum::adl_info<HotKeyType> {
    return { HotKeyType::hk_NULL, HotKeyType::hk_hotkeys_end }; // Assumes my_enum satisfies enum_concept
}

inline bool IsNeedSavedWords(HotKeyType hk_type) {
    return (Utils::is_in(hk_type, hk_RevertLastWord, hk_RevertSeveralWords, hk_RevertAllRecentText));
}

inline std::string_view HotKeyTypeName(HotKeyType hk_type) { return simple_enum::enum_name(hk_type); }
