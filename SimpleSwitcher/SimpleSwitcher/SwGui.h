#pragma once

#include "resource.h"
#include "SimpleSwitcher.h"
#include "Settings.h"

#include "gui/inc_all.h"

namespace SwGui
{
	TStatus APIENTRY StartGui(bool fShowGui);

	void ShowAbout();
	void PageMainHandleEnable();
	LRESULT CALLBACK DlgProcPageMain(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	void ShowPopupMenu();
	LRESULT CALLBACK HandleCtlColor(HWND hwnd, LPARAM lParam, WPARAM wParam);
	LRESULT CALLBACK DlgProcHotKey(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	TStatus InitDialogPageBreak(HWND hwnd);
	BOOL CALLBACK HandleWMCommandPageBreak(WPARAM wParam, HWND hwnd);
	TStatus HandleExitGui(HWND);
	LRESULT CALLBACK DlgProcPageAdv(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK DlgProcPageWordSep(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	LRESULT CALLBACK DlgProcPageLay(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK DlgProcPageClip(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	LRESULT CALLBACK DlgProcPageLang(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK DlgProcAbout(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);



	const static int c_nTrayIconId = 6332;
	const static int WM_TRAYICON = WM_USER + 100;
	const static int ID_TRAYITEM_EXIT = 3000;
	const static int ID_TRAYITEM_SHOW = 3001;
	const static int ID_TRAYITEM_ENABLE = 3002;
	const static int ID_TRAYITEM_ABOUT = 3003;

	struct DlgHotKeyData
	{
		CHotKey keyRevert;
		CHotKey keyDefault;
		CHotKey keyDefault2;

		bool fOk;

		WNDPROC oldEditBreakProc = NULL;
		HWND hwndEditRevert = NULL;
		CAutoHHOOK hHookKeyGlobal;
		HWND hwnd;

		bool fAltDisable = true;

	};
	bool ChangeHotKey(HotKeyType type, int dlgId, HWND hwnd, bool fAltDisable=true);

	inline void KeyToDlg(CHotKey key, int dlgId, HWND hwnd)
	{
		std::wstring sKeyRevert;
		key.ToString(sKeyRevert);
		SetDlgItemText(hwnd, dlgId, sKeyRevert.c_str());
	}
	inline void KeyToDlg(HotKeyType type, int dlgId, HWND hwnd)
	{
		std::wstring sKeyRevert;
		CHotKey key = SettingsGlobal().GetHk(type).key;
		key.ToString(sKeyRevert);
		SetDlgItemText(hwnd, dlgId, sKeyRevert.c_str());
	}

	struct DialogData
	{
		CAutoCloseHBRUSH hEditBrush;
		const static COLORREF colEditBg = 0xffffff;
		const static COLORREF colEditTxt = 0x000000;
		TStatus Init(HWND hwnd)
		{
			LOGBRUSH lb;
			lb.lbStyle = BS_SOLID;
			lb.lbColor = colEditBg;
			lb.lbHatch = 0;
			hEditBrush = CreateBrushIndirect(&lb);
			IFW_RET(hEditBrush.IsValid());



			//{
			//	NONCLIENTMETRICS metrics;
			//	metrics.cbSize = sizeof(metrics);
			//	::SystemParametersInfo(
			//		SPI_GETNONCLIENTMETRICS,
			//		sizeof(metrics),
			//		&metrics, 0);
			//	LOGFONTW font = metrics.lfMessageFont;
			//	font.lfHeight = -20;
			//	fontLabel = ::CreateFontIndirect(&font);
			//}

			RETURN_SUCCESS;
		}
		HWND hWndFix = NULL;
		HWND hwndMainMenu = NULL;

		HWND hwndPageMain;
		gui_tools::SimpleTabs smTabs;
		gui_tools::TrayIcon<WM_TRAYICON, c_timerIdAddToTray, c_nTrayIconId> trayIcon;

		CAutoHandleIcon hIcon;
		CAutoHandleIcon hIconBig;
		//CAutoHMENU hPopupMenu;
		CAutoHFONT fontLabel;
	};

	extern DialogData g_dlgData;

	inline LRESULT CommonDlgPageProcess(gui_tools::Wmsg& wmsg)
	{
		switch (wmsg.msg)
		{
		case WM_RBUTTONDOWN:
			ShowPopupMenu();
			return TRUE;
		}
		return FALSE;
	}

}