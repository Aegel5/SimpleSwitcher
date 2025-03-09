#include "stdafx.h"

#include "Hooker.h"
#include "Settings.h"
#include "Dispatcher.h"
#include "Encrypter.h"

#include "gui/decent_gui.h"

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

	//if(g_setsgui.isDashSeparate)
	//{
	//	if(key.ValueKey() == 189 && key.Size() == 1)
	//	{
	//		return KEYTYPE_SPACE;
	//	}
	//}

	BYTE keyState[256] = { 0 };
	keyState[VK_SHIFT] = m_curKeyState.HasMod(VK_SHIFT) ? 0x80 : 0;
	TCHAR sBufKey[0x10] = { 0 };
	int res = ToUnicodeEx(key.ValueKey(), m_curScanCode.scan, keyState, sBufKey, ARRAYSIZE(sBufKey), 0, NULL);
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

TStatus Hooker::ProcessKeyMsg(KeyMsgData& keyData)
{
	KBDLLHOOKSTRUCT* k = &keyData.ks;
	WPARAM wParam = keyData.wParam;

	TKeyCode vkCode = (TKeyCode)k->vkCode;
	KeyState curKeyState = GetKeyState(wParam);
    bool isInjected      = TestFlag(k->flags, LLKHF_INJECTED);
    bool isAltDown      = TestFlag(k->flags, LLKHF_ALTDOWN);
	bool isSysKey = wParam == WM_SYSKEYDOWN || wParam == WM_SYSKEYUP;
	bool isExtended = TestFlag(k->flags, LLKHF_EXTENDED);

	m_curScanCode.scan = k->scanCode;
	m_curScanCode.is_ext = isExtended;


	m_curStateWrap.Update(k, curKeyState);
	m_curKeyState = m_curStateWrap.state;

	LOG_INFO_3(L"ProcessKeyMsg %s curState=%s", CHotKey::ToString(vkCode).c_str(), m_curKeyState.ToString().c_str());

	if (curKeyState != KEY_STATE_DOWN)
		RETURN_SUCCESS;

	auto conf = conf_get();

	// Чтобы не очищался буфер клавиш на нажатии наших хоткеев.
	//for (const auto& [hkId, key] : conf->All_hot_keys())
	//{
	//	if (m_curKeyState.Compare(key, CHotKey::COMPARE_IGNORE_KEYUP))
	//	{
	//		auto s1 = m_curKeyState.ToString();
	//		auto s2 = key.ToString();
	//		LOG_INFO_2(L"skip hk diff only flags k1=%s, k2=%s hId=%u", s1.c_str(), s2.c_str(), hkId);
	//		RETURN_SUCCESS;
	//	}
	//}

	//if (CHotKey::IsKnownMods(vkCode))
	//	RETURN_SUCCESS;

	if (CHotKey::Normalize(vkCode) == VK_SHIFT) {
		RETURN_SUCCESS;
	}

	HandleSymbolDown();

	RETURN_SUCCESS;
	
}


TStatus Hooker::Init()
{
	ClearAllWords();
    IFS_LOG(GetPath_fileExe_lower(m_sSelfExeName));
	//IFS_RET(m_clipWorker.Init());

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

	//m_caseAnalizer.Clear();

	{
		//std::unique_lock<std::recursive_mutex > lock(m_mtxKeyList);
		m_wordList.clear();
		ClearCycleRevert();
	}

	//CHotKey curCopy = m_curKeyState;
	//for (TKeyCode* k = curCopy.ModsBegin(); k != curCopy.ModsEnd(); ++k)
	//{
	//	if (GetAsyncKeyState(*k) & 0x8000)
	//	{
	//	}
	//	else
	//	{
	//		LOG_INFO_2(L"Up key ? because GetAsyncKeyState"
	//			// , CHotKey::ToString(*k).c_str()
	//		);
	//		m_curKeyState.Remove(*k);
	//	}
	//}
	
}

