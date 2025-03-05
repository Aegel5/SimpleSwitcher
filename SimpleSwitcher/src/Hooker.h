#pragma once

#include "Settings.h"
#include "KeysStruct.h"
#include "KeyTools.h"

#include <deque>
#include <list>

#include "InputSender.h"
//#include "CaseAnalazer.h"
#include "CClipWorker.h"

#include "lay_notif_from_dll.h"

#include "proc_enum.h"

static const int c_nMaxLettersSave = 100;



class Hooker
{
private:

	typedef std::deque<TKeyHookInfo> TWordList;
	typedef std::vector<CHotKey> TKeyToRevert;

	//TSyncVal32 requestCount = 0;

	//CaseAnalazer m_caseAnalizer;

public:



	Hooker()  	{}
	~Hooker() 	{}

	void ClearAllWords();
	bool HasAnyWord();
	void ClearCycleRevert();
	//bool IsOurInput();
	TStatus NeedRevert(HotKeyType typeRevert);
	TStatus NeedRevert2(ContextRevert& data);
	//void ThreadInputSender();
	TStatus AnalizeTopWnd();
	TStatus SwitchLangByEmulate(HKL_W lay);
	void CliboardChanged() 
	{ 
		IFS_LOG(ClipboardChangedInt()); 
		LOG_INFO_1(L"ClipboardChangedInt complete");
	}
	TStatus ClipboardChangedInt();
    TStatus GetClipStringCallback();
	TStatus ClipboardClearFormat2();
	TStatus ClipboardToSendData(std::wstring& clipdata, TKeyRevert& keylist);

	void ChangeForeground(HWND hwnd);
	TKeyType GetCurKeyType(CHotKey hotkey);
	TStatus ProcessKeyMsg(KeyMsgData& keyData);
	TStatus ClearModsBySend(CHotKey key);
	void HandleSymbolDown();
	void AddKeyToList(TKeyType type, CHotKey hotkey, TScanCode_Ext scan_code);
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

	TStatus SetNewLay(HKL lay) {

		LOG_INFO_1(L"Try set 0x%x lay", lay);


		if (conf_get()->AlternativeLayoutChange)
		{
			IFS_RET(SwitchLangByEmulate(lay));
		}
		else
		{
			LOG_INFO_1(L"post WM_INPUTLANGCHANGEREQUEST");
			PostMessage(m_hwndTop, WM_INPUTLANGCHANGEREQUEST, 0, (LPARAM)lay);
		}

		RETURN_SUCCESS;
	}

	void WaitOtherLay(HKL lay) {
		// Дождемся смены языка. Нет смысла переходить в асинхронный режим. Можем ждать прямо здесь.
		auto start = GetTickCount64();
		while (true)
		{
			auto curL = GetKeyboardLayout(topWndInfo2.threadid);
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

		if (SettingsGui::IsNeedSavedWords(hk) && !HasAnyWord()) {
			bool found = false;
			for (const auto& [hk2,key2] : conf_get()->All_hot_keys()) {
				if (!SettingsGui::IsNeedSavedWords(hk2) && key.Compare(key2)) {
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

		IFS_LOG(NeedRevert(hk));

	}

	HKL CurLay() { return        topWndInfo2.lay; }

public:

	ULONGLONG m_dwLastCtrlCReqvest = 0;



	EClipRequest m_clipRequest = CLRMY_NONE;
	//DWORD m_clipCounter = 0;

	DWORD m_dwIdThreadForeground = -1;
	DWORD m_dwIdProcoreground = -1;

	//DWORD m_dwIdThreadTopWnd = 0;
	//DWORD m_dwTopPid = 0;
	//HKL m_layoutTopWnd = 0;
	HWND m_hwndTop = 0;

	TopWndInfo topWndInfo2;

	std::wstring m_sTopProcName;
	std::wstring m_sTopProcPath;

	//CClipWorker m_clipWorker;

	CClipWorker m_clipWorker; 

	tstring m_savedClipData;



	HotKeyType m_lastRevertRequest;

	struct CycleRevert
	{
		int nIndexWordList;
		bool fNeedLanguageChange;
	};
	typedef std::vector<CycleRevert> TCycleRevertList;

	TStatus GenerateCycleRevertList();
	TStatus FillKeyToRevert(TKeyRevert& keyList, HotKeyType typeRevert);
	TStatus ProcessRevert(ContextRevert& ctxRevert);
	//TStatus TimerProcWaitClip2();

	static const int c_maxWordRevert = 7;

	std::wstring m_sSelfExeName;

	TWordList m_wordList;
	TCycleRevertList m_CycleRevertList;
	int m_nCurrentRevertCycle = -1;

	//TSyncVal32 m_fOurSend = 0;
	CHotKey m_curKeyState;
	CurStateWrapper m_curStateWrap;
	TScanCode_Ext m_curScanCode; 

	void UpAllKeys();

};

inline Hooker* g_hooker;













