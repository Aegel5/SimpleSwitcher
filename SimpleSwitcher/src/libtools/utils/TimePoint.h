class TimePoint {
	// h, min, s, ms
	std::chrono::time_point<std::chrono::steady_clock> time{};
public:
	static TimePoint Now() {
		TimePoint res;
		res.SetNow();
		return res;
	}
	auto DeltToNow() const {
		auto now = std::chrono::steady_clock::now();
		//return now > time ? now - time : time - now;
		return now - time;
	}
	int DeltToNowMs() const {
		return (int)std::chrono::duration_cast<std::chrono::milliseconds>(DeltToNow()).count();
	}
	void SetNow() {
		time = std::chrono::steady_clock::now();
	}
};

using namespace std::chrono_literals;
