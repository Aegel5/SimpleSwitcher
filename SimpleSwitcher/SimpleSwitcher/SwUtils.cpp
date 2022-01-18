#include "stdafx.h"
#include <Shellapi.h>

#include "SwUtils.h"
#include "SimpleSwitcher.h"



TStatus CheckService(bool& isAdmin)
{
	CAutoSCHandle hSCManager = ::OpenSCManager(NULL, NULL, SC_MANAGER_ENUMERATE_SERVICE);
	IFW_RET(hSCManager.IsValid());

	CAutoSCHandle hService = ::OpenService(hSCManager, c_sServiceName, SERVICE_QUERY_STATUS);
	isAdmin = hService.IsValid();

	RETURN_SUCCESS;
}




TStatus CreateService()
{
	CAutoSCHandle hSCManager = ::OpenSCManager(NULL, NULL, SC_MANAGER_CREATE_SERVICE);
	IFW_RET(hSCManager.IsValid());

	CAutoSCHandle hService = ::OpenService(hSCManager, c_sServiceName, SERVICE_CHANGE_CONFIG);

	std::wstring sPath;
	GetPath(sPath, PATH_TYPE_EXE_PATH, SW_BIT_32);

	TChar buf[0x1000];
	buf[0] = 0;

	IFNEG(swprintf_s(buf, L"%s %s", sPath.c_str(), c_sArgService))
		RET_ERRNO();
	if (hService.IsInvalid())
	{
		hService = ::CreateService(
			hSCManager,
			c_sServiceName,
			c_sServiceName,
			SERVICE_ALL_ACCESS,
			SERVICE_WIN32_OWN_PROCESS,
			SERVICE_AUTO_START,
			SERVICE_ERROR_NORMAL,
			buf,
			NULL,
			NULL,
			NULL,
			NULL,
			NULL);

		IFW_RET(hService.IsValid());

	}
	else
	{
		IFW_RET(ChangeServiceConfig(
			hService,
			SERVICE_WIN32_OWN_PROCESS,
			SERVICE_AUTO_START,
			SERVICE_ERROR_NORMAL,
			buf,
			NULL,
			NULL,
			NULL,
			NULL,
			NULL,
			c_sServiceName));
	}

	RETURN_SUCCESS;
}



TStatus RemoveService()
{
	CAutoSCHandle hSCManager = ::OpenSCManager(NULL, NULL, SC_MANAGER_CREATE_SERVICE);
	IFW_RET(hSCManager.IsValid());

	CAutoSCHandle hService = ::OpenService(hSCManager, c_sServiceName, DELETE);
	IFW_RET(hService.IsValid());
	IFW_RET(DeleteService(hService));

	RETURN_SUCCESS;
}








