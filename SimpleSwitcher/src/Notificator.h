#pragma once
#include <chrono>
#include "ImGuiDatePicker.hpp"

class Notificator {
	using DateTime = std::chrono::time_point<std::chrono::system_clock>;
	enum class Period {
		None,
		Day,
		Month,
	};
	struct Entry {
		string name = "Test";
		Period period = Period::Month;
		DateTime time{};
		bool enabled = true;
		bool todel = false;
	};
	std::vector<Entry> entries;
public:
	Notificator() {
	}

	void DrawNotify() {


	}
	void DrawSettings() {
		float w = ImGui::GetFrameHeight();
		bool del = false;
		//Gui::GetStyle().
		for (auto& it : entries) {
			ImGui::Checkbox("##check", &it.enabled);
			ImGui::SameLine();
			//w = ImGui::GetCursorPosX() - ImGui::GetStyle().ItemSpacing.x*2;
			ImGui::InputText("##input", &it.name);
			if (ImGui::Button("-", { w,0 })) {
				it.todel = true;
				del = true;
			}
			ImGui::SameLine();
			std::time_t currentTime = std::chrono::system_clock::to_time_t(it.time);
			tm t = *std::gmtime(&currentTime);
			if (ImGui::DatePicker("##", t)) {
				it.time = std::chrono::system_clock::from_time_t(std::mktime(&t));
			}
			ImGui::Separator();
		}
		if (ImGui::Button("+", { w,0 })) {
			entries.emplace_back();
		}
		if (del) {
			for (int i = std::ssize(entries) - 1; i >= 0; i--) {
				if (entries[i].todel) Utils::RemoveAt(entries, i);
			}
		}
	}
};
