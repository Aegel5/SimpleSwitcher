#include "stdafx.h"

#include "Settings.h"

#include "SwGui.h"

#include <string>        
#include <locale>
#include <algorithm>

SW_NAMESPACE(SwGui)



LRESULT CALLBACK DlgProcPageClip(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	gui_tools::Wmsg wmsg(hwnd, msg, wParam, lParam);

	if (CommonDlgPageProcess(wmsg))
		return TRUE;

	if (msg == WM_INITDIALOG)
	{
		SetDlgItemText(hwnd, IDC_CHECK_CLIPCLEAR, GetMessageById(AM_CHECK_CLIPCLEAR_NAME));
		gui_tools::SetCheckBox(hwnd, IDC_CHECK_CLIPCLEAR, SettingsGlobal().fClipboardClearFormat);

		KeyToDlg(hk_EmulCopyNoFormat, IDC_EDIT_BREAK, hwnd);
		KeyToDlg(hk_EmulCopyWithFormat, IDC_EDIT_BREAK2, hwnd);

		return FALSE;
	}
	else if (msg == WM_CTLCOLORSTATIC)
	{
		int id = GetDlgCtrlID((HWND)lParam);
		if (id == IDC_EDIT_BREAK || id == IDC_EDIT_BREAK2)
			return HandleCtlColor(hwnd, lParam, wParam);
		return TRUE;
	}
	else if (msg == WM_COMMAND)
	{
		WORD dlgId = LOWORD(wParam);
		if (dlgId == IDC_CHECK_CLIPCLEAR)
		{
			SettingsGlobal().fClipboardClearFormat = gui_tools::IsCheckBox(hwnd, IDC_CHECK_CLIPCLEAR);
			SettingsGlobal().SaveAndPostMsg();
			return TRUE;
		}
		else if (dlgId == IDC_BUTTON_SET)
		{
			ChangeHotKey(hk_EmulCopyNoFormat, IDC_EDIT_BREAK, hwnd, false);
			return TRUE;
		}
		else if (dlgId == IDC_BUTTON_SET2)
		{
			ChangeHotKey(hk_EmulCopyWithFormat, IDC_EDIT_BREAK2, hwnd, false);
			return TRUE;
		}
	}
	return FALSE;
}

SW_NAMESPACE_END