﻿#pragma once

#define WIN32_LEAN_AND_MEAN             

#include <windows.h>
#include <Winuser.h>
#include <Psapi.h>




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

#include "inc_basic.h"
#include "utils/ScanMap.h"
#include "utils/getpath.h"
#include "utils/procstart.h"
#include "utils/layout-utils.h"
#include "consts.h"

#include "Settings.h"
#include "InjectSkipper.h"
#include "KeysStruct.h"
#include "InputSender.h"
#include "KeysCurState.h"

#include "ver.h"

#define ___SW_ADD_STR_UT(X) L##X
#define _SW_ADD_STR_UT(X) ___SW_ADD_STR_UT(X)








