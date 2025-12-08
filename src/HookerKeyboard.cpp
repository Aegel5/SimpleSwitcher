
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

		if (curKeys.Size() == 0) { disable_up = 0; } // все отпущено, ничего запрещать не надо.
		curKeys.Update(vkCode, curKeyState); // сразу обновляем
		const auto& curk = curKeys.GetOneValueHotKey();

		auto request_disable = [&]() {
			auto isDown = curKeyState == KEY_STATE_DOWN;
			if (isDown) {
				if (curk.ValueKey() != vkCode) {
					LOG_ANY("CRITICAL: urk.ValueKey() != vkCode");
				}
				disable_up = vkCode; // up тоже будет в будущем запрещать.
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

		bool is_our_key_down = false;

		if (curKeyState == KeyState::KEY_STATE_DOWN) {
			if (curk.IsEmpty())
				return;
			int need_disable = 0;
			for (const auto& [hk, key] : cfg->All_hot_keys()) { // всегда полный обход всего цикла
				if (!check_is_our_key(key, curk)) continue;
				is_our_key_down = true;
				double_exists |= key.IsDouble();
				if (!key.GetKeyup()) {

					int prior = 0;
					if (!key.IsDouble() && curKeys.IsDouble()) prior = 5;
					if (key.IsDouble() == curKeys.IsDouble()) prior = 10;

					if (prior > need_disable) {
						need_disable = prior;

						if (!curKeys.IsHold()) { 
							msg_hotkey.hotkey = key;
							msg_hotkey.hk = hk;
						}
					}
				}
				else {
					possible_hk_up = curk;
				}
			}
			if (need_disable) {
				possible_hk_up.Clear();  // не поддерживаем одновременно хоткей на up and down (down в приоритете).
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
						msg_hotkey.hk = hk_Fix_RAlt;
						msg_hotkey.hotkey = curk;
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
				msg_hotkey.cur_keys_down = curKeys.AllKeys(); // todo curKey_no_disabled?
				if(need_disable_event)
					Utils::RemoveFirst(msg_hotkey.cur_keys_down, vkCode); // удалим то что запретили, так как поднимать их не нужно.
				Worker()->PostMsg(std::move(msg_hotkey), delay);
			}
		}

		if (
			// https://github.com/Aegel5/SimpleSwitcher/issues/70
			// на наши хоткеи не заходим, не важно up или double или полное совпадение
			// если они хотят очистить буфер - должны делать сами
			// если они состоят из одной буквы или shift + буквы - не важно - все равно запрет происходит.
			!is_our_key_down 
			&& curKeyState == KeyState::KEY_STATE_DOWN 
			) {
			if (!curk.IsEmpty()) {
				Worker()->PostMsg(Message_KeyType{
					.vkCode = vkCode,
					.scan_ext = { (TScanCode)scan_code, isExtended },
					.cur_hotKey = curk, // без учета disabled, но не критично
					.is_caps = iscaps == 1,
					});
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
