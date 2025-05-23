﻿#include "sw-base.h"


void CMainWorker::WorkerInt()
{
	IFW_LOG(SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_ABOVE_NORMAL));

	COM::CAutoCOMInitialize autoCom;
	IFS_LOG(autoCom.Init());

	WorkerImplement& workerImpl = *worker_impl.get();

	bool exit = false;
	while (!exit) {
		auto msg = m_queue.GetMessageWait();
		std::visit([&workerImpl, &exit](auto&& arg) {
			using T = std::decay_t<decltype(arg)>;
			if constexpr (std::is_same_v<T, Message_KeyType>) {
				workerImpl.ProcessKeyMsg(arg);
			}
			else if constexpr (std::is_same_v<T, Message_Hotkey>) {
				if (arg.fix_ralt) {
					workerImpl.FixCtrlAlt(arg.hotkey);
				}
				else {
					workerImpl.ProcessOurHotKey(arg);
				}
			}
			else if constexpr (std::is_same_v<T, Message_ChangeForeg>) {
				workerImpl.ChangeForeground(arg.hwnd);
			}
			else if constexpr (std::is_same_v<T, Message_ClearWorlds>) {
				workerImpl.ClearAllWords();
			}
			else if constexpr (std::is_same_v<T, Message_Func>) {
				arg(&workerImpl);
			}
			else if constexpr (std::is_same_v<T, Message_Quit>) {
				exit = true;
			}
		}, msg);
	}

	LOG_ANY(L"Exit main worker");
}

void CMainWorker::Init() {
	worker_impl = MAKE_UNIQUE(worker_impl);
	m_thread = std::thread(std::bind(&CMainWorker::WorkerInt, this));
}


