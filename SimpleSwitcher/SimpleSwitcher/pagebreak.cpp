#include "stdafx.h"

#include <shellapi.h>

#include "Settings.h"
#include "SwAutostart.h"

#include "SwGui.h"

SW_NAMESPACE(SwGui)


TStatus InitDialogPageBreak(HWND hwnd)
{
	KeyToDlg(hk_RevertLastWord, IDC_EDIT_BREAK, hwnd);
	KeyToDlg(hk_RevertCycle, IDC_EDIT_BREAK2, hwnd);
	KeyToDlg(hk_RevertSel, IDC_EDIT_BREAK3, hwnd);
	//KeyToDlg(hk_ChangeCase, IDC_EDIT_BREAK_CASE, hwnd);

	RETURN_SUCCESS;
}

BOOL CALLBACK HandleWMCommandPageBreak(WPARAM wParam, HWND hwnd)
{
	switch (LOWORD(wParam))
	{
	case IDC_BUTTON_SET:
		ChangeHotKey(hk_RevertLastWord, IDC_EDIT_BREAK, hwnd);
		return TRUE;
	case IDC_BUTTON_SET2:
		ChangeHotKey(hk_RevertCycle, IDC_EDIT_BREAK2, hwnd);
		return TRUE;
	case IDC_BUTTON_SET3:
		ChangeHotKey(hk_RevertSel, IDC_EDIT_BREAK3, hwnd);
		return TRUE;
	//case IDC_BUTTON_SET_CASE:
	//	ChangeHotKey(hk_ChangeCase, IDC_EDIT_BREAK_CASE, hwnd);
	//	return TRUE;

	default:
		return FALSE;
	}
}

SW_NAMESPACE_END