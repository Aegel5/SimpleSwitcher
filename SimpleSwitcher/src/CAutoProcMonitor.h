
struct TSWCheckRunRes
{
	bool found = false;
	bool admin = false;
};

class CAutoProcMonitor
{
public:

    const TChar* m_sWndName = 0;
    std::wstring m_sCmd     ;
    std::wstring m_sExe     ;
    bool m_autoClose        = true;

	~CAutoProcMonitor()
	{
		if(m_autoClose)
		{
			IFS_LOG(Stop());
		}
	}


	TStatus Stop()
	{
		HWND hwnd = FindWindow(m_sWndName, NULL);

		LOG_INFO_1(L"Stop monitor for hwnd=%p", hwnd);

		if(!hwnd)
			RETURN_SUCCESS;

		DWORD pid = 0;
		GetWindowThreadProcessId(hwnd, &pid);
		DWORD desireAccess = SYNCHRONIZE;
		CAutoHandle hProc = OpenProcess(SYNCHRONIZE, FALSE, pid);
		IFW_RET(hProc.IsValid());
		PostMessage(hwnd, WM_QUIT, 0, 0);
		IF_WAITDWORD_RET(WaitForSingleObject(hProc, c_nCommonWaitProcess));

		RETURN_SUCCESS;
	}
	TSWCheckRunRes CheckRunning()
	{
		TSWCheckRunRes res;

		HWND hwnd = FindWindow(m_sWndName, NULL);

		if (hwnd)
		{
			res.found = true;

			DWORD pid = 0;
			GetWindowThreadProcessId(hwnd, &pid);
			if (Utils::IsElevatedByPid(pid))
				res.admin = true;
		}

		LOG_INFO_1(L"CheckRunning result: found=%u, admin=%u", res.found, res.admin);

		return res;
	}
	TStatus EnsureStarted(TSWAdmin admin)
	{
		TSWCheckRunRes res = CheckRunning();

		if(res.found)
		{
			bool fCheckAdmin = (admin == SW_ADMIN_ON);
			if(admin == SW_ADMIN_SELF)
			{
				fCheckAdmin = Utils::IsSelfElevated();
			}

			if(fCheckAdmin != res.admin)
			{
				IFS_RET(Stop());
			}
			else
			{
				RETURN_SUCCESS;
			}
		}

		LOG_INFO_1(L"Create hooker process...");
		IFS_RET(SwCreateProcessOurWaitIdle(m_sExe.c_str(),
			m_sCmd.c_str(),
			admin));

		RETURN_SUCCESS;
	}



};