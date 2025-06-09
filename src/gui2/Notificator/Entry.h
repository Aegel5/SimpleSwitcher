#pragma once
#include "Utils.h"

namespace Notific {

	struct Entry {

		enum class Period {
			OneTime = 0,
			Month = 1,
			NDays = 2,
			Year = 3,
		};
		int days_period = 0;

		Period period = Period::Month;

		UStr PeriodName(Period p) {
			if (p == Period::Month) return LOC("Every month");
			if (p == Period::Year) return LOC("Every year");
			if (p == Period::OneTime) return LOC("One time");
			return "Error";
		}

		string next_activ_string;

		string name = LOC("<Name>");
		bool enabled = true;

		bool wnd_top = true;
		bool play_sound = false;

		DateTime nextActivate{};
		DateTime point{};
		DateTime lastQuick{};
		DateTime lastStartPlaySound{};

		void correct_days_period() {
			days_period = std::clamp(days_period, 1, 1000);
		}

		bool IsPeriodic() {
			return period != Period::OneTime;
		}
		void SetPoint(DateTime t) {
			point = t;
			SetupNextActivate();
		}

		DateTime Unset() { return {}; }

		bool IsIsFutureAndEnabled() { return enabled && nextActivate > Now(); }
		void SetupNextActivate() {

			auto now = Now();
			if (point >= now) {
				SetActivate(point);
				return;
			}

			if (!IsPeriodic()) {
				SetActivate({});
				return;
			}

			if (period == Period::Month) {
				auto now = Convert(Now());
				auto pt = Convert(point);
				pt.m = now.m;
				pt.y = now.y;
				while (Convert(pt) <= Now()) {
					pt.AddMon();
				}
				SetActivate(Convert(pt));
			}
			else if (period == Period::NDays) {
				correct_days_period();
				auto delt_d = floor<days>(now - point).count();
				delt_d /= days_period;
				SetActivate(point + days(delt_d * days_period + days_period)); // accurate calc
			}
			else if (period == Period::Year) {
				auto now = Convert(Now());
				auto pt = Convert(point);
				pt.y = now.y;
				while (Convert(pt) <= Now()) {
					pt.AddYear();
				}
				SetActivate(Convert(pt));
			}

		}
		void SetActivate(DateTime dt) {
			nextActivate = dt;
			AdjastActiveString();
		}
		void AdjastActiveString() {
			if (IsIsFutureAndEnabled()) {
				next_activ_string = PrintLocal(nextActivate);
			}
			else {
				next_activ_string = IsTrigger() ? LOC("NOW!") : LOC("Never");
			}
		}
		bool IsTrigger() {
			return enabled && nextActivate != Unset() && nextActivate <= Now();
		}

		int Draw(int& id) {
			int changes = 0;
			ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);
			ImGui::PushID(++id);
			ImGui::BeginChild("", {}, ImGuiChildFlags_AlwaysAutoResize | ImGuiChildFlags_Borders | ImGuiChildFlags_AutoResizeY, ImGuiWindowFlags_MenuBar);
			
