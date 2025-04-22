#pragma once

struct Message_KeyType {
	TKeyCode   vkCode = 0;
	TScanCode_Ext scan_ext;
	HotKeyType hk = hk_NULL;
	KeyState keyState = KEY_STATE_NONE;
};

struct Message_Hotkey {
	bool fix_ralt = false;
	CHotKey hotkey;
	HotKeyType hk = hk_NULL;
	ULONGLONG delayed_from = 0;
};

struct Message_ChangeForeg {
	HWND hwnd = 0;
};

struct Message_ClearWorlds {};
struct Message_Quit {};

class WorkerImplement;
using Message_Func = std::move_only_function<void(WorkerImplement*)>; // все захваченное должно быть копией.

using Message_Variant = std::variant< Message_KeyType, Message_Hotkey, Message_ClearWorlds, Message_Func, Message_ChangeForeg, Message_Quit >;



