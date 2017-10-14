#include "stdafx.h"



#include "OpenSwitcher.h"
#include "SwUtils.h"
#include "SettingsGui.h"

#include "service.h"





TStatus TokenFromSessionId(DWORD sessionId, CAutoHandle& hToken)
{
	CAutoHandle pUserToken;
	SW_BOOL_RET(WTSQueryUserToken(sessionId, &pUserToken));

	SW_BOOL_RET(DuplicateTokenEx(
		pUserToken,
		MAXIMUM_ALLOWED,
		NULL,
		SecurityImpersonation,
		TokenPrimary,
		&hToken));

	SW_RETURN_SUCCESS;
}
TStatus SetPrivilege(
	HANDLE hToken,          // access token handle
	const LPCTSTR lpszPrivilege,  // name of privilege to enable/disable
	BOOL bEnablePrivilege   // to enable or disable privilege
	)
{
	TOKEN_PRIVILEGES tp;
	LUID luid;

	SW_BOOL_RET(LookupPrivilegeValue(
		NULL,            // lookup privilege on local system
		lpszPrivilege,   // privilege to lookup 
		&luid));

	//SW_LOG_INFO(LOG_LEVEL_1, L"LastErr=%d", GetLastError());

	tp.PrivilegeCount = 1;
	tp.Privileges[0].Luid = luid;
	if (bEnablePrivilege)
		tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	else
		tp.Privileges[0].Attributes = 0;

	// Enable the privilege or disable all privileges.

	SW_BOOL_RET(AdjustTokenPrivileges(
		hToken,
		FALSE,
		&tp,
		sizeof(TOKEN_PRIVILEGES),
		(PTOKEN_PRIVILEGES)NULL,
		(PDWORD)NULL));
	SW_BOOL_RET(GetLastError()==ERROR_SUCCESS);

	//SW_LOG_INFO(LOG_LEVEL_1, L"LastErr=%d", GetLastError());

	SW_RETURN_SUCCESS;
}
TStatus StartService(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPTSTR lpCmdLine,
	_In_ int nCmdShow)
{

	CAutoWTSSessionInfo sessionInfo;
	DWORD nCount = 0;
	SW_BOOL_RET(WTSEnumerateSessions(
		WTS_CURRENT_SERVER_HANDLE,
		0,
		1,
		&sessionInfo,
		&nCount));

	for(size_t i = 0; i < nCount; ++i)
	{
		CAutoWTSBuffer pAutoBuffer = NULL;
		DWORD pBytesReturned = 0;

		DWORD sessionId = ((PWTS_SESSION_INFO)sessionInfo)[i].SessionId;
		BOOL res = WTSQuerySessionInformation(
			WTS_CURRENT_SERVER_HANDLE,
			sessionId,
			WTSConnectState,
			&pAutoBuffer,
			&pBytesReturned);

		if(!res)
			continue;

		UINT* state = (UINT*)(pAutoBuffer.Get());
		WTS_CONNECTSTATE_CLASS connectionState = (WTS_CONNECTSTATE_CLASS)(*state);
		if (connectionState == WTSActive)
		{
			// found
			SwUtils procCtrl;
			CAutoHandle hToken;
			SW_RET_TSTATUS(TokenFromSessionId(sessionId, hToken));

			SettingsGui settighs;
			settighs.Load();
			//TOKEN_ELEVATION_TYPE tokenElev = settighs.isMonitorAdmin ? TokenElevationTypeFull : TokenElevationTypeLimited;
			//SW_BOOL_LOG1(LOG_LEVEL_1, SetTokenInformation(
			//	hToken,
			//	TokenElevationType,
			//	&tokenElev,
			//	sizeof(tokenElev)));
			//SW_BOOL_LOG1(LOG_LEVEL_1, ImpersonateLoggedOnUser(hToken));
			//SW_RET_TSTATUS(SetPrivilege(hToken, L"SeCreateGlobalPrivilege", TRUE));
			//SW_RET_TSTATUS(SetPrivilege(hToken, L"SeChangeNotifyPrivilege", TRUE));
			//SW_RET_TSTATUS(SetPrivilege(hToken, L"SeImpersonatePrivilege", TRUE));

			CAutoHandle hProc;
			SW_RET_TSTATUS(procCtrl.StartSwProc(c_sArgMonitorOn, SW_BIT_32, NULL, SW_ADMIN_SELF, hProc));
			//SW_RET_TSTATUS(procCtrl.StartSwProc(c_sArgMonitorOn, SW_BIT_32, NULL, SW_ADMIN_SELF, hProc));

			SW_RETURN_SUCCESS;
		}
	}

	SW_RETURN_SUCCESS;
}


