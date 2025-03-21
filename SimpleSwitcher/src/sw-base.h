﻿#pragma once

// wx widgets
#include "wx/wxprec.h"

// windows
#define WIN32_LEAN_AND_MEAN // убрать лишние header из winapi
#include <windows.h>
#include <Winuser.h>
#include <Psapi.h>

// std
#include <stdlib.h>
#include <assert.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <thread>
#include <mutex>
#include <vector>
#include <string>
#include <set>
#include <format>
#include <deque>
#include <list>
#include <generator>
#include <ranges>
#include <chrono>

// утилиты
#include "inc_basic.h"
#include "utils/ScanMap.h"
#include "utils/getpath.h"
#include "utils/procstart.h"
#include "utils/layout-utils.h"

// base
#include "ver.h"
#include "consts.h"
#include "Settings.h"

// core-worker-utils
#include "InjectSkipper.h"
#include "KeysStruct.h"
#include "InputSender.h"
#include "KeysCurState.h"
#include "CycleRevertList.h"
#include "AnalyzeTyped.h"

// core-worker
#include "globals.h"
#include "CClipWorker.h"
#include "CMainWorkerMsg.h"
#include "WorkerImplement.h"
#include "CMainWorker.h"
#include "Hooker.h"
#include "HookerThread.h"









