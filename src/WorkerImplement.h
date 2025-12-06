#pragma once

class WorkerImplement
{
	enum {
		SW_CLIENT_PUTTEXT = 1 << 0,
		SW_CLIENT_BACKSPACE = 1 << 1,
		SW_CLIENT_SetLang = 1 << 2,
		SW_CLIENT_CTRLC = 1 << 3,
		SW_CLIENT_CTRLV = 1 << 4,
		SW_CLIENT_NO_WAIT_LANG = 1 << 5,
	};

	struct ContextRevert {
		TKeyRevert keylist;
		HKL lay = 0;
		TUInt32 flags = 0;
	};

	typedef std::vector<CHotKey> TKeyToRevert;

public:

	WorkerImplement() {
		//IFS_LOG(PathUtils::GetPath_fileExe_lower(m_sSelfExeName));
		TimerClear();
		TimerCheckLay();
	}

	void TimerClear() {
		m_cycleList.ClearByTimer();
		Worker()->PostMsg([](auto p) {p->TimerClear(); }, 30000);
	}

	void TimerCheckLay() {
		CheckCurLay();
		Worker()->PostMsg([](auto p) {p->TimerCheckLay(); }, 200);
	}

	void ClearAllWords() { m_cycleList.Clear(); }
	TStatus AnalizeTopWnd();
	void SwitchLangByEmulate(HKL lay);
	void CliboardChanged();
    TStatus GetClipStringCallback();
	void ClipboardClearFormat2() { IFS_LOG(m_clipWorker.ClipboardClearFormat()); }
	void ClipboardToSendData(const std::wstring& clipdata) {

		m_cycleList.Clear();

		HKL layouts[10];
		int count = GetKeyboardLayoutList(std::ssize(layouts), layouts);
		if (count == 0) {
			IFW_LOG(false);
			return;
		}


		for (auto c : clipdata) {
			if (c == L'\r')
				continue;
			auto lay = CurLay();
			SHORT res = VkKeyScanEx(c, lay);
			if (res == -1) {
				for (int i = 0; i < count; ++i) {
					if (layouts[i] != lay)
						res = VkKeyScanEx(c, layouts[i]);
					if (res != -1)
						break;
				}
			}
			if (res == -1) {
				IFS_LOG(SW_ERR_UNKNOWN, L"Cant scan char %c", c);
				continue;
			}
			BYTE mods = HIBYTE(res);
			BYTE code = LOBYTE(res);

			TKeyType type = KEYTYPE_LETTER;
			// Пока сделаем супер-простое разделение
			if (StrUtils::IsSpace(c)) type = KEYTYPE_SPACE;
			m_cycleList.AddKeyToList(type, {}, {}, TestFlag(mods, 0x1), code);
		}

		RevertText(hk_RevertAllRecentText, true, true);
		if (clear_alfter_selected) {
			m_cycleList.Clear();
		}
	}

	void RevertText(HotKeyType typeRevert, bool no_backs = false, bool always_full_text = false) {
		auto nextLng = getNextLang();
		auto to_revert = m_cycleList.FillKeyToRevert(typeRevert, always_full_text);
		if (to_revert.keys.empty()) {
			LOG_ANY(L"nothing to revert. skip");
			return;
		}
		m_cycleList.SetSeparateLast();
		bool isNeedLangChange = to_revert.needLanguageChange;
		ContextRevert data;
		data.keylist = std::move(to_revert.keys);
		data.flags = SW_CLIENT_PUTTEXT | SW_CLIENT_SetLang | (no_backs ? 0 : SW_CLIENT_BACKSPACE);
		data.lay = isNeedLangChange ? nextLng : 0;
		IFS_LOG(ProcessRevert(std::move(data)));
	}

	void ChangeForeground(HWND hwnd);
	void ProcessKeyMsg(const Message_KeyType& keyData);

	void RequestWaitClip(EClipRequest clRequest)
	{
		m_clipRequest = clRequest;
		//m_clipCounter = GetClipboardSequenceNumber();
        m_dwLastCtrlCReqvest = GetTickCount64();
	}
    HKL getNextLang();

