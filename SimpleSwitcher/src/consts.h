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

inline const char* LOC(const char* s) { return s; }



