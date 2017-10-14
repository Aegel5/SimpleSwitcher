#pragma  once

namespace gui_tools
{
	typedef std::function<TStatus(HWND hwnd)> GuiHandler;
	typedef std::function<void(HWND hwnd)> GuiHandler2;

	struct Wmsg
	{
		UINT msg;
		WPARAM wparm;
		LPARAM lparm;
		HWND hwnd;
		Wmsg(HWND h, UINT m, WPARAM w, LPARAM l) :msg(m), wparm(w), lparm(l), hwnd(h) {}
	};

	inline bool IsCheckBox(HWND hwnd, UINT dlgId)
	{
		return (SendDlgItemMessage(hwnd, dlgId, BM_GETCHECK, 0, 0) == BST_CHECKED);
	}
	inline void SetCheckBox(HWND hwnd, UINT dlgId, bool val)
	{
		CheckDlgButton(hwnd, dlgId, val ? BST_CHECKED : BST_UNCHECKED);
	}

	inline COLORREF ColorFromRGB(int rgb)
	{
		int rr = (rgb >> 16) & 0xFF;
		int gg = (rgb >> 8) & 0xFF;
		int bb = (rgb) & 0xFF;
		return RGB(rr, gg, bb);
	}
}