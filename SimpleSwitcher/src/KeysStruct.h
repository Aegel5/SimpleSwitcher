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

	struct {
		TKeyBaseInfo key;
		TInt64 _random_data;
	} crypted;


	TKeyBaseInfo& key() {
		return crypted.key;
	}

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

	int Size() {
		return all_keys.size();
	}

	bool IsDownNow(TKeyCode vk) {
		return all_keys.contains(vk);
	}

	std::generator<TKeyCode> EnumVk() {
		for (const auto& it : all_keys) {
			co_yield it.first;
		}
	}

	void Update(KBDLLHOOKSTRUCT* kStruct, KeyState curKeyState) {

		CheckOk();

		TKeyCode vkCode = (TKeyCode)kStruct->vkCode;
		bool isAltDown = TestFlag(kStruct->flags, LLKHF_ALTDOWN);

		if (curKeyState == KEY_STATE_DOWN) {
			if (isAltDown && vkCode == VK_LCONTROL) {
				LOG_INFO_1(L"fake LCtrl");
			}
			state.Add3(vkCode, CHotKey::ADDKEY_CHECK_EXIST | CHotKey::ADDKEY_ENSURE_ONE_VALUEKEY);
			all_keys[vkCode] = GetTickCount64();
		}

		if (curKeyState == KEY_STATE_UP){
			state.Remove(vkCode);
			if (all_keys.erase(vkCode) == 0) {
				LOG_WARN(L"Key was already upped {}", CHotKey::GetName(vkCode));
			}
		}

	}
private:
	std::map<TKeyCode, ULONGLONG> all_keys;
	void CheckOk() {
		std::vector<TKeyCode> to_del;
		for (const auto& it : all_keys) {
			if (GetTickCount64() > (it.second + 10000)) {
				if (!(GetAsyncKeyState(it.first) & 0x8000)) { // TODO: не понятно как это работает в remote сценарии....
					to_del.push_back(it.first);
				}
			}
		}
		for (auto it : to_del) {
			LOG_WARN(L"delete key because it not down now {}", CHotKey::ToString(it));
			all_keys.erase(it);
			state.Remove(it);
		}
	}
};









