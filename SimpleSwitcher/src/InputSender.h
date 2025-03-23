#pragma once

struct InputSender
{
private:
	std::vector<INPUT> list;
public:

	void Send()
	{
		if (list.empty())
			return;
		for (auto& i : list)
		{
			LOG_INFO_2(L"SEND %s %s", TestFlag(i.ki.dwFlags, KEYEVENTF_KEYUP) ? L"UP" : L"DW", CHotKey::ToString(i.ki.wVk));
		}

		bool doPause = false;

		// https://github.com/Aegel5/SimpleSwitcher/issues/53
		//if (g_hooker->m_sTopProcName == L"notepad.exe") { 
		//	// костыль для нового notepad...
		//	// не используем по дефолту, так как работаем медленнее...
		//	doPause = true; 
		//}

		if (doPause) {
			//for (auto& elem : list) {
			//	InjectSkipper::Inst().AddOur(1);
			//	auto res = SendInput(1, &elem, sizeof(INPUT));
			//	IFW_LOG(res == 1);
			//	Sleep(1);
			//}
		}
		else {
			InjectSkipper::LocSkipper loc;
			IFW_LOG(SendInput((UINT)list.size(), &list[0], sizeof(INPUT)) == list.size()); // синхронно вызывается наш хук.
		}

		return;
	}

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
	void AddScanCode(const TKeyBaseInfo& key, KeyState keyState = KEY_STATE_DOWN)
	{
		if (key.shift_key != 0) {
			Add(key.shift_key, keyState);
		}

		if (key.scan_code.scan == 0) {
			Add(key.vk_code, keyState);
		}
		else {
			Add(0, keyState, key.scan_code);
		}
	}
	void AddPressVk(TKeyCode vk, int num = 1)	{
		for (int i = 0; i < num; i++) {
			Add(vk, KEY_STATE_DOWN);
			Add(vk, KEY_STATE_UP);
		}
	}
	static void SendVkKey(TKeyCode vk, int num = 1) {
		InputSender is;
		is.AddPressVk(vk, num);
		is.Send();
	}
	void AddDownVk(const CHotKey& key) {
		for (const auto& k : key | std::views::reverse) {
			Add(k, KEY_STATE_DOWN);
		}
	}
	void AddUpVk(const CHotKey& key) {
		for (const auto& k : key) {
			Add(k, KEY_STATE_UP);
		}
	}
	static void SendWithPause(const CHotKey& key)
	{
		InputSender is;
		is.AddDownVk(key);
		is.Send();
		Sleep(1);
		is.Clear();
		is.AddUpVk(key);
		is.Send();
	}
	void AddPressVk(const CHotKey& key)
	{
		AddDownVk(key);
		AddUpVk(key);
	}
	static void SendHotKey(const CHotKey& key) {
		InputSender is;
		is.AddPressVk(key);
		is.Send();
	}
	void AddPressBase(const TKeyBaseInfo& key)
	{
		AddScanCode(key, KEY_STATE_DOWN);
		AddScanCode(key, KEY_STATE_UP);
	}
	static void SendKeys(const TKeyRevert& sendData) {

		InputSender inputSender;

		LOG_INFO_1(L"Send %u keys", sendData.size());

		for (const auto& key : sendData)
		{
			inputSender.AddPressBase(key);
		}

		inputSender.Send();
	}

};