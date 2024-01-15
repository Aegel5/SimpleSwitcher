#pragma once 

inline HWND g_guiHandle = nullptr;

inline void SetLogLevel_v3(TLogLevel logLevel)
{
	SwLoggerGlobal().SetLogLevel(logLevel);
	LOG_INFO_1(L"Log level now %d. Version=%s", logLevel, SW_VERSION);
}

//inline bool conf_bad = false;