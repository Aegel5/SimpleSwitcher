#pragma once

// one-time write...
inline HWND g_guiHandle = nullptr;

inline TStatus OpenClipboard(CAutoClipBoard& clip) { return clip.Open(g_guiHandle); }

