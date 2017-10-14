#include "stdafx.h"

#include "SwGui.h"

namespace SwGui {

TStatus InitDlgAbout(HWND hwnd)
{
	std::wstring sFilePath;
	IFS_LOG(GetPath(sFilePath, PATH_TYPE_EXE_PATH, GetSelfBit()));
	//SetDlgItemText(hwnd, IDC_EDIT_PATH, sFilePath.c_str());

	TChar buf[1024];
	buf[0] = 0;

	swprintf_s(
		buf,

		L"%s %s\r\n"
		L"Build date: %s\r\n\r\n"

		"%s\r\n"
		"%s\r\n\r\n"

		"%s\r\n%s",

		SW_PROGRAM_NAME_L,
		c_sVersion,

		SW_UT(__DATE__),

		c_sSiteLink,
		L"http://simpleswitcher.ru",

		L"Путь до программы:",
		sFilePath.c_str()
	);

	IFW_LOG(SetDlgItemText(hwnd, IDC_EDIT_VERS, buf));



	RETURN_SUCCESS;
}
LRESULT CALLBACK DlgProcAbout(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	gui_tools::Wmsg wmsg(hwnd, msg, wParam, lParam);

	if (gui_tools::HandleInit(wmsg, InitDlgAbout))
		return TRUE;
	if (CommonDlgPageProcess(wmsg))
		return TRUE;

	//if (msg == WM_CTLCOLORSTATIC)
	//{
	//	int id = GetDlgCtrlID((HWND)lParam);
	//	if (id == IDC_EDIT_PATH)
	//		return HandleCtlColor(hwnd, lParam, wParam);
	//}

	return FALSE;
}



}