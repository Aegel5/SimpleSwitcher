#pragma once

class CMainWorker {
	ThreadQueue::CThreadQueue<MainWorkerMsg> m_queue;
	TStatus WorkerInt();
	void Worker() {
		IFS_LOG(WorkerInt());
	}
public:
	void ReStart() {
		m_queue.ReStartWorker(std::bind(&CMainWorker::Worker, this));
	}
	void PostMsg(MainWorkerMsg&& msg, int delay = 0) {
#ifdef SW_INT_CHECK
		auto count = m_queue.CountMsg();
		if (count >= 100) {
			LOG_INFO_1(L"[WARN] Too many messages %u", count);
		}
		static auto dwLast = GetTickCount64();
		auto dwCur = GetTickCount64();
		auto elapsed = dwCur - dwLast;
		if (elapsed >= 10000) {
			dwLast = dwCur;
			if (count > 0) {
				LOG_INFO_1(L"Now have %u messages", count);
			}
		}
#endif
		m_queue.PostMsg(std::move(msg), delay);
	}
	void PostMsg(EHWorker mode, int delay = 0) {
		MainWorkerMsg msg(mode);
		PostMsg(std::move(msg), delay);
	}
	void PostMsgW(EHWorker mode, WPARAM wparm) {
		MainWorkerMsg msg(mode);
		msg.data.wparm = wparm;
		PostMsg(std::move(msg));
	}

	static CMainWorker& Inst() {
		static CMainWorker inst;
		return inst;
	}

};
inline CMainWorker* Worker() { return &CMainWorker::Inst(); }