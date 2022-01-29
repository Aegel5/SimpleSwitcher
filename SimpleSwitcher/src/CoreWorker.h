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
    std::thread core_work;
    MtxHolder mtx;

public:
    CoreWorker() {}

    void Start() {
        if (IsStarted())
            return;

        if(!mtx.take())
            return;

        HINSTANCE hInstance = wxGetInstance();
        core_work = std::thread(std::bind(StartMonitor, hInstance, SW_BIT_32));
    }
    void Stop() {
        mtx.clear();
        if (!IsStarted())
            return;
        PostMessage(gdata().hWndMonitor, c_MSG_Quit, 0, 0);
        core_work.join();
    }
    bool IsStarted() {
        return core_work.joinable();
    }
    ~CoreWorker() {
        Stop();
    }
};
