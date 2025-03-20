#pragma once



enum EHWorker
{
	HWORKER_NULL,

	HWORKER_ClearWords,
	HWORKER_ChangeForeground,
	HWORKER_KeyMsg,
	HWORKER_ClipNotify,
	HWORKER_WM_TIMER,
    HWORKER_Getcurlay,
    HWORKER_Setcurlay,
    HWORKER_FixCtrlAlt,
    HWORKER_OurHotKey,
};

struct MainWorkerMsg
{
	EHWorker mode = HWORKER_NULL;
	MainWorkerMsg(EHWorker m): mode(m) {}

	union U
	{
		struct Key_Message {
			TKeyCode   vkCode;
			TScanCode   scanCode;
			DWORD   flags;
			HotKeyType hk;
			KeyState keyState;
		} key_message;
		struct
		{
			WPARAM wparm;
			//LPARAM lparm;
		};
		struct {
			CHotKey hotkey_to_fix;
		};
		struct {
			CHotKey hotkey;
			HotKeyType hk;
		};
		struct {
			HKL lay;
		};
		U() {} // ничего не делаем - никаких конструкторов, клиенты должны заполнять сами нужные данные...
		
	}data;
};


