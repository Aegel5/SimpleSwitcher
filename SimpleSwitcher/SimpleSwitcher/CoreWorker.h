#pragma once

#include <thread>
#include "CMainWorker.h"

class CoreWorker {
    CMainWorker mainWorker;

public:
    void Start() {
        IFS_RET(mainWorker.Init());
    }
    void Stop() {
    }
    bool IsStarted() {

    }


}
