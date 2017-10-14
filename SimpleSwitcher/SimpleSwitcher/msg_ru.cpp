#include "stdafx.h"
#include "resource.h"

void InitMessages_Ru()
{
	ZeroMemory(c_messages, sizeof(c_messages));

	c_messages[AM_COMMON] = L"Общие";
	c_messages[AM_2] = L"CapsLock/ScrollLock";
	c_messages[AM_3] = L"Смена раскладки";
	c_messages[AM_4] = L"Модификатор Alt не поддерживается из-за особенностей Windows";
	c_messages[AM_Not_setted] = L"[Не задано]";
	c_messages[AM_6] = L" (требуется перезагрузка Windows)";
	c_messages[AM_7] = L"Раздел 'Scancode Map' уже существует";
	c_messages[AM_ERROR] = L"Ошибка";
	c_messages[AM_LANG] = L"Настройки";
	c_messages[AM_ENG] = L"Английский";
	c_messages[AM_RUS] = L"Русский";
	c_messages[AM_NEED_REST] = L"Требуется перезапуск программы";
	c_messages[AM_VERS] = L"Версия: ";
	c_messages[AM_CaptAb] = L"О программе - SimpleSwitcher";
	c_messages[AM_CLOSE] = L"Закрыть";

	c_messages[AM_SHOW] = L"Показать";
	c_messages[AM_ENABLE] = L"Включить";
	c_messages[AM_ABOUT] = L"О программе";
	c_messages[AM_EXIT] = L"Выход";

	c_messages[AM_DBG_MODE] = L"Включить логирование";

	c_messages[AM_PAGE_CLIP_NAME] = L"Буфер обмена";
	c_messages[AM_CHECK_CLIPCLEAR_NAME] = L"При копировании текста очищать форматирование";

	c_messages[AM_DBG_PATH_TO_PRG] = L"Путь до программы:";

	c_messages[AM_IDC_STATIC_1] = L"Горячая клавиша";
	//c_messages[AM_IDC_STATIC_1] = L"Ввести";
	c_messages[AM_IDC_RADIO_CLEAR] = L"Ввести/очистить";
	c_messages[AM_IDC_CHECK_LEFTRIGHT] = L"Различать правые и левые модификаторы";
	c_messages[AM_IDC_CHECK_KEYUP] = L"Срабатывание на KEYUP";

	c_messages[AM_IDD_DIALOG_HOTKEY] = L"Задать горячую клавишу";

	c_messages[AM_DLG_WORD_SEP] = L"Разделение слов";



	

}

void InitDialogs_Ru()
{
	ZeroMemory(c_dialogs, sizeof(c_dialogs));

	c_dialogs[SD_HOTKEY] = IDD_DIALOG_hk_RU;
	c_dialogs[SD_LAY] = IDD_DIALOG_PAGE_CHLAY_RU;
	c_dialogs[SD_ADV] = IDD_DIALOG_PAGEADV_RU;
	//c_dialogs[SD_MAINMENU] = IDD_DIALOG_PAGEMAIN_RU;
}