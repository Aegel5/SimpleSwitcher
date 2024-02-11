#pragma once
namespace gui_tools
{
	template<UINT WM_TRAYICON, UINT TIMER_ID, UINT ICON_ID>
	class TrayIcon
	{
	public:
		typedef std::function<void()> TMenu;
		void Init(HWND hwnd, HICON icon, TMenu menu)
		{
			m_hwnd = hwnd;
			m_icon = icon;
			m_menu = menu;
		}
		void SetEnabled(bool enabled)
		{
			if (enabled != m_enabled)
			{
				m_enabled = enabled;
				if (m_enabled)
				{
					AddIconToTrayTrySeveral();
				}
				else
				{
					DeleteNotifyIcon();
				}
			}
		}
		bool ProcMsg(Wmsg& wmsg)
		{
			if (wmsg.msg == WM_TRAYICON)
			{
				if (wmsg.lparm == WM_RBUTTONUP)
				{
					if (m_menu)
					{
						m_menu();
					}
				}
				else if (wmsg.lparm == WM_LBUTTONUP)
				{
					if (IsWindowVisible(m_hwnd))
					{
						ShowWindow(m_hwnd, SW_HIDE);
					}
					else
					{
						ShowWindow(m_hwnd, SW_SHOW);
						SetForegroundWindow(m_hwnd);
					}
				}
				return TRUE;
			}
			if (wmsg.msg == WM_INITDIALOG)
			{
				m_uTaskbarRestart = RegisterWindowMessage(TEXT("TaskbarCreated"));
				return false;
			}
			else if (wmsg.msg == m_uTaskbarRestart)
			{
				if (m_enabled)
				{
					AddIconToTray();
				}
				return true;
			}
			else if (wmsg.msg == WM_TIMER)
			{
				UINT_PTR timerId = wmsg.wparm;
				if (timerId == TIMER_ID)
				{
					static int tryCount = 0;

					bool fKillTimer = true;

					if (m_enabled)
					{
						bool res = AddIconToTray();

						++tryCount;
						bool cancelTry = (tryCount >= 300);

						if (cancelTry)
						{
							LOG_INFO_1(L"Cancel try add to tray");
						}

						fKillTimer = (res || cancelTry);
					}


					if (fKillTimer)
					{
						tryCount = 0;
						IFW_LOG(KillTimer(m_hwnd, TIMER_ID));
					}

					return true;
				}
			}

			return false;
		}
	private:
		void DeleteNotifyIcon()
		{
			NOTIFYICONDATA nid = { 0 };
			nid.cbSize = sizeof(nid);
			nid.uID = ICON_ID;
			nid.hWnd = m_hwnd;
			IFW_LOG(Shell_NotifyIcon(NIM_DELETE, &nid));
		}
		bool AddIconToTray()
		{
			// Add a Shell_NotifyIcon notificaion
			NOTIFYICONDATA nid = { 0 };
			nid.cbSize = sizeof(nid);
			nid.uID = ICON_ID;
			nid.hWnd = m_hwnd;
			nid.uFlags = NIF_ICON | NIF_MESSAGE;
			nid.hIcon = m_icon;
			nid.uCallbackMessage = WM_TRAYICON;

			static const size_t tryAddCount = 30;
			BOOL fResAdd = FALSE;
			size_t tryed = 0;
			fResAdd = Shell_NotifyIcon(NIM_ADD, &nid);
			LOG_INFO_1(L"Add to tray. res=%u", fResAdd);

			return fResAdd ? true : false;
		}
		void AddIconToTrayTrySeveral()
		{
			bool res = AddIconToTray();
			if (!res)
			{
				auto timeId = SetTimer(m_hwnd, TIMER_ID, 500, NULL);
				IFW_LOG(timeId != 0);
			}
		}

		bool m_enabled = false;
		HWND m_hwnd = NULL;
		HICON m_icon = 0;
		UINT m_uTaskbarRestart = -1;
		TMenu m_menu;
	};
}
