#pragma once

// one per thread 
class WinTimer {

	int lastTimerId = 1;
	std::vector<std::function<void()>> timerCallbacks;
	HWND hwnd = 0;
	inline thread_local static WinTimer* Inst = 0;

	static LRESULT CALLBACK WindowProc2(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
		switch (uMsg) {
		case WM_TIMER:
		{
			UINT_PTR timerId = wParam - 1;
			if (timerId < Inst->timerCallbacks.size()) {
				Inst->timerCallbacks[timerId]();
			}
			return 0;
		}
		}

		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
public:
	WinTimer() {
		Inst = this;
		hwnd = WinUtils::CreateMsgWin(L"SimpleSwitcher_Timer_001", WindowProc2);
		IFW_LOG(hwnd != 0);
	}
	void CycleTimer(auto&& func, int ms) {
		timerCallbacks.push_back(func);
		auto timeId = SetTimer(hwnd, lastTimerId, ms, NULL);
		IFW_LOG(timeId != 0);
		lastTimerId++;
	}
};
