#pragma once

class WorkerImplement
{
private:

	struct ContextRevert
	{
		TKeyRevert keylist;
		HotKeyType typeRevert;
		HKL lay = 0;
		TUInt32 flags = 0;
		//tstring txtToInsert;
	};

	typedef std::vector<CHotKey> TKeyToRevert;

public:

	void ClearAllWords() {
		LOG_INFO_2(L"ClearsKeys");
		m_cycleList.Clear();
	}
	TStatus NeedRevert(HotKeyType typeRevert);
	TStatus NeedRevert2(ContextRevert& data);
	TStatus AnalizeTopWnd();
	TStatus SwitchLangByEmulate(HKL lay);
	void CliboardChanged() 
	{ 
		IFS_LOG(ClipboardChangedInt()); 
		LOG_INFO_1(L"ClipboardChangedInt complete");
	}
	TStatus ClipboardChangedInt();
    TStatus GetClipStringCallback();
	void ClipboardClearFormat2() { IFS_LOG(m_clipWorker.ClipboardClearFormat()); }
	TStatus ClipboardToSendData(std::wstring& clipdata, TKeyRevert& keylist);

	void ChangeForeground(HWND hwnd);
	void ProcessKeyMsg(MainWorkerMsg::U::Key_Message& keyData);
	TStatus Init();
	TStatus SendCtrlC(EClipRequest clRequest);
	void RequestWaitClip(EClipRequest clRequest)
	{
		m_clipRequest = clRequest;
		//m_clipCounter = GetClipboardSequenceNumber();
        m_dwLastCtrlCReqvest = GetTickCount64();
	}
    HKL getNextLang();

	void CheckCurLay(bool forceSend = false);

	TStatus FixCtrlAlt(CHotKey key);

	void SetNewLay(HKL lay) {

		LOG_INFO_1(L"Try set 0x%x lay", lay);


		if (conf_get_unsafe()->AlternativeLayoutChange)
		{
			IFS_LOG(SwitchLangByEmulate(lay));
		}
		else
		{
			Utils::SetLayPost(topWndInfo2.hwnd_default, lay);
		}
	}
	void WaitOtherLay(HKL lay) {
		// Дождемся смены языка. Нет смысла переходить в асинхронный режим. Можем ждать прямо здесь.
		auto start = GetTickCount64();
		while (true)
		{
			auto curL = GetKeyboardLayout(topWndInfo2.threadid_default);
			if (curL != lay) {
				LOG_INFO_2(L"new lay arrived after %u", GetTickCount64() - start);
				break;
			}

			if ((GetTickCount64() - start) >= 150) {
				LOG_WARN(L"wait timeout language change for proc {}", m_sTopProcName.c_str());
				break;
			}

			Sleep(5);
		}
	}

	void ProcessOurHotKey(MainWorkerMsg& keyData) {

		auto hk = keyData.data.hk;
		const auto& key = keyData.data.hotkey;

		GETCONF;

		if (IsNeedSavedWords(hk) && !m_cycleList.HasAnySymbol()) {
			bool found = false;
			for (const auto& [hk2,key2] : cfg->All_hot_keys()) {
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

		IFS_LOG(NeedRevert(hk));

	}

	HKL CurLay() { return        topWndInfo2.lay; }

public:

	ULONGLONG m_dwLastCtrlCReqvest = 0;
	EClipRequest m_clipRequest = CLRMY_NONE;
	DWORD m_dwIdThreadForeground = -1;
	DWORD m_dwIdProcoreground = -1;
	TopWndInfo topWndInfo2;
	std::wstring m_sTopProcName;
	std::wstring m_sTopProcPath;
	CClipWorker m_clipWorker; 
	tstring m_savedClipData;
	HotKeyType m_lastRevertRequest = hk_NULL;
	TStatus ProcessRevert(ContextRevert& ctxRevert);
	std::wstring m_sSelfExeName;
	CycleRevertList m_cycleList;
	CurStateWrapper m_curStateWrap;

};













