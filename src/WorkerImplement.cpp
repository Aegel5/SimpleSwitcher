#include "WorkerImplement.h"
#include "ParseSnippet.h"

void WorkerImplement::ProcessKeyMsg(const Message_KeyType& keyData)
{
	TKeyCode vkCode = keyData.vkCode;
	auto scan_ext = keyData.scan_ext;

	const auto& cur_hotkey = keyData.cur_hotKey;

	LOG_ANY("ProcessKeyMsg {} curState={}", CHotKey::ToString(vkCode), cur_hotkey.ToString());

	m_is_last_caps = keyData.is_caps; // сохраним последнее известное значение. Нажатие caps по идее должно нам привести сюда.

	if (CHotKey::IsKnownMods(vkCode)) {
		return; // не очищаем текущий буфер нажатых клавиш так как они могут быть частью наших хот-кеев
	}

	//if (keyData.hk != hk_NULL) { // это событие down для нашей hot key up. 
	//	if (!IsNeedSavedWords(keyData.hk)) {
	//		ClearAllWords();
	//	}
	//	return; // пропуск.
	//}

	TKeyBaseInfo key {
		.vk_code = vkCode,
		.scan_code = scan_ext,
		.is_shift = cur_hotkey.HasMod(VK_SHIFT),
		.is_caps = keyData.is_caps,
	};
	auto get_lay = [this] {
		CheckCurLay(); // попробуем добавить получение текущей раскладки на каждое нажатие для более точного определения символа (ради capslock handle).
		return topWndInfo2.lay;
	};
	key.type = AnalizeTyped(cur_hotkey, vkCode, scan_ext, get_lay, key);

	switch (key.type) {

	case KEYTYPE_BACKSPACE:
	{
		m_cycleList.DeleteLastSymbol();
		break;
	}
	default:
	{
		bool is_shift = cur_hotkey.HasMod(VK_SHIFT);

		// todo: так это не работает. Нужно добавлять Shift только буквам (сложно) или временно включать/отключать capslock.
		//if (keyData.is_caps) is_shift = !is_shift; 

		m_cycleList.AddKeyToList(key);
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

//TStatus ClipHasTextFormating(bool& fres)
//{
//	fres = false;
//
//	CAutoClipBoard clip;
//	IFS_RET(Open2(clip));
//
//	UINT format = 0;
//	while (1)
//	{
//		format = EnumClipboardFormats(format);
//		//LOG_ANY(L"Found format %u", format);
//		if (format == 0)
//		{
//			break;
//		}
//		if (format >= 40000)
//		{
//			fres = true;
//			break;
//		}
//	}
//
//	RETURN_SUCCESS;
//}

void toUpper(std::wstring& buf) {
	auto copy = buf;
	StrUtils::ToUpper(buf);
    if (copy == buf){
		StrUtils::ToLower(buf);
    }
}

void InvertCase(std::wstring& buf) {
	for (auto& c : buf) {
		if (std::iswupper(c)) {
			c = std::towlower(c);
		}
		else {
			c = std::towupper(c);
		}
	}
}

TStatus WorkerImplement::GetClipStringCallback() {
	LOG_ANY(L"GetClipStringCallback");

	auto data = m_clipWorker.getCurString();

	// bool needResotore = !m_savedClipData.empty();

	if (data.empty()) {
        LOG_ANY(L"data empty");
    }
	else if (data.length() > 100) {
		LOG_ANY(L"TOO MANY TO REVERT. SKIP");
	}
	else {
		if (m_lastRevertRequest == hk_RevertSelelected) {
            ClipboardToSendData(data);
        } else if(m_lastRevertRequest == hk_toUpperSelected || m_lastRevertRequest == hk_InvertCaseSelected) {

			if (m_lastRevertRequest == hk_toUpperSelected)
				toUpper(data);
			else
				InvertCase(data);

            m_clipWorker.setString(data);

			IFS_LOG(ProcessRevert({.flags = SW_CLIENT_CTRLV }));

    //        if (needResotore) {
    //            needResotore = data != m_savedClipData;
				//if(needResotore)
				//	Sleep(20); // подождем немного, чтобы не перезатереть наши данные восстановлением буфера.
    //        }
			
		}
	}

	if (m_clipWorker.HasBackup()) {
		Sleep(20); // подождем немного, чтобы не перезатереть наши данные восстановлением буфера.
		RequestWaitClip(CLRMY_hk_RESTORE); // делаем это только чтобы не вызывалась очистка формата
		m_clipWorker.Restore();
        //m_clipWorker.setString(m_savedClipData);
        //m_savedClipData.clear();
    }

	RETURN_SUCCESS;
}

void WorkerImplement::CliboardChanged()
{
	LOG_ANY(L"ClipboardChangedInt");

	if (!g_enabled.IsEnabled()) {
		LOG_ANY(L"skip because disabled");
	}

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
				Worker()->PostMsg([](WorkerImplement* w) {w->GetClipStringCallback(); }, 20);
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
		Worker()->PostMsg([](WorkerImplement* w) {w->ClipboardClearFormat2(); }, 500);
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

namespace {
	void ProcessSnippet(const string& s) {
		if (s.empty()) return;
		auto parts = ParseSnippet(s);
		InputSender is;
		for (const auto& it : parts) {

			// VK_CODE
			if (it.inBrackets) {
				for (const auto& hk_string : StrUtils::Split(it.text, ',')) {
					auto hk = CHotKey::FromString(hk_string);
					is.AddPressVk(hk);
				}
				continue;
			}

			// UNICODE
			auto str = StrUtils::Convert(it.text);
			for (auto c : str) {
				is.AddUnicodePress(c);
			}

		}
		is.Send();
	}
}

TStatus WorkerImplement::RunProcess(HotKeyType hk, bool after_wait) {

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
		Worker()->PostMsg([hk](auto p) {p->RunProcess(hk, true); }, it.delay);
		RETURN_SUCCESS;
	}

	if (it.type == CommandType::Snippet) {
		ProcessSnippet(it.cmd);
		RETURN_SUCCESS;
	}

	auto wpath = StrUtils::Convert(it.cmd);
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

void WorkerImplement::ProcessOurHotKey(Message_Hotkey&& keyData) {

	auto hk = keyData.hk;
	const auto& key = keyData.hotkey;

	if (keyData.delayed_from != 0 && keyData.delayed_from <= m_lastHotKeyTime) {
		LOG_ANY("skip hotkey {} possible was double press", key.ToString());
		return;
	}
	m_lastHotKeyTime = GetTickCount64();

	GETCONF;

	m_clear_alfter_selected = hk == hk_RevertSelelected;

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
		// дадим событию up время на обработку в ОС, так как для нас, она уже поднята 
		// (т.е. мы ее не поднимаем и она может конфликтовать с тем, что мы собираемся ввести)
		LOG_ANY(L"pause for #up key");
		Sleep(5);
	}

	if (!IsNeedSavedWords(hk) && !Utils::is_in(hk, hk_EmulateCapsLock, hk_EmulateScrollLock)) {
		ClearAllWords();
	}

	m_lastRevertRequest = hk;

	LOG_ANY("Hotkey start {}({})", HotKeyTypeName(hk), (int)hk);

	if (!g_enabled.IsEnabled() && hk != hk_ToggleEnabled) {
		LOG_ANY("Skip hk because disabled");
		return;
	}

	Fix_AltOrWin(keyData.cur_keys_down);

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

	if (TestFlag(hk, hk_RunProgram_flag)) {
		IFS_LOG(RunProcess(hk));
		return;
	}

	// Если нам нужно самим что-то вводить, то сбросим сразу все клавиши для системы. 
	// Будет двойной (или даже тройной и более) up, но пока что это не проблема... 
	UpAllKeys(keyData.cur_keys_down);

	if (hk == hk_Fix_RAlt) {
		FixCtrlAlt(keyData.hotkey);
		return;
	}

	if (Utils::is_in(hk, hk_EmulateCapsLock, hk_EmulateScrollLock)) {
		TKeyCode k = (hk == hk_EmulateCapsLock) ? VK_CAPITAL : VK_SCROLL;
		InputSender::SendVkKey(k);
		return;
	}

	auto process = [this, hk, cfg]() -> TStatus {

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
			//m_savedClipData = m_clipWorker.getCurString();
			RequestWaitClip(CLRMY_GET_FROM_CLIP); // регистрируем запрос.
			LOG_ANY(L"save buff");
			m_clipWorker.BackupCurrent();
			if (m_clipWorker.HasBackup()) {
				// пулим очистку памяти на всякий случай
				Worker()->PostMsg([this](auto w) {
					if ((GetTickCount64() - m_dwLastCtrlCReqvest) > 7000) {
						m_clipWorker.ClearBackup();
					}
				}, 10000);
			}
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

TStatus WorkerImplement::ProcessRevert(ContextRevert&& ctxRevert)
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

	if (m_sTopProcName == L"notepad.exe" && IsWindows11OrGreater())
		Sleep(15);

	if (TestFlag(ctxRevert.flags, SW_CLIENT_PUTTEXT))
	{
		InputSender::SendKeys(ctxRevert.keylist, m_is_last_caps);
	}

	if (TestFlag(ctxRevert.flags, SW_CLIENT_CTRLC))
	{
		LOG_ANY(L"Send ctrlc...");
        CHotKey ctrlc(VK_CONTROL, VKE_C);
        InputSender::SendWithPause(ctrlc);
	}

	if (TestFlag(ctxRevert.flags, SW_CLIENT_CTRLV))
	{
        CHotKey ctrlc(VK_CONTROL, VKE_V);
		InputSender::SendWithPause(ctrlc);
	}

	LOG_ANY(L"Revert complete");

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

void WorkerImplement::SwitchLangByEmulate(HKL lay) {

	GETCONF;

	CHotKey altshift = cfg->win_hotkey_cycle_lang;
	bool switch_until = false;

	if ((size_t)lay != HKL_NEXT) { 

		auto info = cfg->layouts_info.GetLayoutInfo(lay);
		if (info != nullptr && !info->win_hotkey.IsEmpty()) {
			altshift = info->win_hotkey;
		}
		else {
			if (topWndInfo2.lay == lay) { // на верх пока не выносим проверку, так как нет 100% гарантии в корректности определения тек. раскладки, пока тестим.
				return;
			}
			if (cfg->layouts_info.info.size() != 2)
				switch_until = true;
		}
	}

	if (altshift.IsEmpty()) {
		LOG_WARN(L"hot key not setup. skip switch");
		return;
	}

	LOG_ANY("Emulate with {}", altshift.ToString());

	InputSender::SendHotKey(altshift);

	if (switch_until) {
		auto cur = topWndInfo2.lay;
		for (int i = 0; i < std::ssize(cfg->layouts_info.info) - 2; i++) {
			auto next = WaitOtherLay(cur);
			if (next == 0) {
				LOG_WARN(L"cant't wait in emulate");
				return;
			}
			if (next == lay) {
				LOG_ANY("ok. found lay");
				return;
			}
			LOG_ANY("skip lay={}", (void*)next);
			cur = next;
			InputSender::SendHotKey(altshift);
		}
		LOG_WARN("not found needed lay!");
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

	HKL temp = 0;
	bool just_send = false;

	if (cfg->layouts_info.GetLayoutInfo(lay) == nullptr) {
		auto str = std::format(L"{:x}", (size_t)lay);
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









