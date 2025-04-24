#pragma once

extern int StartGui(bool show);

class GuiWorker {
	std::jthread thread;
	bool is_run = false;
public:
	void Start(bool show = true) {
		if (is_run) {
			return;
		}
		if (thread.joinable()) {
			thread.join();
		}
		thread = std::jthread([show,this]() {
			is_run = true; 
			try {
				StartGui(show);
			}
			catch (...){
				LOG_WARN(L"exception gui");
			}
			is_run = false; 
			});
	}
	~GuiWorker() {
		PostMessage(g_guiHandle2, WM_QUIT, 0,0);
	}
};
