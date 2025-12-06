#include "sw-base.h"

void WorkerImplement::ProcessKeyMsg(const Message_KeyType& keyData)
{
	TKeyCode vkCode = keyData.vkCode;
	auto scan_ext = keyData.scan_ext;

	const auto& cur_hotkey = keyData.cur_hotKey;

	LOG_ANY(L"ProcessKeyMsg {} curState={}", CHotKey::ToString(vkCode), cur_hotkey.ToString());

	if (CHotKey::IsKnownMods(vkCode)) {
		return; // не очищаем текущий буфер нажатых клавиш так как они могут быть частью наших хот-кеев
	}

	//if (keyData.hk != hk_NULL) { // это событие down для нашей hot key up. 
	//	if (!IsNeedSavedWords(keyData.hk)) {
	//		ClearAllWords();
	//	}
	//	return; // пропуск.
	//}

	TKeyTypeData data;
	TKeyType type = AnalizeTyped(cur_hotkey, vkCode, scan_ext, topWndInfo2.lay, data);

	switch (type) {
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

		m_cycleList.AddKeyToList(type, data, scan_ext, is_shift);
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

	bool needResotore = !m_savedClipData.empty();

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
		InputSender::SendKeys(ctxRevert.keylist);
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

	LOG_ANY(L"Emulate with {}", altshift.ToString());

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









