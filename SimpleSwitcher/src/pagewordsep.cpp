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
		gui_tools::SetCheckBox(hwnd, IDC_CHECK_OEM2_R, setsgui.isTryOEM2);
		//SetCheckBox(hwnd, IDC_CHECK_DASH_R, setsgui.isDashSeparate);

		return FALSE;
	}
	else if (msg == WM_COMMAND)
	{
		WORD dlgId = LOWORD(wParam);
		//if (dlgId == IDC_CHECK_DASH_R)
		//{
			//setsgui.isDashSeparate = IsCheckBox(hwnd, dlgId);
			//setsgui.SaveAndPostMsg();
		//}
		if (dlgId == IDC_CHECK_OEM2_R)
		{
			setsgui.isTryOEM2 = gui_tools::IsCheckBox(hwnd, dlgId);
			setsgui.SaveAndPostMsg();
		}
	}
	return FALSE;
}

SW_NAMESPACE_END