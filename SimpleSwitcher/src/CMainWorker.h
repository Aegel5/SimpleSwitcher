#pragma once

class CMainWorker
{
	ThreadQueue::CThreadQueue<MainWorkerMsg> m_queue;
	TStatus WorkerInt();
	void Worker()
	{
		IFS_LOG(WorkerInt());
	}
public:
	TStatus ReStart()
	{
		IFS_RET(m_queue.ReStartWorker(std::bind(&CMainWorker::Worker, this)));
		RETURN_SUCCESS;
	}
	void PostMsg(MainWorkerMsg& msg)
	{
#ifdef SW_INT_CHECK
		auto count = m_queue.CountMsg();
		if (count >= 100)
		{
			LOG_INFO_1(L"[WARN] Too many messages %u", count);
		}
        static auto dwLast = GetTickCount64();
        auto dwCur         = GetTickCount64();
		auto elapsed = dwCur - dwLast;
		if (elapsed >= 10000)
		{
			dwLast = dwCur;
			if (count > 0)
			{
				LOG_INFO_1(L"Now have %u messages", count);
			}
		}
#endif
		m_queue.PostMsg(msg);
	}
	void PostMsg(EHWorker mode)
	{
		MainWorkerMsg msg(mode);
		PostMsg(msg);
	}
	void PostMsgW(EHWorker mode, WPARAM wparm)
	{
		MainWorkerMsg msg(mode);
		msg.data.wparm = wparm;
		PostMsg(msg);
	}

	static CMainWorker& Inst() {
		static CMainWorker inst;
		return inst;
	}


};
inline CMainWorker* Worker() { return &CMainWorker::Inst(); }