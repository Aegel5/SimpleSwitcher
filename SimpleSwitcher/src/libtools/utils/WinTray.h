#pragma once

class WinTray {

	static const int WM_TRAYICON = WM_USER + 123;
	static const int ID_TRAY_EXIT = 1002;

	inline static WinTray* Inst = 0;

	std::function<void()> double_click;

	NOTIFYICONDATA nid {};
	HWND hwnd = 0;

	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
		switch (uMsg) {
		case WM_TRAYICON:
			if (lParam == WM_RBUTTONDOWN) {
				// Show context menu on right-click
				POINT cursorPos;
				GetCursorPos(&cursorPos);

				HMENU hMenu = CreatePopupMenu();
				AppendMenu(hMenu, MF_STRING, ID_TRAY_EXIT, L"Exit");

				SetForegroundWindow(hwnd);
				TrackPopupMenu(hMenu, TPM_BOTTOMALIGN | TPM_LEFTALIGN, cursorPos.x, cursorPos.y, 0, hwnd, NULL);
				DestroyMenu(hMenu);
			}
			else if (lParam == WM_LBUTTONDBLCLK) {
				Inst->double_click();
			}
			break;

		case WM_COMMAND:
			if (LOWORD(wParam) == ID_TRAY_EXIT) {
				// Exit application
				PostQuitMessage(0);
			}
			break;


		default:
			return DefWindowProc(hwnd, uMsg, wParam, lParam);
		}
		return 0;
	}
public:
	void OnDouble(auto&& func) {
		double_click = std::move(func);
	}
	WinTray() {

		Inst = this;

		HINSTANCE hInstance = GetModuleHandle(NULL);

		const wchar_t CLASS_NAME[] = L"TrayAppClass_fjikdfuj8iauf47837483";

		WNDCLASS wc = {};
		wc.lpfnWndProc = WindowProc;
		wc.hInstance = hInstance;
		wc.lpszClassName = CLASS_NAME;

		RegisterClass(&wc);

		hwnd = CreateWindow(
			CLASS_NAME,
			CLASS_NAME,
			0,
			0, 0, 0, 0,
			HWND_MESSAGE, NULL, hInstance, NULL);

		nid.cbSize = sizeof(NOTIFYICONDATA);
		nid.hWnd = hwnd;
		//nid.uID = ID_TRAY_APP_ICON;
		nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
		nid.uCallbackMessage = WM_TRAYICON;
		
		//wcscpy_s(nid.szTip, L"Tray Application");


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
