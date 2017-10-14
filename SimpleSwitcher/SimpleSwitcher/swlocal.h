#pragma once

enum SwLang
{
	SLANG_RUS = 0,
	SLANG_ENG = 1,

	SLANG_UNKNOWN = 2
};

enum Id_Messages
{
	AM_COMMON = 0,
	AM_ADD,
	AM_2,
	AM_3,
	AM_4,
	AM_Not_setted,
	AM_6,
	AM_7,
	AM_ERROR,
	AM_LANG,
	AM_RUS,
	AM_ENG,
	AM_NEED_REST,
	AM_VERS,
	AM_CaptAb,
	AM_CLOSE,

	AM_SHOW,
	AM_ENABLE,
	AM_ABOUT,
	AM_EXIT,
	AM_DBG_MODE,

	AM_DBG_PATH_TO_PRG,

	AM_PAGE_CLIP_NAME,
	AM_CHECK_CLIPCLEAR_NAME,

	AM_IDC_STATIC_1,
	AM_IDC_RADIO_TYPE,
	AM_IDC_RADIO_CLEAR,
	AM_IDC_CHECK_LEFTRIGHT,
	AM_IDC_CHECK_KEYUP,

	AM_IDD_DIALOG_HOTKEY,

	AM_DLG_WORD_SEP,

	AM_PGMAIN_ENABLE,
	AM_PGMAIN_add_to,
	AM_PGMAIN_work_admin,
	AM_PGMAIN_change_layout,
	AM_PGMAIN_last_word,
	AM_PGMAIN_few_word,
	AM_PGMAIN_selected_text,

	SM_SIZE
};

enum Id_Dialog
{
	SD_HOTKEY = 0,
	SD_LAY,
	SD_ADV,
	//SD_MAINMENU,

	SD_SIZE

};

void InitMessages_Eng();
void InitDialogs_Eng();

void InitMessages_Ru();
void InitDialogs_Ru();

extern const wchar_t* c_messages[SM_SIZE];
extern int c_dialogs[SD_SIZE];



void InitializeLang(SwLang idLang);

const wchar_t* GetMessageById(int idMessage);

int GetDialogById(int idDialog);

SwLang GetCurLang();



