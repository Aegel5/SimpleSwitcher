#include "stdafx.h"

#include "Settings.h"

#include "SwGui.h"

#include <string>        
#include <locale>
#include <algorithm>

SW_NAMESPACE(SwGui)

void LangChanged(HWND hwnd, int id)
{
	int cur = (int)SendDlgItemMessage(hwnd, id, CB_GETCURSEL, (WPARAM)0, (LPARAM)0);
	if (cur != CB_ERR)
	{
		SettingsGlobal().idLang = (SwLang)cur;
		InitializeLang(SettingsGlobal().idLang);
		SettingsGlobal().Save();
		int res = MessageBox(hwnd, GetMessageById(AM_NEED_REST), SW_PROGRAM_NAME_L, MB_ICONINFORMATION | MB_OKCANCEL);
		if (res == IDOK)
		{
			PostQuitMessage(0);
			CAutoHandle hProc32;
			IFS_LOG(SwCreateProcessOur(0, SW_BIT_32, SW_ADMIN_SELF, hProc32));
		}
	}
}

LRESULT CALLBACK DlgProcPageLang(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	gui_tools::Wmsg wmsg(hwnd, msg, wParam, lParam);

	if (CommonDlgPageProcess(wmsg))
		return TRUE;

	if (msg == WM_INITDIALOG)
	{
		SendDlgItemMessage(hwnd, IDC_COMBO_LANG, CB_ADDSTRING, 0, (LPARAM)L"Russian");
		SendDlgItemMessage(hwnd, IDC_COMBO_LANG, CB_ADDSTRING, 0, (LPARAM)L"English");

		SendDlgItemMessage(hwnd, IDC_COMBO_LANG, CB_SETCURSEL, (WPARAM)SettingsGlobal().idLang, (LPARAM)0);

		SetDlgItemText(hwnd, IDC_CHECK_DBG_MODE, GetMessageById(AM_DBG_MODE));
		SetDlgItemText(hwnd, IDC_STATIC_PATH, GetMessageById(AM_DBG_PATH_TO_PRG));

		gui_tools::SetCheckBox(hwnd, IDC_CHECK_DBG_MODE, SettingsGlobal().fDbgMode);
		gui_tools::SetCheckBox(hwnd, IDC_CHECK_ESC_CLOSE, SettingsGlobal().fCloseByEsc); 
		gui_tools::SetCheckBox(hwnd, IDC_CHECK_KEY_LOG_DEF, SettingsGlobal().fEnableKeyLoggerDefence);

		return FALSE;
	}
	else if (msg == WM_CTLCOLORSTATIC)
	{
		int id = GetDlgCtrlID((HWND)lParam);
		if (id == IDC_EDIT_PATH)
			return HandleCtlColor(hwnd, lParam, wParam);
	}
	else if (msg == WM_COMMAND)
	{
		WORD dlgId = LOWORD(wParam);

		if (HIWORD(wParam) == CBN_SELCHANGE)
		{
			int id = LOWORD(wParam);
			LangChanged(hwnd, id);
		}
		else if (dlgId == IDC_CHECK_DBG_MODE)
		{
			SettingsGlobal().fDbgMode = gui_tools::IsCheckBox(hwnd, dlgId);
			SettingsGlobal().SaveAndPostMsg();
			SettingsGlobal().SetLogLevelBySettings();
		}
		else if (dlgId == IDC_CHECK_ESC_CLOSE)
		{
			SettingsGlobal().fCloseByEsc = gui_tools::IsCheckBox(hwnd, dlgId);
			SettingsGlobal().SaveAndPostMsg();
		}
		else if (dlgId == IDC_CHECK_KEY_LOG_DEF)
		{
			SettingsGlobal().fEnableKeyLoggerDefence = gui_tools::IsCheckBox(hwnd, dlgId);
			SettingsGlobal().SaveAndPostMsg();
		}
		//else if (LOWORD(wParam) == IDC_CHECK_USE_DLL_HOOK)
		//{
		//	SettingsGlobal().fHookDll = (SendDlgItemMessage(hwnd, IDC_CHECK_USE_DLL_HOOK, BM_GETCHECK, 0, 0) == BST_CHECKED);
		//	SettingsGlobal().SaveAndPostMsg();
		//}

	}
	return FALSE;
}

SW_NAMESPACE_END