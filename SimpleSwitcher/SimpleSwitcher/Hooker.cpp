#include "stdafx.h"

#include "Hooker.h"
#include "Settings.h"
#include "Dispatcher.h"
#include "Encrypter.h"

TKeyType Hooker::GetCurKeyType(CHotKey hotkey)
{
	CHotKey key = hotkey;
	if(key.Size() == 0)
	{
		return KEYTYPE_COMMAND_NO_CLEAR;
	}
	else if(key.Size() > 2)
	{
		return KEYTYPE_COMMAND_CLEAR;
	}
	else if (key.Size() == 2)
	{
		if (!key.HasMod(VK_SHIFT))
			return KEYTYPE_COMMAND_CLEAR;
	}

	switch (key.ValueKey())
	{
		case VK_CAPITAL:
		case VK_SCROLL:
		case VK_PRINT:
		case VK_NUMLOCK:
		case VK_INSERT:
			return KEYTYPE_COMMAND_NO_CLEAR;
		case VK_RETURN:
			return KEYTYPE_COMMAND_CLEAR;
		case VK_TAB:
		case VK_SPACE:
			return KEYTYPE_SPACE;
		case VK_BACK:
			return KEYTYPE_BACKSPACE;
	}

	//if(SettingsGlobal().isDashSeparate)
	//{
	//	if(key.ValueKey() == 189 && key.Size() == 1)
	//	{
	//		return KEYTYPE_SPACE;
	//	}
	//}

	BYTE keyState[256];
	SwZeroMemory(keyState);
	keyState[VK_SHIFT] = m_curKeyState.HasMod(VK_SHIFT) ? 0x80 : 0;
	TCHAR sBufKey[0x10] = { 0 };
	int res = ToUnicodeEx(key.ValueKey(), m_curScanCode, keyState, sBufKey, ARRAYSIZE(sBufKey), 0, NULL);
	if (res == 1)
	{
		auto c = sBufKey[0];
		LOG_INFO_3(L"print char %c", c);
		if (wcschr(L" \t-=+*()%", c) != NULL)
		{
			return KEYTYPE_SPACE;
		}

		return KEYTYPE_SYMBOL;
	}
	else
	{
		return KEYTYPE_COMMAND_CLEAR;
	}
}


void PrintHwnd(HWND hwnd, const TChar* name=L"name1")
{
	if(GetLogLevel() >= 2)
	{
		DWORD pid = 0;
		DWORD threadid = GetWindowThreadProcessId(hwnd, &pid);
		//SW_LOG_INFO_2(L"%s=%p, pid=%d threadid=%d", name, hwnd, pid, threadid);
	}
} 


bool Hooker::GetTypeForKey(CHotKey curkey, HotKeyType& type, bool& isUp)
{
	for (int iPrior = 0; iPrior < 2; ++iPrior)
	{
		for (auto it : settings_thread.hotkeysList)
		{
			auto& info = it.second;
			auto hkId = it.first;

			CHotKey key = info.key;
			if (curkey.Compare(key, CHotKey::COMPARE_IGNORE_KEYUP))
			{
				if (info.fNeedSavedWord && !HasAnyWord())
				{
					LOG_INFO_2(L"skip key %u because not has any word", info.key);
					// skip current, add chance for other hotkey
					continue;
				}
				
				if (iPrior == 0 && !info.fNeedSavedWord)
				{
					continue;
				}

				//isUp = key.GetWorkOnKeyUp();
				type = hkId;
				isUp = key.GetKeyup();

				return true;
			}
		}
	}

	return false;
}
TStatus Hooker::ProcessKeyMsg(KeyMsgData& keyData)
{
	KBDLLHOOKSTRUCT* k = &keyData.ks;
	WPARAM wParam = keyData.wParam;

	if(k->vkCode > 255)
	{
		LOG_INFO_1(L"k->vkCode > 255: %d", k->vkCode);
		RETURN_SUCCESS;
	}

	m_curScanCode = k->scanCode;

	TKeyCode vkCode = (TKeyCode)k->vkCode;
	KeyState curKeyState = GetKeyState(wParam);

	if (TestFlag(k->flags, LLKHF_INJECTED))
	{
		LOG_INFO_3(L"skip enjected");
		RETURN_SUCCESS;
	}

	bool isSkipRepeat = false;

	if(curKeyState == KEY_STATE_UP)
	{
		m_curKeyState.SetHold(false);

		if(!m_curKeyState.Remove(vkCode))
		{
			if (CHotKey::IsKnownMods(vkCode))
			{
				std::wstring s1;
				CHotKey::ToString(vkCode, s1);
				IFS_LOG(SW_ERR_UNKNOWN, L"Not found up for key %s", s1.c_str());
			}
		}
	}
	else if(curKeyState == KEY_STATE_DOWN)
	{
		CHotKey hk_save = m_curKeyState;
		m_curKeyState.Add(vkCode,  CHotKey::ADDKEY_ORDERED | CHotKey::ADDKEY_ENSURE_ONE_VALUEKEY);
		if (m_curKeyState == hk_save)
		{
			if (m_curKeyState.IsHold()) // already hold
			{
				isSkipRepeat = true;
			}
			else
			{
				m_curKeyState.SetHold(true);
			}
		}
		else
		{
			// была нажата другая клавиша, сбрасываем флаг hold
			m_curKeyState.SetHold(false);
		}
		//m_curHotKey = m_curKeyState;
	}
	else
	{
		return SW_ERR_UNKNOWN;
	}


	if (GetLogLevel() >= LOG_LEVEL_3)
	{
		std::wstring s2;
		m_curKeyState.ToString(s2, true);
		//std::wstring s3;
		//m_curHotKey.ToString(s3, true);
		LOG_INFO_1(L"curState =%s", s2.c_str());
		//LOG_INFO_1(L"curHotKey=%s", s3.c_str());
	}

	if (m_needRevertUnderUP != hk_MAX)
	{
		auto needRevert = m_needRevertUnderUP;
		m_needRevertUnderUP = hk_MAX;
		if (curKeyState == KEY_STATE_UP)
		{
			IFS_LOG(NeedRevert((HotKeyType)needRevert));
		}
	}

	if (curKeyState != KEY_STATE_DOWN)
		RETURN_SUCCESS;

	HotKeyType hotKeyType = hk_MAX;
	bool isUp;
	if (GetTypeForKey(m_curKeyState, hotKeyType, isUp))
	{
		if (isSkipRepeat)
		{
			LOG_INFO_2(L"Skip repeat evt");
			RETURN_SUCCESS;
		}

		if (isUp)
		{
			m_needRevertUnderUP = hotKeyType;
			RETURN_SUCCESS;
		}
		else
		{
			IFS_LOG(NeedRevert((HotKeyType)hotKeyType));
		}

		RETURN_SUCCESS;
	}

	// Чтобы не очищался буфер клавиш на нажатии наших хоткеев.
	for (auto& it : settings_thread.hotkeysList)
	{
		auto& info = it.second;
		auto hkId = it.first;

		CHotKey key = info.key;
		if (m_curKeyState.Compare(key, CHotKey::TCompareFlags(CHotKey::COMPARE_IGNORE_HOLD | CHotKey::COMPARE_IGNORE_KEYUP)))
		{
			auto s1 = m_curKeyState.ToString();
			auto s2 = key.ToString();
			LOG_INFO_2(L"skip hk diff only flags k1=%s, k2=%s hId=%u", s1.c_str(), s2.c_str(), hkId);
			RETURN_SUCCESS;
		}
	}

	if (CHotKey::IsKnownMods(vkCode))
		RETURN_SUCCESS;

	m_caseAnalizer.Clear();

	HandleSymbolDown();

	RETURN_SUCCESS;
	
}