void Hooker::AddKeyToList(TKeyType type, CHotKey hotkey, TScanCode_Ext scan_code)
{
	ClearCycleRevert();

	if (m_wordList.size() >= c_nMaxLettersSave)
	{
		m_wordList.pop_front();
	}

	TKeyHookInfo key2;

	key2.key().vk_code = hotkey.ValueKey();
	key2.key().scan_code = scan_code;
	if (hotkey.Size() == 2) {
		key2.key().shift_key = hotkey.At(1); // надеемся это shift, пока так.
	}
	key2.type = type;
	if (hotkey.ValueKey() == VK_OEM_2 && conf_get()->isTryOEM2) {
		SetFlag(key2.keyFlags, TKeyFlags::SYMB_SEPARATE_REVERT);
	}

	IFS_LOG(Encrypter::Encrypt(key2));
	m_wordList.push_back(key2);
}
TStatus Hooker::FillKeyToRevert(TKeyRevert& keyList, HotKeyType typeRevert) // m_wordList -> keyList
{
	auto get_decrtypted = [this](int i) {
		TKeyHookInfo cur;
		cur.crypted = m_wordList[i].crypted;
		Encrypter::Decrypt(cur);
		return cur.key();
	};

	//if (typeRevert == hk_RevertRecentTyped)
	//{
	//	// reset this
	//	m_nCurrentRevertCycle = -1;

	//	// get all
	//	for (int i = 0; i < (int)m_wordList.size(); ++i)
	//	{
	//		keyList.push_back(get_decrtypted(i));
	//	}

	//	RETURN_SUCCESS;
	//}

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
        auto lay  = CurLay();
		SHORT res = VkKeyScanEx(c, lay);
		if (res == -1)
		{
			for (int i = 0; i < count; ++i)
			{
                if (layouts[i] != lay)
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

		TKeyBaseInfo key;

		key.vk_code = code;
		if (TestFlag(mods, 0x1))
			key.shift_key = VK_SHIFT;
		
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
	//m_clipWorker.PostMsg(ClipMode_ClipClearFormat);
	IFS_LOG(m_clipWorker.ClipboardClearFormat());
	RETURN_SUCCESS;
}

TStatus RequestClearFormat()
{
	auto timeId = SetTimer(gdata().hWndMonitor, c_timerIdClearFormat, 500, NULL);
	IFW_RET(timeId != 0);
	RETURN_SUCCESS;
}

void toUpper(std::wstring& buf) {

	wxString str1 = buf;

	str1.UpperCase();
    auto r1 = str1.ToStdWstring();
    if (r1 != buf)
        buf = r1;
    else {
        wxString str2 = buf;
        str2.MakeLower();
        buf = str2.ToStdWstring();
    }

}

TStatus Hooker::GetClipStringCallback() {
	LOG_INFO_1(L"GetClipStringCallback");

	auto data = m_clipWorker.getCurString();

	bool needResotore = !m_savedClipData.empty();

	if (data.empty()) {
        LOG_INFO_1(L"data empty");
    }
	else if (data.length() > 100) {
		LOG_INFO_1(L"TOO MANY TO REVERT. SKIP");
	}
	else {
        ContextRevert ctxRev;
        ctxRev.typeRevert = m_lastRevertRequest;
		if (m_lastRevertRequest == hk_RevertSel) {

            IFS_LOG(ClipboardToSendData(data, ctxRev.keylist));

    //        for (auto k : ctxRev.keylist) {
				//TKeyType type = GetCurKeyType(k);
				//AddKeyToList(type, k);
    //        }

            ctxRev.lay        = getNextLang();
            ctxRev.flags      = SW_CLIENT_PUTTEXT | SW_CLIENT_SetLang;
            IFS_LOG(ProcessRevert(ctxRev));

        } else if(m_lastRevertRequest == hk_toUpperSelected) {

			toUpper(data);
            m_clipWorker.setString(data);

            ctxRev.flags      = SW_CLIENT_CTRLV;
            IFS_LOG(ProcessRevert(ctxRev));
            if (needResotore) {
                needResotore = data != m_savedClipData;
				if(needResotore)
					Sleep(20); // подождем немного, чтобы не перезатереть наши данные восстановлением буфера.
            }
			
        }
	}

	if (needResotore) {
		RequestWaitClip(CLRMY_hk_RESTORE); // делаем это только чтобы не вызывалась очистка формата
        m_clipWorker.setString(m_savedClipData);
        m_savedClipData.clear();
    }

	RETURN_SUCCESS;
}

TStatus Hooker::ClipboardChangedInt()
{
	LOG_INFO_1(L"ClipboardChangedInt");

	auto dwTime = GetTickCount64() - m_dwLastCtrlCReqvest;
	bool isRecent = dwTime <= 500;

	EClipRequest request = m_clipRequest;
	m_clipRequest = CLRMY_NONE;

	if (request != CLRMY_NONE) {
        if (!isRecent) {
            LOG_WARN(L"Request not recent");
        }
        else {
            // LOG_INFO_1(
            //	L"dwTime=%u, request=%u, clear=%u, sec=%u",
            //	dwTime,
            //	request,
            //	g_settings_thread.fClipboardClearFormat,
            //	GetClipboardSequenceNumber());

            if (request == CLRMY_GET_FROM_CLIP) {
                // DWORD deltSec = GetClipboardSequenceNumber() - m_clipCounter;
                // LOG_INFO_1(L"delt=%u", deltSec);

                Sleep(20); // wait here, no need async
                
				//m_clipWorker.PostMsg(ClipMode_GetClipString);

                GetClipStringCallback();

                RETURN_SUCCESS;
            }

            if (request == CLRMY_hk_COPY) {
                // if (m_lastRevertRequest == hk_EmulCopyNoFormat)
                //{
                //	IFS_LOG(RequestClearFormat());
                //}
                RETURN_SUCCESS;
            }

            // if (request == CLR_hk_INSERT)
            //{
            //	if (isRecent)
            //	{

            //	}
            //	ContextRevert ctxRev;
            //	ctxRev.flags = SW_CLIENT_CTRLV;
            //	IFS_LOG(ProcessRevert(ctxRev));
            //}

            RETURN_SUCCESS;
        }
    }


	// --- This is user request ----

	if (conf_get()->fClipboardClearFormat)
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
						  AddKeyToList(type, m_curKeyState, m_curScanCode);
						  break;
	}
	case KEYTYPE_COMMAND_NO_CLEAR:
		break;
	default:
		ClearAllWords();
		break;
	}
}
void Hooker::UpAllKeys() {

	if (m_curStateWrap.Size() == 0) return;

	InputSender inputSender;
	if (m_curStateWrap.IsDownNow(VK_LMENU) || m_curStateWrap.IsDownNow(VK_LWIN)) {
		// если нажата только клавиша alt - то ее простое отжатие даст хрень - нужно отжать ее еще раз
		//inputSender.Add(VK_LMENU, KEY_STATE_DOWN);
		//inputSender.Add(VK_LMENU, KEY_STATE_UP); 	
		inputSender.Add(VK_CAPITAL, KEY_STATE_UP);
	}
	for (const auto& key : m_curStateWrap.EnumVk()) {
		inputSender.Add(key, KEY_STATE_UP);
	}

	IFS_LOG(SendOurInput(inputSender));

}
TStatus Hooker::ProcessRevert(ContextRevert& ctxRevert)
{
	bool fDels = false;

	bool needWaitLang = false;
    auto prevLay      = CurLay();
	if (TestFlag(ctxRevert.flags, SW_CLIENT_SetLang) && ctxRevert.lay){

		SetNewLay(ctxRevert.lay);

		needWaitLang =
            IsWindows10OrGreater(); // true; //        m_sTopProcName == L"searchapp.exe"; // возможно теперь всегда
                                                 //        нужно ждать?
	}


	if (TestFlag(ctxRevert.flags, SW_CLIENT_PUTTEXT) && TestFlag(ctxRevert.flags, SW_CLIENT_BACKSPACE))
	{
		if (fDels)
		{
			//IFS_RET(SendDels(ctxRevert.keylist.size()));
		}
		else
		{
			IFS_RET(SendBacks(ctxRevert.keylist.size()));
		}
	}


	//if (m_layoutTopWnd == 0 || m_sTopProcName == L"far.exe") {
 //       needWaitLang = false;
 //   }

	//needWaitLang = false;
	if (needWaitLang && !TestFlag(ctxRevert.flags, SW_CLIENT_NO_WAIT_LANG)) {
		WaitOtherLay(prevLay);
	}

	if (TestFlag(ctxRevert.flags, SW_CLIENT_PUTTEXT))
	{
		IFS_RET(SendKeys(ctxRevert.keylist));

	}
    auto send = [&](CHotKey k) {

        InputSender inputSender;
        inputSender.AddDown(k);
        IFS_RET(inputSender.Send());
        Sleep(1);
        inputSender.Clear();
        inputSender.AddUp(k);
        IFS_RET(inputSender.Send());

        RETURN_SUCCESS;
	};

	if (TestFlag(ctxRevert.flags, SW_CLIENT_CTRLC))
	{
        CHotKey ctrlc(VK_CONTROL, 67);
        IFS_LOG(send(ctrlc));
	}

	if (TestFlag(ctxRevert.flags, SW_CLIENT_CTRLV))
	{
        CHotKey ctrlc(VK_CONTROL, 0x56);
        IFS_LOG(send(ctrlc));
	}

	LOG_INFO_1(L"Revert complete");

	RETURN_SUCCESS;
}


