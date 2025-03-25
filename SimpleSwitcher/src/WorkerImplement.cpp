#include "sw-base.h"

void WorkerImplement::ProcessKeyMsg(const Message_KeyType& keyData)
{
	TKeyCode vkCode = keyData.vkCode;
	KeyState curKeyState = keyData.keyState;
	bool isExtended = TestFlag(keyData.flags, LLKHF_EXTENDED);
	TScanCode_Ext scan_ext{ keyData.scanCode, isExtended };

	m_curStateWrap.Update(vkCode, curKeyState);
	auto cur_hotkey = m_curStateWrap.state;

	LOG_ANY(L"ProcessKeyMsg {} curState={}", CHotKey::ToString(vkCode), cur_hotkey.ToString());

	if (curKeyState != KEY_STATE_DOWN)
		return;

	if (CHotKey::IsKnownMods(vkCode)) {
		return; // не очищаем текущий буфер нажатых клавиш так как они могут быть частью наших хот-кеев
	}

	if (keyData.hk != hk_NULL) { // это событие down для нашей hot key up. https://github.com/Aegel5/SimpleSwitcher/issues/70
		if (!IsNeedSavedWords(keyData.hk)) {
			ClearAllWords();
		}
		return; // пропуск.
	}

	TKeyType type = AnalizeTyped(cur_hotkey, vkCode, scan_ext, topWndInfo2.lay);

	switch (type) {
	case KEYTYPE_BACKSPACE:
	{
		m_cycleList.DeleteLastSymbol();
		break;
	}
	default:
	{
		m_cycleList.AddKeyToList(type, scan_ext, cur_hotkey.HasMod(VK_SHIFT));
		break;
	}
	case KEYTYPE_COMMAND_NO_CLEAR:
		break;
	case KEYTYPE_NONE:
	case KEYTYPE_COMMAND_CLEAR: {
		ClearAllWords();
		break;
	}
	}
}

