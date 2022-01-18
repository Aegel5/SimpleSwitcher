
struct TSWCheckRunRes
{
	bool found = false;
	bool admin = false;
};

class CAutoProcMonitor
{
public:
	~CAutoProcMonitor()
	{
		if(m_autoClose)
		{
			IFS_LOG(Stop());
		}
	}

	CAutoProcMonitor(const TChar* sWndName, const TChar* sCmd, TSWBit bit,  bool autoClose) :
		m_sWndName(sWndName), 
		m_sCmd(sCmd), 
		m_autoClose(autoClose),
		m_bit(bit)
	{
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
		PostMessage(hwnd, c_MSG_Quit, 0, 0);
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
			if (Utils::IsElevated(pid))
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

		// hack for WaitInputIdle
		if (m_bit == SW_BIT_32 && admin != SW_ADMIN_SELF)
		{
			bool selfAdmin = Utils::IsSelfElevated();
			if ((!selfAdmin && admin == SW_ADMIN_ON) /*|| selfAdmin && admin == SW_ADMIN_OFF*/)
			{
				LOG_INFO_1(L"Try enable via starter...");
				IFS_RET(SwCreateProcessOurWaitFinished(
					c_sArgStarter,
					m_bit,
					admin));
				RETURN_SUCCESS;
			}
		}

		LOG_INFO_1(L"Create hooker process...");
		IFS_RET(SwCreateProcessOurWaitIdle(
			m_sCmd,
			m_bit,
			admin));

		RETURN_SUCCESS;
	}
private:
	const TChar* m_sWndName = 0;
	const TChar* m_sCmd = 0;
	TSWBit m_bit = SW_BIT_32;
	bool m_autoClose = false;

};