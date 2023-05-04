#include "stdafx.h"

#include "CMainWorker.h"
#include "Hooker.h"
#include "Dispatcher.h"


TStatus CMainWorker::WorkerInt()
{
	IFW_LOG(SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_ABOVE_NORMAL));

	Hooker hooker;
	IFS_RET(hooker.Init());

	MainWorkerMsg msg;
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
		//else if (mode == HWORKER_SavePrevDataCallback)
		//{
		//	hooker.SavePrevDataCallback((EClipRequest)msg.data.wparm);
		//}
		//else if (mode == HWORKER_GetClipStringCallback)
		//{
		//	hooker.GetClipStringCallback();
		//}
		else if (mode == HWORKER_ChangeForeground)
		{
			hooker.ChangeForeground((HWND)msg.data.wparm);
		}
		else if (mode == HWORKER_KeyMsg)
		{
			KeyMsgData& kmsg = msg.data.keyData;
			IFS_LOG(hooker.ProcessKeyMsg(kmsg));
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
                if (settings_thread.showFlags) {
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
		else if (mode == HWORKER_LoadSettings)
		{
			// todo load not from file but from safe copy.
            IFS_LOG(LoadConfig(settings_thread));
			//IFS_LOG(ResetAllHotKey(*gdata().hotkeyHolders));
        } else if (mode == HWORKER_Getcurlay) {
            hooker.CheckCurLay(true);
        }
		else
		{
			LOG_INFO_1(L"[WARN] Unknown m2=%u", mode);
		}
	}

	LOG_INFO_1(L"Exit main worker");
	RETURN_SUCCESS;
}


