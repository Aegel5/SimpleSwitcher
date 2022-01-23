#pragma once

#include <thread>
#include "CMainWorker.h"

#ifdef __WXMSW__
#include "wx/msw/private.h"
#endif

class CoreWorker {
    std::thread core_work;

public:
    void Start() {
        if (IsStarted())
            return;
        HINSTANCE hInstance = wxGetInstance();
        core_work = std::thread(std::bind(StartMonitor, hInstance, SW_BIT_32));
    }
    void Stop() {
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
