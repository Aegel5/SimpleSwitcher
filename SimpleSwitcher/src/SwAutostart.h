#pragma once

#include "startup/scheduler.h"
#include "startup/reguser.h"

inline TStatus SetRegRun()
{
	std::wstring sPath;
    IFS_RET(PathUtils::GetPath_exe_noLower(sPath));
	IFS_RET(Startup::AddWindowsRun(c_sRegRunValue, sPath.c_str(), c_sArgAutostart));

	RETURN_SUCCESS;
}
inline TStatus CheckRegRun(bool& isAllOk, bool& isHasTask)
{
	std::wstring sPath;
    IFS_RET(PathUtils::GetPath_exe_noLower(sPath));
	IFS_RET(Startup::CheckAutoStartUser(isAllOk, isHasTask, c_sRegRunValue, sPath.c_str(), c_sArgAutostart));

	RETURN_SUCCESS;
}
inline TStatus DelRegRun() {
	bool isUserAllOk = false;
	bool isUserHasTask = false;
	IFS_RET(CheckRegRun(isUserAllOk, isUserHasTask));
	if (isUserHasTask) {
		IFS_RET(Startup::RemoveWindowsRun(c_sRegRunValue));
	}
	RETURN_SUCCESS;
}

inline TStatus SetSchedule()
{

	std::wstring sPath;
    IFS_RET(PathUtils::GetPath_exe_noLower(sPath));

	Startup::CreateTaskSheduleParm parm;
	parm.fAsAdmin = true;
	parm.sPath = sPath.c_str();
	parm.taskName = c_wszTaskName;
	parm.sArgs = c_sArgAutostart;
	parm.removeOld = true;

	IFS_RET(Startup::CreateTaskShedule2(parm));


	RETURN_SUCCESS;

}



inline TStatus CheckSchedule(bool& isAllOk, bool& isHasTask)
{
	std::wstring sPath;
    IFS_RET(PathUtils::GetPath_exe_noLower(sPath));

	Startup::CheckTaskSheduleParm parm;
	parm.taskName = c_wszTaskName;
	parm.sPath = sPath.c_str();
	parm.sArgs = c_sArgAutostart;
	IFS_RET(Startup::CheckTaskShedule(parm));
	isHasTask = parm.isTaskExists;
	isAllOk = parm.isPathEqual && parm.isSettingsCorrect;
	RETURN_SUCCESS;
}

inline TStatus DelSchedule() {
	bool isAdminAllOk = false;
	bool isAdminHasTask = false;
	IFS_RET(CheckSchedule(isAdminAllOk, isAdminHasTask));
	if (isAdminHasTask)
		IFS_RET(Startup::RemoveTaskShedule(c_wszTaskName));
	RETURN_SUCCESS;
}

inline bool autostart_get() {
	bool isUserAllOk = false;
	bool isUserHasTask = false;
	IFS_LOG(CheckRegRun(isUserAllOk, isUserHasTask));

	bool isAdminAllOk = false;
	bool isAdminHasTask = false;
	IFS_LOG(CheckSchedule(isAdminAllOk, isAdminHasTask));

	return conf_get_unsafe()->isMonitorAdmin ? isAdminAllOk && !isUserHasTask
		: isUserAllOk && !isAdminHasTask;

}
inline bool autostart_set(bool enable) {
	auto func = [enable]() -> TStatus {
		if (conf_get_unsafe()->isMonitorAdmin) {
			IFS_RET(DelRegRun());
			if (enable) {
				IFS_RET(SetSchedule());
			}
			else {
				IFS_RET(DelSchedule());
			}
		}
		else {
			IFS_RET(DelSchedule());
			if (enable) {
				IFS_RET(SetRegRun());
			}
			else {
				IFS_RET(DelRegRun());
			}
		}
		RETURN_SUCCESS;
	};
	return func() == TStatus::SW_ERR_SUCCESS;
}



