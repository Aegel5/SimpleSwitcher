#pragma once


enum TKeyType : TUInt8
{
	KEYTYPE_NONE,

	KEYTYPE_SYMBOL,
	KEYTYPE_SPACE,
	KEYTYPE_CUSTOM,
	KEYTYPE_LEADING_POSSIBLE_LETTER,
	KEYTYPE_BACKSPACE,

	KEYTYPE_COMMAND_CLEAR,
	KEYTYPE_COMMAND_NO_CLEAR,
};

enum class TKeyFlags : TUInt8 {
	None = 0,
	SYMB_SEPARATE_REVERT = 1,
	SYMB_SEPARATE_IF_SEVERAL_REVERT = 1<<1,
};
DEFINE_ENUM_FLAG_OPERATORS(TKeyFlags);


struct TScanCode_Ext { 
	WORD scan = 0;
	bool is_ext = false;
};

// базовая структура для хранения нажатия одной клавиши.
struct TKeyBaseInfo {
	TKeyCode vk_code = 0;
	TScanCode_Ext scan_code;
	TKeyCode shift_key = 0;
};

using TKeyRevert = std::vector<TKeyBaseInfo>;

struct TKeyHookInfo{

	union U {
		struct {
			TKeyBaseInfo key;
			//wchar_t typed_symbol;
			TUInt64 _random_data;
		};
		struct {
			char _all_data[16];
		};
		U() {
			key = {};
			//typed_symbol = 0;
		}
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

	TKeyType type = (TKeyType)0;
	TKeyFlags keyFlags = (TKeyFlags)0;
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














