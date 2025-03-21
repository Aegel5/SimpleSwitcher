
LRESULT CALLBACK Hooker::HookerKeyboard::LowLevelKeyboardProc(
	_In_  int nCode,
	_In_  WPARAM wParam,
	_In_  LPARAM lParam
) {
	if (nCode < 0) {
		return CallNextHookEx(0, nCode, wParam, lParam);
	}

	MainWorkerMsg msg_type(HWORKER_KeyMsg);
	MainWorkerMsg msg_hotkey(HWORKER_OurHotKey);
	msg_hotkey.data.hk = hk_NULL;
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
			HotKeyNames::Global().GetName(vkCode),
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
			LOG_INFO_1(L"k->vkCode > 255: %d", k->vkCode);
		}

		if (scan_code == 541) {
			LOG_INFO_2(L"skip bugged lctrl");
			return 0;
		}

		if (isInjected) {
			if (InjectSkipper::Inst().IsInjectOur())
				return 0;
		}

		bool is_hold = false;
		if (curKeyState != KeyState::KEY_STATE_DOWN) {
			vk_last_down = 0;
		}
		else {
			is_hold = vk_last_down == vkCode && vkCode != 0;
			vk_last_down = vkCode;
		}

		CHotKey possible;
		std::swap(possible, possible_hk_up); // ����� �������
		{
			auto& data = msg_type.data.key_message;
			data.vkCode = vkCode;
			data.scanCode = scan_code;
			data.flags = k->flags;
			data.keyState = curKeyState;
			data.hk = hk_NULL;
		}
		send_key = true; // ������������ �������, ���� �� ����� �������

		curKey.Update(vkCode, curKeyState); // ����� ���������
		const auto& curk = curKey.state;

		int check_disabled_status = -1;

		auto check_is_our_key = [&check_disabled_status, cfg](const CHotKey& k1, const CHotKey& k2) {
			if (k1.Compare(k2, CHotKey::COMPARE_IGNORE_KEYUP)) {
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
				if (!key.GetKeyup() && check_is_our_key(key, curk)) {
					need_our_action = true;
					possible_hk_up.Clear(); // �������, ������ ��� ����� ��������� ����� � ���� �����.
					if (!is_hold) { // �� ���� ���� � ����, ������� ����� ���������.
						msg_hotkey.data.hotkey = key;
						msg_hotkey.data.hk = hk;
					}
					break;
				}
				if (key.GetKeyup() && check_is_our_key(key, curk)) {
					possible_hk_up = curk; // ��� break
					msg_type.data.key_message.hk = hk; // ��������, ��� ��� ���� �������.
				}
			}
			if (need_our_action) {
				// � ��� ���� ����� ���-���, ��������� ��� ������� ��� ���������.
				disable_up = curk.ValueKey(); // up ���� ����� � ������� ���������.
				LOG_INFO_1(L"Key %s was disabled(down)", CHotKey::GetName(disable_up));
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
					LOG_INFO_1(L"fix ctrl+alt");
					MainWorkerMsg msg(HWORKER_FixCtrlAlt);
					msg.data.hotkey_to_fix = curk;
					if (!is_hold) { // ���� ������ ���������
						Worker()->PostMsg(msg);
					}
					LOG_INFO_1(L"Key %s was disabled(fix)", CHotKey::GetName(vkCode));
					disable_up = vkCode;
					return 1; // ���� ���������, ����� ������ �������...
				}
			}
			return 0;
		}

		if (curKeyState == KeyState::KEY_STATE_UP) {
			if (disable_up == vkCode) {
				LOG_INFO_1(L"Key %s was disabled(up)", CHotKey::GetName(disable_up));
				disable_up = 0;
				return 1;
			}
			else {
				// ���� ��� ���-���.
				// ���� ���� �����, up ������� �� ���������.
				if (!possible.IsEmpty()) {
					for (const auto& [hk, key] : cfg->All_hot_keys()) {
						if (key.GetKeyup() && check_is_our_key(key, possible)) {
							msg_hotkey.data.hotkey = key;
							msg_hotkey.data.hk = hk;
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
		if (curKey.state.IsEmpty()) {
			disable_up = 0;
		}
		if (
			!res && send_key) {
			Worker()->PostMsg(msg_type);
		}
		if (msg_hotkey.data.hk != hk_NULL) {
			Worker()->PostMsg(msg_hotkey);
		}
		if (res)
			return 1; // ������
	}


	if (cfg->EnableKeyLoggerDefence) {
		return 0;
	}
	else {
		return CallNextHookEx(0, nCode, wParam, lParam);
	}

}
