#pragma once

#include "CAutoCleanup.h"

struct __Handle2Wrapper
{
	HANDLE obj = INVALID_HANDLE_VALUE;
	HANDLE& ref() { return obj; }
	bool IsValid() { return obj != INVALID_HANDLE_VALUE && obj != NULL; }
	void ToNull() { obj = INVALID_HANDLE_VALUE; }
	void CleanupFunction() { ::CloseHandle(obj); }
};
using CAutoHandle = TemplateAutoClose2<HANDLE, __Handle2Wrapper>;

struct __HandleFindWrapper
{
	HANDLE obj = INVALID_HANDLE_VALUE;
	HANDLE& ref() { return obj; }
	bool IsValid() { return obj != INVALID_HANDLE_VALUE; }
	void ToNull() { obj = INVALID_HANDLE_VALUE; }
	void CleanupFunction() { ::FindClose(obj); }
};
using CAutoHandleFind = TemplateAutoClose2<HANDLE, __HandleFindWrapper>;


//MAKE_AUTO_CLEANUP(HANDLE, ::CloseHandle, NULL, CAutoHandle__)
MAKE_AUTO_CLEANUP(HHOOK, ::UnhookWindowsHookEx, nullptr, CAutoHHOOK)
MAKE_AUTO_CLEANUP(HKEY, ::RegCloseKey, nullptr, CAutoCloseHKey)
MAKE_AUTO_CLEANUP(HWINEVENTHOOK, ::UnhookWinEvent, nullptr, CAutoHWINEVENTHOOK)
//MAKE_AUTO_CLEANUP(HMODULE, ::FreeLibrary, nullptr, CAutoHMODULE)
//MAKE_AUTO_CLEANUP(SC_HANDLE, ::CloseServiceHandle, nullptr, CAutoSCHandle)
MAKE_AUTO_CLEANUP(LPVOID, ::LocalFree, nullptr, CAutoWinMem)
//MAKE_AUTO_CLEANUP(LPVOID, ::GlobalUnlock, nullptr, CAutoGlobalLock)
MAKE_AUTO_CLEANUP(HICON, ::DestroyIcon, nullptr, CAutoHIcon)  // todo use unique_ptr
MAKE_AUTO_CLEANUP(HBITMAP, ::DeleteObject, nullptr, CAutoHBitmap)




