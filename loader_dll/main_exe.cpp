
#include <windows.h>

#include <string>

#include "../SimpleSwitcher/src/loader_api.h"
#include "../libtools/core/winAutoCleanBase.h"

inline bool IsSelf64()
{
#ifdef _WIN64
	return true;
#elif _WIN32
	return false;
#else
	!!ERROR!!
#endif
}

int APIENTRY WinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPTSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	
	std::string cmd = GetCommandLine();
	if (cmd.find("/load") == cmd.npos)
		return 0;

	auto* clname = IsSelf64() ? c_sClassName64 : c_sClassName32;
	auto dllname = !IsSelf64() ? "SimpleSwitcher.dll" : "SimpleSwitcher64.dll";

	if (FindWindow(clname, NULL))
		return 0; // Already exist
	WNDCLASSEX wcex = { 0 };

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.lpfnWndProc = DefWindowProc;
	wcex.hInstance = hInstance;
	wcex.lpszClassName = clname;

	RegisterClassEx(&wcex);

	HWND hWnd = CreateWindow(
		clname,
		"Title",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

	if (hWnd == 0)
		return 0;

	ChangeWindowMessageFilterEx(hWnd, WM_QUIT, MSGFLT_ALLOW, 0);

	CAutoHMODULE hHookDll = LoadLibrary(dllname);
	if (hHookDll.IsInvalid()) {
		hHookDll = LoadLibrary("SimpleSwitcher.dll");
	}
	if (hHookDll.IsInvalid()) {
		return 0;
	}

	HOOKPROC pfCallWndProc = (HOOKPROC)GetProcAddress(hHookDll, "hook_proc");
	if (pfCallWndProc == NULL) {
		auto err = GetLastError();
		return 0;
	}

	CAutoHHOOK hHookCallWndProc = SetWindowsHookEx(WH_CALLWNDPROC, pfCallWndProc, hHookDll, 0);
	if (hHookCallWndProc.IsInvalid())
		return 0;

	MSG msg;
	while (true)
	{
		BOOL bRet = GetMessage(&msg, NULL, 0, 0);

		if (bRet == 0)
			break;

		if (bRet == -1)		{
			break;
		}

		auto mesg = msg.message;

		//if (mesg == c_MSG_Quit_2)		{
		//	break;
		//}
		//else{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		//}
	}

	return 0;
	
}