#pragma once


enum TKeyType
{
	KEYTYPE_NONE,

	KEYTYPE_SYMBOL,
	KEYTYPE_SPACE,
	KEYTYPE_BACKSPACE,

	KEYTYPE_COMMAND_CLEAR,
	KEYTYPE_COMMAND_NO_CLEAR,
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









