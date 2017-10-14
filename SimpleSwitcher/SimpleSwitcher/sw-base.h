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

#include "inc_basic.h"
#include "utils/ScanMap.h"
#include "consts.h"
#include "swlocal.h"
#include "SwUtils.h"
#include "KeysStruct.h"

typedef HKL HKL_W;

# pragma comment(lib, "Psapi.lib")










