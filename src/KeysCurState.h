#pragma once

// Класс для отслеживания текущего состояния нажатых клавиш

class CurStateWrapper {
	std::map<TKeyCode, TimePoint> all_keys;
	CHotKey one_value; 
	//CHotKey multi_value; 
	TKeyCode vk_last_down = 0;
	TimePoint last_down_time;
	TimePoint hotkey_start_time;
	bool is_hold = false;
	int cnt_quick_press = 0;
	TKeyCode possible_vk_quick = 0;
public:

	//void Clear() {
	//	all_keys.clear();
	//	one_value.Clear();
	//	vk_last_down = 0;
	//	is_hold = false;
	//	cnt_quick_press = 0;
	//	possible_vk_quick = 0;
	//}

	auto AllKeys() {
		vector<TKeyCode> keys;
		for (const auto& k : all_keys) {
			keys.push_back(k.first);
		}
		return keys;
	}

	const TimePoint& StartOfLastHotKey() {
		return hotkey_start_time;
	}

	bool IsDouble() const { return cnt_quick_press & 1; }
	bool IsHold() {	return is_hold;	}
	const CHotKey& GetOneValueHotKey() { return one_value; }
	int Size() { return all_keys.size(); }
	bool IsDownNow(TKeyCode vk) { return all_keys.contains(vk); }
	std::generator<TKeyCode> EnumVk() {
		for (const auto& it : all_keys) {
			co_yield it.first;
		}
	}



	void DebugPrint() {
		if (GetLogLevel() < LOG_LEVEL_2) 
			return;
		wstring str;
		for (const auto& key : EnumVk()) {
			str += CHotKey::ToString(key);
			str += L" ";
		}
		LOG_ANY(L"all={}, one_value={}", str, one_value.ToString());
	}

	void Update(TKeyCode vkCode, KeyState curKeyState) {

		CheckOk();

		if (all_keys.empty() && curKeyState == KeyState::KEY_STATE_DOWN) {
			hotkey_start_time.SetNow();
		}

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
					if (last_down_time.DeltToNowMs() <= conf_get_unsafe()->quick_press_ms) {
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
			last_down_time.SetNow();
		}


		// update stores
		if (curKeyState == KEY_STATE_DOWN) {
			one_value.Add(vkCode, CHotKey::ADDKEY_CHECK_EXIST | CHotKey::ADDKEY_ENSURE_ONE_VALUEKEY);
			all_keys[vkCode].SetNow();
		}
		else {
			one_value.RemoveFirst(vkCode);
			if (all_keys.erase(vkCode) == 0) {
				LOG_WARN(L"Key was already upped {}", CHotKey::ToString(vkCode));
			}
		}
	}
private:

	void CheckOk() {

		// !!!! 
		// https://github.com/Aegel5/SimpleSwitcher/issues/91
		// Возможно придется вернуть.

		// Это убираем, так как не понятно, нужно ли, а проблемы может доставлять...

		/*

		std::vector<TKeyCode> to_del;
		for (const auto& it : all_keys) {
			if (it.second.DeltToNow() > 10s) {

				// Нужен thread queue??
				if (!(GetAsyncKeyState(it.first) & 0x8000)) { // TODO: не понятно как это работает в remote сценарии....
					to_del.push_back(it.first);
				}
			}
		}
		for (auto it : to_del) {
			LOG_WARN(L"delete key because it not down now {}", CHotKey::ToString(it));
			all_keys.erase(it);
			one_value.RemoveFirst(it);
		}

		*/
	}
};
