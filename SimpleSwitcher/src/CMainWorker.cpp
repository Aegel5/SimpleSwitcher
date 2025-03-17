#include "stdafx.h"

#include "CMainWorker.h"
#include "Hooker.h"
#include "Dispatcher.h"


TStatus CMainWorker::WorkerInt()
{
	IFW_LOG(SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_ABOVE_NORMAL));

	COM::CAutoCOMInitialize autoCom;
	IFS_LOG(autoCom.Init());

	Hooker hooker;
	IFS_RET(hooker.Init());
	g_hooker = &hooker;

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
			hooker.ClearAllWords();
		}
		else if (mode == HWORKER_ClipNotify)
		{
			hooker.CliboardChanged();
		}
		else if (mode == HWORKER_ChangeForeground)
		{
			hooker.ChangeForeground((HWND)msg.data.wparm);
		}
		else if (mode == HWORKER_KeyMsg)
		{
			IFS_LOG(hooker.ProcessKeyMsg(msg.data.keyData));
		}
		else if (mode == HWORKER_OurHotKey)
		{
			hooker.ProcessOurHotKey(msg);
		}
		else if (mode == HWORKER_FixCtrlAlt) {
			IFS_LOG(hooker.FixCtrlAlt(msg.data.hotkey_to_fix));
		}
		else if (mode == HWORKER_WM_TIMER)
		{
			UINT_PTR timerId = msg.data.wparm;
			if (timerId == c_timerIdClearFormat)
			{
				IFS_LOG(hooker.ClipboardClearFormat2());
			}
            else if (timerId == c_timerGetcurlay)
            {
                if (conf_get_unsafe()->showFlags) {
                    hooker.CheckCurLay();
                }
            }
			//else if (timerId == c_timerWaitClip)
			//{
			//	IFS_LOG(hooker.TimerProcWaitClip2());
			//}
			else
			{
				LOG_INFO_1(L"[WARN] Unknown timerId=%Iu", timerId);
			}
		}
		else if (mode == HWORKER_Getcurlay) {
            hooker.CheckCurLay(true);
        }
		else if (mode == HWORKER_Setcurlay) {
			hooker.SetNewLay(msg.data.lay);
		}
		else
		{
			LOG_INFO_1(L"[WARN] Unknown m2=%u", mode);
		}
	}

	LOG_INFO_1(L"Exit main worker");
	RETURN_SUCCESS;
}