TStatus Hooker::SendCtrlC(EClipRequest clRequest)
{
    m_savedClipData = m_clipWorker.getCurString();

	RequestWaitClip(clRequest);

    LOG_INFO_1(L"Send ctrlc...");

    ContextRevert ctxRev;
    ctxRev.flags = SW_CLIENT_CTRLC;

    IFS_RET(ProcessRevert(ctxRev));

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
		sender.Add(k, KEY_STATE_UP);
	}
	IFS_RET(SendOurInput(sender));
	RETURN_SUCCESS;
}

HKL Hooker::getNextLang () {
	HKL result = (HKL)HKL_NEXT;
	auto conf = conf_get();

	// если все enabled - то обычная циклическая смена
	if (conf->layouts_info.AllLayoutEnabled()) {
		return result;
	}

    auto lay = CurLay();
    if (lay == 0) {
        // к сожалению наш список работать не будет (
        // TODO возможно будет работать если послать несколь NEXT чтобы пропустить ненужную раскладку (нужен монитор тек
        // языка)

		LOG_WARN(L"___ NOT FOUND CUR LAY while customLangList.size > 1");
        return result;
    }

    HKL toSet = 0;
	const auto& lst = conf->layouts_info.info;
	for(int i = -1; const auto& it : lst){
		i++;
        if (lay == lst[i].layout) {
            if (i == lst.size() - 1) {
                toSet = lst[0].layout;
            } else {
                toSet = lst[i + 1].layout;
            }
            break;
        }
    }
    if (toSet == 0) // not found
    {
        LOG_WARN(L"not found hwnd lay in our list");
        toSet = lst[0].layout;
    }

    return toSet;
};


