#pragma once



enum EHWorker
{
	HWORKER_NULL,

	HWORKER_ClearWords,
	HWORKER_ChangeForeground,
	HWORKER_KeyMsg,
	HWORKER_ClipNotify,
	HWORKER_GetClipStringCallback,
	HWORKER_SavePrevDataCallback,
	HWORKER_WM_TIMER,
	HWORKER_LoadSettings,
};

struct KeyMsgData
{
	KBDLLHOOKSTRUCT ks;
	WPARAM wParam;
};

struct MainWorkerMsg
{
	EHWorker mode = HWORKER_NULL;

	union
	{
		struct 
		{
			KeyMsgData keyData;
		};
		struct
		{
			WPARAM wparm;
			LPARAM lparm;
		};
	}data;
};

class CMainWorker
{
	ThreadQueue::CThreadQueue<MainWorkerMsg> m_queue;
	TStatus WorkerInt();
	void Worker()
	{
		IFS_LOG(WorkerInt());
	}
public:
	TStatus Init()
	{
		IFS_RET(m_queue.StartWorker(std::bind(&CMainWorker::Worker, this)));
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
		static DWORD dwLast = GetTick();
		DWORD dwCur = GetTick();
		DWORD elapsed = dwCur - dwLast;
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
		MainWorkerMsg msg;
		msg.mode = mode;
		PostMsg(msg);
	}
	void PostMsgW(EHWorker mode, WPARAM wparm)
	{
		MainWorkerMsg msg;
		msg.mode = mode;
		msg.data.wparm = wparm;
		PostMsg(msg);
	}


};
