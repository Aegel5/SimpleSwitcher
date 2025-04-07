#pragma once

#include <thread>
#include "CMainWorker.h"

#ifdef __WXMSW__
#include "wx/msw/private.h"
#endif

class MtxHolder {

    HANDLE mtxGui = NULL;

public:
    bool take() {
        if (mtxGui != NULL)
            return true;
        mtxGui = CreateMutex(NULL, FALSE, L"E629E6ED27B64671");
        if (GetLastError() != ERROR_SUCCESS) {
            if (mtxGui != NULL && mtxGui != INVALID_HANDLE_VALUE) {
                CloseHandle(mtxGui);
                mtxGui = NULL;
            }
        }
        return mtxGui != NULL;
    }
    void clear(){
        if (mtxGui != NULL) {
            CloseHandle(mtxGui);
            mtxGui = NULL;
        }
    }
    ~MtxHolder() {
        clear();
    }
};

class CoreWorker {

	inline static const wchar_t* c_sClassNameServer2 = L"SimpleSw_325737FD_Serv";
    std::thread core_work;
    MtxHolder mtx;
	std::atomic<HWND> m_hWnd = nullptr;

	TStatus StartMonitor(_In_ HINSTANCE hInstance) {

		LOG_ANY(L"StartMonitor...");
		IFW_LOG(SetPriorityClass(GetCurrentProcess(), ABOVE_NORMAL_PRIORITY_CLASS));
		Worker()->ReStart(); // полностью обнулим рабочий поток

		WNDCLASSEX wcex{};
		wcex.cbSize = sizeof(WNDCLASSEX);
		wcex.lpfnWndProc = DefWindowProc;
		wcex.hInstance = hInstance;
		wcex.lpszClassName = c_sClassNameServer2;

		IFW_LOG(RegisterClassEx(&wcex) != 0);

		m_hWnd = nullptr;
		auto hWnd = CreateWindow(
			c_sClassNameServer2,
			L"SimpleSwitcher_wnd_class",
			0,
			0, 0, 0, 0, 
			HWND_MESSAGE, NULL, hInstance, NULL);

		IFW_RET(hWnd != NULL);
		m_hWnd = hWnd;

		IFW_LOG(AddClipboardFormatListener(hWnd));
		//IFW_LOG(ChangeWindowMessageFilterEx(hWnd, WM_LayNotif, MSGFLT_ALLOW, 0));

		//auto timeId = SetTimer(hWnd, c_timerKeyloggerDefence, 5000, NULL);
		//IFW_LOG(timeId != 0);

		Hooker hooker;
		IFS_LOG(hooker.StartHook());

		MSG msg;
		while (true) {
			BOOL bRet = GetMessage(&msg, NULL, 0, 0);

			if (bRet == 0)
				break;

			if (bRet == -1) {
				IFW_RET(FALSE);

			}

			auto mesg = msg.message;

			if (mesg == WM_HOTKEY) {
			}
			else if (mesg == c_MSG_Quit) {
				break;
				//PostQuitMessage(0);
			}
			else if (mesg == WM_CLIPBOARDUPDATE) {
				Worker()->PostMsg([](auto& w) {w.CliboardChanged(); });
			}
			else {
				//TranslateMessage(&msg);
				//DispatchMessage(&msg);
			}
		}

		RETURN_SUCCESS;
	}

public:
    CoreWorker() {}

    void Start() {
        if (IsStarted())
            return;

		if (Utils::IsDebug()) {
			if (!mtx.take()) {
				auto hk = conf_get_unsafe()->GetHk(hk_ToggleEnabled).keys.key();
				for (auto& it : hk) if (it == VKE_WIN) it = VK_LWIN;
				InputSender::SendHotKey(hk);
				Sleep(50);
			}
		}

        if(!mtx.take())
            return;

        HINSTANCE hInstance = wxGetInstance();
        core_work = std::thread(std::bind(&CoreWorker::StartMonitor, this, hInstance));
    }

    void Stop() {
        if (!IsStarted()) {
            mtx.clear();
            return;
        }
        PostMessage(m_hWnd, c_MSG_Quit, 0, 0);
		if(core_work.joinable())
			core_work.join();
		m_hWnd = nullptr;
        mtx.clear();
    }

    bool IsStarted() {
        return core_work.joinable();
    }

    ~CoreWorker() {
        Stop();
    }
};