TStatus Hooker::NeedRevert2(ContextRevert& data)
{
	HotKeyType typeRevert = data.typeRevert;

	// --------------- skip

    bool allow_do_revert = true;

	auto conf = conf_get();

    if (m_sTopProcName == m_sSelfExeName) {
        LOG_INFO_1(L"Skip hotkey in self program");
        allow_do_revert = false;
    }

	// Сбросим сразу все клавиши для программы. Будет двойной (или даже тройной и более) up, но пока что это не проблема... 
	
	//// если нужно просто сменить язык - то не будем делать up 
	//// https://github.com/Aegel5/SimpleSwitcher/issues/61
	//// Для клавиши LCtrl событие отсылается дважды, причем второй раз без флага inject (баг windows?)
	//// в остальных случаях мы эмулируем нажатия каких-то клавиш, поэтому нужно сбросить текущее состояние нажатых клавиш.
	//bool skipUpKeys = (TestFlag(typeRevert, hk_SetLayout_flag) || typeRevert == hk_CycleCustomLang) && conf_get()->AlternativeLayoutChange == false;
	//if (!skipUpKeys) {
	//	UpAllKeys();
	//}

	UpAllKeys();

	if (TestFlag(typeRevert, hk_RunProgram_flag)) {
		int i = typeRevert;
		ResetFlag(i, hk_RunProgram_flag);
		auto conf = conf_get();
		if (i >= conf->run_programs.size()) {
			return SW_ERR_UNKNOWN;
		}
		const auto& it = conf->run_programs[i];
		LOG_ANY(L"run program {} {}", it.path.wc_str(), it.args.wc_str());

		procstart::CreateProcessParm parm;
		parm.sExe = it.path.wc_str();
		parm.sCmd = it.args.wc_str();
		parm.admin = it.elevated ? TSWAdmin::SW_ADMIN_ON : TSWAdmin::SW_ADMIN_OFF;
		parm.mode = procstart::SW_CREATEPROC_SHELLEXE;
		CAutoHandle hProc;
		IFS_RET(procstart::SwCreateProcess(parm, hProc));

		RETURN_SUCCESS;
	}


	// CHANGE LAYOUT WITHOUT REVERT

    if (Utils::is_in(typeRevert, hk_CapsGenerate, hk_ScrollGenerate)) {
        TKeyCode k = (typeRevert == hk_CapsGenerate) ? VK_CAPITAL : VK_SCROLL;
        InputSender inputSender;
        inputSender.Add(k, KEY_STATE_DOWN);
        inputSender.Add(k, KEY_STATE_UP);
        IFS_LOG(SendOurInput(inputSender));
        RETURN_SUCCESS;
    }

    IFS_RET(AnalizeTopWnd());

    if (typeRevert == hk_CycleCustomLang) {
        data.flags = SW_CLIENT_SetLang;
        data.lay   = getNextLang();
        IFS_RET(ProcessRevert(data));
        RETURN_SUCCESS;
    }

    if (TestFlag(typeRevert, hk_SetLayout_flag)) {

		int i = typeRevert;
		ResetFlag(i, hk_SetLayout_flag);

		auto conf = conf_get();

		auto info = conf->layouts_info.GetLayoutIndex(i);
		if (info == nullptr) {
			LOG_WARN(L"not found hot key for set layout");
			RETURN_SUCCESS;
		}

        data.flags = SW_CLIENT_SetLang | SW_CLIENT_NO_WAIT_LANG;
        data.lay   = info->layout;
        IFS_RET(ProcessRevert(data));

        RETURN_SUCCESS;
    }


	// REVERT AND CHANGE LAYOUT

    if(!allow_do_revert)
        RETURN_SUCCESS;


	if (Utils::is_in(typeRevert, hk_RevertSel, hk_toUpperSelected)) {
        IFS_RET(SendCtrlC(CLRMY_GET_FROM_CLIP));
        RETURN_SUCCESS;
    }

	auto nextLng = getNextLang();

	// ---------------classic revert---------------

	if (!(typeRevert == hk_RevertLastWord || typeRevert == hk_RevertCycle))
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
	LOG_INFO_1(L"NeedRevert %S, curstate=\"%s\"", HotKeyTypeName(typeRevert), m_curKeyState.ToString().c_str());

	ContextRevert ctxRevert;
	ctxRevert.typeRevert = typeRevert;
	m_lastRevertRequest = typeRevert;

	IFS_LOG(NeedRevert2(ctxRevert));

	RETURN_SUCCESS;
}

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

