// SimpleLayout.cpp: определяет точку входа для приложения.
//

#include "stdafx.h"
#include "SimpleSwitcher.h"
#include "CMainWorker.h"
#include "Settings.h"


TStatus resethook();

struct HookGlobalHandles
{
	CAutoHHOOK hHookKeyGlobal;
	CAutoHHOOK hHookMouseGlobal;
	CAutoHWINEVENTHOOK hHookEventGlobal;
	CAutoHWINEVENTHOOK hHookEventGlobalSwitchDesk;
};

TStatus HookGlobal(HookGlobalHandles& handles);

TStatus StartCycle(_In_ HINSTANCE hInstance,_In_ int nCmdShow)
{
	LOG_INFO_1(L"StartCycle...");

	IFW_LOG(SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_ABOVE_NORMAL));

	WNDCLASSEX wcex = { 0 };

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.lpfnWndProc = DefWindowProc;
	wcex.hInstance = hInstance;
	wcex.lpszClassName = c_sClassName;

	IFW_RET(RegisterClassEx(&wcex) != 0);

	HWND hWnd = CreateWindow(
		c_sClassName,
		L"Title",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

	IFW_RET(hWnd != NULL);

	gdata().hWndMonitor = hWnd;

	if (gdata().curModeBit == SW_BIT_32)
	{
		if (WinApiInt::AddClipboardFormatListener)
		{
			IFW_LOG(WinApiInt::AddClipboardFormatListener(hWnd));
		}
	}

	if (WinApiInt::ChangeWindowMessageFilterEx)
	{
		IFW_LOG(WinApiInt::ChangeWindowMessageFilterEx(hWnd, c_MSG_Quit, MSGFLT_ALLOW, 0));
		IFW_LOG(WinApiInt::ChangeWindowMessageFilterEx(hWnd, c_MSG_SettingsChanges, MSGFLT_ALLOW, 0));
	}

	HookGlobalHandles hookHandles;
	if (gdata().curModeBit == SW_BIT_32)
	{
		IFS_RET(HookGlobal(hookHandles));
	}


	auto timeId = SetTimer(hWnd, c_timerKeyloggerDefence, 5000, NULL);
	IFW_LOG(timeId != 0);


	MSG msg;
	while (true)
	{
		BOOL bRet = GetMessage(&msg, NULL, 0, 0);

		if (bRet == 0)
			break;

		if (bRet == -1)
		{
			RETW();
		}

		auto mesg = msg.message;

		if (mesg == WM_HOTKEY)
		{
		}
		else if (mesg == c_MSG_SettingsChanges)
		{
			if (gdata().curModeBit == SW_BIT_32)
			{
				Worker()->PostMsg(HWORKER_LoadSettings);
			}
		}
		else if (mesg == c_MSG_Quit)
		{
			PostQuitMessage(0);
		}
		else if (mesg == WM_CLIPBOARDUPDATE)
		{
			if (gdata().curModeBit == SW_BIT_32)
			{
				Worker()->PostMsg(HWORKER_ClipNotify);
			}
		}
		else if (mesg == WM_TIMER)
		{
			UINT_PTR timerId = msg.wParam;
			if (timerId == 0)
			{
				int k = 0;
			}
			if (timerId == c_timerKeyloggerDefence) {
				if (SettingsGlobal().fEnableKeyLoggerDefence) {
					resethook();
				}
			}else{
				IFW_LOG(KillTimer(gdata().hWndMonitor, timerId));
				Worker()->PostMsgW(HWORKER_WM_TIMER, timerId);
			}

		}
		else
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	RETURN_SUCCESS;
}


static void CALLBACK WinEventProc_SwitchDesk(
	HWINEVENTHOOK hWinEventHook,
	DWORD event,
	HWND hwnd,
	LONG idObject,
	LONG idChild,
	DWORD dwEventThread,
	DWORD dwmsEventTime
	)
{
	Worker()->PostMsg(HWORKER_ClearWords);
}


void CALLBACK WinEventProc(HWINEVENTHOOK hWinEventHook, DWORD event, HWND hwnd, LONG idObject, LONG idChild, DWORD dwEventThread, DWORD dwmsEventTime)
{
	Worker()->PostMsgW(HWORKER_ChangeForeground, (WPARAM)hwnd);
}

LRESULT CALLBACK LowLevelMouseProc(
	_In_  int nCode,
	_In_  WPARAM wParam,
	_In_  LPARAM lParam
)
{
	if (nCode == HC_ACTION)
	{
		if (wParam == WM_MOUSEMOVE)
		{
			// nothing
		}
		else
		{
			Worker()->PostMsg(HWORKER_ClearWords);
		}
	}

	return CallNextHookEx(NULL, nCode, wParam, lParam);
}

LRESULT CALLBACK LowLevelKeyboardProc(
	_In_  int nCode,
	_In_  WPARAM wParam,
	_In_  LPARAM lParam
	)
{
	if (nCode == HC_ACTION)
	{
		KBDLLHOOKSTRUCT* kStruct = (KBDLLHOOKSTRUCT*)lParam;

		//DWORD dwStart = GetTickCount();

		MainWorkerMsg msg;
		msg.mode = HWORKER_KeyMsg;
		auto& kData = msg.data.keyData;
		kData.ks = *kStruct;
		kData.wParam = wParam;
		Worker()->PostMsg(msg);

		//DWORD dwElapsed = GetTickCount() - dwStart;
		//if (dwElapsed > 250)
		//{
		//	LOG_INFO_1(L"[WARN]. Too long processing: %u", dwElapsed);
		//}
	}

	if (SettingsGlobal().fEnableKeyLoggerDefence) {

		return 0;

	} else {

		return CallNextHookEx(0, nCode, wParam, lParam);
	}

}
namespace {
	HookGlobalHandles* hh = nullptr;
}
TStatus resethook() {
	hh->hHookKeyGlobal = WinApiInt::SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, 0, 0);
	IFW_RET(hh->hHookKeyGlobal.IsValid());

	RETURN_SUCCESS;
}

TStatus HookGlobal(HookGlobalHandles& handles)
{
	LOG_INFO_1(L"HookGlobal...");

	hh = &handles;
	IFS_RET(resethook());

#ifndef _DEBUG
	handles.hHookMouseGlobal = WinApiInt::SetWindowsHookEx(WH_MOUSE_LL, LowLevelMouseProc, 0, 0);
	IFW_RET(handles.hHookMouseGlobal.IsValid());
#endif

	handles.hHookEventGlobal = WinApiInt::SetWinEventHook(
		EVENT_SYSTEM_FOREGROUND,
		EVENT_SYSTEM_FOREGROUND,
		NULL,
		WinEventProc,
		0, 0,
		WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS);
	IFW_LOG(handles.hHookEventGlobal.IsValid());

	RETURN_SUCCESS;
}


TStatus StartMonitor(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPTSTR    lpCmdLine,
	_In_ int       nCmdShow,
	TSWBit bit)
{

	LOG_INFO_1(L"StartMonitor...");

	IFW_LOG(SetPriorityClass(GetCurrentProcess(), ABOVE_NORMAL_PRIORITY_CLASS));

	gdata().curModeBit = bit;

	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	if(bit == SW_BIT_32)
	{
		if (Utils::ProcSingleton(c_mtxHook32))
		{
			LOG_INFO_1(L"hook32 already running. Exit");
			RETURN_SUCCESS;
		}
	}

	CMainWorker mainWorker;
	IFS_RET(mainWorker.Init());
	gdata().mainWorker = &mainWorker;

	IFS_RET(StartCycle(hInstance, nCmdShow));

	RETURN_SUCCESS;
}