	void CheckCurLay() {
		auto old_lay = topWndInfo2.lay;

		topWndInfo2 = Utils::GetFocusedWndInfo();

		if (topWndInfo2.lay == 0) {
			// оставим прежний
			topWndInfo2.lay = old_lay;
		}
		else {
			if (old_lay != topWndInfo2.lay) {
				new_layout_request(topWndInfo2.lay);
			}
		}
	}

	TStatus FixCtrlAlt(CHotKey key);

	void SetNewLay(HKL lay) {

		LOG_ANY(L"Try set {} lay", (void*)lay);

		if (conf_get_unsafe()->AlternativeLayoutChange)
		{
			SwitchLangByEmulate(lay);
		}
		else
		{
			Utils::SetLayPost(topWndInfo2.hwnd_default, lay);
		}
	}
	HKL WaitOtherLay(HKL lay) {
		// Дождемся смены языка. Нет смысла переходить в асинхронный режим. Можем ждать прямо здесь.
		auto start = GetTickCount64();
		while (true)
		{
			auto curL = GetKeyboardLayout(topWndInfo2.threadid_default);
			if (curL != lay) {
				LOG_ANY(L"new lay arrived after {}", GetTickCount64() - start);
				return curL;
			}

			if ((GetTickCount64() - start) >= 150) {
				LOG_WARN(L"wait timeout language change for proc {}", m_sTopProcName.c_str());
				return 0;
			}

			Sleep(5);
		}
	}

	void ProcessOurHotKey(Message_Hotkey&& keyData) {

		auto hk = keyData.hk;
		const auto& key = keyData.hotkey;

		if (keyData.delayed_from != 0 && keyData.delayed_from <= m_lastHotKeyTime) {
			LOG_ANY(L"skip hotkey {} possible was double press", key.ToString());
			return;
		}
		m_lastHotKeyTime = GetTickCount64();

		GETCONF;

		clear_alfter_selected = hk == hk_RevertSelelected;

		if (IsNeedSavedWords(hk) && !m_cycleList.HasAnySymbol()) {
			bool found = false;
			for (const auto& [hk2, key2] : cfg->All_hot_keys()) {
				if (!IsNeedSavedWords(hk2) && key.Compare(key2)) {
					// Есть точно такой же хот-кей, не требующий сохраненных слов, используем его.
					hk = hk2;
					found = true;
					break;
				}
			}
			if (!found) {
				LOG_ANY(L"skip hotkey {} because no saved word", (int)hk);
				return;
			}
		}

		if (key.GetKeyup()) {
			LOG_ANY(L"pause for #up key"); // дадим событию up время на обработку в ОС, так как для нас, она уже поднята.
			Sleep(5);
		}

		if (!IsNeedSavedWords(hk)) {
			ClearAllWords();
		}

		m_lastRevertRequest = hk;

		LOG_ANY("Hotkey start {}({})", HotKeyTypeName(hk), (int)hk);

		if (!g_enabled.IsEnabled() && hk != hk_ToggleEnabled) {
			LOG_ANY("Skip hk because disabled");
			return;
		}

		// Сбросим сразу все клавиши для программы. Будет двойной (или даже тройной и более) up, но пока что это не проблема... 
		UpAllKeys(keyData.cur_keys_down);

		if (hk == hk_Fix_RAlt) {
			FixCtrlAlt(keyData.hotkey);
			return;
		}

		if (hk == hk_ToggleEnabled) {
			try_toggle_enable();
			return;
		}

		if (hk == hk_ShowMainWindow) {
			show_main_wind();
			return;
		}

		if (hk == hk_ShowRemainderWnd) {
			show_main_wind(1);
			return;
		}

		auto process = [this, hk, cfg]() -> TStatus {

			if (TestFlag(hk, hk_RunProgram_flag)) {
				int i = hk;
				ResetFlag(i, hk_RunProgram_flag);
				if (i >= cfg->run_programs.size()) {
					return SW_ERR_UNKNOWN;
				}
				const auto& it = cfg->run_programs[i];
				auto path = it.path;
				PathUtils::NormalizeDelims(path);
				LOG_ANY(L"run program {} {}", path.c_str(), it.args.c_str());

				procstart::CreateProcessParm parm;
				parm.sExe = path.c_str();
				parm.sCmd = it.args.c_str();

				// todo - use proxy process for unelevated.
				//parm.admin = it.elevated ? TSWAdmin::SW_ADMIN_ON : TSWAdmin::SW_ADMIN_OFF;

				parm.mode = procstart::SW_CREATEPROC_SHELLEXE;
				CAutoHandle hProc;
				IFS_RET(procstart::SwCreateProcess(parm, hProc));

				RETURN_SUCCESS;
			}

			if (Utils::is_in(hk, hk_EmulateCapsLock, hk_EmulateScrollLock)) {
				TKeyCode k = (hk == hk_EmulateCapsLock) ? VK_CAPITAL : VK_SCROLL;
				InputSender::SendVkKey(k);
				RETURN_SUCCESS;
			}

			if (hk == hk_InsertWithoutFormat) {
				IFS_RET(m_clipWorker.ClipboardClearFormat());
				CHotKey ctrlv(VK_CONTROL, VKE_V);
				InputSender::SendWithPause(ctrlv);
				RETURN_SUCCESS;
			}

			// CHANGE LAYOUT WITHOUT REVERT

			IFS_RET(AnalizeTopWnd());

			if (hk == hk_CycleSwitchLayout) {
				IFS_RET(ProcessRevert({ .lay = getNextLang(), .flags = SW_CLIENT_SetLang }));
				RETURN_SUCCESS;
			}

			if (TestFlag(hk, hk_SetLayout_flag)) {

				int i = hk;
				ResetFlag(i, hk_SetLayout_flag);

				auto info = cfg->layouts_info.GetLayoutIndex(i);
				if (info == nullptr) {
					LOG_WARN(L"not found hot key for set layout");
					RETURN_SUCCESS;
				}

				IFS_RET(ProcessRevert({ .lay = info->layout , .flags = SW_CLIENT_SetLang | SW_CLIENT_NO_WAIT_LANG }));

				RETURN_SUCCESS;
			}


			// REVERT AND CHANGE LAYOUT

			if (Utils::is_in(hk, hk_RevertSelelected, hk_toUpperSelected, hk_InvertCaseSelected)) {
				LOG_ANY(L"save buff");
				m_savedClipData = m_clipWorker.getCurString();
				RequestWaitClip(CLRMY_GET_FROM_CLIP);
				IFS_RET(ProcessRevert({ .flags = SW_CLIENT_CTRLC }));
				RETURN_SUCCESS;
			}

			// ---------------classic revert---------------

			if (!Utils::is_in(hk, hk_RevertLastWord, hk_RevertSeveralWords, hk_RevertAllRecentText)) {
				IFS_RET(SW_ERR_UNKNOWN, L"Unknown typerevert {}", (int)hk);
			}

			RevertText(hk);

			RETURN_SUCCESS;
			};

		IFS_LOG(process());
	}