TStatus Hooker::SwitchLangByEmulate(HKL lay)
{
	InputSender inputSender;

	CHotKey altshift = conf_get()->GetHk(hk_CycleLang_win_hotkey).keys.key();

	if ((int)lay != HKL_NEXT) {
		auto info = conf_get()->layouts_info.GetLayoutInfo(lay);
		if (info == nullptr) {
			LOG_WARN(L"not found lay info");
			RETURN_SUCCESS;
		}
		if (info->win_hotkey.IsEmpty()) {
			LOG_WARN(L"hot key not setup");
			RETURN_SUCCESS;
		}
		altshift = info->win_hotkey;
	}
	//IFS_LOG(FoundEmulateHotKey(altshift));

	LOG_INFO_2(L"Add press %s", altshift.ToString().c_str());
	inputSender.AddPressVk(altshift);

	IFS_RET(SendOurInput(inputSender));

	// ------
	// TODO switch until we get needed layout.


	RETURN_SUCCESS;
}

void Hooker::CheckCurLay(bool forceSend) {

	auto old_lay = topWndInfo2.lay;

	topWndInfo2 = Utils::GetFocusedWndInfo();

	if (topWndInfo2.lay == 0) {
		// оставим прежний
		topWndInfo2.lay = old_lay;
	}
	else {
		if ((forceSend || old_lay != topWndInfo2.lay) && g_guiHandle != nullptr) {
			PostMessage(g_guiHandle, WM_LayNotif, (WPARAM)topWndInfo2.lay, 0);
		}
	}
}

