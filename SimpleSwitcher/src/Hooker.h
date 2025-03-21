#pragma once


class Hooker {

	CAutoHHOOK hHookKeyGlobal;
	CAutoHHOOK hHookMouseGlobal;
	CAutoHWINEVENTHOOK hHookEventGlobal;
	CAutoHWINEVENTHOOK hHookEventGlobalSwitchDesk;

public: class HookerKeyboard {
		CurStateWrapper curKey;
		TKeyCode disable_up = 0;
		CHotKey possible_hk_up;
		TKeyCode vk_last_down = 0;
	public:
		LRESULT CALLBACK LowLevelKeyboardProc(
			_In_  int nCode,
			_In_  WPARAM wParam,
			_In_  LPARAM lParam
		);
	};

private: inline static HookerKeyboard hookerKeyb;

	static LRESULT CALLBACK LowLevelKeyboardProc(
		_In_  int nCode,
		_In_  WPARAM wParam,
		_In_  LPARAM lParam
	) {
		return hookerKeyb.LowLevelKeyboardProc(nCode, wParam, lParam);
	}

	static void CALLBACK WinEventProc_SwitchDesk(
		HWINEVENTHOOK hWinEventHook,
		DWORD event,
		HWND hwnd,
		LONG idObject,
		LONG idChild,
		DWORD dwEventThread,
		DWORD dwmsEventTime
	) {
		Worker()->PostMsg(HWORKER_ClearWords);
	}


	static void CALLBACK WinEventProc(HWINEVENTHOOK hWinEventHook, DWORD event, HWND hwnd, LONG idObject, LONG idChild, DWORD dwEventThread, DWORD dwmsEventTime) {
		Worker()->PostMsgW(HWORKER_ChangeForeground, (WPARAM)hwnd);
	}

	static LRESULT CALLBACK LowLevelMouseProc(
		_In_  int nCode,
		_In_  WPARAM wParam,
		_In_  LPARAM lParam
	) {
		if (nCode == HC_ACTION) {
			if (wParam == WM_MOUSEMOVE) {
				// nothing
			}
			else {
				Worker()->PostMsg(HWORKER_ClearWords);
			}
		}

		return CallNextHookEx(NULL, nCode, wParam, lParam);
	}

public:

	TStatus StartHook() {

		LOG_INFO_1(L"HookGlobal...");

		hookerKeyb = {}; // �������.

		hHookKeyGlobal = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, 0, 0);
		IFW_RET(hHookKeyGlobal.IsValid());

#ifndef _DEBUG
		hHookMouseGlobal = SetWindowsHookEx(WH_MOUSE_LL, LowLevelMouseProc, 0, 0);
		IFW_RET(hHookMouseGlobal.IsValid());
#endif

		hHookEventGlobal = SetWinEventHook(
			EVENT_SYSTEM_FOREGROUND,
			EVENT_SYSTEM_FOREGROUND,
			NULL,
			WinEventProc,
			0, 0,
			WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS);
		IFW_LOG(hHookEventGlobal.IsValid());

		RETURN_SUCCESS;
	}
};