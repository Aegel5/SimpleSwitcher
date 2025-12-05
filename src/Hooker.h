#pragma once


class Hooker {

	CAutoHHOOK hHookKeyGlobal;
	CAutoHHOOK hHookMouseGlobal;
	CAutoHWINEVENTHOOK hHookEventGlobal;
	CAutoHWINEVENTHOOK hHookEventGlobalSwitchDesk;

public:
	void ClearAllKeys() {
		hookerKeyb.curKeys.Clear();
		Worker()->PostMsg(Message_ClearWorlds{});
	}

public: class HookerKeyboard {
		public: CurStateWrapper curKeys;
		TKeyCode disable_up = 0;
		CHotKey possible_hk_up;
		public: TimePoint last_mouse_click_time;

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

	//static void CALLBACK WinEventProc_SwitchDesk(
	//	HWINEVENTHOOK hWinEventHook,
	//	DWORD event,
	//	HWND hwnd,
	//	LONG idObject,
	//	LONG idChild,
	//	DWORD dwEventThread,
	//	DWORD dwmsEventTime
	//) {
	//	LOG_ANY("WinEventProc_SwitchDesk");
	//	Worker()->PostMsg(Message_ClearWorlds{});
	//}


	static void CALLBACK WinEventProc(HWINEVENTHOOK hWinEventHook, DWORD event, HWND hwnd, LONG idObject, LONG idChild, DWORD dwEventThread, DWORD dwmsEventTime) {
		Worker()->PostMsg(Message_ChangeForeg{ hwnd });
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
				hookerKeyb.last_mouse_click_time.SetNow();
				Worker()->PostMsg(Message_ClearWorlds{});
			}
		}

		return CallNextHookEx(NULL, nCode, wParam, lParam);
	}

public:

	TStatus StartHook() {

		LOG_ANY(L"HookGlobal...");

		hookerKeyb = {}; // очистка.

		hHookKeyGlobal = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, 0, 0);
		IFW_RET(hHookKeyGlobal.IsValid());

		if (!Utils::IsDebug()) {
			hHookMouseGlobal = SetWindowsHookEx(WH_MOUSE_LL, LowLevelMouseProc, 0, 0);
			IFW_RET(hHookMouseGlobal.IsValid());
		}

		hHookEventGlobal = SetWinEventHook(
			EVENT_SYSTEM_FOREGROUND,
			EVENT_SYSTEM_FOREGROUND,
			NULL,
			WinEventProc,
			0, 0,
			WINEVENT_OUTOFCONTEXT 
			//| WINEVENT_SKIPOWNPROCESS
		);
		IFW_LOG(hHookEventGlobal.IsValid());

		RETURN_SUCCESS;
	}
};
