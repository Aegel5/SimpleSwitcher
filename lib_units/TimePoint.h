#pragma once

#include <chrono>

namespace details {

	using namespace std::chrono_literals; 	// h, min, s, ms,  d, y
	using namespace std::chrono;

	class TimePoint {
		using Time = time_point<steady_clock>;
		static constexpr auto safe_max = Time(
			std::chrono::steady_clock::duration::max() / 2
		);
	public:
		bool IsEmpty() { return time == Time{}; }
		auto operator<=>(const TimePoint& other) const = default;
		static TimePoint Now() { TimePoint res;	res.time = now(); return res; }
		static TimePoint MaxValue() { TimePoint res;	res.time = safe_max; return res; }
		void SetToNow() { time = now(); }
		auto DeltTo(TimePoint point) const { return abs(time - point.time); }
		auto DeltToNow() const { return DeltTo(Now()); }
		int DeltToNowMs() const { return (int)duration_cast<milliseconds>(DeltToNow()).count(); }
	private:
		static Time now() { return steady_clock::now() + 10000h; }
		Time time{};
	};
}

using TimePoint = details::TimePoint;
using Duration = std::chrono::nanoseconds;
