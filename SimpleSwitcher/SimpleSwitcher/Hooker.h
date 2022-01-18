﻿#pragma once

#include "Settings.h"
#include "KeysStruct.h"
#include "KeyTools.h"

#include <deque>
#include <list>

#include "InputSender.h"
#include "CaseAnalazer.h"
#include "CClipWorker.h"

static const int c_nMaxLettersSave = 100;



class Hooker
{
private:




	typedef std::deque<TKeyHookInfo> TWordList;
	typedef std::vector<CHotKey> TKeyToRevert;

	//TSyncVal32 requestCount = 0;

	CaseAnalazer m_caseAnalizer;

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
	void AddToWordsByHotKey(CHotKey key);
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
	void AddKeyToList(TKeyType type, CHotKey hotkey);
	TStatus Init();
	TStatus SendCtrlC(EClipRequest clRequest);
	TStatus SavePrevDataCallback(EClipRequest clRequest);
	bool GetTypeForKey(CHotKey curkey, HotKeyType& type, bool& isUp);
	void RequestWaitClip(EClipRequest clRequest)
	{
		m_clipRequest = clRequest;
		m_clipCounter = GetClipboardSequenceNumber();
		m_dwLastCtrlCReqvest = GetTick();
	}
	void RequestChangeCase()
	{
		LOG_INFO_1(L"RequestChangeCase");
		tstring data;
		RequestWaitClip(CLR_hk_INSERT);
		m_caseAnalizer.GenerateNexCurKeys(data);
		m_clipWorker.SetData(data);

		m_clipWorker.PostMsg(ClipMode_InsertData);
	}
public:

	DWORD m_dwLastCtrlCReqvest = 0;



	EClipRequest m_clipRequest = CLR_NONE;
	DWORD m_clipCounter = 0;

	DWORD m_dwIdThreadForeground = -1;
	DWORD m_dwIdProcoreground = -1;

	DWORD m_dwIdThreadTopWnd = 0;
	DWORD m_dwTopPid = 0;
	HKL m_layoutTopWnd = 0;
	HWND m_hwndTop = 0;

	std::wstring m_sTopProcName;
	std::wstring m_sTopProcPath;

	CClipWorker m_clipWorker;



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
	TStatus TimerProcWaitClip2();

	static const int c_maxWordRevert = 7;

	std::wstring m_sSelfExeName;

	TWordList m_wordList;
	TCycleRevertList m_CycleRevertList;
	int m_nCurrentRevertCycle = -1;

	//TSyncVal32 m_fOurSend = 0;
	CHotKey m_curKeyState;
	DWORD m_curScanCode = 0;

	HotKeyType m_needRevertUnderUP = hk_MAX;

};