			if (ImGui::BeginMenuBar()) {

				{
					auto line = StrUtils::GetLine(name);
					char copy = 0;
					if (line.size() < name.size()) {
						copy = name[line.size()];
						name[line.size()] = '\0';
					}
					if (ImGui::Button(name.c_str())) {
						ImGui::OpenPopup("menu");
					}
					if (line.size() < name.size()) {
						name[line.size()] = copy;
					}
				}

				with_Popup("menu") {
					auto need = std::count(name.begin(), name.end(), '\n') + 1;
					auto h = ImGui::GetFontSize() * need + ImGui::GetStyle().FramePadding.y * 2.0f;
					if (ImGui::InputTextMultiline("##input", &name, { ImGui::GetContentRegionAvail().x, h })) {
						changes = true;
					}
				}
				if (ImGui::Button(LOC("Date"))) {
					ImGui::OpenPopup("edit");
				}
				if (Edit(point)) {
					changes = true;
					SetupNextActivate();
				}
				{
					char buf[100];
					auto title = PeriodName(period);
					if (period == Period::NDays) {
						StrUtils::Sprintf(buf, LOC("Every %d days"), days_period);
						title = buf;
					}

					if (ImGui::Button(title)) {
						ImGui::OpenPopup("menu2");
					}
					with_Popup("menu2") {
					//if (ImGui::BeginMenu(title)) {
						auto add = [&](Period p) {
							if (ImGui::Selectable(PeriodName(p), p == period)) {
								period = p;
								changes = true;
								SetupNextActivate();
							}
							};
						add(Period::OneTime);
						add(Period::Month);
						add(Period::Year);

						//if(ImGui::InputInt(LOC("Days"), &day_period, 1,
						ImGui::SetNextItemWidth(ImGui::CalcTextSize("1111").x * 4);
						if (ImGui::InputInt(LOC("Days"), &days_period, 1, 10)) {
							correct_days_period();
							period = Period::NDays;
							changes = true;
							SetupNextActivate();
						}
					}
				}



				{
					auto text = (const char*)u8"x";
					auto w = ImGui::GetFontSize() + ImGui::GetStyle().FramePadding.x;
					//float w = ImGui::CalcTextSize(text).x + ImGui::GetStyle().FramePadding.x * 2.f + ImGui::GetStyle().ItemSpacing.x;
					ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetContentRegionAvail().x - w);
					if (ImGui::Button(text, {w,0})) { changes = 10; }
				}

				ImGui::EndMenuBar();
			}



			//ImGui::SameLine();
			ImGui::Text(next_activ_string.c_str());
			ImGui::Text("Remains");
			if (IsIsFutureAndEnabled()) {

				char buf[100];
				auto delt = DeltToNow(nextActivate);
				auto days = std::chrono::duration_cast<std::chrono::days>(delt);
				if (days.count() >= 1) {
					StrUtils::Sprintf(buf, "%d days", days.count());
				}
				else {
					auto hh = std::chrono::duration_cast<std::chrono::hours>(delt);
					delt -= hh;
					auto mm = std::chrono::duration_cast<std::chrono::minutes>(delt);
					delt -= mm;
					auto ss = std::chrono::duration_cast<std::chrono::seconds>(delt);
					if (hh.count() >= 1) {
						StrUtils::Sprintf(buf, "%d hours %d minutes", (int)hh.count(), (int)mm.count());
					}
					else {
						if (mm.count() >= 1)
							StrUtils::Sprintf(buf, "%d minutes", (int)mm.count());
						else
							StrUtils::Sprintf(buf, "%d sec", (int)ss.count());
					}
				}
				ImGui::SameLine();
				ImGui::Text(buf);
			}
			if (period == Period::OneTime) {
				if (ImGui::Button("Clear")) {
					lastQuick = {};
					SetPoint({});
					changes = true;
				}
				if (Utils::IsDebug()) {
					ImGui::SameLine();
					if (ImGui::Button("+10 sec")) {
						SetQuick(10s);
						changes = true;
					}
				}
				ImGui::SameLine();
				if (ImGui::Button("+1 min")) {
					SetQuick(1min);
					changes = true;
				}
				ImGui::SameLine();
				if (ImGui::Button("+10 min")) {
					SetQuick(10min);
					changes = true;
				}
				ImGui::SameLine();
				if (ImGui::Button("+30 min")) {
					SetQuick(30min);
					changes = true;
				}
			}

			ImGui::EndChild();

			if (ImGui::BeginPopupContextItem("ctx_menu")) {
				if (ImGui::Checkbox(LOC("Play sound"), &play_sound)) {
					changes = true;
				}
				if (ImGui::Checkbox(LOC("Bring window to top"), &wnd_top)) {
					changes = true;
				}
				ImGui::EndPopup();
			}

			ImGui::PopID();
			ImGui::PopStyleVar();

			return changes;

		}
		void SetQuick(auto time) {
			if (DeltToNow(lastQuick) > 2min) {
				SetPoint(Now());
			}
			SetPoint(point + time);
			lastQuick = Now();
		}
	};
}
