#pragma once

enum TStatus
{
	SW_ERR_SUCCESS                          = 0,
	SW_ERR_UNKNOWN                          ,
	SW_ERR_UNSUPPORTED                      ,
	SW_ERR_INVALID_PARAMETR                 ,
	SW_ERR_PROCESS_NOT_STOPPED              ,
	SW_ERR_WINAPI                           ,
	SW_ERR_CRT                              ,
	SW_ERR_NOT_FOUND                        ,
	SW_ERR_TIMEOUT                          ,
	SW_ERR_TOO_MANY_ELEMENTS                ,
	SW_ERR_CANT_WAIT_MUTEX                  ,
	SW_ERR_GUI_ERROR                        ,
	SW_ERR_HRESULT                          ,
	SW_ERR_LSTATUS                          ,
	SW_ERR_WAIT_PROCESS                     ,
	SW_ERR_WND_NOT_FOUND,
	SW_ERR_PROTOCOL_VER,
	SW_ERR_BUFFER_TOO_SMALL,
	SW_ERR_ALREADY_RUN,
	SW_ERR_LUA_ERROR,
	SW_ERR_ERRNO,
	SW_ERR_BAD_INTERNAL_STATE,
	SW_ERR_NO_MEMORY,
	SW_ERR_JSON,
};



const static TChar* c_StatusNames[] =
{
	L"SW_ERR_SUCCESS", 
	L"SW_ERR_UNKNOWN", 
	L"SW_ERR_UNSUPPORTED", 
	L"SW_ERR_INVALID_PARAMETR", 
	L"SW_ERR_PROCESS_NOT_STOPPED", 
	L"SW_ERR_WINAPI", 
	L"SW_ERR_CRT", 
	L"SW_ERR_NOT_FOUND", 
	L"SW_ERR_TIMEOUT", 
	L"SW_ERR_TOO_MANY_ELEMENTS", 
	L"SW_ERR_CANT_WAIT_MUTEX", 	
	L"SW_ERR_GUI_ERROR", 	
	L"SW_ERR_HRESULT",
	L"SW_ERR_LSTATUS",
	L"SW_ERR_WAIT_PROCESS",
	L"SW_ERR_WND_NOT_FOUND",
	L"SW_ERR_PROTOCOL_VER",
	L"SW_ERR_BUFFER_TOO_SMALL",
	L"SW_ERR_ALREADY_RUN",
	L"SW_ERR_LUA_ERROR",
	L"SW_ERR_ERRNO",
	L"SW_ERR_BAD_INTERNAL_STATE",
	L"SW_ERR_NO_MEMORY",
	L"SW_ERR_JSON",
};

//#define SAFEENTER try{
//#define SAFELEAVE 

//inline void ShowError(TStatus stat, HWND hwnd, const TChar* sMessage = nullptr)
//{
//	TChar buf[1024];
//	swprintf_s(
//		buf,
//		L"%s\n%s (%d)\nWinapi error: %d",
//		sMessage,
//		c_StatusNames[stat],
//		stat,
//		GetLastError());
//	MessageBox(
//		hwnd, 
//		buf,
//		SW_PROGRAM_NAME_L L" Error", 
//		MB_OK | MB_ICONEXCLAMATION);
//}








