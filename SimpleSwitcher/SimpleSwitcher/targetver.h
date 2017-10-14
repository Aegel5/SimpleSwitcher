#pragma once

// Включение SDKDDKVer.h обеспечивает определение самой последней доступной платформы Windows.

// Если требуется выполнить сборку приложения для предыдущей версии Windows, включите WinSDKVer.h и
// задайте для макроса _WIN32_WINNT значение поддерживаемой платформы перед включением SDKDDKVer.h.

#include <SDKDDKVer.h>

#define SW_VER_1    2
#define SW_VER_2    0
#define SW_VER_3    7
#define SW_VER_4    1

#define _SW_QT(X) #X
#define SW_QT(X) _SW_QT(X)

#define _SW_UT(X) L##X
#define SW_UT(X) _SW_UT(X)

#define SW_VERSION    SW_QT(SW_VER_1.SW_VER_2.SW_VER_3.SW_VER_4)  // "2.0.2.3"
#define SW_VERSION_L SW_UT(SW_VERSION)

#define SW_PROGRAM_NAME    "SimpleSwitcher"
#define SW_PROGRAM_NAME_L SW_UT(SW_PROGRAM_NAME)
