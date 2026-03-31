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
			BYTE vk_code = LOBYTE(res);

			TKeyType type = KEYTYPE_LETTER;
			// Пока сделаем супер-простое разделение
			if (StrUtils::IsSpace(c)) type = KEYTYPE_SPACE;

			TKeyBaseInfo key{
				.vk_code = vk_code,
				.scan_code = {},
				.is_shift = TestFlag(mods, 0x1),
				.is_caps = m_is_last_caps,
				.type = type
			};
			m_cycleList.AddKeyToList(key);
		}

		RevertText(hk_RevertAllRecentText, true, true);
		if (m_clear_alfter_selected) {
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

	TStatus RunProcess(HotKeyType hk, bool after_wait=false) {

		GETCONF;

		int i = hk;
		ResetFlag(i, hk_RunProgram_flag);
		if (i >= cfg->run_programs.size()) {
			return SW_ERR_UNKNOWN;
		}
		const auto& it = cfg->run_programs[i];

		if (!it.enabled) {
			RETURN_SUCCESS;
		}

		if (it.delay > 0 && !after_wait) {
			Worker()->PostMsg([hk](auto p) {p->RunProcess(hk,true); }, it.delay);
			RETURN_SUCCESS;
		}

		if (it.type == CommandType::Snippet) {
			if (it.snippet.empty()) RETURN_SUCCESS;
			auto str = StrUtils::Convert(it.snippet);
			InputSender is;
			for (auto c : str) {
				is.AddUnicodePress(c);
			}
			is.Send();
			RETURN_SUCCESS;
		}

		auto wpath = StrUtils::Convert(it.path);
		auto wargs = StrUtils::Convert(it.args);

		PathUtils::NormalizeDelims(wpath);

		LOG_ANY(L"run program {} {}", wpath.c_str(), wargs.c_str());

		procstart::CreateProcessParm parm;
		parm.sExe = wpath.c_str();
		parm.sCmd = wargs.c_str();

		// todo - use proxy process for unelevated.
		//parm.admin = it.elevated ? TSWAdmin::SW_ADMIN_ON : TSWAdmin::SW_ADMIN_OFF;

		parm.mode = procstart::SW_CREATEPROC_SHELLEXE;
		CAutoHandle hProc;
		IFS_RET(procstart::SwCreateProcess(parm, hProc));

		RETURN_SUCCESS;
	}

	void ProcessOurHotKey(Message_Hotkey&& keyData);

	HKL CurLay() { return topWndInfo2.lay; }
	TStatus ProcessRevert(ContextRevert&& ctxRevert);

	void UpAllKeys(const vector<TKeyCode>& keys) {

		if (keys.size() == 0) return;

		LOG_ANY(L"UpAllKeys {}", (int)keys.size());

		InputSender inputSender;

		for (const auto& key : keys) {
			inputSender.Add(key, KEY_STATE_UP);
		}

		inputSender.Send();

	}


	void Fix_AltOrWin(const vector<TKeyCode>& keys) {

		// если хоткей вида Win + X и мы задисейблили клавишу X для системы, то произойдет появление меню
		// сделаем хак чтобы этого не было.

		if (keys.size() == 0) return;

		if (!std::any_of(keys.begin(), keys.end(), [](auto x) {
			return x == VK_LMENU || x == VK_LWIN;
			})) {
			return;
		}

		LOG_ANY(L"FixAltOrWin {}", (int)keys.size());
		InputSender inputSender;
		inputSender.Add(VK_CAPITAL, KEY_STATE_UP);
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
	//std::wstring m_savedClipData;
	HotKeyType m_lastRevertRequest = hk_NULL;
	//std::wstring m_sSelfExeName;
	CycleRevertList m_cycleList;
	bool m_clear_alfter_selected = false;
	bool m_is_last_caps = false;
};













