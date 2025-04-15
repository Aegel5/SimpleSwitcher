#pragma once

class CMainWorker {

	MultiThreadQueue<Message_Variant> m_queue;
	std::thread m_thread;

	void WorkerInt();

	void StopAndWait() {
		m_queue.QuitRequest();
		if (m_thread.joinable())
			m_thread.join();
	}

	void ReStart() {
		StopAndWait();
		m_queue.Reinitialize();
		m_thread = std::thread(std::bind(&CMainWorker::WorkerInt, this));
	}

public:
	CMainWorker() {
		ReStart();
	}
	~CMainWorker() {
		StopAndWait();
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
