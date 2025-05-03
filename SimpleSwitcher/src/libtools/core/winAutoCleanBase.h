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
MAKE_AUTO_CLEANUP(HMODULE, ::FreeLibrary, nullptr, CAutoHMODULE)
MAKE_AUTO_CLEANUP(SC_HANDLE, ::CloseServiceHandle, nullptr, CAutoSCHandle)
MAKE_AUTO_CLEANUP(LPVOID, ::LocalFree, nullptr, CAutoWinMem)
MAKE_AUTO_CLEANUP(LPVOID, ::GlobalUnlock, nullptr, CAutoGlobalLock)
MAKE_AUTO_CLEANUP(HICON, ::DestroyIcon, nullptr, CAutoHIcon)  // todo use unique_ptr
MAKE_AUTO_CLEANUP(HBITMAP, ::DeleteObject, nullptr, CAutoHBitmap)

class CAutoWinEvent
{
	BOOL SetEvent(HANDLE evt)
	{
		Cleanup();
		BOOL res = ::SetEvent(evt);
		if(res)
			m_evt = evt;
	}
	~CAutoWinEvent()
	{
		Cleanup();
	}
	void Cleanup()
	{
		if (m_evt)
		{
			ResetEvent(m_evt);
			m_evt = nullptr;
		}
	}
private:
	HANDLE m_evt = nullptr;
};

class CAutoWinMutexWaiter
{
public:
	CAutoWinMutexWaiter() {}
	CAutoWinMutexWaiter(HANDLE mtx) 
	{
		Wait(mtx);
	}
	DWORD Wait(HANDLE mtx, DWORD nTimeOut)
	{
		Cleanup();
		DWORD res = WaitForSingleObject(mtx, nTimeOut);
		if(res == WAIT_OBJECT_0)
			m_mtx = mtx;
		return res;
	}
	void Wait(HANDLE mtx)
	{
		Wait(mtx, INFINITE);
	}
	~CAutoWinMutexWaiter()
	{
		Cleanup();
	}
	void Cleanup()
	{
		if(m_mtx)
		{
			ReleaseMutex(m_mtx);
			m_mtx = nullptr;
		}
	}
private:
	HANDLE m_mtx = nullptr;
};
