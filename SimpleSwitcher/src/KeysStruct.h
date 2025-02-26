#pragma once


enum TKeyType : TUInt8
{
	KEYTYPE_NONE,

	KEYTYPE_SYMBOL,
	KEYTYPE_SPACE,
	KEYTYPE_BACKSPACE,

	KEYTYPE_COMMAND_CLEAR,
	KEYTYPE_COMMAND_NO_CLEAR,
};

enum class TKeyFlags : TUInt8 {
	None = 0,
	SYMB_SEPARATE_REVERT = 1,
};
DEFINE_ENUM_FLAG_OPERATORS(TKeyFlags);


// базовая структура для хранения нажатия одной клавиши.
struct TKeyBaseInfo {
	union {
		TUInt64 _unused;
		struct {
			TKeyCode vk_code;
			TScanCode scan_code;
			TKeyCode shift_key;
		};
	};
};

struct TKeyHookInfo
{
	struct {
		TKeyBaseInfo key;
		TInt64 _random_data;

	} crypted;


	TKeyBaseInfo& key() {
		return crypted.key;
	}

	TKeyType type;
	TKeyFlags keyFlags;

	TKeyHookInfo() {}
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

inline const char* GetKeyStateName(KeyState state)
{
	switch (state)
	{
	case KEY_STATE_DOWN:
		return "DOWN";
	case KEY_STATE_UP:
		return "UP  ";
	default:
		return "NONE";
	}
}

template<int c_size>
class CKeyInfoList
{
public:
	void Add(CHotKey& key)
	{
		if (m_size >= c_size)
			return;
		m_list[m_size] = key;
		++m_size;
	}
	void DelLast()
	{
		if(m_size)
			--m_size;
	}
	void Clear()	{ m_size = 0; }
	TUInt32 Size() { return m_size; }
	CHotKey* begin()	{ return m_list; }
	CHotKey* end()	{ return m_list + m_size; }
private:
	CHotKey m_list[c_size];
	TUInt32 m_size = 0;
};

struct CurStateWrapper {

	CHotKey state;
	TScanCode scan_code;
	std::map<int, ULONGLONG> times;
	bool isSkipRepeat = false;

	void Update(KBDLLHOOKSTRUCT* kStruct, KeyState curKeyState) {

		scan_code = kStruct->scanCode;
		TKeyCode vkCode = (TKeyCode)kStruct->vkCode;
		bool isAltDown = TestFlag(kStruct->flags, LLKHF_ALTDOWN);

		isSkipRepeat = false;


		if (curKeyState == KEY_STATE_UP)
		{
			state.SetHold(false);
			auto it = times.find(vkCode);
			if (it != times.end()) { times.erase(it); }
			if (!state.Remove(vkCode))
			{
				if (CHotKey::IsKnownMods(vkCode))
				{
					IFS_LOG(SW_ERR_UNKNOWN, L"Key was already upped %s", CHotKey::GetName(vkCode));
				}
			}
		}
		else if (curKeyState == KEY_STATE_DOWN)
		{
			if (isAltDown && vkCode == VK_LCONTROL) {
				LOG_INFO_1(L"Skip fake LCtrl");
			}
			else {
				CHotKey hk_save = state;
				state.Add(vkCode, CHotKey::ADDKEY_ORDERED | CHotKey::ADDKEY_ENSURE_ONE_VALUEKEY);
				times[vkCode] = GetTickCount64() + 10000;
				if (state.Compare(hk_save))
				{
					if (state.IsHold()) // already hold
					{
						isSkipRepeat = true;
					}
					else
					{
						state.SetHold(true);
					}
				}
				else
				{
					// была нажата другая клавиша, сбрасываем флаг hold
					state.SetHold(false);
				}
			}
		}
		else
		{
			// err?
		}

		// Может быть так, что событие UP - не придет. Поэтому, очистим базу, если прошло много времени.
		while (true) {
			bool found = false;
			for (auto& el : times) {
				if (GetTickCount64() > el.second) {
					if (!(GetAsyncKeyState(el.first) & 0x8000)) {
						std::wstring s1;
						CHotKey::ToString(el.first, s1);
						LOG_WARN(L"delete key because it not down now %s", s1.c_str());
						times.erase(el.first);
						state.Remove(el.first);
						found = true;
						break;
					}
				}
			}
			if (!found) {
				break;
			}
		}
	}
};









