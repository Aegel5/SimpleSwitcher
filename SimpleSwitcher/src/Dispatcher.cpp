// SimpleLayout.cpp: определяет точку входа для приложения.
//

#include "stdafx.h"
#include "Dispatcher.h"
#include "CMainWorker.h"
#include "Settings.h"
#include "InjectSkipper.h"

#include "gui/decent_gui.h"

#include "loader_api.h"
#include "lay_notif_from_dll.h"


TStatus resethook();

struct HookGlobalHandles
{
	CAutoHHOOK hHookKeyGlobal;
	CAutoHHOOK hHookKeyGlobal_2;
	CAutoHHOOK hHookMouseGlobal;
	CAutoHWINEVENTHOOK hHookEventGlobal;
	CAutoHWINEVENTHOOK hHookEventGlobalSwitchDesk;
};

TStatus HookGlobal(HookGlobalHandles& handles);

TStatus StartCycle(_In_ HINSTANCE hInstance)
{
	LOG_INFO_1(L"StartCycle...");

	IFW_LOG(SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_ABOVE_NORMAL));

	WNDCLASSEX wcex {};

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.lpfnWndProc = DefWindowProc;
	wcex.hInstance = hInstance;
    wcex.lpszClassName = c_sClassNameServer2;

	IFW_LOG(RegisterClassEx(&wcex) != 0);

	HWND hWnd = CreateWindow(
		c_sClassNameServer2,
		L"Title",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

	IFW_RET(hWnd != NULL);

	gdata().hWndMonitor = hWnd;

	if (gdata().curModeBit == SW_BIT_32) {
        IFW_LOG(AddClipboardFormatListener(hWnd));
    }

	IFW_LOG(ChangeWindowMessageFilterEx(hWnd, WM_LayNotif, MSGFLT_ALLOW, 0));

	//if (WinApiInt::ChangeWindowMessageFilterEx)
	//{
	//	IFW_LOG(WinApiInt::ChangeWindowMessageFilterEx(hWnd, c_MSG_Quit, MSGFLT_ALLOW, 0));
	//	IFW_LOG(WinApiInt::ChangeWindowMessageFilterEx(hWnd, c_MSG_SettingsChanges, MSGFLT_ALLOW, 0));
	//}
    //CAutoProcMonitor loader;
    //CAutoProcMonitor loader64;
    g_laynotif.inited = false;

	HookGlobalHandles hookHandles;
	if (gdata().curModeBit == SW_BIT_32)
	{
		IFS_RET(HookGlobal(hookHandles));
	}

	auto timeId = SetTimer(hWnd, c_timerKeyloggerDefence, 5000, NULL);
	IFW_LOG(timeId != 0);

	timeId = SetTimer(hWnd, c_timerGetcurlay, 100, NULL);
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

		//auto curl = GetKeyboardLayout(0);
  //      LOG_INFO_1(L"loop new layout: 0x%x", curl);

		if (mesg == WM_HOTKEY)
		{
		}
		else if (mesg == c_MSG_Quit)
		{
			PostQuitMessage(0);
        } else if (mesg == WM_LayNotif) {

			HKL newLay = (HKL)msg.lParam;

			if (newLay != g_laynotif.g_curLay) {
                g_laynotif.g_curLay = (HKL)msg.lParam;
                LOG_INFO_1(L"notify layout now: 0x%x", g_laynotif.g_curLay.load());

                if (g_guiHandle != nullptr) {
                    PostMessage(g_guiHandle, msg.message, msg.wParam, msg.lParam);
                }
            }
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
                if (conf_get()->EnableKeyLoggerDefence && g_hotkeyWndOpened == 0) {
                    IFS_LOG(resethook()); // ???
                }
            } else if (timerId == c_timerGetcurlay) {
                Worker()->PostMsgW(HWORKER_WM_TIMER, timerId);
                
			} else {
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

//LRESULT CALLBACK KeyboardProc(
//	_In_ int    nCode,
//	_In_ WPARAM wParam,
//	_In_ LPARAM lParam
//)
//{
//	if (nCode < 0) {
//		return CallNextHookEx(0, nCode, wParam, lParam);
//	}
//
//	DWORD key = DWORD(wParam);
//
//	return 0;
//}

namespace {
	CurStateWrapper curKey;
	TKeyCode disable_up = 0;
	CHotKey possible_hk_up;
	bool was_hot_key_down = false;
};

bool CheckInDisabled() {

	HWND hwndFocused = NULL;
	IFS_LOG(Utils::GetFocusWindow(hwndFocused));
	if (hwndFocused == NULL) return false;

	DWORD dwTopPid = 0;
	DWORD dwIdThreadTopWnd = GetWindowThreadProcessId(hwndFocused, &dwTopPid);
	IFW_LOG(dwIdThreadTopWnd != 0);
	if (dwTopPid == 0) return false;

	std::wstring path;
	std::wstring name;
	IFS_LOG(Utils::GetProcLowerNameByPid(dwTopPid, path, name));

	return conf_get()->IsSkipProgram(name);
}

LRESULT CALLBACK LowLevelKeyboardProc(
	_In_  int nCode,
	_In_  WPARAM wParam,
	_In_  LPARAM lParam
	)
{
	if (nCode < 0) {
		return CallNextHookEx(0, nCode, wParam, lParam);
	}

	MainWorkerMsg msg_type(HWORKER_KeyMsg);
	MainWorkerMsg msg_hotkey(HWORKER_OurHotKey);
	msg_hotkey.data.hk = hk_NULL;
	bool send_key = false;


	auto process = [&]() -> bool
	{
		KBDLLHOOKSTRUCT* k = (KBDLLHOOKSTRUCT*)lParam;

		TKeyCode vkCode = (TKeyCode)k->vkCode;
		KeyState curKeyState = GetKeyState(wParam);
		bool isInjected = TestFlag(k->flags, LLKHF_INJECTED);
		bool isAltDown = TestFlag(k->flags, LLKHF_ALTDOWN);
		bool isSysKey = wParam == WM_SYSKEYDOWN || wParam == WM_SYSKEYUP;
		bool isExtended = TestFlag(k->flags, LLKHF_EXTENDED);
		auto scan_code = k->scanCode;

		LOG_ANY(
			L"KEY_MSG: {}({}),scan={},inject={},altdown={},syskey={},extended={}", 
			HotKeyNames::Global().GetName(vkCode),
			(curKeyState == KEY_STATE_UP ? L"UP": L"DOWN"),
			scan_code,
			isInjected,
			isAltDown,
			isSysKey,
			isExtended
		);

		if (k->vkCode > 255)
		{
			LOG_INFO_1(L"k->vkCode > 255: %d", k->vkCode);
		}

		if (scan_code == 541) {
			LOG_INFO_2(L"skip bugged lctrl");
			return 0;
		}

		if (isInjected) {
			if (!InjectSkipper::Inst().IsAllowInject()) 
				return 0;
		}

		CHotKey possible;
		std::swap(possible, possible_hk_up); // сразу очищаем

		msg_type.data.keyData.ks = *k;
		msg_type.data.keyData.wParam = wParam;
		send_key = true; // обрабатываем нажатие, если не будет запрета

		curKey.Update(k, curKeyState); // сразу обновляем
		const auto& curk = curKey.state;

		auto check_is_our_key = [&msg_hotkey](const CHotKey& k1, const CHotKey& k2) {
			if (k1.Compare(k2, CHotKey::COMPARE_IGNORE_KEYUP)) {
				if (!CheckInDisabled()) {
					return true;
				}
			}
			return false;
			};

		if (curKeyState == KeyState::KEY_STATE_DOWN) {
			if (!curk.IsEmpty()) {
				auto conf = conf_get();
				bool need_our_action = false;
				for (const auto& [hk, key] : conf->All_hot_keys()) {
					if (!key.GetKeyup() && check_is_our_key(key, curk)) {
						need_our_action = true;
						possible_hk_up.Clear();
						if (!was_hot_key_down) { // запрещаем срабатывать на удержание наших хоткеев
							msg_hotkey.data.hotkey = key;
							msg_hotkey.data.hk = hk;
							was_hot_key_down = true;
						}
						break;
					}
					if (key.GetKeyup() && check_is_our_key(key, curk)) {
						possible_hk_up = curk; // без break
					}
				}
				if (need_our_action) {
					// у нас есть такой хот-кей, запрещаем это событие для программы.
					disable_up = curk.ValueKey(); // up тоже будет в будущем запрещать.
					LOG_INFO_1(L"Key %s was disabled(down)", CHotKey::GetName(disable_up));
					return 1;
				}
				else {
					if (curk.Size() == 3 
						&& curk.HasKey(VK_LMENU, true)
						&& curk.HasKey(VK_CONTROL, false)
						&& !curk.IsKnownMods(vkCode)
						&& vkCode == curk.ValueKey()
						&& conf_get()->fixRAlt
						&& conf_get()->fixRAlt_lay_ != 0
						) {
						LOG_INFO_1(L"fix ctrl+alt");
						MainWorkerMsg msg(HWORKER_FixCtrlAlt);
						msg.data.hotkey_to_fix = curk;
						Worker()->PostMsg(msg);
						LOG_INFO_1(L"Key %s was disabled(fix)", CHotKey::GetName(vkCode));
						disable_up = vkCode;
						return 1; // пока запрещаем, потом заново отошлем...
					}
				}
			}
		}
		else if (curKeyState == KeyState::KEY_STATE_UP) {
			was_hot_key_down = false; // разрешаем наши хоткеи снова, только если был up
			if (disable_up == vkCode) {
				LOG_INFO_1(L"Key %s was disabled(up)", CHotKey::GetName(disable_up));
				disable_up = 0;
				return 1;
			}
			else {
				// ищем наш хот-кей.
				// даже если нашли, up никогда не запрещаем.
				if (!possible.IsEmpty()) {
					for (const auto& [hk, key] : conf_get()->All_hot_keys()) {
						if (key.GetKeyup() && check_is_our_key(key, possible)) {
							msg_hotkey.data.hotkey = key;
							msg_hotkey.data.hk = hk;
							break; 
						}
					}
				}
			}
		}


		if (curk.IsEmpty()) {
			disable_up = 0;
		}

		return 0;
	};

	if (nCode == HC_ACTION) {
		auto res = process();
		if (
			!res && send_key) { 
			Worker()->PostMsg(msg_type);
		}
		if (msg_hotkey.data.hk != hk_NULL) {
			Worker()->PostMsg(msg_hotkey);
		}
		if (res) 
			return 1; // запрет
	}


	if (conf_get()->EnableKeyLoggerDefence) {
		return 0;
	} else {
		return CallNextHookEx(0, nCode, wParam, lParam);
	}

}
namespace {
	HookGlobalHandles* hh = nullptr;
}
TStatus resethook() {
	hh->hHookKeyGlobal = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, 0, 0);

	//hh->hHookKeyGlobal_2.Cleanup();
	//if (g_setsgui.fEnableKeyLoggerDefence) {
	//	DWORD dwTheardId = ::GetWindowThreadProcessId(gdata().hWndMonitor, 0);
	//	hh->hHookKeyGlobal_2 = WinApiInt::SetWindowsHookEx(WH_KEYBOARD, KeyboardProc, GetModuleHandle(NULL), dwTheardId);
	//	IFW_LOG(hh->hHookKeyGlobal_2.IsValid());
	//}

	IFW_RET(hh->hHookKeyGlobal.IsValid());

	RETURN_SUCCESS;
}

TStatus HookGlobal(HookGlobalHandles& handles)
{
	LOG_INFO_1(L"HookGlobal...");

	hh = &handles;
	IFS_RET(resethook());

#ifndef _DEBUG
	handles.hHookMouseGlobal = SetWindowsHookEx(WH_MOUSE_LL, LowLevelMouseProc, 0, 0);
	IFW_RET(handles.hHookMouseGlobal.IsValid());
#endif

	handles.hHookEventGlobal = SetWinEventHook(
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
	TSWBit bit)
{

	LOG_INFO_1(L"StartMonitor...");

	IFW_LOG(SetPriorityClass(GetCurrentProcess(), ABOVE_NORMAL_PRIORITY_CLASS));

	gdata().curModeBit = bit;

	//if(bit == SW_BIT_32)
	//{
	//	if (Utils::ProcSingleton(c_mtxHook32))
	//	{
	//		LOG_INFO_1(L"hook32 already running. Exit");
	//		RETURN_SUCCESS;
	//	}
	//}

	IFS_RET(Worker()->ReStart()); // полностью обнулим рабочий поток

	IFS_RET(StartCycle(hInstance));

	RETURN_SUCCESS;
}


//struct CoreHolders {
//
//	std::unique_ptr<HookGlobalHandles> hooks;
//
//	TStatus Init() {
//		hooks.reset(new HookGlobalHandles());
//		IFS_RET(HookGlobal(*hooks));
//		RETURN_SUCCESS;
//	}
//	TStatus Reset() {
//		IFS_RET(resethook(hooks.get()));
//		RETURN_SUCCESS;
//	}
//
//};






