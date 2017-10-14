#pragma once


#include <Wtsapi32.h>

MAKE_AUTO_CLEANUP(LPTSTR, ::WTSFreeMemory, NULL, CAutoWTSBuffer)
MAKE_AUTO_CLEANUP(PWTS_SESSION_INFO, ::WTSFreeMemory, NULL, CAutoWTSSessionInfo)

TStatus TokenFromSessionId(DWORD sessionId, CAutoHandle& hToken);

class Impers
{
public:
	Impers()
	{
		DWORD sessionId = -1;
		ProcessIdToSessionId(GetCurrentProcessId(), &sessionId);
		if (sessionId == 0)
		{
			SW_LOG_INFO(LOG_LEVEL_1, L"Impper");
			CAutoHandle hToken;
			SW_TSTATUS_LOG1(TokenFromSessionId(1, hToken));
			SW_BOOL_LOG1(ImpersonateLoggedOnUser(hToken));
		}
	}

};