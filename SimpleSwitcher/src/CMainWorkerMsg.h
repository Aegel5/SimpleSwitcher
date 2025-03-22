#pragma once



enum EHWorker
{
	HWORKER_NULL,

	HWORKER_ClearWords,
	HWORKER_ChangeForeground,
	HWORKER_KeyMsg,
	HWORKER_ClipNotify,
	HWORKER_ClipboardClearFormat2,
    HWORKER_Getcurlay,
    HWORKER_Setcurlay,
    HWORKER_FixCtrlAlt,
    HWORKER_OurHotKey,
};

// todo: 1) std::variant, 2)store std::funtion. 3) force std::move
struct MainWorkerMsg {

	EHWorker mode = HWORKER_NULL;

	MainWorkerMsg(EHWorker m): mode(m)  {}
	MainWorkerMsg(){}

	MainWorkerMsg(const MainWorkerMsg&) = delete;
	MainWorkerMsg& operator=(const MainWorkerMsg& temp_obj) = delete;

	MainWorkerMsg(MainWorkerMsg&&) = default;

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
		};
		struct {
			CHotKey hotkey;
			HotKeyType hk;
		};
		U() {} // ничего не делаем - никаких конструкторов, клиенты должны заполнять сами нужные данные...
		
	}data;
};


