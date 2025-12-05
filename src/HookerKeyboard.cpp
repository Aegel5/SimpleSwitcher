
LRESULT CALLBACK Hooker::HookerKeyboard::LowLevelKeyboardProc(
	_In_  int nCode,
	_In_  WPARAM wParam,
	_In_  LPARAM lParam
) {

	if (nCode != HC_ACTION) {
		return CallNextHookEx(0, nCode, wParam, lParam);
	}

	Message_Hotkey msg_hotkey;
	bool need_disable_event = false;

	auto process = [&]() {

		Message_KeyType msg_type;
		bool double_exists = false;

		GETCONF;

		KBDLLHOOKSTRUCT* k = (KBDLLHOOKSTRUCT*)lParam;

		TKeyCode vkCode = (TKeyCode)k->vkCode;
		auto curKeyState = GetKeyState(wParam);
		bool isInjected = TestFlag(k->flags, LLKHF_INJECTED);
		bool is_low_inject = TestFlag(k->flags, LLKHF_LOWER_IL_INJECTED);
		bool isAltDown = TestFlag(k->flags, LLKHF_ALTDOWN);
		bool isSysKey = wParam == WM_SYSKEYDOWN || wParam == WM_SYSKEYUP;
		bool isExtended = TestFlag(k->flags, LLKHF_EXTENDED);
		bool is_pressed = !TestFlag(k->flags, LLKHF_UP);
		auto scan_code = k->scanCode;
		int iscaps = -1;
		if (curKeyState == KEY_STATE_DOWN) iscaps = Utils::IsCapslockEnabled() ? 1 : 0;
		msg_type.is_caps = iscaps == 1;

		LOG_ANY(
			L"KEY_MSG: {}({:x}) {},scan=0x{:x},inject={},low_inject={},altdown={},syskey={},extended={},is_pressed={},flags=0x{:b},caps={}",
			CHotKey::ToString(vkCode),
			vkCode,
			(curKeyState == KEY_STATE_UP ? L"UP" : L"DOWN"),
			scan_code,
			isInjected,
			is_low_inject,
			isAltDown,
			isSysKey,
			isExtended,
			is_pressed,
			k->flags,
			iscaps
		);

		if (k->vkCode > 255) {
			LOG_ANY(L"k->vkCode > 255: {}", k->vkCode);
		}

		if (k->vkCode == 0) {
			LOG_ANY(L"skip vk 0");
			return;
		}

		if (scan_code == 541) {
			LOG_ANY(L"skip bugged lctrl");
			return;
		}

		if (is_low_inject && cfg->SkipLowLevelInjectKeys) {
			LOG_ANY(L"skip low_inject");
			return;
		}

		if (isInjected) {
			if (InjectSkipper::Inst().IsSkipInject())
				return;
		}

		CHotKey possible_up;
		std::swap(possible_up, possible_hk_up); // сразу очищаем

		msg_type.vkCode = vkCode;
		msg_type.scan_ext = { (TScanCode)scan_code, isExtended };

		curKeys.Update(vkCode, curKeyState); // сразу обновляем
		auto curk = curKeys.GetOneValueHotKey();


		auto request_disable = [&]() {
			auto isDown = curKeyState == KEY_STATE_DOWN;
			if (isDown) {
				if (curk.ValueKey() != vkCode) {
					LOG_ANY("CRITICAL: urk.ValueKey() != vkCode");
				}
				disable_up = vkCode; // up тоже будет в будущем запрещать.
				// убираем из состояния сразу
				curKeys.Update(vkCode, KeyState::KEY_STATE_UP);
				curk = curKeys.GetOneValueHotKey(); // перечитываем
			}
			else {
				disable_up = 0;
			}

			need_disable_event = true;
			LOG_ANY(L"Key {} was disabled({})", CHotKey::ToString(vkCode), isDown ? L"down": L"up");
		};

		curKeys.DebugPrint();

		int check_disabled_status = -1;

		auto check_is_our_key = [&check_disabled_status, cfg](const CHotKey& k1, const CHotKey& k2) {
			if (k1.Compare(k2, CHotKey::COMPARE_IGNORE_KEYUP | CHotKey::COMPARE_IGNORE_DOUBLE)) {
				if (check_disabled_status == -1) {
					check_disabled_status = cfg->IsSkipProgramTop() ? 1 : 0;
				}
				return check_disabled_status == 0;
			}
			return false;
			};

		if (curKeyState == KeyState::KEY_STATE_DOWN) {
			if (curk.IsEmpty())
				return;
			int need_our_action = 0;
			for (const auto& [hk, key] : cfg->All_hot_keys()) { // всегда полный обход всего цикла
				if (!check_is_our_key(key, curk)) continue;
				double_exists |= key.IsDouble();
				if (!key.GetKeyup()) {

					int prior = 0;
					if (!key.IsDouble() && curKeys.IsDouble()) prior = 5;
					if (key.IsDouble() == curKeys.IsDouble()) prior = 10;

					if (prior > need_our_action) {
						need_our_action = prior;

						// очищаем, возможное нахождение up
						{
							possible_hk_up.Clear();
							msg_type.hk = hk_NULL;
						}

						if (!curKeys.IsHold()) { // но даже если и холд, клавиши нужно запретить.
							msg_hotkey.hotkey = key;
							msg_hotkey.hk = hk;
						}
					}
				}
				else {
					possible_hk_up = curk;
					//LOG_ANY_4("new possible_hk_up {}", possible_hk_up);
					msg_type.hk = hk; // уведомим, что это наша клавиша.
				}
			}
			if (need_our_action) {
				// у нас есть такой хот-кей, запрещаем это событие для программы.
				request_disable();
			}
			else {
				if (curk.Size() == 3
					&& curk.HasKey(VK_LMENU, true)
					&& curk.HasKey(VK_CONTROL, false)
					&& !curk.IsKnownMods(vkCode)
					&& vkCode == curk.ValueKey()
					&& cfg->fixRAlt
					&& cfg->fixRAlt_lay_ != 0
					) {
					LOG_ANY(L"fix ctrl+alt");
					if (!curKeys.IsHold()) { // пока просто запрещаем
						Worker()->PostMsg(Message_Hotkey{ .fix_ralt = true, .hotkey = curk });
					}
					request_disable();
				}
			}
		}

		if (curKeyState == KeyState::KEY_STATE_UP) {
			if (disable_up == vkCode) {
				request_disable();
			}
			else {
				// ищем наш хот-кей.
				// даже если нашли, up никогда не запрещаем.
				if (!possible_up.IsEmpty()) {
					for (const auto& [hk, key] : cfg->All_hot_keys()) {
						if (!check_is_our_key(key, possible_up)) continue;
						double_exists |= key.IsDouble();
						if (key.GetKeyup()) {
							msg_hotkey.hotkey = key;
							msg_hotkey.hk = hk;
						}
					}
				}
			}
		}

		if (curKeys.Size() == 0) {
			disable_up = 0;
		}

		if (msg_hotkey.hk != hk_NULL) {

			if (msg_hotkey.hotkey.GetKeyup() && last_mouse_click_time > curKeys.StartOfLastHotKey()) {
				// Possible Ctrl+Click in IDE
				LOG_ANY(L"HotKey {} was canceled by mouse click", msg_hotkey.hotkey.ToString());
			}
			else {

				int delay = 0;
				if (!msg_hotkey.hotkey.IsDouble() && double_exists) {
					delay = cfg->quick_press_ms; // придется подождать.
					msg_hotkey.delayed_from = GetTickCount64();
				}

				LOG_ANY(L"post {} {}. has_double {}", msg_hotkey.hotkey.ToString(), (int)msg_hotkey.hk, double_exists);
				msg_hotkey.cur_down = curKeys.AllKeys(); // список текущих нажатых.
				Worker()->PostMsg(std::move(msg_hotkey), delay);
			}
		}
		else {
			// не хот-кей, отсылаем только down для сохранения слов.
			if (curKeyState == KeyState::KEY_STATE_DOWN) {
				if (!curk.IsEmpty()) {
					msg_type.cur_hotKey = curk; 
					Worker()->PostMsg(std::move(msg_type));
				}
			}
		}
	};


	process();

	if (need_disable_event && (g_enabled.IsEnabled() || msg_hotkey.hk == hk_ToggleEnabled)){
		// делаем вид, что клавиша не была нажата.
		return 1; 
	}

	return CallNextHookEx(0, nCode, wParam, lParam);

}