TStatus Hooker::Init()
{
	ClearAllWords();
	IFS_LOG(GetPath(m_sSelfExeName, PATH_TYPE_EXE_FILENAME, SW_BIT_32));
	IFS_RET(m_clipWorker.Init());

	RETURN_SUCCESS;

}
bool Hooker::HasAnyWord()
{
	//std::unique_lock<std::recursive_mutex > lock(m_mtxKeyList);
	return !m_wordList.empty();
}
void Hooker::ClearAllWords()
{
	LOG_INFO_2(L"ClearsKeys");

	m_caseAnalizer.Clear();

	{
		//std::unique_lock<std::recursive_mutex > lock(m_mtxKeyList);
		m_wordList.clear();
		ClearCycleRevert();
	}

	CHotKey curCopy = m_curKeyState;
	for (TKeyCode* k = curCopy.ModsBegin(); k != curCopy.ModsEnd(); ++k)
	{
		if (GetAsyncKeyState(*k) & 0x8000)
		{
		}
		else
		{
			LOG_INFO_2(L"Up key %s because GetAsyncKeyState", CHotKey::ToString(*k).c_str());
			m_curKeyState.Remove(*k);
		}
	}
	
}

void Hooker::AddKeyToList(TKeyType type, CHotKey hotkey)
{
	ClearCycleRevert();

	if (m_wordList.size() >= c_nMaxLettersSave)
	{
		m_wordList.pop_front();
	}

	TKeyHookInfo key2;
	SwZeroMemory(key2);
	key2.key() = hotkey;
	key2.type = type;
	if (hotkey.ValueKey() == VK_OEM_2 && settings_thread.isTryOEM2) {
		SetFlag(key2.keyFlags, TKeyFlags::SYMB_SEPARATE_REVERT);
	}

	IFS_LOG(Encrypter::Encrypt(key2));
	m_wordList.push_back(key2);
}
TStatus Hooker::FillKeyToRevert(TKeyRevert& keyList, HotKeyType typeRevert)
{
	auto get_decrtypted = [this](int i) {
		TKeyHookInfo cur;
		cur.crypted = m_wordList[i].crypted;
		Encrypter::Decrypt(cur);
		return cur.key();
	};

	if (typeRevert == hk_RevertRecentTyped)
	{
		// reset this
		m_nCurrentRevertCycle = -1;

		// get all
		for (int i = 0; i < (int)m_wordList.size(); ++i)
		{
			keyList.push_back(get_decrtypted(i));
		}

		RETURN_SUCCESS;
	}

	if (m_nCurrentRevertCycle == -1)
		RETURN_SUCCESS;
	if (m_CycleRevertList.empty())
		RETURN_SUCCESS;

	CycleRevert curRevertInfo;

	if (typeRevert == hk_RevertLastWord)
	{
		if (m_nCurrentRevertCycle > 0)
		{
			m_nCurrentRevertCycle -= 1;
			curRevertInfo = m_CycleRevertList[m_nCurrentRevertCycle];
			m_nCurrentRevertCycle = 0;
		}
		else
		{
			curRevertInfo = m_CycleRevertList[m_nCurrentRevertCycle];
			m_nCurrentRevertCycle = 1;
			if (m_nCurrentRevertCycle >= (int)m_CycleRevertList.size())
				m_nCurrentRevertCycle = 0;
		}
	}
	else
	{
		curRevertInfo = m_CycleRevertList[m_nCurrentRevertCycle];
		++m_nCurrentRevertCycle;
		if (m_nCurrentRevertCycle >= (int)m_CycleRevertList.size())
			m_nCurrentRevertCycle = 0;
	}

	if (curRevertInfo.nIndexWordList == -1)
		RETURN_SUCCESS;
	if (m_wordList.empty())
		RETURN_SUCCESS;

	for (int i = curRevertInfo.nIndexWordList; i < (int)m_wordList.size(); ++i)
	{
		keyList.push_back(get_decrtypted(i));
	}

	RETURN_SUCCESS;
}
TStatus Hooker::GenerateCycleRevertList()
{
	bool isNeedLangChange = true;

	m_CycleRevertList.clear();

	int countWords = 0;
	if (!m_wordList.empty())
	{
		for (int i = (int)m_wordList.size() - 1; i >= 0; --i)
		{
			auto issep = [&](int i) {return TestFlag(m_wordList[i].keyFlags, TKeyFlags::SYMB_SEPARATE_REVERT); };
			auto add = [&](int i) {
				CycleRevert cycleRevert = { i, m_CycleRevertList.empty() };
				m_CycleRevertList.push_back(cycleRevert);
				if (++countWords >= c_maxWordRevert)
					return true;
				return false;
			};
			if (issep(i))
			{
				if (add(i)) 
					break;
			}
			else if (m_wordList[i].type != KEYTYPE_SPACE && (i == 0 || m_wordList[i - 1].type == KEYTYPE_SPACE || issep(i-1)))
			{
				if(add(i)) 
					break;
			}

		}
	}

	if(m_CycleRevertList.size() > 1)
	{
		m_CycleRevertList.push_back(m_CycleRevertList.back());
		m_CycleRevertList.back().fNeedLanguageChange = true;
	}

	if(m_CycleRevertList.empty())
	{
		CycleRevert cycleRevert = { -1, true };
		m_CycleRevertList.push_back(cycleRevert);
	}

	m_nCurrentRevertCycle = 0;

	RETURN_SUCCESS;
}

