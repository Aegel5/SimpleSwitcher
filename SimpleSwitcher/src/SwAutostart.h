#pragma once

#include "startup/scheduler.h"
#include "startup/reguser.h"

inline TStatus SetRegRun()
{
	std::wstring sPath;
    IFS_RET(GetPath_exe_noLower(sPath));

	IFS_RET(Startup::AddWindowsRun(c_sRegRunValue, sPath.c_str(), c_sArgAutostart));

	RETURN_SUCCESS;
}
inline TStatus DelRegRun()
{
	IFS_RET(Startup::RemoveWindowsRun(c_sRegRunValue));
	RETURN_SUCCESS;
}
inline TStatus CheckRegRun(bool& isAllOk, bool& isHasTask)
{
	std::wstring sPath;
    IFS_RET(GetPath_exe_noLower(sPath));

	IFS_RET(Startup::CheckAutoStartUser(isAllOk, isHasTask, c_sRegRunValue, sPath.c_str(), c_sArgAutostart));

	RETURN_SUCCESS;
}

inline TStatus SetSchedule()
{

	std::wstring sPath;
    IFS_RET(GetPath_exe_noLower(sPath));

	Startup::CreateTaskSheduleParm parm;
	parm.fAsAdmin = true;
	parm.sPath = sPath.c_str();
	parm.taskName = c_wszTaskName;
	parm.sArgs = c_sArgAutostart;
	parm.removeOld = true;

	IFS_RET(Startup::CreateTaskShedule2(parm));


	RETURN_SUCCESS;

}

inline TStatus DelSchedule()
{
	IFS_RET(Startup::RemoveTaskShedule(c_wszTaskName));
	RETURN_SUCCESS;
}

inline TStatus CheckSchedule(bool& isAllOk, bool& isHasTask)
{
	std::wstring sPath;
    IFS_RET(GetPath_exe_noLower(sPath));

	Startup::CheckTaskSheduleParm parm;
	parm.taskName = c_wszTaskName;
	parm.sPath = sPath.c_str();
	parm.sArgs = c_sArgAutostart;
	IFS_RET(Startup::CheckTaskShedule(parm));
	isHasTask = parm.isTaskExists;
	isAllOk = parm.isPathEqual && parm.isSettingsCorrect;
	RETURN_SUCCESS;
}



