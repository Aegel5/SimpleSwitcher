#pragma once

#include "resource.h"
#include "gui/inc_all.h"

#ifdef __WXMSW__
#include "wx/msw/private.h"
#endif

class DecentTray {

	const static int c_nTrayIconId = 6332;
	const static int WM_TRAYICON = WM_USER + 100;
	const static int ID_TRAYITEM_EXIT = 3000;
	const static int ID_TRAYITEM_SHOW = 3001;
	const static int ID_TRAYITEM_ENABLE = 3002;
	const static int ID_TRAYITEM_ABOUT = 3003;

	HWND hwnd;


	gui_tools::TrayIcon<WM_TRAYICON, c_timerIdAddToTray, c_nTrayIconId> trayIcon;
	CAutoHandleIcon hIcon;
	CAutoHandleIcon hIconBig;



	void ShowPopupMenu()
	{
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
			//ExitMainMenu();
		}
		else if (clicked == ID_TRAYITEM_SHOW)
		{
			//ShowGui();
		}
		else if (clicked == ID_TRAYITEM_ENABLE)
		{
			//PageMainHandleEnable();
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

public:
	void Init(wxFrame* frame) {

		auto inst = wxGetInstance();
		auto wnd = frame->GetHandle();
		hwnd = wnd;

		//frame->processme

		hIcon = (HICON)LoadImage(
			inst,
			MAKEINTRESOURCE(IDI_ICON1),
			IMAGE_ICON,
			GetSystemMetrics(SM_CXICON),
			GetSystemMetrics(SM_CYICON),
			0);
		hIconBig = (HICON)LoadImage(
			inst,
			MAKEINTRESOURCE(IDI_ICON1),
			IMAGE_ICON,
			256,
			256,
			0);

		if (hIcon)
			SendMessage(hwnd, WM_SETICON, ICON_SMALL, (LPARAM)hIcon.get());
		if (hIconBig)
			SendMessage(hwnd, WM_SETICON, ICON_BIG, (LPARAM)hIconBig.get());

		trayIcon.Init(hwnd, hIcon, std::bind(&DecentTray::ShowPopupMenu, this));
	}
	void SetEnabled(bool enable) {
		trayIcon.SetEnabled(enable);
	}
};
