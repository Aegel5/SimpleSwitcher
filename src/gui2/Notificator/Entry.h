#pragma once
#include "Utils.h"

namespace Notific {

struct Entry {
    enum class Period {
        OneTime = 0,
        Month = 1,
        NDays = 2,
        NMinutes = 4,
        Year = 3,
    };
    int days_period = 0;
    bool center = true;
    int id = 0;

    Period period = Period::Month;

    UStr PeriodNameLOC(Period p) {
        if (p == Period::Month)
            return LOC("Every month");
        if (p == Period::Year)
            return LOC("Every year");
        if (p == Period::OneTime)
            return LOC("One time");
        return "Error";
    }

    string next_activ_string;

    string name = LOC("<Name>");
    bool enabled = true;

    bool wnd_top = true;
    bool play_sound = false;

    DateTime nextActivate{};
    DateTime point{};  // reference point
    DateTime lastStartPlaySound{};

    void correct_days_period() { days_period = std::clamp(days_period, 1, 1000); }

    bool IsPeriodic() { return period != Period::OneTime; }
    void SetPoint(DateTime t) {
        point = t;
        SetupPeriodic();
    }

    DateTime Unset() { return {}; }

    bool IsIsFutureAndEnabled() const { return enabled && nextActivate > Now(); }

    void SetupPeriodic() {
        if (!IsPeriodic()) {
            SetActivate({});
            return;
        }

        auto now = Now();

        if (period == Period::Month) {
            auto now = Convert(Now());
            auto pt = Convert(point);
            pt.m = now.m;
            pt.y = now.y;
            while (Convert(pt) <= Now()) {
                pt.AddMon();
            }
            SetActivate(Convert(pt));
        } else if (period == Period::NDays) {
            correct_days_period();
            if (point > now)
                point = now;
            auto delt_d = floor<days>(now - point).count();
            delt_d /= days_period;
            SetActivate(point + days(delt_d * days_period + days_period));  // accurate calc
        } else if (period == Period::NMinutes) {
            correct_days_period();
            SetActivate(now + minutes(days_period));  // for minutes always do from now.
        } else if (period == Period::Year) {
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
        } else {
            next_activ_string = IsTrigger() ? LOC("NOW!") : LOC("Never");
        }
    }
    bool IsTrigger() { return enabled && nextActivate != Unset() && nextActivate <= Now(); }

    int Draw(int& id) {
        int changes = 0;
        ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);
        ImGui::PushID(++id);
        ImGui::BeginChild("", {},
                          ImGuiChildFlags_AlwaysAutoResize | ImGuiChildFlags_Borders | ImGuiChildFlags_AutoResizeY,
                          ImGuiWindowFlags_MenuBar);

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
                if (ImGui::InputTextMultiline("##input", &name, {ImGui::GetContentRegionAvail().x, h})) {
                    changes = true;
                }
            }
            if (ImGui::Button(LOC("Date"))) {
                ImGui::OpenPopup("edit");
            }
            if (period == Period::OneTime) {
                // for one time point - is start_point
                auto active = nextActivate;
                if (Edit(active)) {
                    SetPoint(Now());
                    SetActivate(std::max(active, Now() + 1min));
                    changes = true;
                }
            } else {
                if (Edit(point)) {
                    SetupPeriodic();
                    changes = true;
                }
            }
            {
                char buf[100];
                UStr title = 0;
                if (period == Period::NDays) {
                    StrUtils::Sprintf(buf, LOC("Every %d days"), days_period);
                    title = buf;
                } else if (period == Period::NMinutes) {
                    StrUtils::Sprintf(buf, LOC("Every %d minutes"), days_period);
                    title = buf;
                } else {
                    title = PeriodNameLOC(period);
                }

                if (ImGui::Button(title)) {
                    ImGui::OpenPopup("menu2");
                }
                with_Popup("menu2") {
                    // if (ImGui::BeginMenu(title)) {
                    auto add = [&](Period p) {
                        if (ImGui::Selectable(PeriodNameLOC(p), p == period)) {
                            period = p;
                            changes = true;
                            SetupPeriodic();
                        }
                    };
                    add(Period::OneTime);
                    add(Period::Month);
                    add(Period::Year);

                    // if(ImGui::InputInt(LOC("Days"), &day_period, 1,
                    int per = 0;

                    per = period == Period::NDays ? days_period : 0;
                    ImGui::SetNextItemWidth(ImGui::CalcTextSize("1111").x * 4);
                    if (ImGui::InputInt(LOC("Days"), &per, 1, 10)) {
                        period = Period::NDays;
                        correct_days_period();
                        days_period = per;
                        changes = true;
                        SetupPeriodic();
                    }

                    per = 0;
                    if (period == Period::NMinutes)
                        per = days_period;
                    ImGui::SetNextItemWidth(ImGui::CalcTextSize("1111").x * 4);
                    if (ImGui::InputInt(LOC("Minutes"), &per, 1, 10)) {
                        period = Period::NMinutes;
                        correct_days_period();
                        days_period = per;
                        changes = true;
                        SetupPeriodic();
                    }
                }
            }

            ImGui::EndMenuBar();
        }

        // ImGui::SameLine();
        ImGui::TextUnformatted(next_activ_string.c_str());
        ImGui::TextUnformatted("Remains");
        if (IsIsFutureAndEnabled()) {
            char buf[100];
            auto delt = DeltToNow(nextActivate);
            DeltToHuman(buf, delt);
            ImGui::SameLine();
            ImGui::TextUnformatted(buf);
        }
        if (period == Period::OneTime) {
            if (ImGui::Button("Clear")) {
                SetPoint({});
                changes = true;
            }
            // if (Utils::IsDebug()) {
            ImGui::SameLine();
            if (ImGui::Button("+10 sec")) {
                SetQuick(10s);
                changes = true;
            }
            //}
            ImGui::SameLine();
            if (ImGui::Button("+1 min")) {
                SetQuick(1min);
                changes = true;
            }
            ImGui::SameLine();
            if (ImGui::Button("+5 min")) {
                SetQuick(5min);
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
            ImGui::SameLine();
            if (ImGui::Button("+1 day")) {
                SetQuick(std::chrono::days(1));
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
            if (ImGui::Selectable(LOC("Delete"))) {
                changes = 10;
            }
            ImGui::EndPopup();
        }

        ImGui::PopID();
        ImGui::PopStyleVar();

        return changes;
    }
    void SetQuick(auto time) {
        period = Period::OneTime;
        if (nextActivate > point) {
            SetActivate(nextActivate + time);
        } else {
            auto now = Now();
            point = now;
            SetActivate(now + time);
        }
    }
};
}  // namespace Notific
