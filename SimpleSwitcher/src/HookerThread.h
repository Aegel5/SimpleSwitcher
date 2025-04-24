#pragma once

#include <thread>
#include "CMainWorker.h"

#ifdef __WXMSW__
#include "wx/msw/private.h"
#endif

class CoreWorker {

    std::thread core_work;
	std::atomic<HWND> m_hWnd = nullptr;

	void StartMonitor() {

		LOG_ANY(L"StartMonitor...");
		IFW_LOG(SetPriorityClass(GetCurrentProcess(), ABOVE_NORMAL_PRIORITY_CLASS));

		m_hWnd = WinUtils::CreateMsgWin(L"SimpleSw_325737FD_Serv");
		IFW_LOG(m_hWnd != NULL);
		
		IFW_LOG(AddClipboardFormatListener(m_hWnd));
		//IFW_LOG(ChangeWindowMessageFilterEx(hWnd, WM_LayNotif, MSGFLT_ALLOW, 0));

		Hooker hooker;
		IFS_LOG(hooker.StartHook());

		MSG msg;
		while (GetMessage(&msg, NULL, 0, 0) > 0) {
			if (msg.message == WM_CLIPBOARDUPDATE) {
				Worker()->PostMsg([](auto w) {w->CliboardChanged(); });
			}
		}
	}

public:

    CoreWorker() {
        core_work = std::thread(std::bind(&CoreWorker::StartMonitor, this));
    }

    ~CoreWorker() {

		PostMessage(m_hWnd, WM_QUIT, 0, 0);
		if (core_work.joinable())
			core_work.join();
		m_hWnd = nullptr;
    }
};
