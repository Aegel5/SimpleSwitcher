#pragma once

class WinTray {

public:struct TrayItem {
		UStr name;
		std::function<void()> callback;
		bool is_separator = false;
	};
private:

	static const int WM_TRAYICON = WM_USER + 1;
	static const int ID_TRAY_EXIT = 1002;

	inline static WinTray* Inst = 0;

	std::function<void()> double_click;
	std::function<void()> r_click;
	std::function<std::vector<TrayItem>()> createMenu;
	std::vector<TrayItem> last_menu;

	int lastTimerId = 1;
	std::vector<std::function<void()>> timerCallbacks;

	NOTIFYICONDATA nid {};
	HWND hwnd = 0;

	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
		switch (uMsg) {
		case WM_TRAYICON: {
			if (lParam == WM_RBUTTONUP) {
				if (Inst->r_click) {
					Inst->r_click();
				}
			}
			else if (lParam == WM_RBUTTONDOWN) {
				if (Inst->createMenu) {
					Inst->last_menu = Inst->createMenu();
					// Show context menu on right-click
					POINT cursorPos;
					GetCursorPos(&cursorPos);

					HMENU hMenu = CreatePopupMenu();
					for (int i = 0; auto & it : Inst->last_menu) {
						i++;
						AppendMenu(hMenu, MF_STRING, i, StrUtils::Convert((string)it.name).c_str());
					}

					SetForegroundWindow(hwnd);
					TrackPopupMenu(hMenu, TPM_BOTTOMALIGN | TPM_LEFTALIGN, cursorPos.x, cursorPos.y, 0, hwnd, NULL);
					DestroyMenu(hMenu);
				}
			}
			else if (lParam == WM_LBUTTONDBLCLK) {
				if (Inst->double_click) {
					Inst->double_click();
				}
			}
			break;
		}


		case WM_COMMAND: {
			for (int i = 0; auto & it : Inst->last_menu) {
				i++;
				if (i == LOWORD(wParam)) {
					it.callback();
					break;
				}
			}
			break;
		}


		case WM_TIMER:
		{
			UINT_PTR timerId = wParam-1;
			if (timerId < Inst->timerCallbacks.size()) {
				Inst->timerCallbacks[timerId]();
			}
			break;
		}

		default:
			return DefWindowProc(hwnd, uMsg, wParam, lParam);
		}
		return 0;
	}
public:
	void OnCreateMenu(auto&& func) {
		createMenu = func;
	}
	void OnDouble(auto&& func) {
		double_click = std::move(func);
	}
	void OnRight(auto&& func) {
		r_click = std::move(func);
	}
	void CycleTimer(auto&& func, int ms) {
		timerCallbacks.push_back(func);
		auto timeId = SetTimer(hwnd, lastTimerId, ms, NULL);
		IFW_LOG(timeId != 0);
		lastTimerId++;
	}
	WinTray() {
		Inst = this;
		hwnd = WinUtils::CreateMsgWin(L"SimpleSwitcher_Tray_000", WindowProc);
		//hwnd = CreateWindowEx(0, CLASS_NAME, NULL, 0, 0, 0, 0, 0, 0, 0, 0, 0);
		//UpdateWindow(hwnd);
		nid.cbSize = sizeof(NOTIFYICONDATA);
		nid.hWnd = hwnd;
		//nid.uID = ID_TRAY_APP_ICON;
		nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
		nid.uCallbackMessage = WM_TRAYICON;
		wcscpy_s(nid.szTip, L"SimpleSwitcher");


	}
	~WinTray() {
		DeleteIcon();
	}

	void SetIcon(HICON icon) {
		//nid.hIcon = LoadIcon(NULL, IDI_APPLICATION); // Load your icon here
		nid.hIcon = icon; // Load your icon here
		if (!Shell_NotifyIcon(NIM_MODIFY, &nid)) {
			if (!Shell_NotifyIcon(NIM_ADD, &nid)) {
				int k = 0;
				// todo - by timer
			}
		}
	}
	void DeleteIcon() {
		Shell_NotifyIcon(NIM_DELETE, &nid); // Remove icon from tray
	}

};
