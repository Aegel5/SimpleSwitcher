#pragma once

namespace ImGuiUtils {
	inline void ToCenter(bool always = false) {
		auto rec = ImGui::GetPlatformIO().Monitors[0].MainSize;
		rec.x *= 0.5f;
		rec.y *= 0.5f;
		ImGui::SetNextWindowPos(rec, always ? ImGuiCond_Appearing: ImGuiCond_FirstUseEver, { 0.5f,0.5f });
	}

	class WindowHelper {
	protected:
		bool to_top = true;
		bool show_wnd = false;
		void process_helper() {
			if (to_top) {
				auto hwnd = (HWND)ImGui::GetWindowViewport()->PlatformHandle;
				if (hwnd) {
					to_top = false;
					SetForegroundWindow(hwnd);
				}
			}
		}
	public:
		//void bring_to_top() {
		//	to_top = true;
		//}
		void ShowHide() {
			if (show_wnd) show_wnd = false;
			else {
				show_wnd = true; to_top = true;
			}
		}
	};
}
