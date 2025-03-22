﻿#pragma once

struct Message_KeyType {
	TKeyCode   vkCode = 0;
	TScanCode   scanCode = 0;
	DWORD   flags = 0;
	HotKeyType hk = hk_NULL;
	KeyState keyState = KEY_STATE_NONE;
};

struct Message_Hotkey {
	bool fix_ralt = false;
	CHotKey hotkey;
	HotKeyType hk = hk_NULL;
};

struct Message_ChangeForeg {
	HWND hwnd = 0;
};

struct Message_GetCurLay {
	bool force = false;
};

struct Message_ClearWorlds {
};

class WorkerImplement;
using Message_Func = std::function<void(WorkerImplement&)>; // все захваченное должно быть копией.

using Message_Variant = std::variant< Message_KeyType, Message_Hotkey, Message_ClearWorlds, Message_Func, Message_ChangeForeg, Message_GetCurLay >;



