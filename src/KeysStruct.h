#pragma once



enum TKeyType : uint8_t
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

// Базовая НЕЗАВИСИМАЯ (т.е. не зависит от символов слева или справа) структура для хранения нажатия одной клавиши.
struct TKeyBaseInfo {

	TKeyCode vk_code = 0;
	TScanCode_Ext scan_code; // имеет приоритет над vk_code
	bool is_shift = false;
	bool is_caps = false;
	//bool is_letter = false;
	TKeyType type = TKeyType::KEYTYPE_NONE;
	bool space_on_extended = false; // всегда space если extended separation.

	void revert_shift_for_letter() {
		is_shift = !is_shift;
	}

	auto operator<=>(const TKeyBaseInfo&) const = default;
};

using TKeyRevert = std::vector<TKeyBaseInfo>;

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














