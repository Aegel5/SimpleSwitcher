#pragma once

#pragma comment(lib, "winmm.lib")

class SoundManager {

	inline static std::atomic_bool allow_play = false;

	static void Play(string file, std::chrono::duration<float> period) {

		auto start = std::chrono::steady_clock::now();

		while (allow_play && std::chrono::steady_clock::now() - start < period) {
			PlaySound(StrUtils::Convert(file).c_str(), NULL, SND_FILENAME);
			Sleep(500);
		}

	}
	std::future<void> task;
public:
	~SoundManager() {
		allow_play = false;
	}
	static SoundManager& Inst() {
		static SoundManager inst;
		return inst;
	}
	void StopAllSound() {
		allow_play = false;
	}
	void PlayRandom(std::chrono::duration<float> period = 1min) {

		if (task.valid() && task.wait_for(std::chrono::seconds(0)) == std::future_status::timeout) {
			return;
		}

		namespace fs = std::filesystem;
		auto dir = "c:\\windows\\media";
		if (!fs::is_directory(dir)) return;
		std::array skip = {
			"recycle.wav"s,
			"Windows Feed Discovered.wav"s,
			"Windows Menu Command.wav"s,
			"Windows Minimize.wav"s,
			"Windows Navigation Start.wav"s,
			"Windows Startup.wav"s,
		};
		vector<string> files;
		for (const auto& entry : fs::directory_iterator(dir)) {
			if (entry.is_regular_file()) {
				if (entry.path().extension() == ".wav") {
					if (!std::any_of(skip.begin(), skip.end(), [&](const auto& s) {
						return StrUtils::EqualsCI(entry.path().filename().string(), s);
						})) {
						files.push_back(entry.path().string());
					}
				}
			}
		}
		if (files.empty()) return;

		auto file = files[Rand::Int(0, files.size() - 1)];

		allow_play = true;
		task = std::async(std::launch::async, Play, file, period);
	}
};
