#include "stdafx.h"

#include "Settings.h"

#include "SwGui.h"

#include <string>        
#include <locale>
#include <algorithm>

SW_NAMESPACE(SwGui)

static HKL g_laylist[20] = {0};
static int g_laySize = 0;
void FillLayList()
{
	g_laySize = GetKeyboardLayoutList(SW_ARRAY_SIZE(g_laylist), g_laylist);
}

void CustomLangListToStr(std::vector<HKL>& lst, std::wstring& str)
{
	str.clear();
	for (size_t i = 0; i < lst.size(); ++i)
	{
		str += Utils::GetNameForHKL(lst[i]);
		if (i != lst.size() - 1)
		{
			str += L", ";
		}
	}
}


void FillCombo(HWND hwnd, int id, int indexSettings)
{
	HKL saved = indexSettings >= 0 ? SettingsGlobal().hkl_lay[indexSettings] : NULL;
	bool found = false;
	for (int i = 0; i < g_laySize; i++)
	{
		HKL cur = g_laylist[i];

		auto name = Utils::GetNameForHKL(cur);
		SendDlgItemMessage(hwnd, id, CB_ADDSTRING, 0, (LPARAM)name.c_str());
		if (saved == cur)
		{
			found = true;
			SendDlgItemMessage(hwnd, id, CB_SETCURSEL, (WPARAM)i, (LPARAM)0);
		}
	}
	if(!found)
	{
		SendDlgItemMessage(hwnd, id, CB_SETCURSEL, (WPARAM)0, (LPARAM)0);
	}
}
void CustomLangToGui(HWND hwnd)
{
	std::wstring LstName;
	CustomLangListToStr(SettingsGlobal().customLangList, LstName);
	SetDlgItemText(hwnd, IDC_EDIT_K6, LstName.c_str());
}
void ComboChanged(HWND hwnd, int id)
{
	int curItemIndex = (int)SendDlgItemMessage(hwnd, id, CB_GETCURSEL, (WPARAM)0, (LPARAM)0);
	if (curItemIndex != CB_ERR)
	{
		if (id == IDC_COMBO_K5)
		{
			HKL hlk = g_laylist[curItemIndex];
			auto pos = std::find(SettingsGlobal().customLangList.begin(), SettingsGlobal().customLangList.end(), hlk);
			if (pos != SettingsGlobal().customLangList.end())
			{
				//SettingsGlobal().customLangList.erase(pos);
			}
			else
			{
				SettingsGlobal().customLangList.push_back(hlk);
			}

			CustomLangToGui(hwnd);
		}
		else
		{
			int index = 0;
			if (id == IDC_COMBO_K2)
				index = SettingsGui::SW_HKL_1;
			else if (id == IDC_COMBO_K3)
				index = SettingsGui::SW_HKL_2;
			else if (id == IDC_COMBO_K4)
				index = SettingsGui::SW_HKL_3;
			else
				return;

			SettingsGlobal().hkl_lay[index] = g_laylist[curItemIndex];
		}

		SettingsGlobal().Save();
		PostMsgSettingChanges();
	}
}

LRESULT CALLBACK DlgProcPageLay(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	gui_tools::Wmsg wmsg(hwnd, msg, wParam, lParam);

	if (CommonDlgPageProcess(wmsg))
		return TRUE;

	if (msg == WM_INITDIALOG)
	{
		KeyToDlg(hk_ChangeLayoutCycle, IDC_EDIT_K1, hwnd);
		KeyToDlg(hk_ChangeSetLayout_1, IDC_EDIT_K2, hwnd);
		KeyToDlg(hk_ChangeSetLayout_2, IDC_EDIT_K3, hwnd);
		KeyToDlg(hk_ChangeSetLayout_3, IDC_EDIT_K4, hwnd);
		KeyToDlg(hk_CycleCustomLang, IDC_EDIT_K5, hwnd);

		FillLayList();
		FillCombo(hwnd, IDC_COMBO_K2, SettingsGui::SW_HKL_1);
		FillCombo(hwnd, IDC_COMBO_K3, SettingsGui::SW_HKL_2);
		FillCombo(hwnd, IDC_COMBO_K4, SettingsGui::SW_HKL_3);
		FillCombo(hwnd, IDC_COMBO_K5, -1);

		CustomLangToGui(hwnd);

		return FALSE;
	}
	else if (msg == WM_CTLCOLORSTATIC)
	{
		int id = GetDlgCtrlID((HWND)lParam);
		if (id == IDC_EDIT_K1 || id == IDC_EDIT_K2 || id == IDC_EDIT_K3 || id == IDC_EDIT_K4 || id == IDC_EDIT_K5 || id == IDC_EDIT_K6)
			return HandleCtlColor(hwnd, lParam, wParam);
		return TRUE;
	}
	else if (msg == WM_COMMAND)
	{
		if (HIWORD(wParam) == CBN_SELCHANGE)
		{
			int id = LOWORD(wParam);
			ComboChanged(hwnd, id);
		}
		else if (LOWORD(wParam) == IDC_BUTTON_K1)
		{
			ChangeHotKey(hk_ChangeLayoutCycle, IDC_EDIT_K1, hwnd, false);
			return TRUE;
		}
		else if (LOWORD(wParam) == IDC_BUTTON_K2)
		{
			if(ChangeHotKey(hk_ChangeSetLayout_1, IDC_EDIT_K2, hwnd, false))
			{
				ComboChanged(hwnd, IDC_COMBO_K2);
			}
			return TRUE;
		}
		else if (LOWORD(wParam) == IDC_BUTTON_K3)
		{
			if(ChangeHotKey(hk_ChangeSetLayout_2, IDC_EDIT_K3, hwnd, false))
			{
				ComboChanged(hwnd, IDC_COMBO_K3);
			}
			return TRUE;
		}
		else if (LOWORD(wParam) == IDC_BUTTON_K4)
		{
			if(ChangeHotKey(hk_ChangeSetLayout_3, IDC_EDIT_K4, hwnd, false))
			{
				ComboChanged(hwnd, IDC_COMBO_K4);
			}
			return TRUE;
		}
		else if (LOWORD(wParam) == IDC_BUTTON_K5)
		{
			ChangeHotKey(hk_CycleCustomLang, IDC_EDIT_K5, hwnd, false);
			return TRUE;
		}
		else if (LOWORD(wParam) == IDC_BUTTON_K6)
		{
			SettingsGlobal().customLangList.clear();
			CustomLangToGui(hwnd);
			SettingsGlobal().SaveAndPostMsg();
			return TRUE;
		}
	}
	return FALSE;
}

SW_NAMESPACE_END