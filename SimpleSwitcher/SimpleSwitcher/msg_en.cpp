#include "stdafx.h"
#include "resource.h"

void InitMessages_Eng()
{
	ZeroMemory(c_messages, sizeof(c_messages));

	c_messages[AM_COMMON] = L"Common";
	c_messages[AM_2] = L"CapsLock/ScrollLock";
	c_messages[AM_3] = L"Layout change";
	c_messages[AM_4] = L"Alt modifier is not supported due to the nature of Windows";
	c_messages[AM_Not_setted] = L"[Not setted]";
	c_messages[AM_6] = L" (windows requires a reboot)";
	c_messages[AM_7] = L"Section 'Scancode Map' already exists";
	c_messages[AM_ERROR] = L"Error";
	c_messages[AM_LANG] = L"Settings";
	c_messages[AM_ENG] = L"English";
	c_messages[AM_RUS] = L"Russian";
	c_messages[AM_NEED_REST] = L"Need restart program";
	c_messages[AM_VERS] = L"Version: ";
	c_messages[AM_CaptAb] = L"About - SimpleSwitcher";
	c_messages[AM_CLOSE] = L"Close";

	c_messages[AM_SHOW] = L"Show";
	c_messages[AM_ENABLE] = L"Enable";
	c_messages[AM_ABOUT] = L"About";
	c_messages[AM_EXIT] = L"Exit";

	c_messages[AM_DBG_MODE] = L"Enable logging";

	c_messages[AM_PAGE_CLIP_NAME] = L"Clipboard";
	c_messages[AM_CHECK_CLIPCLEAR_NAME] = L"Clear format when copying text to clipboard";

	c_messages[AM_DBG_PATH_TO_PRG] = L"Path to program:";

	c_messages[AM_IDC_STATIC_1] = L"Hotkey";
	//c_messages[AM_IDC_STATIC_1] = L"Input";
	c_messages[AM_IDC_RADIO_CLEAR] = L"Input";
	c_messages[AM_IDC_CHECK_LEFTRIGHT] = L"Distinguish between right and left modifiers";
	c_messages[AM_IDC_CHECK_KEYUP] = L"Operation on KEYUP";

	c_messages[AM_IDD_DIALOG_HOTKEY] = L"Set hotkey";

	c_messages[AM_DLG_WORD_SEP] = L"Word separation";

	c_messages[AM_PGMAIN_ENABLE] = L"Enable";
	c_messages[AM_PGMAIN_add_to] = L"Add to Startup";
	c_messages[AM_PGMAIN_work_admin] = L"Work in applications running as administrator";
	c_messages[AM_PGMAIN_change_layout] = L"Change layout";
	c_messages[AM_PGMAIN_last_word] = L"Last word";
	c_messages[AM_PGMAIN_few_word] = L"Few words";
	c_messages[AM_PGMAIN_selected_text] = L"Selected text";
	
}

void InitDialogs_Eng()
{
	ZeroMemory(c_dialogs, sizeof(c_dialogs));

	c_dialogs[SD_HOTKEY] = IDD_DIALOG_HOTKEY;
	c_dialogs[SD_LAY] = IDD_DIALOG_PAGE_CHLAY;
	c_dialogs[SD_ADV] = IDD_DIALOG_PAGEADV;
	//c_dialogs[SD_MAINMENU] = IDD_DIALOG_PAGEMAIN;
}