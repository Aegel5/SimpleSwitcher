#pragma once

class WinTray {

public:struct TrayItem {
		string name;
		std::function<void()> callback;
		bool is_separator = false;
		bool is_checkbox = false;
		bool edit_val = false;
	};
private:

	static const int WM_TRAYICON = WM_USER + 1;
	static const int ID_TRAY_EXIT = 1002;

	inline static WinTray* Inst = 0;

	std::function<void()> double_click;
	std::function<void()> r_click;
	std::function<std::vector<TrayItem>()> createMenu;
	std::vector<TrayItem> last_menu;

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
						if (it.is_separator) {
							AppendMenu(hMenu, MF_SEPARATOR, 0, NULL);
							continue;
						}
						if (it.is_checkbox) {
							AppendMenu(hMenu, MF_STRING | (it.edit_val ? MF_CHECKED : MF_UNCHECKED), i, StrUtils::Convert(it.name).c_str());
							continue;
						}
						AppendMenu(hMenu, MF_STRING, i, StrUtils::Convert(it.name).c_str());
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

		default:
			return DefWindowProc(hwnd, uMsg, wParam, lParam);
		}
		return 0;
	}
public:
	HWND GetHandler() { return hwnd; }
	void OnCreateMenu(auto&& func) {
		createMenu = func;
	}
	void OnDouble(auto&& func) {
		double_click = std::move(func);
	}
	void OnRight(auto&& func) {
		r_click = std::move(func);
	}

	WinTray() {
		Inst = this;
		hwnd = WinUtils::CreateMsgWin(L"SimpleSwitcher_Tray_001", WindowProc);
		//hwnd = CreateWindowEx(0, CLASS_NAME, NULL, 0, 0, 0, 0, 0, 0, 0, 0, 0);
		//UpdateWindow(hwnd);
		nid.cbSize = sizeof(NOTIFYICONDATA);
		nid.hWnd = hwnd;
		//nid.uID = ID_TRAY_APP_ICON;
		nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
		nid.uCallbackMessage = WM_TRAYICON;
		swprintf_s(nid.szTip, L"SimpleSwitcher %S", SW_VERSION);


	}
	~WinTray() {
		DeleteIcon();
	}

	void SetIcon(HICON icon) {
		//nid.hIcon = LoadIcon(NULL, IDI_APPLICATION); // Load your icon here
		nid.hIcon = icon; // Load your icon here
		if (!Shell_NotifyIcon(NIM_MODIFY, &nid)) {
			if (!Shell_NotifyIcon(NIM_ADD, &nid)) {
				LOG_WARN("Can't modify or add icon");
				// todo - by timer
			}
		}
	}
	void DeleteIcon() {
		Shell_NotifyIcon(NIM_DELETE, &nid); // Remove icon from tray
	}

};
