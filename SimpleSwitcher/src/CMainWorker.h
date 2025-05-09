#pragma once

struct Message_KeyType {
	TKeyCode   vkCode = 0;
	TScanCode_Ext scan_ext;
	HotKeyType hk = hk_NULL;
	KeyState keyState = KEY_STATE_NONE;
};

struct Message_Hotkey {
	bool fix_ralt = false;
	CHotKey hotkey;
	HotKeyType hk = hk_NULL;
	ULONGLONG delayed_from = 0;
};

struct Message_ChangeForeg {
	HWND hwnd = 0;
};

struct Message_ClearWorlds {};
struct Message_Quit {};

class WorkerImplement;
using Message_Func = std::move_only_function<void(WorkerImplement*)>; // все захваченное должно быть копией.

using Message_Variant = std::variant< Message_KeyType, Message_Hotkey, Message_ClearWorlds, Message_Func, Message_ChangeForeg, Message_Quit >;

class CMainWorker {

	std::unique_ptr<WorkerImplement> worker_impl;
	MultiThreadQueue<Message_Variant> m_queue;
	std::thread m_thread;

	void WorkerInt();

	void StopAndWait() {
		m_queue.PostMsg(Message_Quit{});
		if (m_thread.joinable())
			m_thread.join();
	}

	void ReStart() {
		StopAndWait();
		m_queue.Reinitialize();
		m_thread = std::thread(std::bind(&CMainWorker::WorkerInt, this));
	}

public:
	WorkerImplement* WorkerImpl() { return worker_impl.get(); }

	void Init();

	~CMainWorker() {
		StopAndWait();
	}


	void PostMsg(auto&& msg, int delay = 0) {
		m_queue.PostMsg(std::move(msg), delay);
	}

};

namespace details {
	inline CMainWorker* g_worker; // one-time write
}

inline CMainWorker* Worker() { return details::g_worker; }
inline WorkerImplement* WorkerImpl() { return Worker()->WorkerImpl(); }
