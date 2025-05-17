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

	inline void Combo(UStr name, string& data, auto&& choices, auto&& apply, ImGuiComboFlags flags = 0) {
		if (ImGui::BeginCombo(name, data.c_str(), flags)) {
			if constexpr (std::is_invocable_v<DECLTYPE_DECAY(choices)>) {
				for (UStr s : choices()) {
					if (ImGui::Selectable(s, data == s)) {
						data = s;
						apply();
					}
				}
			}
			else {
				for (UStr s : choices) {
					if (ImGui::Selectable(s, data == s)) {
						data = s;
						apply();
					}
				}
			}

			ImGui::EndCombo();
		}
	}


	//inline float ButtonRightFrom(UStr text, int& new_offset, int w =0, float offset = 0) { // temporary solution
	//	auto cur = ImGui::GetCursorPos();
	//	w = ImGui::CalcTextSize(text).x + ImGui::GetStyle().FramePadding.x * 2.f + ImGui::GetStyle().ItemSpacing.x;
	//	auto r = ImGui::GetCursorPosX() + ImGui::GetContentRegionAvail().x - w - offset;
	//	ImGui::SetCursorPosX(r);
	//	auto res = false;
	//	if (ImGui::Button(text)) { res = true; }
	//	ImGui::SetCursorPos(cur);
	//	return res;
	//}
}