void Hooker::ClearCycleRevert()
{
	//SW_LOG_INFO_2(L"ClearCycleRevert");
	{
		//std::unique_lock<std::recursive_mutex > lock(m_mtxKeyList);
		m_CycleRevertList.clear();
		m_nCurrentRevertCycle = -1;
	}
	
}





void Hooker::AddToWordsByHotKey(CHotKey key)
{
	TKeyType type = GetCurKeyType(key);

	AddKeyToList(type, key);
}
TStatus ClipHasTextFormating(bool& fres)
{
	fres = false;

	CAutoClipBoard clip;
	IFS_RET(clip.Open(gdata().hWndMonitor));

	UINT format = 0;
	while (1)
	{
		format = EnumClipboardFormats(format);
		//LOG_INFO_1(L"Found format %u", format);
		if (format == 0)
		{
			break;
		}
		if (format >= 40000)
		{
			fres = true;
			break;
		}
	}

	RETURN_SUCCESS;
}
void PrintClipboardFormats()
{
	CAutoClipBoard clip;
	IFS_LOG(clip.Open(gdata().hWndMonitor));

	UINT format = 0;
	while (1)
	{
		format = EnumClipboardFormats(format);
		LOG_INFO_1(L"Found format %u", format);
		if (format == 0)
		{
			break;
		}
	}
}
TStatus Hooker::ClipboardToSendData(std::wstring& clipdata, TKeyRevert& keylist)
{

	HKL layouts[10];
	int count = GetKeyboardLayoutList(10, layouts);
	IFW_RET(count != 0);

	//SwZeroMemory(m_sendData);
	keylist.clear();

	//SW_LOG_INFO_2(L"layout=%u", m_layoutTopWnd);
	for (size_t i = 0; i < clipdata.length(); ++i)
	{
		TCHAR c = clipdata[i];
		if (c == L'\r')
			continue;
		SHORT res = VkKeyScanEx(c, m_layoutTopWnd);
		if (res == -1)
		{
			for (int i = 0; i < count; ++i)
			{
				if (layouts[i] != m_layoutTopWnd)
					res = VkKeyScanEx(c, layouts[i]);
				if (res != -1)
					break;
			}
		}
		if (res == -1)
		{
			IFS_LOG(SW_ERR_UNKNOWN, L"Cant scan char %c", c);
			continue;
		}
		BYTE mods = HIBYTE(res);
		BYTE code = LOBYTE(res);

		//SW_LOG_INFO_2(L"found vkcode=%u mods=%u", code, mods);
		CHotKey key;
		key.Add(code);
		if (TestFlag(mods, 0x1))
			key.Add(VK_SHIFT);

		//AddToWordsByHotKey(key);
		keylist.push_back(key);
	}
	RETURN_SUCCESS;
}

