#pragma once

// Класс для отслеживания текущего состояния нажатых клавиш

class CurStateWrapper {
	std::map<TKeyCode, ULONGLONG> all_keys;
	CHotKey one_value; 
	CHotKey multi_value; 
	TKeyCode vk_last_down = 0;
	ULONGLONG last_down_time = 0;
	bool is_hold = false;
	int cnt_quick_press = 0;
	TKeyCode possible_vk_quick = 0;
	void Clear() {
	}
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

		// считаем, что для системы теперь все отжато, поэтому просто чистим.
		Clear();

	}

	void Update(TKeyCode vkCode, KeyState curKeyState) {

		CheckOk();

		is_hold = false;
		if (curKeyState == KeyState::KEY_STATE_UP) {

			// QUICK PRESS
			if (vkCode == vk_last_down) {
				possible_vk_quick = vkCode;
			}
			else {
				possible_vk_quick = 0;
				cnt_quick_press = 0;
			}

			vk_last_down = 0;
		}
		else {

			// QUICK PRESS
			{
				bool ok_quick = false;
				if (possible_vk_quick == vkCode) {
					// снова нажали ту же клавишу, теперь проверим время.
					if (GetTickCount64() - last_down_time <= conf_get_unsafe()->quick_press_ms) {
						// засчитываем за срабатывание
						ok_quick = true;
					}
				}
				if (ok_quick) 
					cnt_quick_press++;
				else 
					cnt_quick_press = 0; // что-то пошло не так.
				possible_vk_quick = 0; // очищаем в любом случае.
			}

			// HOLD
			is_hold = vk_last_down == vkCode && vkCode != 0;
			vk_last_down = vkCode;
			last_down_time = GetTickCount64();
		}


		// update stores
		if (curKeyState == KEY_STATE_DOWN) {
			one_value.Add3(vkCode, CHotKey::ADDKEY_CHECK_EXIST | CHotKey::ADDKEY_ENSURE_ONE_VALUEKEY);
			all_keys[vkCode] = GetTickCount64();
		}
		else {
			one_value.Remove(vkCode);
			if (all_keys.erase(vkCode) == 0) {
				LOG_WARN(L"Key was already upped {}", CHotKey::ToString(vkCode));
			}
		}

		// double press мы устанавливаем в отличии от up.
		one_value.SetDouble(cnt_quick_press == 1);


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