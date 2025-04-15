﻿#pragma once

// one-time write...
inline HWND g_guiHandle = nullptr;

inline TStatus OpenClipboard(CAutoClipBoard& clip) { return clip.Open(g_guiHandle); }

inline constexpr bool g_usenewgui = true;
inline std::atomic_bool g_exit = false;
inline std::atomic_bool g_show_gui = true;
