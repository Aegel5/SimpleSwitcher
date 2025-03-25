#pragma once

class CAutoClipBoard
{
public:
	CAutoClipBoard() {}
	TStatus Open(HWND hwnd = NULL, DWORD dwTimeout=50)
	{
		if (m_stat)
			RETURN_SUCCESS;

		auto start = GetTickCount64();
		int countTry = 0;
		while (true)
		{
			++countTry;
			m_stat = OpenClipboard(hwnd);
			if (m_stat)
			{
				break;
			}
			if (GetTickCount64() - start > dwTimeout)
			{
				break;
			}
			Sleep(2);
		}

		if (countTry > 1)
		{
			LOG_ANY(L"Clip open try {} count. result={}", countTry, m_stat);
		}

		IFW_RET(m_stat); 
		RETURN_SUCCESS;
	}
	void Cleanup()
	{
		if (m_stat)
		{
			LOG_ANY(L"close clip");
			IFW_LOG(CloseClipboard());
		}
		m_stat = FALSE;
	}
	~CAutoClipBoard()
	{
		Cleanup();
	}
private:
	BOOL m_stat = FALSE;
};

class CAutoHotKeyRegister
{
public:
	BOOL Register(HWND hwnd, int id, UINT mods, DWORD key)
	{
		Cleanup();
		m_hwnd = hwnd;
		m_id = id;
		m_res = RegisterHotKey(hwnd, id, mods, key);
		return m_res;
	}
	~CAutoHotKeyRegister()
	{
		Cleanup();
	}
	void Cleanup()
	{
		if (m_res)
		{
			UnregisterHotKey(m_hwnd, m_id);
			m_res = FALSE;
		}
	}
private:
	BOOL m_res = FALSE;
	HWND m_hwnd;
	int m_id;
};

class CAutoThreadAttach
{
public:
	CAutoThreadAttach(){}
	TStatus Attach(DWORD id)
	{
		threadId = id;
		res = AttachThreadInput(threadId, GetCurrentThreadId(), TRUE);
		IFW_RET(res);
		RETURN_SUCCESS;
	}
	~CAutoThreadAttach()
	{
		if(res)
		{
			IFW_LOG(AttachThreadInput(threadId, GetCurrentThreadId(), FALSE));
			res = 0;
		}
	}
private:
	BOOL res = false;
	DWORD threadId = 0;
};


