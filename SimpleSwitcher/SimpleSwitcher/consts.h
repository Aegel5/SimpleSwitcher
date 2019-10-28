#pragma once
#include "targetver.h"

// #define SW_INT_CHECK


static const UINT c_MSG_TypeHotKey = 0xBFFF - 37;
static const UINT c_MSG_SettingsChanges = 0xBFFF - 36;
static const UINT c_MSG_Quit = 0xBFFF - 35;
static const UINT WM_SWRevert = 0xBFFF - 34;
static const UINT c_MSG_SHOW = 0xBFFF - 33;
//static const UINT WM_ClipDataArrave = 0xBFFF - 32;
//static const UINT WM_CtrlC = 0xBFFF - 31;
static const UINT WM_HWorker = 0xBFFF - 30;

static const UINT c_timerIdClearFormat = 7;
static const UINT c_timerIdAddToTray = 8;
static const UINT c_timerPrintAlive = 9;
static const UINT c_timerWaitClip = 10;
static const UINT c_timerGetCurLayout = 11;

static const UINT c_msgRevertID = WM_NULL;
static const WPARAM c_msgWParm = (WPARAM)0x9F899984;
static const WPARAM c_msgLParm = (WPARAM)0x34729EC9;

static const S_Char c_sClassName32[] = "C01D72259X32";
static const S_Char c_sClassName64[] = "C01D72259X64";
static const TChar c_sClassNameGUI[] = L"C03a3bb9a011";

static const TChar c_mtxHook32[] = L"Local\\SW_MTX_HOOK_32_12243434";
static const TChar c_mtxSingltonGui[] = L"Local\\SW_MTX_SINGLTON_GUI_AFSDJF";

#ifdef _WIN64
static const S_Char* c_sClassName = c_sClassName64;
#else
static const S_Char* c_sClassName = c_sClassName32;
#endif 

static const S_Char c_sArgShedulerOFF[] = "/sheduleremove";
static const S_Char c_sArgSchedulerON[] = "/sheduleadd";
static const S_Char c_sArgStarter[] = "/startmonitor";
static const S_Char c_sArgHook32[] = "/hook32";
static const S_Char c_sArgHook64[] = "/hook64";
static const S_Char c_sArgService[] = "/service";
static const S_Char c_sArgAutostart[] = "/autostart";
static const S_Char c_sArgCapsRemapAdd[] = "/capsremapadd";
static const S_Char c_sArgCapsRemapRemove[] = "/capsremapremove";
static const S_Char c_sArgScrRemapAdd[] = "/scrremapadd";
static const S_Char c_sArgScrRemapRemove[] = "/scrremapremove";

enum 
{
	SW_CLIENT_PUTTEXT = 0x2,
	SW_CLIENT_BACKSPACE   = 0x4,
	SW_CLIENT_SetLang = 0x8,
	SW_CLIENT_CTRLC = 0x10,
	SW_CLIENT_CTRLV = 0x20,
};



enum
{
	SendTop_NeedRestore = 0x10000,
};

static const int c_nCommonWaitProcess = 5000;
static const int c_nCommonWaitMtx = 30000;

static const TChar c_sSiteLink[] = L"http://vk.com/simpleswitcher";

static const TChar c_sProgramName[] = SW_PROGRAM_NAME_L;

static const TChar c_sVersion[] = SW_VERSION_L;

static const LPCWSTR c_wszTaskName = SW_PROGRAM_NAME_L L"Task";