	HKL CurLay() { return topWndInfo2.lay; }
	TStatus ProcessRevert(ContextRevert&& ctxRevert);

	void UpAllKeys(const vector<TKeyCode>& keys) {

		if (keys.size() == 0) return;

		LOG_ANY(L"UpAllKeys {}", (int)keys.size());

		InputSender inputSender;
		if (std::any_of(keys.begin(), keys.end(), [](auto x) {
			return x == VK_LMENU || x == VK_LWIN;
			})) {
			// если нажата только клавиша alt - то ее простое отжатие даст хрень - нужно отжать ее еще раз
			//inputSender.Add(VK_LMENU, KEY_STATE_DOWN);
			//inputSender.Add(VK_LMENU, KEY_STATE_UP); 	
			inputSender.Add(VK_CAPITAL, KEY_STATE_UP);
		}

		for (const auto& key : keys) {
			inputSender.Add(key, KEY_STATE_UP);
		}

		inputSender.Send();

	}

private:

	ULONGLONG m_lastHotKeyTime = 0;
	ULONGLONG m_dwLastCtrlCReqvest = 0;
	EClipRequest m_clipRequest = CLRMY_NONE;
	DWORD m_dwIdThreadForeground = -1;
	DWORD m_dwIdProcoreground = -1;
	TopWndInfo topWndInfo2;
	public:std::wstring m_sTopProcName;
	private:std::wstring m_sTopProcPath;
	CClipWorker m_clipWorker; 
	std::wstring m_savedClipData;
	HotKeyType m_lastRevertRequest = hk_NULL;
	//std::wstring m_sSelfExeName;
	CycleRevertList m_cycleList;
	bool clear_alfter_selected = false;
};













