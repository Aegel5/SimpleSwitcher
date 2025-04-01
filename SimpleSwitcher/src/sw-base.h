#pragma once

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
#include <variant>
#include <queue>
#include <map>
#include <spanstream>
#include <filesystem>
#include <regex>

// утилиты
#include "core/inc_all.h"
#include "utils/CAutoCleanupWin.h"
#include "utils/COM_utils.h"
#include "utils/str_utils.h"
#include "utils/utils-func.h"
#include "utils/MultiThreadQueue.h"
#include "utils/CHotKey.h"
#include "utils/file_utils.h"
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









