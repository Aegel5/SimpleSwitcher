#pragma once
#include <deque>
#include <vector>

enum  InputSendPos
{
	//INPUTSEND_FRONT,
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

	TStatus Send()
	{
		if(list.empty())
			RETURN_SUCCESS;
		for (auto i : list)
		{
			LOG_INFO_2(L"SEND %s %s", i.ki.dwFlags == KEYEVENTF_KEYUP ? L"UP" : L"DW", CHotKey::ToString((TKeyCode)i.ki.wVk).c_str());
		}
		IFW_RET(SendInput((UINT)list.size(), &list[0], sizeof(INPUT)) == list.size());
		RETURN_SUCCESS;
	}
	void Clear()
	{
		list.clear();
	}
	TStatus Add(TKeyCode key, KeyState state, InputSendPos pos = INPUTSEND_BACK)
	{
		if(key == 0)
			RETURN_SUCCESS;

		INPUT cur = {0};
		cur.type = INPUT_KEYBOARD;
		cur.ki.wVk = key;
		if (state == KEY_STATE_UP)
			cur.ki.dwFlags = KEYEVENTF_KEYUP;
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
	TStatus AddPress(CHotKey& key, InputSendPos pos = INPUTSEND_BACK)
	{
		IFS_RET(AddDown(key, pos));
		IFS_RET(AddUp(key, pos));
		RETURN_SUCCESS;
	}
};