TStatus Hooker::AnalizeTopWnd() {

    CheckCurLay();

	//HWND hwndFocused = NULL;
	//IFS_RET(Utils::GetFocusWindow(hwndFocused));
	//m_hwndTop = hwndFocused;

	//m_hwndTop = topWndInfo2.hwnd;

	//DWORD dwTopPid = 0;
	//DWORD dwIdThreadTopWnd = GetWindowThreadProcessId(hwndFocused, &dwTopPid);
	//IFW_LOG(dwIdThreadTopWnd != 0);

	IFS_LOG(Utils::GetProcLowerNameByPid(topWndInfo2.pid_top, m_sTopProcPath, m_sTopProcName));

	LOG_ANY(
		L"AnalizeTopWnd pid_top={}, pid_default={}, lay={:x} prg={}",
		topWndInfo2.pid_top,
		topWndInfo2.pid_default,
		(ULONGLONG)topWndInfo2.lay,
		m_sTopProcName
	);

	RETURN_SUCCESS;
}

TStatus Hooker::FixCtrlAlt(CHotKey key) {

	IFS_RET(AnalizeTopWnd());

	auto lay = conf_get()->fixRAlt_lay_;
	auto curLay = CurLay();

	// сбросим любые нажатые клавиши
	UpAllKeys();

	HKL temp = 0;
	bool just_send = false;

	if (conf_get()->layouts_info.GetLayoutInfo(lay) == nullptr) {
		auto str = std::format(L"{:x}", (int)lay);
		//const TChar* s = L"00000409";
		LOG_ANY(L"load temp layout {}", str);
		temp = LoadKeyboardLayout(str.c_str(), KLF_ACTIVATE);
		IFW_LOG(temp != NULL);

		if (temp == 0) {
			// не получится...
			just_send = true;
		}
		else {
			SetNewLayPost(temp);
		}

	}
	else {
		SetNewLay(lay);
	}

	WaitOtherLay(curLay);

	// отправляем
	InputSender inputSender;
	inputSender.AddPressVk(key);
	IFS_LOG(SendOurInput(inputSender));

	if (!just_send) {
		// переключаемся обратно.
		Sleep(200);
		SetNewLay(curLay);
	}

	if (temp != 0) {
		LOG_ANY(L"unload temp layout");
		IFW_LOG(UnloadKeyboardLayout(temp));
	}

	RETURN_SUCCESS;
}









