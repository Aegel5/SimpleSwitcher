#pragma once
#include "Utils.h"

namespace Notific {

	struct Entry {

		enum class Period {
			OneTime = 0,
			Month = 1
		};

		Period period = Period::Month;

		UStr PeriodName(Period p) {
			if (p == Period::Month) return LOC("Every month");
			if (p == Period::OneTime) return LOC("One time");
			return "Error";
		}

		string next_activ_string;

		string name = "Item";
		bool enabled = true;

		DateTime nextActivate;
		DateTime point;

		DateTime lastQuick;

		bool IsPeriodic() {
			return period == Period::Month;
		}
		void SetPoint(DateTime t) {
			point = t;
			SetupNextActivate();
		}

		DateTime Unset() { return {}; }

		bool IsFuture() {return nextActivate > Now();}
		bool IsIsFutureAndEnabled() { return enabled && IsFuture(); }
		void SetupNextActivate() {

			SetActivate(point);
			if (IsFuture()) return;

			if (IsPeriodic()) {
				if (period == Period::Month) {
					auto now = Convert(Now());
					auto pt = Convert(point);
					pt.m = now.m;
					pt.y = now.y;
					if (Convert(pt) <= Now()) {
						pt.AddMon();
					}
					SetActivate(Convert(pt));
				}
			}
			else {
				SetActivate({});
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
				next_activ_string = IsTrigger() ? "NOW!" : "Never";
			}
		}
		bool IsTrigger() {
			return enabled && nextActivate != Unset() && nextActivate <= Now();
		}
		Entry() {
			//SetPoint(Now());
		}

		int Draw(int& id) {
			int changes = 0;
			ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);
			ImGui::PushID(++id);
			ImGui::BeginChild("", {}, ImGuiChildFlags_AlwaysAutoResize | ImGuiChildFlags_Borders | ImGuiChildFlags_AutoResizeY, ImGuiWindowFlags_MenuBar);
			
			if (ImGui::BeginMenuBar()) {
				if (ImGui::Button(name.c_str())) {
					ImGui::OpenPopup("menu");
				}
				with_Popup("menu") {
					{
						auto need = std::count(name.begin(), name.end(), '\n') + 1;
						auto h = ImGui::GetFontSize() * need + ImGui::GetStyle().FramePadding.y * 2.0f;
						if (ImGui::InputTextMultiline("##input", &name, { ImGui::GetContentRegionAvail().x, h })) {
							changes = true;
						}
					}
					//if (ImGui::MenuItem("Delete")) {
					//	changes = 10;
					//}
				}
				if (ImGui::Button("Date")) {
					ImGui::OpenPopup("edit");
				}
				if (Edit(point)) {
					changes = true;
					SetupNextActivate();
				}
				if (ImGui::BeginMenu(PeriodName(period))) {
					auto add = [&](Period p) {
						if (ImGui::Selectable(PeriodName(p), p == period)) {
							period = p;
							changes = true;
						}
						};
					add(Period::OneTime);
					add(Period::Month);
					//if (ImGui::MenuItem("OneTime")) {
					//	changes = 10;
					//}
					ImGui::EndMenu();
				}

				{
					auto text = (const char*)u8"x";
					auto sz = ImGui::GetFontSize() + ImGui::GetStyle().FramePadding.x;
					//float w = ImGui::CalcTextSize(text).x + ImGui::GetStyle().FramePadding.x * 2.f + ImGui::GetStyle().ItemSpacing.x;
					ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetContentRegionAvail().x - sz);
					if (ImGui::Button(text, {sz,0})) { changes = 10; }
				}

				ImGui::EndMenuBar();
			}


			{
				set_ID(++id);

			}

			//ImGui::SameLine();
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
					delt -= mm;
					auto ss = std::chrono::duration_cast<std::chrono::seconds>(delt);
					if (hh.count() >= 1) {
						sprintf_s(buf, "%d hours %d minutes", (int)hh.count(), (int)mm.count());
					}
					else {
						if(mm.count() >= 1)	
							sprintf_s(buf, "%d minutes", (int)mm.count());
						else 
							sprintf_s(buf, "%d sec", (int)ss.count());
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
