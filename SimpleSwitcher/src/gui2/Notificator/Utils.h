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
		void AddDay() {
			d++;
			if (d > MaxDay()) {
				d = 1;
				AddMon();
			}
		}
		void AddMon() {
			m++;
			if (m > 12) {
				m = 1;
				y++;
			}
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
			for (int i = 1; i <= time.MaxDay(); i++) {
				sprintf_s(buf, "%d", i);
				if (((i - 1) % 7) != 0) ImGui::SameLine();
				bool pop = false;
				if (time.d == i) {
					pop = true;
					ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.300f, 0.697f, 0.601f, 1.000f));
				}
				if (ImGui::Button(buf, { ImGui::CalcTextSize("000").x, 0 })) {
					time.d = i;
					edit = true;
				}
				if (pop) {
					ImGui::PopStyleColor();
				}
			}

			if (ImGui::SliderInt("year", &time.y, 2025, 2040)) {
				edit = true;
				time.FixDay();
			}

			if (ImGui::SliderInt("month", &time.m, 1, 12)) {
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
