#pragma once

class CMainWorker {

	CMultiThreadQueue<Message_Variant> m_queue;
	std::thread m_thread;

	void WorkerInt();

	void StopAndWait() {
		m_queue.QuitRequest();
		if (m_thread.joinable())
			m_thread.join();
	}

public:
	~CMainWorker() {
		StopAndWait();
	}
	void ReStart() {
		StopAndWait();
		m_queue.Reinitialize();
		m_thread = std::thread(std::bind(&CMainWorker::WorkerInt, this));
	}

	void PostMsg(auto&& msg, int delay = 0) {
		m_queue.PostMsg(std::move(msg), delay);
	}

	static CMainWorker& Inst() {
		static CMainWorker inst;
		return inst;
	}

};
inline CMainWorker* Worker() { return &CMainWorker::Inst(); }