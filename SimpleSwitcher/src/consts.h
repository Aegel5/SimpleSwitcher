#pragma once

// #define SW_INT_CHECK


static const UINT c_MSG_TypeHotKey = 0xBFFF - 37;
static const UINT c_MSG_Quit = 0xBFFF - 35;
static const UINT WM_SWRevert = 0xBFFF - 34;
static const UINT c_MSG_SHOW = 0xBFFF - 33;
//static const UINT WM_ClipDataArrave = 0xBFFF - 32;
//static const UINT WM_CtrlC = 0xBFFF - 31;
static const UINT WM_HWorker = 0xBFFF - 30;
//static const UINT WM_LayNotif = 0xBFFF - 29; 

static const UINT c_timerIdClearFormat = 7;
static const UINT c_timerIdAddToTray = 8;
static const UINT c_timerPrintAlive = 9;
//static const UINT c_timerWaitClip = 10;
static const UINT c_timerGetCurLayout = 11;
static const UINT c_timerKeyloggerDefence = 12;
static const UINT c_timerGetcurlay = 13;

static const UINT c_msgRevertID = WM_NULL;
static const WPARAM c_msgWParm = (WPARAM)0x9F899984;
static const WPARAM c_msgLParm = (WPARAM)0x34729EC9;

//static const TCHAR c_sClassName32[] = L"C01D72259X32";
//static const TCHAR c_sClassName64[] = L"C01D72259X64";
//static const TCHAR c_sClassNameGUI[] = L"C03a3bb9a011";

static const TChar c_mtxHook32[] = L"Local\\SW_MTX_HOOK_32_12243434";
static const TChar c_mtxSingltonGui[] = L"Local\\SW_MTX_SINGLTON_GUI_AFSDJF";



static const TChar c_sArgShedulerOFF[] = L"/SheduleRemove";
static const TChar c_sArgSchedulerON[] = L"/SheduleAdd";
static const TChar c_sArgStarter[] = L"/startMonitor";
static const TChar c_sArgHook32[] = L"/hook32";
static const TChar c_sArgHook64[] = L"/hook64";
static const TChar c_sArgService[] = L"/service";
static const TChar c_sArgAutostart[] = L"/autostart";
static const TChar c_sArgCapsRemapAdd[] = L"/CapsRemapAdd";
static const TChar c_sArgCapsRemapRemove[] = L"/CapsRemapRemove";
static const TChar c_sArgScrRemapAdd[] = L"/ScrRemapAdd";
static const TChar c_sArgScrRemapRemove[] = L"/ScrRemapRemove";

enum 
{
	SW_CLIENT_PUTTEXT =      0b1,
	SW_CLIENT_BACKSPACE   =  0b10,
	SW_CLIENT_SetLang =      0b100,
	SW_CLIENT_CTRLC =        0b1000,
	SW_CLIENT_CTRLV =        0b10000,
	SW_CLIENT_NO_WAIT_LANG = 0b100000,
};



enum
{
	SendTop_NeedRestore = 0x10000,
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
    hk_RevertSel,
    hk_CapsGenerate,
    hk_CycleCustomLang,
    hk_CycleLang_win_hotkey,
    hk_ScrollGenerate,
    hk_toUpperSelected,

    hk_SetLayout_flag  = 0b100000000000,
    hk_RunProgram_flag = 0b1000000000000,
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