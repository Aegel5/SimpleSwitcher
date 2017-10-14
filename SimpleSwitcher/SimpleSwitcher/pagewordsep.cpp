#include "stdafx.h"

#include "Settings.h"

#include "SwGui.h"


SW_NAMESPACE(SwGui)


LRESULT CALLBACK DlgProcPageWordSep(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	gui_tools::Wmsg wmsg(hwnd, msg, wParam, lParam);

	if (CommonDlgPageProcess(wmsg))
	{
		return TRUE;
	}

	if (msg == WM_INITDIALOG)
	{
		gui_tools::SetCheckBox(hwnd, IDC_CHECK_OEM2_R, SettingsGlobal().isTryOEM2);
		//SetCheckBox(hwnd, IDC_CHECK_DASH_R, SettingsGlobal().isDashSeparate);

		return FALSE;
	}
	else if (msg == WM_COMMAND)
	{
		WORD dlgId = LOWORD(wParam);
		//if (dlgId == IDC_CHECK_DASH_R)
		//{
			//SettingsGlobal().isDashSeparate = IsCheckBox(hwnd, dlgId);
			//SettingsGlobal().SaveAndPostMsg();
		//}
		if (dlgId == IDC_CHECK_OEM2_R)
		{
			SettingsGlobal().isTryOEM2 = gui_tools::IsCheckBox(hwnd, dlgId);
			SettingsGlobal().SaveAndPostMsg();
		}
	}
	return FALSE;
}

SW_NAMESPACE_END