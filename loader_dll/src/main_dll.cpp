#include <windows.h>

__declspec(dllexport)
LRESULT CALLBACK CallWndProc(
	_In_  int nCode,
	_In_  WPARAM wParam,
	_In_  LPARAM lParam
	)
{
	if (nCode == HC_ACTION)
	{

	}
	return CallNextHookEx(NULL, nCode, wParam, lParam);
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