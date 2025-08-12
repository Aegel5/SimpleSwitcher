#pragma once

namespace Notific {

	using namespace std::chrono;

	using DateTime = std::chrono::time_point<std::chrono::system_clock>;
	using DateTimeLocal = std::chrono::local_time<std::chrono::system_clock::duration>;

	struct MyTimeLocal {
		int y;
		int m;
		int d;
		int hh;
		int mm;
		void FixDay() { d = std::min(MaxDay(), d); }
		int MaxDay() {
			using namespace std::chrono;
			auto cur = year(y) / month(m) / last;
			return (unsigned int)cur.day();
		}

		void AddMon() {
			m++;
			if (m > 12) {
				m = 1;
				y++;
			}
			FixDay();
		}
		void AddYear() {
			y++;
			FixDay();
		}
	};

	inline DateTime Now() { return system_clock::now();	}
	inline DateTimeLocal LocalConvert(DateTime dt) { return current_zone()->to_local(dt); }
	inline DateTime LocalConvert(DateTimeLocal dt) { return current_zone()->to_sys(dt); }
	inline  DateTimeLocal NowLocal() { return LocalConvert(Now()); }
	static auto DeltToNow(DateTime t) { auto now = Now(); return t > now ? t - now : now - t; }

	inline std::string PrintLocal(DateTime dt) { return std::format("{:%d.%m.%Y %H:%M}", LocalConvert(dt)); }

	inline std::string Serialize(DateTime dt) { return std::format("{:%d.%m.%Y %H:%M:%S}", dt); }

	inline DateTime Desiarelize(const string& str) {
		std::istringstream in(str);
		sys_seconds tp;
		in >> parse("%d.%m.%Y %H:%M:%S", tp);
		DateTime dt{};
		dt += tp.time_since_epoch();
		return dt;
	}

	inline DateTime Convert(MyTimeLocal mt) {
		auto dt = sys_days(year_month_day(year(mt.y), month(mt.m), day(mt.d))) + hours(mt.hh) + minutes(mt.mm);
		time_point<std::chrono::system_clock::duration> tp;
		DateTimeLocal res{};
		res += dt.time_since_epoch();
		return LocalConvert(res);

	}
	inline MyTimeLocal Convert(DateTime dt_) {
		auto dt = LocalConvert(dt_);
		MyTimeLocal res;
		auto dp = floor<days>(dt);;
		year_month_day ymd{ dp };
		hh_mm_ss time{ floor<milliseconds>(dt - dp) };
		res.y = (int)ymd.year();
		res.m = (unsigned int)ymd.month();
		res.d = (unsigned int)ymd.day();
		res.hh = time.hours().count();
		res.mm = time.minutes().count();
		return res;
	}

	inline void DeltToHuman(auto& buf, auto delt) {
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
	}

	inline bool Edit(DateTime& dt) {
		bool edit = false;
		if (ImGui::BeginPopup("edit")) {
			if (ImGui::Button("Now")) {
				dt = Now();
				edit = true;
			}
			MyTimeLocal time = Convert(dt);
			char buf[5];
			if (ImGui::IsWindowAppearing()) {
				// todo - store local time in windows for cache
			}
			auto wday = weekday(year(time.y) / month(time.m) / day(1));
			int to_skip = wday.iso_encoding() - 1;
			auto btn_w = ImGui::CalcTextSize("000").x;
			for (int i = 1 - to_skip; i <= time.MaxDay(); i++) {
				StrUtils::Sprintf(buf, "%d", i);
				if (((i - (1 - to_skip)) % 7) != 0) 
					ImGui::SameLine();
				if (i < 1) {
					// skip
					set_ID(i);
					ImGui::Button("", {btn_w , 0});
					continue;
				}

				bool pop = false;
				if (time.d == i) {
					pop = true;
					ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.300f, 0.697f, 0.601f, 1.000f));
				}
				if (ImGui::Button(buf, {btn_w , 0 })) {
					time.d = i;
					edit = true;
				}
				if (pop) {
					ImGui::PopStyleColor();
				}
			}

			ImGui::Separator();

			btn_w = ImGui::CalcTextSize("_Jan_").x;
			for (int i = 1; i <= 12; i++) {
				bool pop = false;
				if (i == time.m) {
					ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.300f, 0.697f, 0.601f, 1.000f));
					pop = true;
				}
				UStr name = 0;
				if (i == 1) name = "Jan";
				if (i == 2) name = "Feb";
				if (i == 3) name = "Mar";
				if (i == 4) name = "Apr";
				if (i == 5) name = "May";
				if (i == 6) name = "Jun";
				if (i == 7) name = "Jul";
				if (i == 8) name = "Aug";
				if (i == 9) name = "Sep";
				if (i == 10) name = "Oct";
				if (i == 11) name = "Nov";
				if (i == 12) name = "Dec";
				if((i-1)%4 != 0) ImGui::SameLine();
				if (ImGui::Button(name, { btn_w , 0 })) {
					time.m = i;
					edit = true;
					time.FixDay();
				}
				if (pop) {
					ImGui::PopStyleColor();
				}
			}

			//if (ImGui::SliderInt("month", &time.m, 1, 12)) {
			//	edit = true;
			//	time.FixDay();
			//}

			if (ImGui::InputInt("year", &time.y, 1, 10)) {
				Clamp(time.y, 1900, 2200);
				edit = true;
				time.FixDay();
			}

			if (ImGui::SliderInt("hours", &time.hh, 0, 23)) {
				edit = true;
			}
			if (ImGui::SliderInt("minutes", &time.mm, 0, 59)) {
				edit = true;
			}
			if (edit) {
				dt = Convert(time);
			}
			ImGui::EndPopup();
		}
		return edit;
	}

}
