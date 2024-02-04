#pragma once

#include "InputSender.h"
#include "Hooker.h"

TStatus InputSender::Send()
{
	if(list.empty())
		RETURN_SUCCESS;
	for (auto& i : list)
	{
		LOG_INFO_2(L"SEND %s ?", i.ki.dwFlags == KEYEVENTF_KEYUP ? L"UP" : L"DW"
			// не пишем персональную инфу
			// ,CHotKey::ToString((TKeyCode)i.ki.wVk).c_str()
		);
	}
	IFW_RET(SendInput((UINT)list.size(), &list[0], sizeof(INPUT)) == list.size());

	if (conf_get()->AllowRemoteKeys) {
		g_hooker->skipdata.emplace_back(GetTickCount64() + 500, list.size());
		if (g_hooker->skipdata.size() >= 10000) {
			g_hooker->skipdata.clear();
		}
	}

	RETURN_SUCCESS;
}