//void Hooker::PostMsgThread(InputSenderMode mode)
//{
//	std::unique_lock<std::mutex> lock(mtxInputSenderQue);
//	inputSenderQue.push_back(mode);
//	cvInputSender.notify_all();
//}
TStatus Hooker::ClipboardClearFormat2()
{
	m_clipWorker.PostMsg(ClipMode_ClipClearFormat);
	RETURN_SUCCESS;
}

TStatus RequestClearFormat()
{
	auto timeId = SetTimer(gdata().hWndMonitor, c_timerIdClearFormat, 500, NULL);
	IFW_RET(timeId != 0);
	RETURN_SUCCESS;
}
TStatus Hooker::GetClipStringCallback()
{
	LOG_INFO_1(L"GetClipStringCallback");

	std::wstring data;
	m_clipWorker.GetData(data);

	if (data.length() > 100) {
		LOG_INFO_1(L"TOO MANY TO REVERT. SKIP");
	}
	else {
		ContextRevert ctxRev;
		IFS_LOG(ClipboardToSendData(data, ctxRev.keylist));

		for (auto k : ctxRev.keylist)
		{
			AddToWordsByHotKey(k);
		}

		ctxRev.typeRevert = hk_RevertSel;
		ctxRev.lay = (HKL)HKL_NEXT;
		ctxRev.flags = SW_CLIENT_PUTTEXT | SW_CLIENT_SetLang;

		IFS_LOG(ProcessRevert(ctxRev));
	}

	m_clipWorker.PostMsg(ClipMode_RestoreClipData);

	RETURN_SUCCESS;
}
TStatus Hooker::TimerProcWaitClip2()
{
	if (m_clipRequest == CLRMY_GET_FROM_CLIP)
	{
		m_clipRequest = CLRMY_NONE;

		LOG_INFO_1(L"Actual get clip");

		m_clipWorker.PostMsg(ClipMode_GetClipString);
	}

	RETURN_SUCCESS;
}

DWORD GetClipTimeout(std::wstring& procName)
{
	if (procName == L"searchui.exe")
		return 210;
	if (procName == L"qip.exe")
		return 90;

	return u_conf.msDelayAfterCtrlC;

}
TStatus Hooker::ClipboardChangedInt()
{
	LOG_INFO_1(L"ClipboardChangedInt");

	DWORD dwTime = GetTick() - m_dwLastCtrlCReqvest;
	bool isRecent = dwTime <= 500;

	EClipRequest request = m_clipRequest;
	m_clipRequest = CLRMY_NONE;

	if (request == CLRMY_NONE)
	{
		LOG_INFO_1(L"Reqest not found");
	}
	else
	{
		if (!isRecent)
		{
			LOG_INFO_1(L"Skip no recent");
			RETURN_SUCCESS;
		}

		if (!IsClipboardFormatAvailable(CF_UNICODETEXT))
		{
			LOG_INFO_1(L"skip no unicodetext");
			RETURN_SUCCESS;
		}

		LOG_INFO_1(
			L"dwTime=%u, request=%u, clear=%u, sec=%u",
			dwTime,
			request,
			settings_thread.fClipboardClearFormat,
			GetClipboardSequenceNumber());

		if (request == CLRMY_GET_FROM_CLIP)
		{
			DWORD deltSec = GetClipboardSequenceNumber() - m_clipCounter;
			LOG_INFO_1(L"delt=%u", deltSec);

			//auto clipTimeoutSpecific = GetClipTimeout(m_sTopProcName);
			TUInt32 clipTimeoutDefault = (TUInt32)u_conf.msDelayAfterCtrlC;

			//bool fUncomplete = deltSec <= 7;
			auto timeout = clipTimeoutDefault;// fUncomplete ? clipTimeoutSpecific : clipTimeoutDefault;
			LOG_INFO_1(L"delay=%u", timeout);
			m_clipRequest = CLRMY_GET_FROM_CLIP;
			auto timeId = SetTimer(gdata().hWndMonitor, c_timerWaitClip, timeout, NULL);
			IFW_LOG(timeId != 0);


			RETURN_SUCCESS;
		}

		if (request == CLRMY_hk_COPY)
		{
			if (m_lastRevertRequest == hk_EmulCopyNoFormat)
			{
				IFS_LOG(RequestClearFormat());
			}
			RETURN_SUCCESS;
		}

		//if (request == CLR_hk_INSERT)
		//{
		//	if (isRecent)
		//	{

		//	}
		//	ContextRevert ctxRev;
		//	ctxRev.flags = SW_CLIENT_CTRLV;
		//	IFS_LOG(ProcessRevert(ctxRev));
		//}
	}


	// --- This is user request ----

	if (settings_thread.fClipboardClearFormat)
	{
		IFS_LOG(RequestClearFormat());
	}

	RETURN_SUCCESS;
}

