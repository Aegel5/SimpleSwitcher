#include "stdafx.h"

const wchar_t* c_messages[SM_SIZE];
int c_dialogs[SD_SIZE];

static bool g_fLangInit = false;
static SwLang g_curLang = SLANG_RUS;

SwLang GetCurLang()
{
	return g_curLang;
}

void InitializeLang(SwLang idLang)
{
	g_curLang = idLang;

	g_fLangInit = true;
	if (idLang == SLANG_ENG || idLang == SLANG_UNKNOWN)
	{
		InitMessages_Eng();
		InitDialogs_Eng();
	}
	else if (idLang == SLANG_RUS)
	{
		InitMessages_Ru();
		InitDialogs_Ru();
	}
	else
	{
		IFS_LOG(SW_ERR_INVALID_PARAMETR, L"Unknown idlang %d", idLang);
	}
}


void EnsureLangInit()
{
	if (!g_fLangInit)
	{
		InitializeLang(SLANG_ENG);
		g_fLangInit = true;
	}
}



const wchar_t* GetMessageById(int idMessage)
{
	EnsureLangInit();
	if (idMessage >= SM_SIZE)
	{
		return L"Int_err";
	}
	const wchar_t* msg = c_messages[idMessage];
	if (msg == NULL)
	{
		msg = L"No_translate";
	}

	return msg;
}

int GetDialogById(int idDialog)
{
	EnsureLangInit();
	return c_dialogs[idDialog];
}

