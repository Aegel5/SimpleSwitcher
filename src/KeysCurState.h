#pragma once

// Класс для отслеживания текущего состояния нажатых клавиш

class CurStateWrapper {
	std::map<TKeyCode, TimePoint> all_keys;
	CHotKey one_value; 
	//CHotKey multi_value; 
	TimePoint hotkey_start_time;

	// double
	int double_cnt {};
	TKeyCode last_down_vk {};
	TimePoint last_down_time{};

	// hold
	TKeyCode last_down_vk_clear_up{};
	bool is_hold{};

	//struct Event {
	//	TKeyCode vk{};
	//	bool isDown{};
	//	TimePoint time{};
	//};
	//Event events[2];




public:

	//void Clear() {
	//	all_keys.clear();
	//	one_value.Clear();
	//	vk_last_down = 0;
	//	is_hold = false;
	//	cnt_quick_press = 0;
	//	possible_vk_quick = 0;
	//}

	auto AllKeys()const {
		vector<TKeyCode> keys;
		for (const auto& k : all_keys) {
			keys.push_back(k.first);
		}
		return keys;
	}

	const TimePoint& StartOfLastHotKey()const {
		return hotkey_start_time;
	}

	bool IsHold() const { return is_hold; }
	bool IsDouble() const { return last_down_vk_clear_up != 0 && double_cnt != 0; } // проверяем четность, чтобы 3-е нажатие не дало hit, а 4-е дало.
	int DoubleCnt() const {
		return double_cnt;
	}

	const CHotKey& GetOneValueHotKey()const { return one_value; }
	int Size() const { return all_keys.size(); }
	bool IsDownNow(TKeyCode vk) const { return all_keys.contains(vk); }
	std::generator<TKeyCode> EnumVk() const {
		for (const auto& it : all_keys) {
			co_yield it.first;
		}
	}



	void DebugPrint() const {
		if (GetLogLevel() < LOG_LEVEL_2) 
			return;
		wstring str;
		for (const auto& key : EnumVk()) {
			str += CHotKey::ToString(key);
			str += L" ";
		}
		LOG_ANY(L"all={}, one_value={}", str, one_value.ToString());
	}

	void Update(TKeyCode vkCode, bool isDown) {

		is_hold = false;

		if (isDown) {
			if (all_keys.empty()) {
				hotkey_start_time.SetToNow(); // todo: добавить очистку.
			}
			if (vkCode == last_down_vk_clear_up) {
				is_hold = true;
			}
			if (!is_hold && vkCode == last_down_vk && last_down_time.DeltToNowMs() <= conf_get_unsafe()->quick_press_ms) {
				double_cnt++;
			}
			else {
				double_cnt = 0;
			}
			last_down_vk_clear_up = last_down_vk = vkCode;
			last_down_time.SetToNow();
		}else{
			last_down_vk_clear_up = 0; // любой up сбрасывает hold.
		}

		// update stores
		if (isDown) {
			one_value.Add(vkCode, CHotKey::ADDKEY_CHECK_EXIST | CHotKey::ADDKEY_ENSURE_ONE_VALUEKEY);
			all_keys[vkCode].SetToNow();
		}
		else {
			one_value.RemoveFirst(vkCode);
			if (all_keys.erase(vkCode) == 0) {
				LOG_WARN(L"Key was already upped {}", CHotKey::ToString(vkCode));
			}
		}

		//events[0] = events[1];
		//events[1] = { vkCode, isDown, TimePoint::Now() };
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