void Hooker::ChangeForeground(HWND hwnd)
{
	LOG_INFO_2(L"Now foreground hwnd=0x%x", hwnd);
	DWORD procId = 0;
	DWORD threadid = GetWindowThreadProcessId(hwnd, &procId);
	if (threadid != m_dwIdThreadForeground && procId != m_dwIdProcoreground)
	{
		IFS_LOG(Utils::GetProcLowerNameByPid(procId, m_sTopProcPath, m_sTopProcName));
		LOG_INFO_2(L"threadid=%d, procId=%d, sname=%s", threadid, procId, m_sTopProcName.c_str());
		ClearAllWords();
	}
	m_dwIdThreadForeground = threadid;
	m_dwIdProcoreground = procId; 
}
//bool Hooker::IsOurInput()
//{
//	if (m_fOurSend)
//		return true;
//
//	return false;
//}
void Hooker::HandleSymbolDown()
{
	TKeyType type = GetCurKeyType(m_curKeyState);
	switch (type)
	{
	case KEYTYPE_BACKSPACE:
	{
		{
			//std::unique_lock<std::recursive_mutex> lock(m_mtxKeyList);
			if (!m_wordList.empty())
				m_wordList.pop_back();
		}
							  break;
	}
	case KEYTYPE_SYMBOL:
	case KEYTYPE_SPACE:
	{
						  AddKeyToList(type, m_curKeyState);
						  break;
	}
	case KEYTYPE_COMMAND_NO_CLEAR:
		break;
	default:
		ClearAllWords();
		break;
	}
}

TStatus Hooker::ProcessRevert(ContextRevert& ctxRevert)
{
	bool fUseAltMode;
	bool fDels;
	AnalizeProblemByName(m_sTopProcPath, m_sTopProcName, fUseAltMode, fDels);
	LOG_INFO_2(L"fpnw=%u",  fUseAltMode);
	bool fClearedState = false;

	auto ClearState = [&fClearedState, this]()
	{
		if (!fClearedState)
		{
			fClearedState = true;
			IFS_LOG(ClearModsBySend(m_curKeyState));
		}
	};

	bool needWaitLang = false;
	if (TestFlag(ctxRevert.flags, SW_CLIENT_SetLang) && ctxRevert.lay)
	{
		LOG_INFO_1(L"Try set 0x%x lay", ctxRevert.lay);

		HKL lay = ctxRevert.lay;


		if (fUseAltMode || u_conf.fUseAltMode)
		{
			ClearState();
			IFS_RET(SwitchLangByEmulate(lay));
		}
		else
		{
			LOG_INFO_1(L"post change");
			PostMessage(m_hwndTop, WM_INPUTLANGCHANGEREQUEST, 0, (LPARAM)lay);
		}

		needWaitLang = true;
	}

	if (TestFlag(ctxRevert.flags, SW_CLIENT_PUTTEXT) && TestFlag(ctxRevert.flags, SW_CLIENT_BACKSPACE))
	{
		ClearState();

		if (fDels)
		{
			IFS_RET(SendDels(ctxRevert.keylist.size()));
		}
		else
		{
			IFS_RET(SendBacks(ctxRevert.keylist.size()));
		}
	}

	if (needWaitLang) {
		// Дождемся смены языка. Нет смысла переходить в асинхронный режим. Можем ждать прямо здесь.
		auto start = GetTickCount64();
		while (true)
		{
			auto curL = GetKeyboardLayout(m_dwIdThreadTopWnd);
			if (m_layoutTopWnd != curL) {
				break;
			}

			if ((GetTickCount64() - start) >= 200) {
				LOG_WARN(L"wait timeout language change for proc %s", m_sTopProcName.c_str());
				break;
			}

			Sleep(5);
		}
	}

	if (TestFlag(ctxRevert.flags, SW_CLIENT_PUTTEXT))
	{
		IFS_RET(SendKeys(ctxRevert.keylist));

	}
	if (TestFlag(ctxRevert.flags, SW_CLIENT_CTRLC))
	{
		ClearState();

		CHotKey ctrlc(VK_CONTROL, 67);
		InputSender inputSender;
		inputSender.AddPress(ctrlc);

		IFS_RET(SendOurInput(inputSender));
	}

	if (TestFlag(ctxRevert.flags, SW_CLIENT_CTRLV))
	{
		ClearState();

		CHotKey ctrlv(VK_CONTROL, 56);
		InputSender inputSender;
		inputSender.AddPress(ctrlv);

		IFS_RET(SendOurInput(inputSender));
	}

	LOG_INFO_1(L"Revert complete");

	RETURN_SUCCESS;
}

TStatus Hooker::SavePrevDataCallback(EClipRequest clRequest)
{
	RequestWaitClip(clRequest);

	LOG_INFO_1(L"Send ctrlc...");

	ContextRevert ctxRev;
	ctxRev.flags = SW_CLIENT_CTRLC;

	IFS_RET(ProcessRevert(ctxRev));

	RETURN_SUCCESS;
}
TStatus Hooker::SendCtrlC(EClipRequest clRequest)
{
	m_clipWorker.PostMsg(ClipMode_SavePrevData, clRequest);
	RETURN_SUCCESS;
}
TStatus SendUpForKey(CHotKey key)
{
	InputSender sender;
	for (TKeyCode k : key)
	{
		std::wstring s1;
		CHotKey::ToString(k, s1);
		LOG_INFO_2(L"SendUpForKey press up for key %s", s1.c_str());
		IFS_RET(sender.Add(k, KEY_STATE_UP));
	}
	IFS_RET(SendOurInput(sender));
	RETURN_SUCCESS;
}


