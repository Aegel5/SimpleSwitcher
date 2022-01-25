#include "stdafx.h"

#include <shellapi.h>


#include "Settings.h"
#include "SwAutostart.h"
#include "SwShared.h"

#include "SwGui.h"

namespace SwGui{


void UpdateAddToTray(HWND hwnd);
void UpdateAutostartExplain(HWND hwnd);

void UpdateAdmin(HWND hwnd)
{
	gui_tools::SetCheckBox(hwnd, IDC_CHECK_ADMIN, setsgui.isMonitorAdmin);
}
void UpdateEnabled(HWND hwnd)
{
	TSWCheckRunRes res = gdata().procMonitor.CheckRunning();
	if(res.found)
	{
		if(setsgui.isMonitorAdmin != res.admin)
		{
			if (res.admin == false)
			{
				IFS_LOG(gdata().procMonitor.Stop());
				res = gdata().procMonitor.CheckRunning();
			}
		}
	}

	setsgui.isEnabled = res.found;

	LOG_INFO_1(L"Set enabled=%u", setsgui.isEnabled);
	gui_tools::SetCheckBox(hwnd, IDC_CHECK_ENABLE, setsgui.isEnabled);
}
void UpdateAutoStart(HWND hwnd)
{

	bool isUserAllOk = false;
	bool isUserHasTask = false;
	IFS_LOG(CheckRegRun(isUserAllOk, isUserHasTask));

	bool isAdminAllOk = false;
	bool isAdminHasTask = false;
	IFS_LOG(CheckSchedule(isAdminAllOk, isAdminHasTask));

	if (isUserHasTask)
	{
		if (!isUserAllOk || setsgui.isMonitorAdmin)
		{
			IFS_LOG(DelRegRun());
			IFS_LOG(CheckRegRun(isUserAllOk, isUserHasTask));
		}
	}

	if(isAdminHasTask && Utils::IsSelfElevated())
	{
		if (!isAdminAllOk || !setsgui.isMonitorAdmin)
		{
			IFS_LOG(DelSchedule());
			IFS_LOG(CheckSchedule(isAdminAllOk, isAdminHasTask));
		}
	}

	setsgui.isAddToAutoStart = setsgui.isMonitorAdmin ? isAdminAllOk : (isAdminAllOk || isUserAllOk);

	gui_tools::SetCheckBox(hwnd, IDC_CHECK_AUTOSTART, setsgui.isAddToAutoStart);

	UpdateAutostartExplain(hwnd);
}

void UpdateAutostartExplain(HWND hwnd)
{
	Startup::CheckTaskSheduleParm parm;
	parm.taskName = c_wszTaskName;
	IFS_LOG(Startup::CheckTaskShedule(parm));

	bool isHasEntry = false;
	std::wstring value;
	IFS_LOG(Startup::GetString_AutoStartUser(c_sRegRunValue, isHasEntry, value));

	std::wstring registryRes = L"none";
	std::wstring schedulRes = L"none";

	if (isHasEntry)
	{
		registryRes = value;
	}

	if (parm.isTaskExists)
	{
		schedulRes = parm.pathValue;
	}

	std::wstring sLabel = fmt::format(L"Registry: {}\r\nScheduler: {}", registryRes, schedulRes);

	SetDlgItemText(hwnd, IDC_EDIT_AUTOEXPLAIN, sLabel.c_str());
}

TStatus InitDialogPageMain(HWND hwnd)
{
	if (GetCurLang() != SLANG_RUS)
	{
		SetDlgItemText(hwnd, IDC_CHECK_ENABLE, GetMessageById(AM_PGMAIN_ENABLE));
		SetDlgItemText(hwnd, IDC_CHECK_AUTOSTART, GetMessageById(AM_PGMAIN_add_to));
		SetDlgItemText(hwnd, IDC_CHECK_ADMIN, GetMessageById(AM_PGMAIN_work_admin));
		SetDlgItemText(hwnd, IDC_STATIC_1, GetMessageById(AM_PGMAIN_change_layout));
		SetDlgItemText(hwnd, IDC_STATIC_2, GetMessageById(AM_PGMAIN_last_word));
		SetDlgItemText(hwnd, IDC_STATIC_3, GetMessageById(AM_PGMAIN_few_word));
		SetDlgItemText(hwnd, IDC_STATIC_4, GetMessageById(AM_PGMAIN_selected_text));
	}

	if (!IsWindowsVistaOrGreater())
	{
		setsgui.isMonitorAdmin = false;
		EnableWindow(GetDlgItem(hwnd, IDC_CHECK_ADMIN), FALSE);

	}

	UpdateAdmin(hwnd);
	UpdateEnabled(hwnd);
	UpdateAutoStart(hwnd);
	UpdateAddToTray(hwnd);

	//if (setsgui.isEnabledSaved != setsgui.isEnabled)
	//{
	//	PageMainHandleEnable();
	//	UpdateEnabled(hwnd);
	//}

	RETURN_SUCCESS;
}

TStatus AutoAdminOn()
{
	if (Utils::IsSelfElevated())
	{
		IFS_RET(SetSchedule());
	}
	else
	{
		IFS_RET(SwCreateProcessOurWaitFinished(
			c_sArgSchedulerON,
			SW_BIT_32,
			SW_ADMIN_ON));
	}
	RETURN_SUCCESS;
}
TStatus AutoAdminOff()
{
	if (Utils::IsSelfElevated())
	{
		IFS_RET(DelSchedule());
	}
	else
	{
		IFS_RET(SwCreateProcessOurWaitFinished(
			c_sArgShedulerOFF,
			SW_BIT_32,
			SW_ADMIN_ON));
	}
	RETURN_SUCCESS;
}
void PageMainHandleEnable()
{
	HWND hwnd = g_dlgData.hwndPageMain;
	if (setsgui.isEnabled)
	{
		IFS_LOG(gdata().procMonitor.Stop());
	}
	else
	{
		IFS_LOG(gdata().procMonitor.EnsureStarted(
			setsgui.isMonitorAdmin ? SW_ADMIN_ON : SW_ADMIN_SELF
		));
	}

	UpdateEnabled(hwnd);

	//setsgui.isEnabledSaved = setsgui.isEnabled;
	setsgui.Save();
}



void UpdateAddToTray(HWND hwnd)
{
	//gui_tools::SetCheckBox(hwnd, IDC_CHECK_ADDTOTRAY, setsgui.isAddToTray);
	//g_dlgData.trayIcon.SetEnabled(setsgui.IsAddToTray());
}
void EnsureAutostart(bool needAdded)
{
	bool isAdmin = false;
	bool isAdminEntry = false;
	IFS_LOG(CheckSchedule(isAdmin, isAdminEntry));

	bool isUser = false;
	bool isUserEntry = false;
	IFS_LOG(CheckRegRun(isUser, isUserEntry));

	bool needAdmin = needAdded && setsgui.isMonitorAdmin;
	bool needUser = needAdded && !setsgui.isMonitorAdmin;

	if (needAdmin)
	{
		if (!isAdmin)
		{
			IFS_LOG(AutoAdminOn());
		}
	}
	else
	{
		if (isAdminEntry)
		{
			IFS_LOG(AutoAdminOff());
		}
	}

	if (needUser)
	{
		if (!isUser)
		{
			IFS_LOG(SetRegRun());
		}
	}
	else
	{
		if (isUserEntry)
		{
			IFS_LOG(DelRegRun());
		}
	}


}
void HandleAutostart(HWND hwnd)
{
	EnsureAutostart(!setsgui.isAddToAutoStart);
	UpdateAutoStart(hwnd);
}

void HandleAdmin(HWND hwnd)
{
	setsgui.isMonitorAdmin = gui_tools::IsCheckBox(hwnd, IDC_CHECK_ADMIN);
	setsgui.Save();

	if (setsgui.isAddToAutoStart)
	{
		EnsureAutostart(true);
	}

	if (setsgui.isEnabled)
	{
		IFS_LOG(gdata().procMonitor.EnsureStarted(
			setsgui.isMonitorAdmin ? SW_ADMIN_ON : SW_ADMIN_SELF
			));
	}

	UpdateAdmin(hwnd);
	UpdateEnabled(hwnd);
	UpdateAutoStart(hwnd);
}


void HandleAddToTray(HWND hwnd)
{
	//setsgui.isAddToTray = gui_tools::IsCheckBox(hwnd, IDC_CHECK_ADDTOTRAY);
	//setsgui.Save();
	//UpdateAddToTray(hwnd);
}

BOOL CALLBACK HandleWMCommand(WPARAM wParam, HWND hwnd)
{
	switch (LOWORD(wParam))
	{
	case IDC_CHECK_ENABLE:
		PageMainHandleEnable();
		return TRUE;
	case IDC_CHECK_AUTOSTART:
		HandleAutostart(hwnd);
		return TRUE;
	case IDC_CHECK_ADMIN:
		HandleAdmin(hwnd);
		return TRUE;
	case IDC_CHECK_ADDTOTRAY:
		HandleAddToTray(hwnd);
		return TRUE;
	default:
		return FALSE;
	}
}


LRESULT CALLBACK DlgProcPageMain(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
		case WM_RBUTTONUP:
			ShowPopupMenu();
			return TRUE;
		case WM_INITDIALOG:
			InitDialogPageMain(hwnd);
			InitDialogPageBreak(hwnd);
			return TRUE;
		case WM_COMMAND:
			if(HandleWMCommand(wParam, hwnd))
				return TRUE;
			if(HandleWMCommandPageBreak(wParam, hwnd))
				return TRUE;
			break;
		// page break
		case WM_CTLCOLORSTATIC:
		{
			int id = GetDlgCtrlID((HWND)lParam);
			if (id == IDC_EDIT_BREAK || id == IDC_EDIT_BREAK2 || id == IDC_EDIT_BREAK3 || id == IDC_EDIT_BREAK_CASE)
				return HandleCtlColor(hwnd, lParam, wParam);
			break;
		}
	}
	return FALSE;
}

















}











