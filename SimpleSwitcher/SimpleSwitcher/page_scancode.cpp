#include "stdafx.h"

#include "SwGui.h"

namespace {
	struct Data {
		CAutoHHOOK hHookKeyGlobal;
		HWND th;
		DWORD last;
	};
	static Data data;

	LRESULT CALLBACK LowLevelKeyboardProc(
		_In_  int nCode,
		_In_  WPARAM wParam,
		_In_  LPARAM lParam
	)
	{
		if (nCode == HC_ACTION)
		{
			KBDLLHOOKSTRUCT* kStruct = (KBDLLHOOKSTRUCT*)lParam;
			DWORD vkKey = kStruct->vkCode;
			DWORD scKey = kStruct->scanCode;

			PostMessage(data.th, c_MSG_TypeHotKey, (WPARAM)scKey, (WPARAM)vkKey);

		}
		return CallNextHookEx(0, nCode, wParam, lParam);
	}
	std::wstring caption(DWORD sc, DWORD vc) {
		//std::wstring res;
		TChar buf[1024];
		buf[0] = 0;

		swprintf_s(
			buf,
			L"sc: %d, vc: %d, '%s'", sc, vc, HotKeyNames::Global().GetName(vc));

		return buf;
	}

	TStatus UpdateList(HWND hwnd) {
		SendDlgItemMessage(hwnd, IDC_LIST1, LB_RESETCONTENT, 0, 0);

		BufScanMap buf;
		buf.FromRegistry();
		for (auto iter = buf.GetIter(); !iter.IsEnd(); ++iter)
		{
			auto [sc, vc] = iter.curElemSrc();
			auto [sc2, vc2] = iter.curElemDst();
			TChar buf[1024];
			buf[0] = 0;
			swprintf_s(
				buf,
				L"%s - > %s", caption(sc, vc).c_str(), caption(sc2, vc2).c_str());
			SendDlgItemMessage(hwnd, IDC_LIST1, LB_ADDSTRING, 0, (WPARAM)buf);
		}

		RETURN_SUCCESS;
	}
}

namespace SwGui {


TStatus InitDlg_Scancode(HWND hwnd)
{
	IFS_LOG(UpdateList(hwnd));
	

	RETURN_SUCCESS;
}
LRESULT CALLBACK DlgProc_Scancode(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	gui_tools::Wmsg wmsg(hwnd, msg, wParam, lParam);

	if (gui_tools::HandleInit(wmsg, InitDlg_Scancode))
		return TRUE;
	if (CommonDlgPageProcess(wmsg))
		return TRUE;


	if (msg == WM_COMMAND)
	{
		if (LOWORD(wParam) == IDC_EDIT_BREAK10) {
			if (HIWORD(wParam) == EN_SETFOCUS) {
				if (data.hHookKeyGlobal.IsInvalid()) {
					data.last = LOWORD(wParam);
					data.th = hwnd;
					data.hHookKeyGlobal = WinApiInt::SetWindowsHookEx(WH_KEYBOARD_LL, &LowLevelKeyboardProc, 0, 0);
					IFW_LOG(data.hHookKeyGlobal.IsValid());
				}
			}
			else if (HIWORD(wParam) == EN_KILLFOCUS) {
				data.hHookKeyGlobal.Cleanup();
			}
		}
		else if (LOWORD(wmsg.wparm) == IDC_BUTTON_SET8 || LOWORD(wmsg.wparm) == IDC_BUTTON_SET9) {
			if (!Utils::IsSelfElevated()) {
				MessageBox(
					hwnd,
					L"Need admin rights",
					SW_PROGRAM_NAME_L,
					MB_ICONINFORMATION | MB_OK);
				return TRUE;
			}
			if (LOWORD(wmsg.wparm) == IDC_BUTTON_SET8) {
				auto getvc = [hwnd](DWORD id) {
					TChar buf[300];
					buf[0] = 0;
					GetDlgItemText(hwnd, id, buf, 300);
					return StrToInt(buf);
				};
				BufScanMap map;
				auto v1 = getvc(IDC_EDIT_BREAK8);
				auto v2 = getvc(IDC_EDIT_BREAK9);
				map.FromRegistry();
				map.PutRemapKey_BySc(v1, v2);
				map.ToRegistry();
				IFS_LOG(UpdateList(hwnd));
			}
			else if (LOWORD(wmsg.wparm) == IDC_BUTTON_SET9) {

				auto hlist = GetDlgItem(hwnd, IDC_LIST1);
				LRESULT res = SendMessage(hlist, LB_GETCURSEL, 0, 0);
				size_t index = (size_t)res;
				if (res != LB_ERR) {
					BufScanMap map;
					map.FromRegistry();
					map.DelRemapKey_ByIndex(res);
					map.ToRegistry();
					IFS_LOG(UpdateList(hwnd));
				}
			}
		}
		else if (LOWORD(wmsg.wparm) == IDC_BUTTON_SET10) {
			IFS_LOG(UpdateList(hwnd));
		}

	}
	else if (msg == c_MSG_TypeHotKey) {
		DWORD sc = (DWORD)wParam;
		DWORD vc = (DWORD)lParam;
		SetDlgItemText(hwnd, data.last, caption(sc, vc).c_str());

		return TRUE;
	}

	return FALSE;
}



}