TStatus Hooker::NeedRevert2(ContextRevert& data)
{
	HotKeyType typeRevert = data.typeRevert;

	if (typeRevert == hk_CapsGenerate || typeRevert == hk_ScrollGenerate)
	{
		TKeyCode k = (typeRevert == hk_CapsGenerate) ? VK_CAPITAL : VK_SCROLL;
		InputSender inputSender;
		inputSender.Add(k, KEY_STATE_DOWN);
		inputSender.Add(k, KEY_STATE_UP);
		IFS_LOG(SendOurInput(inputSender));
		RETURN_SUCCESS;
	}

	if (typeRevert == hk_EmulCopyNoFormat || typeRevert == hk_EmulCopyWithFormat)
	{
		//IFS_RET(SendUpForKey(m_curKey));
		IFS_RET(SendCtrlC(CLRMY_hk_COPY));
		RETURN_SUCCESS;
	}

	IFS_RET(AnalizeTopWnd());

	if (m_sTopProcName == m_sSelfExeName)
	{
		LOG_INFO_1(L"Skip hotkey in self program");
		RETURN_SUCCESS;
	}

	if (u_conf.IsSkipProgram(m_sTopProcName))
	{
		LOG_INFO_1(L"Skip process %s because of disableInProcess", m_sTopProcName.c_str());
		RETURN_SUCCESS;
	}

	auto getNextLang = [this](const std::vector<HKL>& lst) {
		if (lst.size() < 2)
		{
			return std::make_pair(false, (HKL)0);
		}
		HKL toSet = 0;
		for (size_t i = 0; i < lst.size(); ++i)
		{
			if (m_layoutTopWnd == lst[i])
			{
				if (i == lst.size() - 1)
				{
					toSet = lst[0];
				} else
				{
					toSet = lst[i + 1];
				}
				break;
			}
		}
		if (toSet == 0)
		{
			toSet = lst[0];
		}
		return std::make_pair(true, toSet);
	};

	if (typeRevert == hk_CycleCustomLang)
	{
		auto [res, toSet] = getNextLang(settings_thread.customLangList);
		if (!res)
			RETURN_SUCCESS;
		data.flags = SW_CLIENT_SetLang;
		data.lay = toSet;
		IFS_RET(ProcessRevert(data));
		RETURN_SUCCESS;
	}

	if (typeRevert == hk_ChangeLayoutCycle)
	{
		data.flags = SW_CLIENT_SetLang;
		data.lay = (HKL)HKL_NEXT;
		IFS_RET(ProcessRevert(data));
		RETURN_SUCCESS;
	}

	if (typeRevert == hk_ChangeSetLayout_1 || typeRevert == hk_ChangeSetLayout_2 || typeRevert == hk_ChangeSetLayout_3)
	{
		HKL hkl = 0;
		if (typeRevert == hk_ChangeSetLayout_1)
			hkl = settings_thread.hkl_lay[SettingsGui::SW_HKL_1];
		else if (typeRevert == hk_ChangeSetLayout_2)
			hkl = settings_thread.hkl_lay[SettingsGui::SW_HKL_2];
		else if (typeRevert == hk_ChangeSetLayout_3)
			hkl = settings_thread.hkl_lay[SettingsGui::SW_HKL_3];

		data.flags = SW_CLIENT_SetLang;
		data.lay = (HKL)hkl;
		IFS_RET(ProcessRevert(data));

		RETURN_SUCCESS;
	}

	//if (typeRevert == hk_ChangeTextCase)
	//{
	//	if (m_caseAnalizer.IsInited())
	//	{
	//		RequestChangeCase();
	//	}
	//	else
	//	{
	//		IFS_RET(SendCtrlC(CLR_GET_FROM_CLIP));
	//	}
	//	RETURN_SUCCESS;
	//}
	if (typeRevert == hk_RevertSel)
	{
		IFS_RET(SendCtrlC(CLRMY_GET_FROM_CLIP));
		RETURN_SUCCESS;
	}

	HKL nextLng = (HKL)HKL_NEXT;

	if (typeRevert == hk_RevertLastWord_CustomLang || typeRevert == hk_RevertCycle_CustomLang)
	{
		auto [res, toSet] = getNextLang(settings_thread.revert_customLangList);
		if (!res)
			RETURN_SUCCESS;
		nextLng = toSet;
		typeRevert = typeRevert == hk_RevertLastWord_CustomLang ? hk_RevertLastWord : hk_RevertCycle;
	} 

	// ---------------classic revert---------------

	if (!(typeRevert == hk_RevertLastWord || typeRevert == hk_RevertCycle || typeRevert == hk_RevertRecentTyped))
	{
		IFS_RET(SW_ERR_UNKNOWN, L"Unknown typerevert %d", typeRevert);
	}

	bool isNeedLangChange = true;

	if (m_nCurrentRevertCycle == -1)
	{
		IFS_RET(GenerateCycleRevertList());
	}

	if (typeRevert == hk_RevertLastWord)
	{
		isNeedLangChange = true;
	}
	else
	{
		isNeedLangChange = m_CycleRevertList[m_nCurrentRevertCycle].fNeedLanguageChange;
	}

	IFS_RET(FillKeyToRevert(data.keylist, typeRevert));
	
	data.flags = SW_CLIENT_PUTTEXT | SW_CLIENT_SetLang | SW_CLIENT_BACKSPACE;
	data.lay = isNeedLangChange ? nextLng : 0;
	IFS_RET(ProcessRevert(data));

	RETURN_SUCCESS;
}
TStatus Hooker::NeedRevert(HotKeyType typeRevert)
{
	auto skey = m_curKeyState.ToString();
	LOG_INFO_1(L"NeedRevert %s, curstate=\"%s\"", HotKeyTypeName(typeRevert), skey.c_str());

	ContextRevert ctxRevert;
	ctxRevert.typeRevert = typeRevert;
	m_lastRevertRequest = typeRevert;

	IFS_LOG(NeedRevert2(ctxRevert));

	//PostMessage(CommonDataGlobal().hWndMonitor, WM_PostRevert, 0, 0);
	//IFS_RET(RevertData(m_sendData));

	RETURN_SUCCESS;
}



