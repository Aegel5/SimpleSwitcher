#include "sw-base.h"


void CMainWorker::WorkerInt()
{
	IFW_LOG(SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_ABOVE_NORMAL));

	COM::CAutoCOMInitialize autoCom;
	IFS_LOG(autoCom.Init());

	WorkerImplement workerImpl;

	while (true) {
		auto msg = m_queue.GetMessage();
		if (!msg.has_value()) break;
		std::visit([&workerImpl](auto&& arg) {
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
			else if constexpr (std::is_same_v<T, Message_GetCurLay>) {
				workerImpl.CheckCurLay(arg.force);
			}
			else if constexpr (std::is_same_v<T, Message_ClearWorlds>) {
				workerImpl.ClearAllWords();
			}
			else if constexpr (std::is_same_v<T, Message_Func>) {
				arg(workerImpl);
			}
		}, msg.value());
	}

	LOG_ANY(L"Exit main worker");
}


