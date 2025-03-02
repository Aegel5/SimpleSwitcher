// SimpleLayout.cpp: определяет точку входа для приложения.
//

#include "stdafx.h"
#include "Dispatcher.h"
#include "CMainWorker.h"
#include "Settings.h"

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
    CAutoProcMonitor loader;
    CAutoProcMonitor loader64;
    g_laynotif.inited = false;

	//if (g_setsgui.injectDll) {
 //       tstring sFolder;
 //       IFS_RET(GetPath_folder_noLower(sFolder));

 //       bool locInited    = true;

 //       loader.m_sWndName = c_sClassName32_2;
 //       loader.m_sCmd     = L"/load";
 //       loader.m_sExe     = sFolder + L"loader.exe";
 //       IFS_LOG(loader.EnsureStarted(SW_ADMIN_SELF));
 //       if (!loader.CheckRunning().found)
 //           locInited = false;


 //       if (IsWindows64()) {
 //           loader64.m_sWndName = c_sClassName64_2;
 //           loader64.m_sCmd     = L"/load";
 //           loader64.m_sExe     = sFolder + L"loader64.exe";
 //           IFS_LOG(loader64.EnsureStarted(SW_ADMIN_SELF));
 //           if (!loader64.CheckRunning().found)
 //               locInited = false;
 //       }

	//	g_laynotif.inited = locInited;
 //   }


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
		else if (mesg == c_MSG_SettingsChanges)
		{
			if (gdata().curModeBit == SW_BIT_32)
			{
				Worker()->PostMsg(HWORKER_LoadSettings);
			}
        } 
		else if (mesg == WM_GetCurLay) {
            Worker()->PostMsg(HWORKER_Getcurlay);
        } 
		//else if (mesg == WM_INPUTLANGCHANGE) {
  //          HKL newLayout = (HKL)msg.lParam;
  //          LOG_INFO_1(L"loop new layout: 0x%x", newLayout);
  //      }
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
                if (conf_get()->fEnableKeyLoggerDefence && g_hotkeyWndOpened == 0) {
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

LRESULT CALLBACK KeyboardProc(
	_In_ int    nCode,
	_In_ WPARAM wParam,
	_In_ LPARAM lParam
)
{
	if (nCode < 0) {
		return CallNextHookEx(0, nCode, wParam, lParam);
	}

	DWORD key = DWORD(wParam);
	LOG_INFO_0(L"_PRINTED_ %d", key);

	//if (g_setsgui.fEnableKeyLoggerDefence) {
		return 0;
	//} else {
	//	return CallNextHookEx(0, nCode, wParam, lParam);
	//}
}

namespace {
	CurStateWrapper curKey;
	TKeyCode disable_up = 0;
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

	if (nCode == HC_ACTION)
	{
		KBDLLHOOKSTRUCT* kStruct = (KBDLLHOOKSTRUCT*)lParam;

		MainWorkerMsg msg;
		msg.mode = HWORKER_KeyMsg;
		auto& kData = msg.data.keyData;
		kData.ks = *kStruct;
		kData.wParam = wParam;
		Worker()->PostMsg(msg);

		TKeyCode vkCode = (TKeyCode)kStruct->vkCode;
		KeyState curKeyState = GetKeyState(wParam);
		bool isInjected = TestFlag(kStruct->flags, LLKHF_INJECTED);
		if (!isInjected) {
			// Consume HOT KEY (не отдаем хот-кей для текущей программы)
			curKey.Update(kStruct, curKeyState);
			if (curKeyState == KeyState::KEY_STATE_DOWN) {
				bool skip = curKey.state.IsEmpty();
				//if (curKey.state.HasAllMod()) {
				//	// Скипаем когда только known mod в хоткее
				//	skip = true;
				//}
				if (!skip) {
					auto conf = conf_get();
					for (const auto& it : conf->hotkeysList) {
						if (it.keys.HasKey_skipkeyup(curKey.state, CHotKey::TCompareFlags(CHotKey::COMPARE_IGNORE_HOLD | CHotKey::COMPARE_IGNORE_KEYUP))) {
							// К сожалению, вынуждены дублировать эти проверки, но ничего не поделать, нам нужен второй поток.
							if (!CheckInDisabled()) {
								// у нас есть такой хот-кей, запрещаем это событие для программы.
								disable_up = curKey.state.ValueKey(); // up тоже будет в будущем запрещать.
								LOG_INFO_1(L"Key %s was disabled(down)", CHotKey::GetName(disable_up));
								return 1;
							}
						}
					}
				}
			}
			else if (curKeyState == KeyState::KEY_STATE_UP) {
				if (disable_up == vkCode) {
					LOG_INFO_1(L"Key %s was disabled(up)", CHotKey::GetName(disable_up));
					disable_up = 0;
					return 1;
				}
			}

			if (curKey.state.IsEmpty()) {
				disable_up = 0;
			}
		}

		//if (kStruct->vkCode == VK_LMENU) {
		//	LOG_INFO_1(L"DISABLED");
		//	INPUT cur = { 0 };
		//	cur.type = INPUT_KEYBOARD;
		//	cur.ki.wVk = VK_RMENU;
		//	if (curKeyState != KEY_STATE_DOWN) {
		//		cur.ki.dwFlags = KEYEVENTF_KEYUP;
		//	}
		//	SendInput((UINT)1, &cur, sizeof(INPUT));
		//	return 1;
		//}
	}

	if (conf_get()->fEnableKeyLoggerDefence) {
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

	CMainWorker mainWorker;
	IFS_RET(mainWorker.Init());
	gdata().mainWorker = &mainWorker;

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






