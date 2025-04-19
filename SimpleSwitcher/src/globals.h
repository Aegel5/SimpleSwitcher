#pragma once

#include "EnableHolder.h"

// one-time write...
inline HWND g_guiHandle = nullptr;

inline TStatus OpenClipboard(CAutoClipBoard& clip) { return clip.Open(g_guiHandle); }

#define USE_GUI2

inline EnableHodler g_enabled{};
inline bool g_autostart = false;

inline std::atomic_int64_t g_layout_change_cnt{5};
