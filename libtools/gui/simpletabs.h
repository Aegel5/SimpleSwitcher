#pragma once

namespace gui_tools
{
	class SimpleTabs
	{
	public:
		void Init(HINSTANCE hinst, HWND hmain, int idList)
		{
			m_hwndMain = hmain;
			m_idList = idList;
			m_hwndList = GetDlgItem(hmain, idList);
			m_hInst = hinst;
		}
		TStatus AddTab(int idDialogRes, DLGPROC proc, TStr title/*, TUInt32 idTab = 0*/)
		{
			HWND hPage;
			IFS_RET(InitPage(
				MAKEINTRESOURCE(idDialogRes),
				proc,
				&hPage, m_hwndMain));

			if (m_tabs.empty())
			{
				// show first
				ShowPage(hPage);
			}

			m_tabs.push_back(hPage);

			SendDlgItemMessage(m_hwndMain, m_idList, LB_ADDSTRING, 0, (WPARAM)title);

			RETURN_SUCCESS;
		}
		bool ProcessMessage(Wmsg& wmsg)
		{
			if (wmsg.msg == WM_COMMAND)
			{
				if (HIWORD(wmsg.wparm) == LBN_SELCHANGE)
				{
					HWND hwndTarget = (HWND)wmsg.lparm;
					if (hwndTarget == m_hwndList)
					{
						LRESULT res = SendMessage(hwndTarget, LB_GETCURSEL, 0, 0);
						size_t index = (size_t)res;
						if (res != LB_ERR)
						{
							if (index >= m_tabs.size())
							{
								return false;
							}
							HWND hwndTab = m_tabs[index];
							ShowPage(hwndTab);
						}
						return true;
					}
				}
			}
			return false;
		}
		HWND Back()
		{
			return m_tabs.back();
		}
	private:
		TStatus InitPage(LPCTSTR templ, DLGPROC dlgProc, HWND* hwndStorage, HWND hwndMain)
		{
			HWND hwnd = CreateDialog(
				m_hInst,
				templ,
				m_hwndMain,
				dlgProc);

			IFNULL(hwnd) RETW();

			*hwndStorage = hwnd;

			RECT rect;
			IFW_RET(GetWindowRect(hwnd, &rect));
			int widthPage = rect.right - rect.left;
			IFW_RET(GetClientRect(hwndMain, &rect));
			int widthMain = rect.right - rect.left;
			int x = widthMain - widthPage;
			SetWindowPos(hwnd, 0, x, 0, 0, 0, SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE);
			ShowWindow(hwnd, SW_HIDE);
			UpdateWindow(hwnd);

			RETURN_SUCCESS;
		}
		void ShowPage(HWND hwnd)
		{
			if (m_hwndCurShow)
			{
				ShowWindow(m_hwndCurShow, SW_HIDE);
				UpdateWindow(m_hwndCurShow);
			}
			m_hwndCurShow = hwnd;
			ShowWindow(hwnd, SW_SHOW);
			UpdateWindow(hwnd);
		}

		HWND m_hwndCurShow = NULL;
		HWND m_hwndMain = NULL;
		std::vector<HWND> m_tabs;
		int m_idList = -1;
		HWND m_hwndList = NULL;
		HINSTANCE m_hInst;
	};



}