//void Hooker::SetLayAndWait(HKL lay)
//{
//	HKL old = GetKeyboardLayout(m_dwIdThreadTopWnd);
//	PostMessage(m_hwndTop, WM_INPUTLANGCHANGEREQUEST, 0, (LPARAM)lay);
//
//	int i = 0;
//	for (; i < 100; ++i)
//	{
//		HKL lnew = GetKeyboardLayout(m_dwIdThreadTopWnd);
//		if (old != lnew)
//		{
//			break;
//		}
//		Sleep(10);
//	}
//
//	if (i == 100)
//	{
//		LOG_INFO_1(L"i == 100");
//		int k = 0;
//	}
//}
VOID CALLBACK SendAsyncProc(
	_In_  HWND hwnd,
	_In_  UINT uMsg,
	_In_  ULONG_PTR dwData,
	_In_  LRESULT lResult
	)
{
	TUInt32 request = (TUInt32)dwData;
	LOG_INFO_2(L"SendAsyncProc request=%u", request);
	//LOG_INFO_1(L"5: %u", GetKeyboardLayout(HookerGlobal().m_dwIdThreadTopWnd));
	//HookerGlobal().DoneRevert();
}

void SwitchLayByDll(HWND hwnd, TUInt64 lay)
{
	LOG_INFO_1(L"Try SwitchLayByDll with lay=%I64u for hwnd=0x%p", lay, hwnd);

	//{
	//	CAutoWinMutexWaiter w(G_SwSharedMtx());
	//	G_SwSharedBuf().sendData.lay = lay;
	//}

	LOG_INFO_2(L"Try SendMessageCallback...");
	IFW_LOG(SendMessageCallback(
		hwnd,
		c_msgRevertID,
		c_msgWParm,
		(LPARAM)lay, //c_msgLParm,
		SendAsyncProc,
		NULL));

}

//void Hooker::SwitchLangByPostMsg(HWND hwnd, TUInt64 lay)
//{
//	//PostMessage(hwnd, WM_INPUTLANGCHANGEREQUEST, 0, (LPARAM)lay);
//}

