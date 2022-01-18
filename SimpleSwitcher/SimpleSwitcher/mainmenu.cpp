#include "stdafx.h"

#include <shellapi.h>
#include "Commctrl.h"

#include "Settings.h"
#include "SwAutostart.h"
//#include "SwShared.h"

#include "SwGui.h"

SW_NAMESPACE(SwGui)

LRESULT CALLBACK DlgProcMainMenu(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

DialogData g_dlgData;


TStatus ExitMainMenu(HWND hwnd = NULL)
{
	LOG_INFO_1(L"Request to exit gui");
	if (SettingsGlobal().IsAddToTray())
	{
		if (SettingsGlobal().isEnabled)
		{
			IFS_LOG(gdata().procMonitor.Stop());
		}
		g_dlgData.trayIcon.SetEnabled(false);
	}

	PostQuitMessage(0);
	RETURN_SUCCESS;
}

TStatus HandleExitGui(HWND hwnd)
{
	LOG_INFO_1(L"Request to close gui");

	if (SettingsGlobal().IsAddToTray())
	{
		ShowWindow(hwnd, SW_HIDE);
	}
	else
	{
		ExitMainMenu();
	}
	RETURN_SUCCESS;
}

void ShowGui()
{
	ShowWindow(g_dlgData.hwndMainMenu, SW_SHOW);
	SetForegroundWindow(g_dlgData.hwndMainMenu);
}
void ShowPopupMenu()
{
	HWND hwnd = g_dlgData.hwndMainMenu;

	CAutoHMENU hPopupMenu = CreatePopupMenu();
	AppendMenu(hPopupMenu, MF_STRING, ID_TRAYITEM_SHOW, GetMessageById(AM_SHOW));
	AppendMenu(hPopupMenu, MF_SEPARATOR, 0, 0);
	AppendMenu(hPopupMenu, MF_CHECKED, ID_TRAYITEM_ENABLE, GetMessageById(AM_ENABLE));
	//AppendMenu(hPopupMenu, MF_STRING, ID_TRAYITEM_ABOUT, GetMessageById(AM_ABOUT));
	AppendMenu(hPopupMenu, MF_SEPARATOR, 0, 0);
	AppendMenu(hPopupMenu, MF_STRING, ID_TRAYITEM_EXIT, GetMessageById(AM_EXIT));

	POINT curPoint;
	GetCursorPos(&curPoint);

	MENUITEMINFO inf = { 0 };
	inf.cbSize = sizeof(inf);
	inf.fState = SettingsGlobal().isEnabled ? MF_CHECKED : MFS_UNCHECKED;
	inf.fMask = MIIM_STATE;
	IFW_LOG(SetMenuItemInfo(hPopupMenu, ID_TRAYITEM_ENABLE, false, &inf));

	SetForegroundWindow(hwnd);

	UINT clicked = TrackPopupMenu(
		hPopupMenu,
		TPM_RETURNCMD | TPM_NONOTIFY,
		curPoint.x,
		curPoint.y,
		0,
		hwnd,
		NULL);
	if (clicked == ID_TRAYITEM_EXIT)
	{
		ExitMainMenu();
	}
	else if (clicked == ID_TRAYITEM_SHOW)
	{
		ShowGui();
	}
	else if (clicked == ID_TRAYITEM_ENABLE)
	{
		PageMainHandleEnable();
	}
	//else if (clicked == ID_TRAYITEM_ABOUT)
	//{
	//	g_dlgData.smTabs.AddTab
	//	//ShowAbout();
	//}
	else
	{
		LOG_INFO_1(L"[WARN] Unknown menu=%u", clicked);
	}
}

TStatus InitDialogMainMenu(HWND hwnd)
{
	IFS_RET(g_dlgData.Init(hwnd));
	g_dlgData.hwndMainMenu = hwnd;

	SetDlgItemText(hwnd, IDC_BUTTON_CLOSE, GetMessageById(AM_CLOSE));

	{
		TChar buf[512];
		buf[0] = 0;
		wcscpy_s(buf, c_sProgramName);
		wcscat_s(buf, L" ");
		wcscat_s(buf, c_sVersion);
		if (Utils::IsSelfElevated())
		{
			wcscat_s(buf, L" - Administrator");
		}
		SetWindowText(hwnd, buf);
	}

	g_dlgData.hIcon = (HICON)LoadImage(
		gdata().hInst,
		MAKEINTRESOURCE(IDI_ICON1),
		IMAGE_ICON,
		GetSystemMetrics(SM_CXICON),
		GetSystemMetrics(SM_CYICON),
		0);
	g_dlgData.hIconBig = (HICON)LoadImage(
		gdata().hInst,
		MAKEINTRESOURCE(IDI_ICON1),
		IMAGE_ICON,
		256,
		256,
		0);

	if (g_dlgData.hIcon)
		SendMessage(hwnd, WM_SETICON, ICON_SMALL, (LPARAM)g_dlgData.hIcon.get());
	if (g_dlgData.hIconBig)
		SendMessage(hwnd, WM_SETICON, ICON_BIG, (LPARAM)g_dlgData.hIconBig.get());

	g_dlgData.smTabs.Init(gdata().hInst, hwnd, IDC_LIST_MAIN_MENU);
	g_dlgData.trayIcon.Init(hwnd, g_dlgData.hIcon, ShowPopupMenu);

	IFS_RET(g_dlgData.smTabs.AddTab(IDD_DIALOG_PAGEMAIN_RU, (DLGPROC)DlgProcPageMain, GetMessageById(AM_COMMON)));
	g_dlgData.hwndPageMain = g_dlgData.smTabs.Back();
	IFS_RET(g_dlgData.smTabs.AddTab(GetDialogById(SD_ADV), (DLGPROC)DlgProcPageAdv, GetMessageById(AM_2)));
	IFS_RET(g_dlgData.smTabs.AddTab(IDD_DIALOG_PAGE_WORDSEP2, (DLGPROC)DlgProcPageWordSep, GetMessageById(AM_DLG_WORD_SEP)));
	IFS_RET(g_dlgData.smTabs.AddTab(GetDialogById(SD_LAY), (DLGPROC)DlgProcPageLay, GetMessageById(AM_3)));
	IFS_RET(g_dlgData.smTabs.AddTab(IDD_DIALOG_PAGE_CLIP, (DLGPROC)DlgProcPageClip, GetMessageById(AM_PAGE_CLIP_NAME)));
	IFS_RET(g_dlgData.smTabs.AddTab(IDD_DIALOG_PAGE_LANG2, (DLGPROC)DlgProcPageLang, GetMessageById(AM_LANG)));
	IFS_RET(g_dlgData.smTabs.AddTab(IDD_DIALOG_PAGE_SCAN, (DLGPROC)DlgProc_Scancode, L"Scancode remap"));
	IFS_RET(g_dlgData.smTabs.AddTab(IDD_DIALOG_PAGE_ABOUT, (DLGPROC)DlgProcAbout, GetMessageById(AM_ABOUT)));

	g_dlgData.menu = CreateMenu();
	HMENU hPopMenuFile = CreatePopupMenu();
	HMENU hPopMenuConfigLua = CreatePopupMenu();

	AppendMenu(hPopMenuFile, MF_STRING, MENU_ID_EXIT, L"Выход");
	AppendMenu(hPopMenuConfigLua, MF_STRING, MENU_ID_CONFIG_LUA_OPEN, L"Открыть");
	AppendMenu(hPopMenuConfigLua, MF_STRING, MENU_ID_CONFIG_LUA_RELOAD, L"Перезагрузить");

	AppendMenu(g_dlgData.menu, MF_STRING | MF_POPUP, (UINT)hPopMenuFile, L"File");
	AppendMenu(g_dlgData.menu, MF_STRING | MF_POPUP, (UINT)hPopMenuConfigLua, L"Config.lua");

	SetMenu(hwnd, g_dlgData.menu);


	//auto timeId = SetTimer(hwnd, c_timerGetCurLayout, 250, NULL);
	//IFW_LOG(timeId != 0);

	RETURN_SUCCESS;
}

void HandleCurLayout()
{
	static HKL lastLayout = 0;
	auto curLayout = GetKeyboardLayout(0);

	if (curLayout != lastLayout)
	{
		lastLayout = curLayout;
	}

}

TStatus GetPathBinFile(tstring& sFilePath, TStr fileName)
{
	IFS_RET(GetPath(sFilePath, PATH_TYPE_SELF_FOLDER, GetSelfBit()));
	sFilePath += fileName;

	if (!FileUtils::IsFileExists(sFilePath.c_str()))
	{
		return SW_ERR_SUCCESS;
	}

	RETURN_SUCCESS;
}

TStatus HandleOpenConfigLua(HWND hwnd)
{
	tstring sFilePath;
	IFS_RET(GetPathBinFile(sFilePath, L"config.lua"));

	procstart::CreateProcessParm parm;
	parm.sExe = L"notepad.exe";
	parm.sCmd = sFilePath.c_str();
	parm.mode = procstart::SW_CREATEPROC_SHELLEXE;
	CAutoHandle hProc;
	IFS_RET(procstart::SwCreateProcess(parm, hProc));

	RETURN_SUCCESS;
}

TStatus HandleReloadConfigLua(HWND hwnd)
{
	IFS_LOG(SettingsGlobal().Load());
	PostMsgSettingChanges();

	RETURN_SUCCESS;
}


LRESULT CALLBACK DlgProcMainMenu(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	gui_tools::Wmsg wmsg(hwnd, msg, wParam, lParam);

	if (CommonDlgPageProcess(wmsg))
		return TRUE;

	if (g_dlgData.trayIcon.ProcMsg(wmsg))
		return TRUE;

	if (gui_tools::HandleButton(wmsg, IDC_BUTTON_CLOSE, HandleExitGui))
		return TRUE;
	if (gui_tools::HandleButton(wmsg, IDOK, HandleExitGui))
		return TRUE;
	if (gui_tools::HandleButton(wmsg, IDCANCEL, HandleExitGui))
		return TRUE;
	if (gui_tools::HandleButton(wmsg, MENU_ID_EXIT, ExitMainMenu))
		return TRUE;
	if (gui_tools::HandleButton(wmsg, MENU_ID_CONFIG_LUA_OPEN, HandleOpenConfigLua))
		return TRUE;
	if (gui_tools::HandleButton(wmsg, MENU_ID_CONFIG_LUA_RELOAD, HandleReloadConfigLua))
		return TRUE;

	if (g_dlgData.smTabs.ProcessMessage(wmsg))
		return TRUE;

	if(msg == WM_INITDIALOG)
	{
		InitDialogMainMenu(hwnd);
		return TRUE;
	}
	else if(msg == WM_CLOSE)
	{
		HandleExitGui(hwnd);
		return TRUE;
	}
	else if (msg == WM_TIMER)
	{
		UINT_PTR timerId = wParam;
		if (timerId == c_timerGetCurLayout)
		{
			HandleCurLayout();
		}
		return TRUE;
	}

	return FALSE;
}
VOID CALLBACK TimerProcPrintAlive(
	_In_ HWND     hwnd,
	_In_ UINT     uMsg,
	_In_ UINT_PTR idEvent,
	_In_ DWORD    dwTime
	)
{
	static DWORD startTime = GetTick();
	DWORD elapsed = GetTick() - startTime;
	DWORD elapsedMs = elapsed / 1000;
	LOG_INFO_1(L"%u", elapsedMs);
}
TAStr GetPowerName(WPARAM wparm)
{
	switch (wparm)
	{
	case PBT_APMPOWERSTATUSCHANGE: return "PBT_APMPOWERSTATUSCHANGE";
	case PBT_APMRESUMEAUTOMATIC: return "PBT_APMRESUMEAUTOMATIC";
	case PBT_APMRESUMESUSPEND: return "PBT_APMRESUMESUSPEND";
	case PBT_APMSUSPEND: return "PBT_APMSUSPEND";
	case PBT_POWERSETTINGCHANGE: return "PBT_POWERSETTINGCHANGE";
	default: return "UNKNOWN";
	}
}
LRESULT CALLBACK MainWindowProc(
	_In_ HWND   hwnd,
	_In_ UINT   uMsg,
	_In_ WPARAM wParam,
	_In_ LPARAM lParam
)
{
	if (uMsg == c_MSG_Quit)
	{
		LOG_INFO_1(L"Quit message");
		ExitMainMenu();
	}
	else if (uMsg == c_MSG_SHOW)
	{
		LOG_INFO_1(L"Show msg");
		ShowWindow(g_dlgData.hwndMainMenu, SW_SHOW);
	}
	else if (uMsg == WM_ENDSESSION)
	{
		LOG_INFO_1(L"WM_ENDSESSION");
	}
	else if (uMsg == WM_POWERBROADCAST)
	{
		LOG_INFO_1(L"WM_POWERBROADCAST: %S", GetPowerName(wParam));
	}
	else
	{
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}

	return 0;
}
TStatus StartCycleGui(int& retFromWnd, bool fShowWnd)
{
	//IFW_LOG(SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_HIGHEST));

	WNDCLASSEX wcex = { 0 };

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.lpfnWndProc = MainWindowProc;
	wcex.hInstance = gdata().hInst;
	wcex.lpszClassName = c_sClassNameGUI;

	IFW_RET(RegisterClassEx(&wcex) != 0);

	HWND hWndFix = CreateWindow(
		c_sClassNameGUI,
		L"Title",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		0, CW_USEDEFAULT, 0, NULL, NULL, gdata().hInst, NULL);

	IFNULL(hWndFix) RETW();
	g_dlgData.hWndFix = hWndFix;

	HWND hWnd = CreateDialog(
		gdata().hInst,
		MAKEINTRESOURCE(IDD_DIALOG_MAIN_MENU),
		NULL,
		(DLGPROC)DlgProcMainMenu);
	IFNULL(hWnd) RETW();
	g_dlgData.hwndMainMenu = hWnd;
	//gdata().hWndGuiMain = hWnd;

	if (WinApiInt::ChangeWindowMessageFilterEx)
	{
		IFW_LOG(WinApiInt::ChangeWindowMessageFilterEx(g_dlgData.hWndFix, c_MSG_Quit, MSGFLT_ALLOW, 0));
		IFW_LOG(WinApiInt::ChangeWindowMessageFilterEx(g_dlgData.hWndFix, c_MSG_SHOW, MSGFLT_ALLOW, 0));
	}

	ShowWindow(hWnd, fShowWnd ? SW_SHOW : SW_HIDE);
	UpdateWindow(hWnd);

	//auto timeId = SetTimer(g_dlgData.hwndMainMenu, c_timerPrintAlive, 10000, TimerProcPrintAlive);
	//IFW_LOG(timeId != 0);

	retFromWnd = 0;
	MSG msg;

	BOOL bRet;
	while ((bRet = GetMessage(&msg, NULL, 0, 0)) != 0)
	{
		if (bRet == -1)
		{
			// handle the error and possibly exit
			IFW_RET(FALSE);
		}

		bool fdispatch = true;

		if (msg.message == WM_KEYDOWN)
		{
			auto nVirtKey = msg.wParam;
			if (nVirtKey == VK_ESCAPE)
			{
				if (SettingsGlobal().fCloseByEsc)
				{
					fdispatch = false;
					HandleExitGui(hWnd);
				}
			}
		}
		else if(msg.message == WM_INPUTLANGCHANGEREQUEST)
		{
			LOG_INFO_1(L"WM_INPUTLANGCHANGE");
		}
		
		if(fdispatch)
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		
	}

	retFromWnd = (int)msg.wParam;

	RETURN_SUCCESS;
}
SwLang FindWindowsLang()
{
	if (WinApiInt::GetLocaleInfoEx == NULL)
	{
		return SLANG_ENG;
	}

	TChar sLocName[1000];
	int size = WinApiInt::GetLocaleInfoEx(LOCALE_NAME_USER_DEFAULT, LOCALE_SNAME, sLocName, sizeof(sLocName));
	if (size > 0)
	{
		if (Str_Utils::IsStartWith(sLocName, L"ru-"))
		{
			return SLANG_RUS;
		}
	}

	return SLANG_ENG;
}
void InitLang()
{
	if (SettingsGlobal().idLang == SLANG_UNKNOWN)
	{
		// Пока английский не готов, русский всегда по умолчанию.
		SettingsGlobal().idLang = SLANG_RUS;
		//SettingsGlobal().idLang = FindWindowsLang();

		SettingsGlobal().Save();
	}

	InitializeLang(SettingsGlobal().idLang);
}

TStatus APIENTRY StartGui(bool fShowWnd)
{

	HWND hwndGui = FindWindow(c_sClassNameGUI, NULL);
	if (hwndGui != NULL)
	{
		LOG_INFO_1(L"Found gui=0x%X", hwndGui);
		if (fShowWnd)
		{
			LOG_INFO_1(L"Try activate");
			DWORD lpdwProcessId = 0;
			GetWindowThreadProcessId(hwndGui, &lpdwProcessId);
			PostMessage(hwndGui, c_MSG_SHOW, 0, 0);
		}
		RETURN_SUCCESS;
	}
	if (Utils::ProcSingleton(c_mtxSingltonGui))
	{
		LOG_INFO_1(L"Gui already running.Exit");
		RETURN_SUCCESS;
	}


	InitLang();

	COM::CAutoCOMInitialize autoCom;
	IFS_LOG(autoCom.Init());

	int retWnd = 0;
	TStatus stat = StartCycleGui(retWnd, fShowWnd);

	LOG_INFO_1(L"StartCycle ends");

	//{
	//	CAutoWinMutexWaiter w(G_SwSharedMtx());
	//	G_SwSharedBuf().hwndGui = NULL;
	//}

	IFS_RET(stat);


	RETURN_SUCCESS;
}

SW_NAMESPACE_END