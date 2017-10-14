// slhook.cpp: определяет экспортированные функции для приложения DLL.
//

#include "stdafx.h"

//#include "../OpenSwitcher/Hooker.h"
//#include "../OpenSwitcher/Revert.h"





TStatus GetMsgProcInt(
	_In_  int nCode,
	_In_  WPARAM wParam,
	_In_  LPARAM lParam,
	bool& found)
{
	//SW_LOG_INFO_DEBUG(L"GetMsgProcInt");

	found = false;
	MSG* s = (MSG*)lParam;

	if (s->message != c_msgRevertID || s->wParam != c_msgWParm || s->lParam != c_msgLParm)
	{
		RETURN_SUCCESS;
	}

	LOG_INFO_2(L"GetMsgProcInt found message!");

	found = true;

	RETURN_SUCCESS;



	//if(s->message == WM_KEYDOWN || s->message==WM_SYSKEYDOWN || s->message==WM_KEYUP || s->message == WM_SYSKEYUP)
	//{
	//	TKeyCode k = (TKeyCode)s->wParam;
	//	std::wstring s1;
	//	CHotKey::ToString(k,s1);
	//	WPARAM p = (WPARAM)(s->message);
	//	SW_LOG_INFO_DEBUG(L"GetMsgProcInt k=%s(%S) lparm=0x%x", s1.c_str(), GetKeyStateName(GetKeyState(p)), s->lParam);
	//}
	//if(s->message == c_msgRevertID)
	//{
	//	SW_LOG_INFO_DEBUG(L"c_msgRevertID");
	//}

	RETURN_SUCCESS;
}

TStatus CallWndProcInt(
_In_  int nCode,
_In_  WPARAM wParam,
_In_  LPARAM lParam,
bool& found
)
{
	////SW_LOG_INFO_DEBUG(L"CallWndProcInt");
	//found = false;

	PCWPSTRUCT s = (PCWPSTRUCT) lParam;

	//LOG_INFO_2(L"CallWndProcInt msg=%u, w=%u, l=%u", s->message, s->wParam, s->lParam);

	if (s->message != c_msgRevertID || s->wParam != c_msgWParm /*|| s->lParam != c_msgLParm*/) 
	{
		RETURN_SUCCESS;
	}

	LOG_INFO_2(L"CallWndProcInt found message!");

	found = true;

	HKL lay = (HKL)s->lParam;
	//{
	//	LOG_INFO_2(L"1");
	//	IFS_RET(SwShared::Global().Init(true));
	//	LOG_INFO_2(L"2");
	//	CAutoWinMutexWaiter w(G_SwSharedMtx());
	//	LOG_INFO_2(L"3");
	//	lay = G_SwSharedBuf().sendData.lay;
	//	LOG_INFO_2(L"4");
	//}

	LOG_INFO_2(L"CallWndProcInt SW_CLIENT_SetLang lay=%x", lay);
	HKL hkl_prev = ActivateKeyboardLayout((HKL)lay, KLF_SETFORPROCESS);
	//*(int*)(NULL) = 10;

	//HWND hFocus = GetFocus();
	//DWORD res = Edit_GetSel(hFocus);

	//WORD l = LOWORD(res);
	//WORD r = HIWORD(res);

	//SW_LOG_INFO_DEBUG(L"res=%u,l=%u,r=%u", res, l, r);

	//if (TestFlag(sendData.flags, SW_CLIENT_SetLang))
	//{
	//	if(sendData.lay)
	//	{
	//		SW_LOG_INFO_DEBUG(L"CallWndProcInt SW_CLIENT_SetLang lay=%x", sendData.lay);
	//		HKL hkl_prev = ActivateKeyboardLayout((HKL)sendData.lay, KLF_SETFORPROCESS);
	//		IFW_RET(hkl_prev != NULL);
	//	}
	//}
	//if (TestFlag(sendData.flags, SW_CLIENT_PUTTEXT))
	//{
	//	SW_LOG_INFO_DEBUG(L"CallWndProcInt SW_CLIENT_PUTTEXT");
	//	SW_IFRET_TSTATUS(ClearMods());
	//	SW_IFRET_TSTATUS(SendKeys(sendData));
	//}
	//if (TestFlag(sendData.flags, SW_CLIENT_CTRLC))
	//{
	//	SW_LOG_INFO_DEBUG(L"CallWndProcInt c_msgWParm_Flags SW_CLIENT_CTRLC");
	//	SW_IFRET_TSTATUS(SendCtrlC());
	//}

	//CAutoWinMutexWaiter w(G_SwSharedMtx());
	//G_SwSharedBuf().sendData.fRequestComplite = 1;

	RETURN_SUCCESS;
}

EXTERN_C
{
__declspec(dllexport) 
LRESULT CALLBACK GetMsgProc(
_In_  int nCode,
_In_  WPARAM wParam,
_In_  LPARAM lParam
)
{
	if (nCode == HC_ACTION)
	{
		bool found = false;
		IFS_LOG(GetMsgProcInt(nCode, wParam, lParam, found));
		if (found)
			return 0;
	}
	return CallNextHookEx(0, nCode, wParam, lParam);
}

__declspec(dllexport)
LRESULT CALLBACK CallWndProc(
	_In_  int nCode,
	_In_  WPARAM wParam,
	_In_  LPARAM lParam
	)
{
	if (nCode == HC_ACTION)
	{
		bool found = false;
		IFS_LOG(CallWndProcInt(nCode, wParam, lParam, found));
		if (found)
			return 0;
	}
	return CallNextHookEx(NULL, nCode, wParam, lParam);
}

__declspec(dllexport)
LRESULT CALLBACK KeyboardProc(
	_In_  int code,
	_In_  WPARAM wParam,
	_In_  LPARAM lParam
	)
	{
		return CallNextHookEx(NULL, code, wParam, lParam);
	}

}

BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
	)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		//SW_LOG_INFO_DEBUG(L"Attach To process %d", GetCurrentProcessId());
		break;
	case DLL_THREAD_ATTACH:
		//SW_LOG_INFO_DEBUG(L"Attach To process(thread) %d", GetCurrentThreadId());
		break;
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}







