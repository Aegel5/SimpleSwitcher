﻿#pragma once


enum TKeyType : TUInt8
{
	KEYTYPE_NONE,

	KEYTYPE_LETTER,
	KEYTYPE_SPACE,
	KEYTYPE_CUSTOM,
	KEYTYPE_LETTER_OR_CUSTOM,
	KEYTYPE_LETTER_OR_SPACE,
	KEYTYPE_BACKSPACE,

	KEYTYPE_COMMAND_CLEAR,
	KEYTYPE_COMMAND_NO_CLEAR,
};

using TScanCode = WORD;

struct TScanCode_Ext {
	TScanCode scan = 0;
	bool is_ext = false;
	//void clear() { scan = 0; is_ext = false; }
	UINT value_with_ext() const { return is_ext ? scan | (0xE0 << 8) : scan; }
	UINT to_vk_or_def(HKL lay, UINT vk_def=0) const {
		auto res = MapVirtualKeyEx(value_with_ext(), MAPVK_VSC_TO_VK_EX, lay);
		return res == 0? vk_def : res;
	}
	auto operator<=>(const TScanCode_Ext&) const = default;
};

// базовая структура для хранения нажатия одной клавиши.
struct TKeyBaseInfo {
	TKeyCode vk_code = 0;
	TScanCode_Ext scan_code;
	TKeyCode shift_key = 0;
	auto operator<=>(const TKeyBaseInfo&) const = default;
};

using TKeyRevert = std::vector<TKeyBaseInfo>;

struct TKeyHookInfo{

	struct alignas(16) crypted {
		TKeyBaseInfo key;
		TUInt64 _random_data;
	} crypted;

	void encrypt() {
		static_assert(sizeof(crypted) == CRYPTPROTECTMEMORY_BLOCK_SIZE);
		crypted._random_data = GetTickCount64(); // заполняем случайными данными чтобы одна и та же буква каждый раз шифровалась по разному
		IFW_LOG(CryptProtectMemory(&crypted, CRYPTPROTECTMEMORY_BLOCK_SIZE, CRYPTPROTECTMEMORY_SAME_PROCESS));
	}

	TKeyHookInfo decrypted() {
		TKeyHookInfo res = *this;
		IFW_LOG(CryptUnprotectMemory(&res.crypted, CRYPTPROTECTMEMORY_BLOCK_SIZE, CRYPTPROTECTMEMORY_SAME_PROCESS));
		return res;
	}


	TKeyBaseInfo& key() {
		return crypted.key;
	}

	// open part

	TKeyType type = TKeyType::KEYTYPE_NONE;
	bool is_last_revert = false;
	bool as_previous = false;
};


enum KeyState
{
	KEY_STATE_NONE,
	KEY_STATE_DOWN,
	KEY_STATE_UP,
};

inline KeyState GetKeyState(WPARAM wParam)
{
	if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN)
		return KEY_STATE_DOWN;
	if (wParam == WM_KEYUP || wParam == WM_SYSKEYUP)
		return KEY_STATE_UP;

	return KEY_STATE_NONE;
}














