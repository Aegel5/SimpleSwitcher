
LRESULT CALLBACK Hooker::HookerKeyboard::LowLevelKeyboardProc(
	_In_  int nCode,
	_In_  WPARAM wParam,
	_In_  LPARAM lParam
) {
	if (nCode < 0) {
		return CallNextHookEx(0, nCode, wParam, lParam);
	}

	Message_KeyType msg_type;
	Message_Hotkey msg_hotkey;
	bool send_key = false;

	GETCONF;

	auto process = [&]() -> bool {
		KBDLLHOOKSTRUCT* k = (KBDLLHOOKSTRUCT*)lParam;

		TKeyCode vkCode = (TKeyCode)k->vkCode;
		KeyState curKeyState = GetKeyState(wParam);
		bool isInjected = TestFlag(k->flags, LLKHF_INJECTED);
		bool is_low_inject = TestFlag(k->flags, LLKHF_LOWER_IL_INJECTED);
		bool isAltDown = TestFlag(k->flags, LLKHF_ALTDOWN);
		bool isSysKey = wParam == WM_SYSKEYDOWN || wParam == WM_SYSKEYUP;
		bool isExtended = TestFlag(k->flags, LLKHF_EXTENDED);
		bool is_pressed = !TestFlag(k->flags, LLKHF_UP);
		auto scan_code = k->scanCode;

		LOG_ANY(
			L"KEY_MSG: {}({:x}) {},scan=0x{:x},inject={},low_inject={},altdown={},syskey={},extended={},is_pressed={},flags=0x{:b}",
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
			k->flags
		);

		if (k->vkCode > 255) {
			LOG_ANY(L"k->vkCode > 255: {}", k->vkCode);
		}

		if (scan_code == 541) {
			LOG_ANY(L"skip bugged lctrl");
			return 0;
		}

		if (isInjected) {
			if (InjectSkipper::Inst().IsInjectOur())
				return 0;
		}

		CHotKey possible;
		std::swap(possible, possible_hk_up); // сразу очищаем

		msg_type.vkCode = vkCode;
		msg_type.scan_ext = { (TScanCode)scan_code, isExtended };
		msg_type.keyState = curKeyState;
		send_key = true; // обрабатываем нажатие, если не будет запрета

		curKeys.Update(vkCode, curKeyState); // сразу обновляем
		const auto& curk = curKeys.GetOneValueHotKey();

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
				return 0;
			bool need_our_action = false;
			for (const auto& [hk, key] : cfg->All_hot_keys()) {
				if (!key.GetKeyup() && (!key.IsDouble() || curKeys.IsDouble()) && check_is_our_key(key, curk)) {
					need_our_action = true;
					possible_hk_up.Clear(); // очищаем, потому что могли заполнить прямо в этом цикле.
					if (!curKeys.IsHold()) { // но даже если и холд, клавиши нужно запретить.
						msg_hotkey.hotkey = key;
						msg_hotkey.hk = hk;
					}
					break;
				}
				if (key.GetKeyup() && check_is_our_key(key, curk)) {
					possible_hk_up = curk; // без break
					msg_type.hk = hk; // уведомим, что это наша клавиша.
				}
			}
			if (need_our_action) {
				// у нас есть такой хот-кей, запрещаем это событие для программы.
				disable_up = curk.ValueKey(); // up тоже будет в будущем запрещать.
				LOG_ANY(L"Key {} was disabled(down)", CHotKey::ToString(disable_up));
				return 1;
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
					LOG_ANY(L"Key {} was disabled(fix)", CHotKey::ToString(vkCode));
					disable_up = vkCode;
					return 1; // пока запрещаем, потом заново отошлем...
				}
			}
			return 0;
		}

		if (curKeyState == KeyState::KEY_STATE_UP) {
			if (disable_up == vkCode) {
				LOG_ANY(L"Key {} was disabled(up)", CHotKey::ToString(disable_up));
				disable_up = 0;
				return 1;
			}
			else {
				// ищем наш хот-кей.
				// даже если нашли, up никогда не запрещаем.
				if (!possible.IsEmpty()) {
					for (const auto& [hk, key] : cfg->All_hot_keys()) {
						if (key.GetKeyup() && check_is_our_key(key, possible)) {
							msg_hotkey.hotkey = key;
							msg_hotkey.hk = hk;
							break;
						}
					}
				}
			}
		}

		return 0;
		};

	if (nCode == HC_ACTION) {
		auto res = process();
		if (curKeys.Size() == 0) {
			disable_up = 0;
		}
		if (
			!res && send_key) {
			Worker()->PostMsg(std::move(msg_type));
		}
		if (msg_hotkey.hk != hk_NULL) {
			Worker()->PostMsg(std::move(msg_hotkey));
		}
		if (res)
			return 1; // запрет
	}


	if (cfg->EnableKeyLoggerDefence) {
		return 0;
	}
	else {
		return CallNextHookEx(0, nCode, wParam, lParam);
	}

}