TStatus FoundEmulateHotKey(CHotKey& key)
{
	key = CHotKey(VK_LMENU, VK_LSHIFT);

	HKEY hKey = 0;
	IF_LSTATUS_RET(RegOpenKeyExW(HKEY_CURRENT_USER, L"Keyboard Layout\\Toggle", 0, KEY_READ, &hKey));

	DWORD dataType = REG_SZ;
	TChar sBuf[0x100];
	DWORD sSize = 0x100;
	LONG nError = ::RegQueryValueExW(
		hKey,
		L"Hotkey",
		0,
		&dataType,
		(PBYTE)sBuf,
		&sSize);

	if (nError == ERROR_SUCCESS)
	{
		if (wcscmp(sBuf, L"2") == 0)
		{
			key = CHotKey(VK_LCONTROL, VK_LSHIFT);
		}
	}

	RegCloseKey(hKey);

	RETURN_SUCCESS;
}
TStatus FoundCountEmulate(HKL_W layFrom, HKL_W layTo, int& countRes)
{
	countRes = 1;

	if (layTo == (HKL)HKL_NEXT)
	{
		RETURN_SUCCESS;
	}

	if (layFrom == layTo)
	{
		countRes = 0;
		RETURN_SUCCESS;
		//IFS_RET(SW_ERR_UNKNOWN, L"layfrom == layto");
	}

	RETURN_SUCCESS;

	HKL buf[0x100];
	int count = GetKeyboardLayoutList(SW_ARRAY_SIZE(buf), buf);
	IFW_RET(count != 0);

	int iLayTo = -1;
	int iLayFrom = -1;
	for (int i = 0; i < count; ++i)
	{
		if (buf[i] == (HKL)layFrom)
		{
			iLayFrom = i;
		}
		if (buf[i] == (HKL)layTo)
		{
			iLayTo = i;
		}
	}

	if (iLayTo == -1 || iLayFrom == -1 || iLayTo == iLayFrom)
	{
		IFS_RET(SW_ERR_UNKNOWN, L"not found lays");
	}
	if (iLayTo > iLayFrom)
	{
		countRes = iLayTo - iLayFrom;
	}
	else
	{
		countRes = count - iLayFrom + iLayTo;
	}

	RETURN_SUCCESS;

}
TStatus Hooker::SwitchLangByEmulate(HKL_W lay)
{
	InputSender inputSender;

	CHotKey altshift;
	IFS_LOG(FoundEmulateHotKey(altshift));

	int countRes = 0;
	IFS_LOG(FoundCountEmulate((HKL_W)m_layoutTopWnd, lay, countRes));

	for (int i = 0; i < countRes; ++i)
	{
		std::wstring s1 = altshift.ToString();
		LOG_INFO_2(L"Add press %s", s1.c_str());
		IFS_RET(inputSender.AddPress(altshift));
	}

	IFS_RET(SendOurInput(inputSender));

	RETURN_SUCCESS;
}
//TStatus GetNextLay(DWORD threadId, HKL lay, HKL& requereLay)
//{
//	requereLay = lay;
//	if (lay != (HKL)HKL_NEXT && lay != (HKL)HKL_PREV)
//	{
//		RETURN_SUCCESS;
//	}
//
//	HKL old = GetKeyboardLayout(threadId);
//
//	HKL buf[0x100];
//	int count = GetKeyboardLayoutList(SW_ARRAY_SIZE(buf), buf);
//	IFW_RET(count != 0);
//
//	for (int i = 0; i < count; ++i)
//	{
//		if (buf[i] == old)
//		{
//			if (lay == (HKL)HKL_NEXT)
//			{
//				if (i == count - 1)
//				{
//					requereLay = buf[0];
//				}
//				else
//				{
//					requereLay = buf[i + 1];
//				}
//			}
//			else
//			{
//				if (i == 0)
//				{
//					requereLay = buf[count - 1];
//				}
//				else
//				{
//					requereLay = buf[i - 1];
//				}
//			}
//		}
//	}
//
//	RETURN_SUCCESS;
//
//}





TStatus Hooker::AnalizeTopWnd()
{
	HWND hwndFocused;
	IFS_RET(Utils::GetFocusWindow(hwndFocused));
	PrintHwnd(hwndFocused, L"hwndFocused");

	//HWND hwndForeg = GetForegroundWindow();
	//DWORD prFor;
	//DWORD trFor = GetWindowThreadProcessId(hwndForeg, &prFor);
	//HKL layFor = GetKeyboardLayout(trFor);
	//LOG_INFO_1(
	//	L"hwnd=0x%p, pid=%u, threadid=%u, lay=0x%x",
	//	hwndForeg,
	//	prFor,
	//	trFor,
	//	layFor);

	m_dwIdThreadTopWnd = GetWindowThreadProcessId(hwndFocused, &m_dwTopPid);
	m_layoutTopWnd = GetKeyboardLayout(m_dwIdThreadTopWnd);
	m_hwndTop = hwndFocused;



	

	IFS_LOG(Utils::GetProcLowerNameByPid(m_dwTopPid, m_sTopProcPath, m_sTopProcName));

	LOG_INFO_1(
		L"hwnd=0x%p, pid=%u, threadid=%u, lay=0x%x, prg=%s",
		m_hwndTop,
		m_dwTopPid,
		m_dwIdThreadTopWnd,
		m_layoutTopWnd,
		m_sTopProcName.c_str());

	RETURN_SUCCESS;
}

TStatus Hooker::ClearModsBySend(CHotKey key)
{
	std::wstring s1 = key.ToString();
	LOG_INFO_1(L"ClearModsBySend for key %s", s1.c_str());

	InputSender sender;

	for (TKeyCode k : key)
	{
		if (!CHotKey::IsKnownMods(k))
			continue;
		SHORT res = GetAsyncKeyState(k);
		if (res & 0x8000)
		{
			std::wstring s1;
			CHotKey::ToString(k, s1);
			//LOG_INFO_1(L"ClearModsBySend %s", s1.c_str());
			IFS_RET(sender.Add(k, KEY_STATE_UP));
		}
	}

	IFS_RET(SendOurInput(sender));

	RETURN_SUCCESS;
}

//TStatus Hooker::SendOurInputFromHost2(UINT vk, UINT flags)
//{
//	CAutoCounter autoBool(m_fOurSend);
//
//	INPUT cur = { 0 };
//	cur.type = INPUT_KEYBOARD;
//	cur.ki.wVk = vk;
//	cur.ki.dwFlags = flags;
//
//	IFW_LOG(SendInput((UINT)1, &cur, sizeof(INPUT)));
//
//	RETURN_SUCCESS;
//}
//
//TStatus Hooker::SendOurInputFromHost(InputSender& sender)
//{
//	CAutoCounter autoBool(m_fOurSend);
//	IFS_RET(sender.Send());
//
//	//for (auto i : sender)
//	//{
//	//	PostMessage(CommonDataGlobal().hWndMonitor, WM_Send, i.ki.wVk, i.ki.dwFlags);
//	//}
//
//	RETURN_SUCCESS;
//}







