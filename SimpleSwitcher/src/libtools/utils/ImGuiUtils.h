#pragma once

namespace ImGuiUtils {
	inline void ToCenter(bool always = false) {
		auto rec = ImGui::GetPlatformIO().Monitors[0].MainSize;
		rec.x *= 0.5f;
		rec.y *= 0.5f;
		ImGui::SetNextWindowPos(rec, always ? ImGuiCond_Appearing: ImGuiCond_FirstUseEver, { 0.5f,0.5f });
	}
}
