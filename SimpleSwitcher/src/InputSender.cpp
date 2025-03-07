#pragma once

#include "InputSender.h"
#include "Hooker.h"

TStatus InputSender::Send()
{
	if(list.empty())
		RETURN_SUCCESS;
	for (auto& i : list)
	{
		LOG_INFO_2(L"SEND %s %s", TestFlag(i.ki.dwFlags, KEYEVENTF_KEYUP) ? L"UP" : L"DW", CHotKey::GetName(i.ki.wVk));
	}

	bool doPause = false;

	// https://github.com/Aegel5/SimpleSwitcher/issues/53
	//if (g_hooker->m_sTopProcName == L"notepad.exe") { 
	//	// костыль для нового notepad...
	//	// не используем по дефолту, так как работаем медленнее...
	//	doPause = true; 
	//}

	InjectSkipper::Inst().AddOur(list.size());

	if (doPause) {
		for (auto& elem : list) {
			auto res = SendInput(1, &elem, sizeof(INPUT));
			IFW_LOG(res == 1);
			Sleep(1);
		}
	}
	else {
		IFW_LOG(SendInput((UINT)list.size(), &list[0], sizeof(INPUT)) == list.size());
	}



	RETURN_SUCCESS;
}
