#pragma once
#include <deque>
#include <vector>

enum  InputSendPos
{
	INPUTSEND_BACK
};
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
	TStatus Add(WORD key, KeyState state, InputSendPos pos = INPUTSEND_BACK, bool isScanCode = false)
	{
		if(key == 0)
			RETURN_SUCCESS;

		INPUT cur = {0};
		cur.type = INPUT_KEYBOARD;
		if (isScanCode) {
			cur.ki.wScan = key;
			SetFlag(cur.ki.dwFlags, KEYEVENTF_SCANCODE);
		}
		else {
			cur.ki.wVk = key;
		}

		if (state == KEY_STATE_UP)
			SetFlag(cur.ki.dwFlags, KEYEVENTF_KEYUP);

		if(pos == INPUTSEND_BACK)
			list.push_back(cur);
		else
		{
			IFS_RET(SW_ERR_UNSUPPORTED);
		}

		RETURN_SUCCESS;
	}
	TStatus AddDown(CHotKey& key, InputSendPos pos = INPUTSEND_BACK)
	{
		if(key.Size() == 0)
			RETURN_SUCCESS;
		for(TKeyCode* k = key.ModsBegin(); k != key.ModsEnd(); ++k)
		{
			IFS_RET(Add(*k, KEY_STATE_DOWN, pos));
		}
		IFS_RET(Add(key.ValueKey(), KEY_STATE_DOWN, pos));
		RETURN_SUCCESS;
	}
	TStatus AddScanCode(TKeyBaseInfo& key, KeyState keyState = KEY_STATE_DOWN, InputSendPos pos = INPUTSEND_BACK)
	{
		if (key.shift_key != 0) {
			IFS_RET(Add(key.shift_key, keyState, pos));
		}

		//IFS_RET(Add(key.vk_code, keyState, pos, false));

		if (key.scan_code == 0) {
			IFS_RET(Add(key.vk_code, keyState, pos, false));
		}
		else {
			IFS_RET(Add(key.scan_code, keyState, pos, true));
		}

		RETURN_SUCCESS;
	}
	TStatus AddUp(CHotKey& key, InputSendPos pos = INPUTSEND_BACK)
	{
		if (key.Size() == 0)
			RETURN_SUCCESS;
		IFS_RET(Add(key.ValueKey(), KEY_STATE_UP, pos));
		for (TKeyCode* k = key.ModsBegin(); k != key.ModsEnd(); ++k)
		{
			IFS_RET(Add(*k, KEY_STATE_UP, pos));
		}
		RETURN_SUCCESS;
	}
	TStatus AddPressVk(CHotKey& key, InputSendPos pos = INPUTSEND_BACK)
	{
		IFS_RET(AddDown(key, pos));
		IFS_RET(AddUp(key, pos));
		RETURN_SUCCESS;
	}
	TStatus AddPressBase(TKeyBaseInfo& key, InputSendPos pos = INPUTSEND_BACK)
	{
		IFS_RET(AddScanCode(key, KEY_STATE_DOWN, pos));
		IFS_RET(AddScanCode(key, KEY_STATE_UP, pos));
		RETURN_SUCCESS;
	}
};