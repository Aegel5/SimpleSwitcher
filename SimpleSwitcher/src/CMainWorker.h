#pragma once



enum EHWorker
{
	HWORKER_NULL,

	HWORKER_ClearWords,
	HWORKER_ChangeForeground,
	HWORKER_KeyMsg,
	HWORKER_ClipNotify,
	HWORKER_WM_TIMER,
    HWORKER_Getcurlay,
    HWORKER_Setcurlay,
    HWORKER_FixCtrlAlt,
    HWORKER_OurHotKey,
};

struct MainWorkerMsg
{
	EHWorker mode = HWORKER_NULL;
	MainWorkerMsg(EHWorker m): mode(m) {}

	union U
	{
		struct Key_Message {
			TKeyCode   vkCode;
			TScanCode   scanCode;
			DWORD   flags;
			HotKeyType hk;
			KeyState keyState;
		} key_message;
		struct
		{
			WPARAM wparm;
			//LPARAM lparm;
		};
		struct {
			CHotKey hotkey_to_fix;
		};
		struct {
			CHotKey hotkey;
			HotKeyType hk;
		};
		struct {
			HKL lay;
		};
		U() {} // ничего не делаем - никаких конструкторов, клиенты должны заполнять сами нужные данные...
		
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


};
