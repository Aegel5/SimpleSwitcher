#include <tchar.h>
#include <windows.h>

#include <string>

#include "../libtools/inc_basic.h"

#include "../SimpleSwitcher/src/loader_api.h"

TStatus Main(HINSTANCE hInstance) {
	std::wstring cmd = GetCommandLine();
	//if (cmd.find(L"/load") == cmd.npos) {
	//	LOG_WARN("/load not found");
	//	RETURN_SUCCESS;
	//}

	auto clname = IsSelf64() ? c_sClassName64_2 : c_sClassName32_2;
	auto dllname = IsSelf64() ? L"SimpleSwitcher64.dll" : L"SimpleSwitcher.dll";

	if (FindWindow(clname, NULL)) {
		LOG_WARN("already lang4et");
		RETURN_SUCCESS;
	}
	WNDCLASSEX wcex = { 0 };

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.lpfnWndProc = DefWindowProc;
	wcex.hInstance = hInstance;
	wcex.lpszClassName = clname;

	RegisterClassEx(&wcex);

	HWND hWnd = CreateWindow(
		clname,
		L"Title",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

	IFW_RET(hWnd != NULL);

	IFW_LOG(ChangeWindowMessageFilterEx(hWnd, WM_QUIT, MSGFLT_ALLOW, 0));

	CAutoHMODULE hHookDll = LoadLibrary(dllname);
	//if (hHookDll.IsInvalid()) {
	//	hHookDll = LoadLibrary("SimpleSwitcher.dll");
	//}
	IFW_RET(hHookDll.IsValid());

	HOOKPROC pfCallWndProc = (HOOKPROC)GetProcAddress(hHookDll, "hook_proc");
	IFW_RET(pfCallWndProc != NULL);

	CAutoHHOOK hHookCallWndProc = SetWindowsHookEx(WH_CALLWNDPROC, pfCallWndProc, hHookDll, 0);
	IFW_RET(hHookCallWndProc.IsValid());

	//RETURN_SUCCESS;

	MSG msg;
	while (true)
	{
		BOOL bRet = GetMessage(&msg, NULL, 0, 0);

		if (bRet <= 0)
			break;

		auto mesg = msg.message;

		//if (mesg == c_MSG_Quit_2)		{
		//	break;
		//}
		//else{
		if (!IsDialogMessage(hWnd, &msg)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		//}
	}

	RETURN_SUCCESS;
}

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPTSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);

	if (Utils::IsDebug())
		SetLogLevel(LOG_LEVEL_2);

	IFS_LOG(Main(hInstance));
	return 0;
	
}