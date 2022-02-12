#include <windows.h>
#include "../SimpleSwitcher/src/loader_api.h"
#include "../libtools/inc_basic.h"

EXTERN_C{
	__declspec(dllexport)
	LRESULT CALLBACK hook_proc(
		_In_  int nCode,
		_In_  WPARAM wParam,
		_In_  LPARAM lParam
	)
{

	if (nCode == HC_ACTION) {
		CWPSTRUCT* data = (CWPSTRUCT*)lParam;
		if (data->message == WM_INPUTLANGCHANGE) {
			LOG_INFO_1(L"WM_INPUTLANGCHANGE");
			HWND my = FindWindow(c_sClassNameServer2, NULL);
			if (my != NULL) {
				PostMessage(my, data->message, data->wParam, data->lParam);
			}
			else {
				LOG_WARN(L"main hwnd not found");
			}
		}
	}
	//return 0;
	return CallNextHookEx(NULL, nCode, wParam, lParam);
}
}

HMODULE inst;

BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
	)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		inst = hModule;
		if (Utils::IsDebug())
			SetLogLevel(LOG_LEVEL_2);
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