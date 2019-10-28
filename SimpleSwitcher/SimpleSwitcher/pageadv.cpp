#include "stdafx.h"

#include "Settings.h"

#include "SwGui.h"

#include <string>        
#include <locale>
#include <algorithm>

SW_NAMESPACE(SwGui)

bool HasOtherRemap(BufScanMap& map)
{
	for (auto iter = map.GetIter(); !iter.IsEnd(); ++iter)
	{
		auto src = iter.curKeySrc();
		if (src != VK_CAPITAL && src != VK_SCROLL)
		{
			return true;
		}
	}
	return false;
}
void HandleCapsRemap(HWND hwnd, RemapType rtype)
{
	int idCheckBox = 0;
	if (rtype == RemapKey_CAPS)
	{
		idCheckBox = IDC_CHECK_CAPSREMAP;
	}
	else if (rtype == RemapKey_ScrollLock)
	{
		idCheckBox = IDC_CHECK_CAPSREMAP2;
	}

	bool checked = gui_tools::IsCheckBox(hwnd, idCheckBox);
	if (checked)
	{
		BufScanMap readed;
		readed.FromRegistry();

		if (readed.size > 0 && HasOtherRemap(readed))
		{
			MessageBox(hwnd, GetMessageById(AM_7), GetMessageById(AM_ERROR), MB_ICONERROR);
			return;
		}

		if(Utils::IsSelfElevated())
		{
			IFS_LOG(AddRemap(rtype));
		}
		else
		{
			std::string cmdline = c_sArgCapsRemapAdd;
			cmdline += " " + std::to_string(int(rtype));
			IFS_LOG(SwCreateProcessOurWaitFinished(
				cmdline.c_str(),
				SW_BIT_32,
				SW_ADMIN_ON));
		}
	}
	else
	{
		if (Utils::IsSelfElevated())
		{
			IFS_LOG(DelRemap(rtype));
		}
		else
		{
			std::string cmdline = c_sArgCapsRemapRemove;
			cmdline += std::to_string(int(rtype));
			IFS_LOG(SwCreateProcessOurWaitFinished(
				cmdline.c_str(),
				SW_BIT_32,
				SW_ADMIN_ON));
		}
	}
}
//void UpdateRemapApply()
//{
//	TUInt64 curTime = GetBootTime();
//	BufScanMap buf;
//	buf.FromRegistry();
//	TKeyCode inreg = buf.RemapedKey();
//	TKeyCode saved = SettingsGlobal().capsRemapApply;
//	if ((curTime != SettingsGlobal().bootTime && saved != inreg) || curTime == 0)
//	{
//		SettingsGlobal().capsRemapApply = buf.RemapedKey();
//		SettingsGlobal().bootTime = curTime;
//		SettingsGlobal().Save();
//	}
//}
void UpdateCapsRemap(HWND hwnd, bool fNeedRestart = false)
{
	BufScanMap buf;
	buf.FromRegistry();

	std::wstring capsVal;
	capsVal += L"CapsLock = ";

	TKeyCode keyVal;
	buf.GetRemapedKey(VK_CAPITAL, keyVal);

	std::wstring capsNewName;
	CHotKey::ToString(keyVal, capsNewName);

	capsVal += capsNewName;
	if (fNeedRestart)
	{
		capsVal += GetMessageById(AM_6);
	}
	SetDlgItemText(hwnd, IDC_STATIC_CAPSVAL, capsVal.c_str());
	gui_tools::SetCheckBox(hwnd, IDC_CHECK_CAPSREMAP, keyVal == VK_F24);
}
void UpdateScrRemap(HWND hwnd, bool fNeedRestart = false)
{
	BufScanMap buf;
	buf.FromRegistry();

	std::wstring capsVal;
	capsVal += L"ScrollLock = ";

	TKeyCode keyVal;
	buf.GetRemapedKey(VK_SCROLL, keyVal);

	std::wstring capsNewName;
	CHotKey::ToString(keyVal, capsNewName);

	capsVal += capsNewName;
	if (fNeedRestart)
	{
		capsVal += GetMessageById(AM_6);
	}
	SetDlgItemText(hwnd, IDC_STATIC_CAPSVAL2, capsVal.c_str());
	gui_tools::SetCheckBox(hwnd, IDC_CHECK_CAPSREMAP2, keyVal == VK_F23);
}
LRESULT CALLBACK DlgProcPageAdv(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	gui_tools::Wmsg wmsg(hwnd, msg, wParam, lParam);

	if (CommonDlgPageProcess(wmsg))
		return TRUE;

	if (msg == WM_INITDIALOG)
	{
		//SetDlgItemText(hwnd, IDC_BUTTON_SET2, GetMessageById(AM_SET));
		//SetDlgItemText(hwnd, IDC_BUTTON_SET, GetMessageById(AM_SET));
		//SetDlgItemText(hwnd, IDC_BUTTON_SET, GetMessageById(AM_SET));
		//SetDlgItemText(hwnd, IDC_STATIC_LW, GetMessageById(AM_LW));
		//SetDlgItemText(hwnd, IDC_STATIC_FW, GetMessageById(AM_FW));


		//KeyToDlg(hk_RevertAdv, IDC_EDIT_BREAK, hwnd);
		//KeyToDlg(hk_RevertCycleAdv, IDC_EDIT_BREAK2, hwnd);
		KeyToDlg(hk_CapsGenerate, IDC_EDIT_BREAK3, hwnd);
		KeyToDlg(hk_ScrollGenerate, IDC_EDIT_BREAK8, hwnd);


		//UpdateRemapApply();
		UpdateCapsRemap(hwnd);
		UpdateScrRemap(hwnd);
		//UpdateCancelRemap(hwnd);

		return FALSE;
	}
	else if(msg == WM_CTLCOLORSTATIC)
	{
		int id = GetDlgCtrlID((HWND)lParam);
		if (id == IDC_EDIT_BREAK3 || id == IDC_EDIT_BREAK8 )
			return HandleCtlColor(hwnd, lParam, wParam);
		return TRUE;
	}
	else if (msg == WM_COMMAND)
	{
		//if (LOWORD(wParam) == IDC_BUTTON_SET)
		//{
		//	ChangeHotKey(hk_RevertAdv, IDC_EDIT_BREAK, hwnd);
		//	return TRUE;
		//}
		//else if (LOWORD(wParam) == IDC_BUTTON_SET2)
		//{
		//	ChangeHotKey(hk_RevertCycleAdv, IDC_EDIT_BREAK2, hwnd);
		//	return TRUE;
		//}
		if (LOWORD(wParam) == IDC_BUTTON_SET3)
		{
			ChangeHotKey(hk_CapsGenerate, IDC_EDIT_BREAK3, hwnd, false);
			return TRUE;
		}
		else if (LOWORD(wParam) == IDC_BUTTON_SET8)
		{
			ChangeHotKey(hk_ScrollGenerate, IDC_EDIT_BREAK8, hwnd, false);
			return TRUE;
		}
		else if (LOWORD(wParam) == IDC_CHECK_CAPSREMAP)
		{
			HandleCapsRemap(hwnd, RemapKey_CAPS);
			UpdateCapsRemap(hwnd, true);
			return TRUE;
		}
		else if (LOWORD(wParam) == IDC_CHECK_CAPSREMAP2)
		{
			HandleCapsRemap(hwnd, RemapKey_ScrollLock);
			UpdateScrRemap(hwnd, true);
			return TRUE;
		}

	}
	return FALSE;
}

SW_NAMESPACE_END