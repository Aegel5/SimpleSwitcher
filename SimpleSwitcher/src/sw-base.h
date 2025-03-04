#pragma once

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

#include <generator>

#include "inc_basic.h"
#include "utils/ScanMap.h"
#include "consts.h"

#include "SwUtils.h"
#include "KeysStruct.h"

#include "ver.h"

typedef HKL HKL_W;

#define ___SW_ADD_STR_UT(X) L##X
#define _SW_ADD_STR_UT(X) ___SW_ADD_STR_UT(X)

# pragma comment(lib, "Psapi.lib")










