﻿#include "CAutoCleanup.h"

struct __Handle2Wrapper
{
	HANDLE obj = INVALID_HANDLE_VALUE;
	HANDLE& ref() { return obj; }
	bool IsValid() { return obj != INVALID_HANDLE_VALUE; }
	void ToNull() { obj = INVALID_HANDLE_VALUE; }
	void CleanupFunction() { ::CloseHandle(obj); }
};
using CAutoHandle2 = TemplateAutoClose2<HANDLE, __Handle2Wrapper>;

struct __HandleFindWrapper
{
	HANDLE obj = INVALID_HANDLE_VALUE;
	HANDLE& ref() { return obj; }
	bool IsValid() { return obj != INVALID_HANDLE_VALUE; }
	void ToNull() { obj = INVALID_HANDLE_VALUE; }
	void CleanupFunction() { ::FindClose(obj); }
};
using CAutoHandleFind = TemplateAutoClose2<HANDLE, __HandleFindWrapper>;


MAKE_AUTO_CLEANUP(HANDLE, ::CloseHandle, NULL, CAutoHandle)
MAKE_AUTO_CLEANUP(HHOOK, ::UnhookWindowsHookEx, NULL, CAutoHHOOK)
MAKE_AUTO_CLEANUP(HKEY, ::RegCloseKey, NULL, CAutoCloseHKey)
MAKE_AUTO_CLEANUP(HWINEVENTHOOK, ::UnhookWinEvent, NULL, CAutoHWINEVENTHOOK)
MAKE_AUTO_CLEANUP(HMODULE, ::FreeLibrary, NULL, CAutoHMODULE)
MAKE_AUTO_CLEANUP(SC_HANDLE, ::CloseServiceHandle, NULL, CAutoSCHandle)
MAKE_AUTO_CLEANUP(LPVOID, ::LocalFree, NULL, CAutoWinMem)
MAKE_AUTO_CLEANUP(LPVOID, ::GlobalUnlock, NULL, CAutoGlobalLock)

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