TStatus ClipHasTextFormating(bool& fres)
{
	fres = false;

	CAutoClipBoard clip;
	IFS_RET(OpenClipboard(clip));

	UINT format = 0;
	while (1)
	{
		format = EnumClipboardFormats(format);
		//LOG_ANY(L"Found format %u", format);
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

TStatus WorkerImplement::ClipboardToSendData(std::wstring& clipdata, TKeyRevert& keylist)
{

	HKL layouts[10];
	int count = GetKeyboardLayoutList(std::ssize(layouts), layouts);
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

TStatus WorkerImplement::GetClipStringCallback() {
	LOG_ANY(L"GetClipStringCallback");

	auto data = m_clipWorker.getCurString();

	bool needResotore = !m_savedClipData.empty();

	if (data.empty()) {
        LOG_ANY(L"data empty");
    }
	else if (data.length() > 100) {
		LOG_ANY(L"TOO MANY TO REVERT. SKIP");
	}
	else {
        ContextRevert ctxRev;
        ctxRev.typeRevert = m_lastRevertRequest;
		if (m_lastRevertRequest == hk_RevertSelelected) {

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

void WorkerImplement::CliboardChanged()
{
	LOG_ANY(L"ClipboardChangedInt");

	auto dwTime = GetTickCount64() - m_dwLastCtrlCReqvest;
	bool isRecent = dwTime <= 500;

	EClipRequest request = m_clipRequest;
	m_clipRequest = CLRMY_NONE;

	if (request != CLRMY_NONE) {
        if (!isRecent) {
            LOG_WARN(L"Request not recent");
        }
        else {
            if (request == CLRMY_GET_FROM_CLIP) {
				Worker()->PostMsg([](WorkerImplement& w) {w.GetClipStringCallback(); }, 20);
				return;
            }

            if (request == CLRMY_hk_COPY) {
				return;
            }

			return;
        }
    }


	// --- This is user request ----

	if (conf_get_unsafe()->fClipboardClearFormat) {
		Worker()->PostMsg([](WorkerImplement& w) {w.ClipboardClearFormat2(); }, 500);
	}

	LOG_ANY(L"ClipboardChangedInt complete");
}

void WorkerImplement::ChangeForeground(HWND hwnd)
{
	LOG_ANY(L"Now foreground hwnd={}", (void*)hwnd);
	DWORD procId = 0;
	DWORD threadid = GetWindowThreadProcessId(hwnd, &procId);
	if (threadid != m_dwIdThreadForeground && procId != m_dwIdProcoreground)
	{
		//IFS_LOG(Utils::GetProcLowerNameByPid(procId, m_sTopProcPath, m_sTopProcName));
		//LOG_INFO_2(L"threadid=%d, procId=%d, sname=%s", threadid, procId, m_sTopProcName.c_str());
		ClearAllWords();
	}
	else {
		LOG_ANY(L"skip clear words");
	}
	m_dwIdThreadForeground = threadid;
	m_dwIdProcoreground = procId; 
}

TStatus WorkerImplement::ProcessRevert(ContextRevert& ctxRevert)
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
		InputSender::SendVkKey(VK_BACK, ctxRevert.keylist.size());
	}

	//needWaitLang = false;
	if (needWaitLang && !TestFlag(ctxRevert.flags, SW_CLIENT_NO_WAIT_LANG)) {
		WaitOtherLay(prevLay);
	}

	if (TestFlag(ctxRevert.flags, SW_CLIENT_PUTTEXT))
	{
		InputSender::SendKeys(ctxRevert.keylist);
	}

	if (TestFlag(ctxRevert.flags, SW_CLIENT_CTRLC))
	{
        CHotKey ctrlc(VK_CONTROL, 67);
        InputSender::SendWithPause(ctrlc);
	}

	if (TestFlag(ctxRevert.flags, SW_CLIENT_CTRLV))
	{
        CHotKey ctrlc(VK_CONTROL, 0x56);
		InputSender::SendWithPause(ctrlc);
	}

	LOG_ANY(L"Revert complete");

	RETURN_SUCCESS;
}


TStatus WorkerImplement::SendCtrlC(EClipRequest clRequest)
{
    m_savedClipData = m_clipWorker.getCurString();

	RequestWaitClip(clRequest);

    LOG_ANY(L"Send ctrlc...");

    ContextRevert ctxRev;
    ctxRev.flags = SW_CLIENT_CTRLC;

    IFS_RET(ProcessRevert(ctxRev));

	RETURN_SUCCESS;
}


HKL WorkerImplement::getNextLang () {
	HKL result = (HKL)HKL_NEXT;

	GETCONF;

	// если все enabled - то обычная циклическая смена
	if (cfg->layouts_info.AllLayoutEnabled()) {
		return result;
	}

    auto lay = CurLay();
    if (lay == 0) {
		LOG_WARN(L"___ NOT FOUND CUR LAY");
        return result;
    }

	lay = cfg->layouts_info.NextEnabledLayout(lay);
	if (lay == 0) {
		LOG_WARN(L"___ NOT FOUND NEXT LAY");
	}


    return lay;
};


TStatus WorkerImplement::NeedRevert(HotKeyType typeRevert) {

	LOG_ANY("Hotkey start {}({})", HotKeyTypeName(typeRevert), (int)typeRevert);

	ContextRevert data;
	data.typeRevert = typeRevert;
	m_lastRevertRequest = typeRevert;

	// --------------- skip

	bool allow_do_revert = true;

	GETCONF;

	if (m_sTopProcName == m_sSelfExeName) {
		LOG_ANY(L"Skip hotkey in self program");
		allow_do_revert = false;
	}

	// Сбросим сразу все клавиши для программы. Будет двойной (или даже тройной и более) up, но пока что это не проблема... 

	//// если нужно просто сменить язык - то не будем делать up 
	//// https://github.com/Aegel5/SimpleSwitcher/issues/61
	//// Для клавиши LCtrl событие отсылается дважды, причем второй раз без флага inject (баг windows?)
	//// в остальных случаях мы эмулируем нажатия каких-то клавиш, поэтому нужно сбросить текущее состояние нажатых клавиш.
	//bool skipUpKeys = (TestFlag(typeRevert, hk_SetLayout_flag) || typeRevert == hk_CycleSwitchLayout) && conf_get_unsafe()->AlternativeLayoutChange == false;
	//if (!skipUpKeys) {
	//	UpAllKeys();
	//}

	m_curStateWrap.UpAllKeys();

	if (TestFlag(typeRevert, hk_RunProgram_flag)) {
		int i = typeRevert;
		ResetFlag(i, hk_RunProgram_flag);
		if (i >= cfg->run_programs.size()) {
			return SW_ERR_UNKNOWN;
		}
		const auto& it = cfg->run_programs[i];
		auto path = it.path;
		Utils::NormalizeDelims(path);
		LOG_ANY(L"run program {} {}", path.wc_str(), it.args.wc_str());

		procstart::CreateProcessParm parm;
		parm.sExe = path.wc_str();
		parm.sCmd = it.args.wc_str();

		// todo - use proxy process for unelevated.
		//parm.admin = it.elevated ? TSWAdmin::SW_ADMIN_ON : TSWAdmin::SW_ADMIN_OFF;

		parm.mode = procstart::SW_CREATEPROC_SHELLEXE;
		CAutoHandle hProc;
		IFS_RET(procstart::SwCreateProcess(parm, hProc));

		RETURN_SUCCESS;
	}


	// CHANGE LAYOUT WITHOUT REVERT

	if (Utils::is_in(typeRevert, hk_EmulateCapsLock, hk_EmulateScrollLock)) {
		TKeyCode k = (typeRevert == hk_EmulateCapsLock) ? VK_CAPITAL : VK_SCROLL;
		InputSender::SendVkKey(k);
		RETURN_SUCCESS;
	}

	IFS_RET(AnalizeTopWnd());

	if (typeRevert == hk_CycleSwitchLayout) {
		data.flags = SW_CLIENT_SetLang;
		data.lay = getNextLang();
		IFS_RET(ProcessRevert(data));
		RETURN_SUCCESS;
	}

	if (TestFlag(typeRevert, hk_SetLayout_flag)) {

		int i = typeRevert;
		ResetFlag(i, hk_SetLayout_flag);

		GETCONF;

		auto info = cfg->layouts_info.GetLayoutIndex(i);
		if (info == nullptr) {
			LOG_WARN(L"not found hot key for set layout");
			RETURN_SUCCESS;
		}

		data.flags = SW_CLIENT_SetLang | SW_CLIENT_NO_WAIT_LANG;
		data.lay = info->layout;
		IFS_RET(ProcessRevert(data));

		RETURN_SUCCESS;
	}


	// REVERT AND CHANGE LAYOUT

	if (!allow_do_revert)
		RETURN_SUCCESS;


	if (Utils::is_in(typeRevert, hk_RevertSelelected, hk_toUpperSelected)) {
		IFS_RET(SendCtrlC(CLRMY_GET_FROM_CLIP));
		RETURN_SUCCESS;
	}

	auto nextLng = getNextLang();

	// ---------------classic revert---------------

	if (!Utils::is_in(typeRevert, hk_RevertLastWord, hk_RevertSeveralWords, hk_RevertAllRecentText)) {
		IFS_RET(SW_ERR_UNKNOWN, L"Unknown typerevert {}", (int)typeRevert);
	}

	auto to_revert = m_cycleList.FillKeyToRevert(typeRevert);
	if (to_revert.keys.empty()) {
		LOG_ANY(L"nothing to revert. skip");
		RETURN_SUCCESS;
	}
	m_cycleList.SetSeparateLast();
	bool isNeedLangChange = to_revert.needLanguageChange;
	data.keylist = std::move(to_revert.keys);
	data.flags = SW_CLIENT_PUTTEXT | SW_CLIENT_SetLang | SW_CLIENT_BACKSPACE;
	data.lay = isNeedLangChange ? nextLng : 0;
	IFS_RET(ProcessRevert(data));

	RETURN_SUCCESS;
}


TStatus WorkerImplement::SwitchLangByEmulate(HKL lay) {

	GETCONF;

	CHotKey altshift = cfg->GetHk(hk_CycleLang_win_hotkey).keys.key();

	if ((int)lay != HKL_NEXT) {
		auto info = cfg->layouts_info.GetLayoutInfo(lay);
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

	LOG_ANY(L"Emulate with {}", altshift.ToString());
	InputSender::SendHotKey(altshift);

	RETURN_SUCCESS;
}

void WorkerImplement::CheckCurLay(bool forceSend) {

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

TStatus WorkerImplement::AnalizeTopWnd() {

    CheckCurLay();

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

TStatus WorkerImplement::FixCtrlAlt(CHotKey key) {

	IFS_RET(AnalizeTopWnd());

	GETCONF;

	auto lay = cfg->fixRAlt_lay_;
	auto curLay = CurLay();

	// сбросим любые нажатые клавиши
	m_curStateWrap.UpAllKeys();

	HKL temp = 0;
	bool just_send = false;

	if (cfg->layouts_info.GetLayoutInfo(lay) == nullptr) {
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
			Utils::SetLayPost(topWndInfo2.hwnd_default, temp);
		}

	}
	else {
		SetNewLay(lay);
	}

	WaitOtherLay(curLay);

	// отправляем
	InputSender::SendHotKey(key);

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









