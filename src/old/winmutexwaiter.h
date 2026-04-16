class CAutoWinMutexWaiter {
public:
	CAutoWinMutexWaiter() {}
	CAutoWinMutexWaiter(HANDLE mtx) {
		Wait(mtx);
	}
	DWORD Wait(HANDLE mtx, DWORD nTimeOut) {
		Cleanup();
		DWORD res = WaitForSingleObject(mtx, nTimeOut);
		if (res == WAIT_OBJECT_0)
			m_mtx = mtx;
		return res;
	}
	void Wait(HANDLE mtx) {
		Wait(mtx, INFINITE);
	}
	~CAutoWinMutexWaiter() {
		Cleanup();
	}
	void Cleanup() {
		if (m_mtx) {
			ReleaseMutex(m_mtx);
			m_mtx = nullptr;
		}
	}
private:
	HANDLE m_mtx = nullptr;
};


class CAutoWinEvent {
	BOOL SetEvent(HANDLE evt) {
		Cleanup();
		BOOL res = ::SetEvent(evt);
		if (res)
			m_evt = evt;
	}
	~CAutoWinEvent() {
		Cleanup();
	}
	void Cleanup() {
		if (m_evt) {
			ResetEvent(m_evt);
			m_evt = nullptr;
		}
	}
private:
	HANDLE m_evt = nullptr;
};
