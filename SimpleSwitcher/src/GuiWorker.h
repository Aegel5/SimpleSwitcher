#pragma once

extern int StartGui(std::stop_token token);

class GuiWorker {
	std::jthread thread;
	bool is_run = false;
	std::stop_source stopSource;
public:
	void Start() {
		if (is_run) {
			return;
		}
		if (thread.joinable()) {
			thread.join();
		}
		thread = std::jthread([this]() {
			is_run = true; 
			try {
				StartGui(stopSource.get_token());
			}
			catch (...){
				LOG_WARN(L"exception gui");
			}
			is_run = false; 
			});
	}
	~GuiWorker() {
		stopSource.request_stop();
	}
};
