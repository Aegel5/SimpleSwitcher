#pragma once
#include <deque>
#include <vector>


struct InputSender
{
private:
	//typedef std::deque<INPUT> TInputList;
	typedef std::vector<INPUT> TInputList;
	TInputList list;
public:

	INPUT* begin() { return &list[0]; }
	INPUT* end() { return &list[0] + list.size(); }

	TStatus Send();

	void Clear()
	{
		list.clear();
	}
	void Add(TKeyCode key, KeyState state, TScanCode_Ext scan = {})
	{
		INPUT cur;
		SwZeroMemory(cur);
		cur.type = INPUT_KEYBOARD;

		if (scan.scan != 0) {
			cur.ki.wScan = scan.scan;
			SetFlag(cur.ki.dwFlags, KEYEVENTF_SCANCODE);
			if (scan.is_ext) {
				SetFlag(cur.ki.dwFlags, KEYEVENTF_EXTENDEDKEY);
			}
		}
		else {
			if (key == 0) {
				LOG_WARN(L"try add empty key");
				return;
			}
			cur.ki.wVk = key;
		}

		if (state == KEY_STATE_UP)
			SetFlag(cur.ki.dwFlags, KEYEVENTF_KEYUP);

		list.push_back(cur);
	}
	TStatus AddDown(CHotKey& key)
	{
		if(key.Size() == 0)
			RETURN_SUCCESS;
		for(TKeyCode* k = key.ModsBegin(); k != key.ModsEnd(); ++k)
		{
			Add(*k, KEY_STATE_DOWN);
		}
		Add(key.ValueKey(), KEY_STATE_DOWN);
		RETURN_SUCCESS;
	}
	TStatus AddScanCode(const TKeyBaseInfo& key, KeyState keyState = KEY_STATE_DOWN)
	{
		if (key.shift_key != 0) {
			Add(key.shift_key, keyState);
		}

		//IFS_RET(Add(key.vk_code, keyState, pos, false));

		if (key.scan_code.scan == 0) {
			Add(key.vk_code, keyState);
		}
		else {
			Add(0, keyState, key.scan_code);
		}

		RETURN_SUCCESS;
	}
	TStatus AddUp(CHotKey& key)
	{
		if (key.Size() == 0)
			RETURN_SUCCESS;
		Add(key.ValueKey(), KEY_STATE_UP);
		for (const TKeyCode* k = key.ModsBegin(); k != key.ModsEnd(); ++k)
		{
			Add(*k, KEY_STATE_UP);
		}
		RETURN_SUCCESS;
	}
	TStatus AddPressVk(CHotKey& key)
	{
		IFS_RET(AddDown(key));
		IFS_RET(AddUp(key));
		RETURN_SUCCESS;
	}
	TStatus AddPressBase(const TKeyBaseInfo& key)
	{
		IFS_RET(AddScanCode(key, KEY_STATE_DOWN));
		IFS_RET(AddScanCode(key, KEY_STATE_UP));
		RETURN_SUCCESS;
	}
};