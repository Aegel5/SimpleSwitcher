#pragma once
#include "Utils.h"

namespace Notific {

	struct Entry {

		string next_activ_string;

		string name = "Item";
		string period = "Month";
		bool enabled = true;

		DateTime nextActivate;
		DateTime point;

		bool IsPeriodic() {
			return period != "None";
		}
		void SetPoint(DateTime t) {
			point = t;
			SetupNextActivate();
		}

		bool IsFuture() {return nextActivate > Now();}
		bool IsIsFutureAndEnabled() { return enabled && IsFuture(); }
		void SetupNextActivate() {
			nextActivate = point;

			if (!IsFuture()) {
				if (IsPeriodic()) {
					if (period == "Month") {
						auto cur = Convert(Now());
						cur.d = Convert(point).d;
						if (Convert(cur) <= Now()) {
							cur.AddMon();
						}
						nextActivate = Convert(cur);
					}
				}
				else {
					nextActivate = {};
				}
			}

			AdjastActiveString();

		}
		void AdjastActiveString() {
			if (IsIsFutureAndEnabled()) {
				next_activ_string = std::format("{:%d.%m.%Y %H:%M}", nextActivate);
			}
			else {
				next_activ_string = nextActivate == DateTime::min() ? "Never" : "NOW!";
			}
		}
		Entry() {
			SetPoint(Now());
		}

		int Draw(int& id) {
			int changes = 0;
			ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);
			ImGui::PushID(++id);
			ImGui::BeginChild("", {}, ImGuiChildFlags_AlwaysAutoResize | ImGuiChildFlags_Borders | ImGuiChildFlags_AutoResizeY, ImGuiWindowFlags_MenuBar);
			if (ImGui::BeginMenuBar()) {
				if (ImGui::BeginMenu(name.c_str())) {
					if (ImGui::Button("Delete")) {
						changes = 10;
					}
					ImGui::EndMenu();
				}
				ImGui::EndMenuBar();
			}

			auto need = std::count(name.begin(), name.end(), '\n')+1;
			auto h = ImGui::GetFontSize()* need + ImGui::GetStyle().FramePadding.y * 2.0f;
			ImGui::PushID(++id);
			if (ImGui::InputTextMultiline("##input", &name, { 0, h })) {
				changes = true;
			}
			ImGui::PopID();
			ImGui::PushID(++id);
			if (ImGui::Button("Next activate")) {
				ImGui::OpenPopup("edit");
			}
			if (Edit(point)) {
				changes = true;
				SetupNextActivate();
			}
			ImGui::PopID();
			ImGui::SameLine();

			ImGui::Text(next_activ_string.c_str());
			ImGui::Text("Remains");
			if (IsIsFutureAndEnabled()) {
				char buf[100];
				auto delt = DeltToNow(nextActivate);
				auto days = std::chrono::duration_cast<std::chrono::days>(delt);
				if (days.count() >= 1) {
					sprintf_s(buf, "%d days", days.count());
				}
				else {
					auto hh = std::chrono::duration_cast<std::chrono::hours>(delt);
					delt -= hh;
					auto mm = std::chrono::duration_cast<std::chrono::minutes>(delt);
					sprintf_s(buf, "%d hours %d minutes", hh.count(), mm.count());
				}
				ImGui::SameLine();
				ImGui::Text(buf);
			}
			ImGui::EndChild();
			ImGui::PopID();
			ImGui::PopStyleVar();

			return changes;

		}
	};
}
