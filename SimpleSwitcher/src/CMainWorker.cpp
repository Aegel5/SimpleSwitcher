#include "stdafx.h"


TStatus CMainWorker::WorkerInt()
{
	IFW_LOG(SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_ABOVE_NORMAL));

	COM::CAutoCOMInitialize autoCom;
	IFS_LOG(autoCom.Init());

	WorkerImplement workerImpl;
	IFS_RET(workerImpl.Init());

	MainWorkerMsg msg (HWORKER_NULL);
	while (true)
	{
		if (!m_queue.GetMessage(msg))
		{
			break;
		}
		auto mode = msg.mode;
		if (mode == HWORKER_ClearWords)
		{
			workerImpl.ClearAllWords();
		}
		else if (mode == HWORKER_ClipNotify)
		{
			workerImpl.CliboardChanged();
		}
		else if (mode == HWORKER_ChangeForeground)
		{
			workerImpl.ChangeForeground((HWND)msg.data.wparm);
		}
		else if (mode == HWORKER_KeyMsg)
		{
			workerImpl.ProcessKeyMsg(msg.data.key_message);
		}
		else if (mode == HWORKER_OurHotKey)
		{
			workerImpl.ProcessOurHotKey(msg);
		}
		else if (mode == HWORKER_FixCtrlAlt) {
			IFS_LOG(workerImpl.FixCtrlAlt(msg.data.hotkey_to_fix));
		}
		else if (mode == HWORKER_WM_TIMER)
		{
			UINT_PTR timerId = msg.data.wparm;
			if (timerId == c_timerIdClearFormat)
			{
				IFS_LOG(workerImpl.ClipboardClearFormat2());
			}

			else
			{
				LOG_INFO_1(L"[WARN] Unknown timerId=%Iu", timerId);
			}
		}
		else if (mode == HWORKER_Getcurlay) {
            workerImpl.CheckCurLay(msg.data.wparm);
        }
		else if (mode == HWORKER_Setcurlay) {
			workerImpl.SetNewLay(msg.data.lay);
		}
		else
		{
			LOG_INFO_1(L"[WARN] Unknown m2=%u", mode);
		}
	}

	LOG_INFO_1(L"Exit main worker");
	RETURN_SUCCESS;
}


