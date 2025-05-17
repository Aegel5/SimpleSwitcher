#pragma once

extern int StartGui(bool show);

class GuiWorker {
	std::jthread thread;
	std::atomic_bool is_started = false;
public:
	void Start(bool show = true) { // todo: must not be called quickly several times
		if (is_started) return;
		thread = std::jthread([show,this]() {
			is_started = true;
			try {
				StartGui(show);
			}
			catch (...){
				LOG_WARN(L"exception gui");
			}
			is_started = false;
			});
	}
	~GuiWorker() {
		WinUtils::PostMsg(g_guiHandle2, WM_QUIT);
	}
};
