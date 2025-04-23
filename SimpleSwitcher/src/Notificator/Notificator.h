#pragma once

#include "Entry.h"

namespace Notific {

	class Notificator {

		std::vector<Entry> entries;
		void Save() {
		}
		void Load() {
		}
	public:
		Notificator() {
			entries.emplace_back();
		}

		void DrawNotify() {

		}
		void DrawSettings() {

			int id = 1;
			for (int i = 0; i < entries.size(); i++) {
				auto cur = entries[i].Draw(id);
				if (cur == 10) {
					Utils::RemoveAt(entries, i);
					i--;
				}
				if (cur) {
					Save();
				}
			}

			if (ImGui::Button("+", { ImGui::GetFrameHeight(),0 })) {
				entries.emplace_back();
			}
		}
	};
}
