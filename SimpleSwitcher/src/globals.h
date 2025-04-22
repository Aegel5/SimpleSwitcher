#pragma once

#include "EnableHolder.h"
#include "utils/accessibil.h"

// one-time write...
inline HWND g_guiHandle = nullptr;

inline TStatus OpenClipboard(CAutoClipBoard& clip) { return clip.Open(g_guiHandle); }

inline EnableHodler g_enabled{};
inline std::atomic_bool g_bring_gui_to_top = false;
inline void new_layout_request(HKL layout = 0) { PostMessage(g_guiHandle, WM_LayNotif, (WPARAM)layout, 0); }
inline void try_toggle_enable() { if (g_enabled.TryToggle()) new_layout_request(); }
inline void show_main_wind() {
	PostMessage(g_guiHandle, WM_ShowWindow, 0, 0); 
	g_bring_gui_to_top = true;
}

inline void ApplyAcessebil() {	AllowAccessibilityShortcutKeys(!conf_get_unsafe()->disableAccessebility);}
inline bool IsAdminOk() { return Utils::IsSelfElevated() || !conf_get_unsafe()->isMonitorAdmin; }
