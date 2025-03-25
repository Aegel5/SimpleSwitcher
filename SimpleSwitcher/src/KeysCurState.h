#pragma once

// Класс для отслеживания текущего состояния нажатых клавиш

class CurStateWrapper {
	std::map<TKeyCode, ULONGLONG> all_keys;
	CHotKey one_value; 
	CHotKey multi_value; 
	TKeyCode vk_last_down = 0;
	bool is_hold = false;
public:

	bool IsHold() {	return is_hold;	}
	const CHotKey& GetOneValueHotKey() { return one_value; }
	int Size() { return all_keys.size(); }
	bool IsDownNow(TKeyCode vk) { return all_keys.contains(vk); }
	std::generator<TKeyCode> EnumVk() {
		for (const auto& it : all_keys) {
			co_yield it.first;
		}
	}

	void UpAllKeys() {

		if (Size() == 0) return;

		InputSender inputSender;
		if (IsDownNow(VK_LMENU) || IsDownNow(VK_LWIN)) {
			// если нажата только клавиша alt - то ее простое отжатие даст хрень - нужно отжать ее еще раз
			//inputSender.Add(VK_LMENU, KEY_STATE_DOWN);
			//inputSender.Add(VK_LMENU, KEY_STATE_UP); 	
			inputSender.Add(VK_CAPITAL, KEY_STATE_UP);
		}
		for (const auto& key : EnumVk()) {
			inputSender.Add(key, KEY_STATE_UP);
		}

		inputSender.Send();

	}

	void Update(TKeyCode vkCode, KeyState curKeyState) {

		is_hold = false;
		if (curKeyState != KeyState::KEY_STATE_DOWN) {
			vk_last_down = 0;
		}
		else {
			is_hold = vk_last_down == vkCode && vkCode != 0;
			vk_last_down = vkCode;
		}

		CheckOk();

		if (curKeyState == KEY_STATE_DOWN) {
			one_value.Add3(vkCode, CHotKey::ADDKEY_CHECK_EXIST | CHotKey::ADDKEY_ENSURE_ONE_VALUEKEY);
			all_keys[vkCode] = GetTickCount64();
		}

		if (curKeyState == KEY_STATE_UP) {
			one_value.Remove(vkCode);
			if (all_keys.erase(vkCode) == 0) {
				LOG_WARN(L"Key was already upped {}", CHotKey::ToString(vkCode));
			}
		}

	}
private:

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
			one_value.Remove(it);
		}
	}
};