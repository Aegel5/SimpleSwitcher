#pragma once

#include <taskschd.h>
#pragma comment(lib, "taskschd.lib")

#include "atlbase.h"

#include "cmn.h"
#include "../utils/COM_utils.h"
#include "../utils/procstart.h"

namespace Startup
{
	struct CheckTaskSheduleParm
	{
		TStr taskName = NULL;
		TStr sPath = NULL;
		TStr sArgs = NULL;
		bool isTaskExists = false;
		bool isPathEqual = false;
		bool isSettingsCorrect = false;
	};

	inline TStatus CheckTaskShedule(CheckTaskSheduleParm& parm)
	{
		if (!IsWindowsVistaOrGreater())
			RETURN_SUCCESS;

		CComPtr<IRegisteredTask> pTask;
		CComPtr<ITaskDefinition> pTaskDef;

		// check task exists
		{

			CComPtr<ITaskService> pService;

			IFH_RET(CoCreateInstance(
				CLSID_TaskScheduler,
				NULL,
				CLSCTX_INPROC_SERVER,
				IID_ITaskService,
				(void**)&pService));

			IFH_RET(pService->Connect(
				_variant_t(),
				_variant_t(),
				_variant_t(),
				_variant_t()));

			CComPtr<ITaskFolder> pRootFolder;

			IFH_RET(pService->GetFolder(_bstr_t(L"\\"), &pRootFolder));


			HRESULT res = pRootFolder->GetTask(_bstr_t(parm.taskName), &pTask);
			if (res != S_OK)
				RETURN_SUCCESS;

			IFH_RET(pTask->get_Definition(&pTaskDef));

			parm.isTaskExists = true;
		}


		// check path
		{
			CComPtr<IActionCollection> actions;
			IFH_RET(pTaskDef->get_Actions(&actions));

			long count = 0;
			IFH_RET(actions->get_Count(&count));

			if (count != 1)
			{
				IFS_RET(SW_ERR_UNKNOWN, L"Wrong count=%u", count);
			}


			CComPtr<IAction> act;
			IFH_RET(actions->get_Item(1, &act));

			//TASK_ACTION_TYPE actType;
			//SW_HRESULT_RET(act->get_Type(&actType));

			CComPtr<IExecAction> actExec;
			IFH_RET(act->QueryInterface(&actExec));

			CAuto_Bstr bsPath;
			IFH_RET(actExec->get_Path(&bsPath));

			CAuto_Bstr bsArgs;
			IFH_RET(actExec->get_Arguments(&bsArgs));

			std::wstring actPath = bsPath;
			Str_Utils::trim(actPath, L" \"");

			bool resPath = _wcsicmp(actPath.c_str(), parm.sPath) == 0;
			bool resArgs = false;
			if (bsArgs)
			{
				resArgs = _wcsicmp(bsArgs.get(), parm.sArgs) == 0;
			}

			parm.isPathEqual = (resPath && resArgs);
		}

		// check settings
		{
			CComPtr<ITaskSettings> pSettings;
			IFH_RET(pTaskDef->get_Settings(&pSettings));

			VARIANT_BOOL fDisallowStartIfOnBatteries;
			IFH_RET(pSettings->get_DisallowStartIfOnBatteries(&fDisallowStartIfOnBatteries));

			VARIANT_BOOL fStopIfGoingOnBatteries;
			IFH_RET(pSettings->get_DisallowStartIfOnBatteries(&fStopIfGoingOnBatteries));

			CAuto_Bstr bsTimeLimit;
			IFH_RET(pSettings->get_ExecutionTimeLimit(&bsTimeLimit));

			int priority;
			IFH_RET(pSettings->get_Priority(&priority));

			if (
				priority == 6
				&& fDisallowStartIfOnBatteries == VARIANT_FALSE
				&& fStopIfGoingOnBatteries == VARIANT_FALSE
				&& Str_Utils::IsEqualCI(bsTimeLimit, L"PT0S"))
			{
				parm.isSettingsCorrect = true;
			}
		}

		RETURN_SUCCESS;
	}
	inline TStatus RemoveTaskShedule(TStr sTaskName)
	{
		CComPtr<ITaskService> pService;

		IFH_RET(CoCreateInstance(
			CLSID_TaskScheduler,
			NULL,
			CLSCTX_INPROC_SERVER,
			IID_ITaskService,
			(void**)&pService));

		IFH_RET(pService->Connect(
			_variant_t(),
			_variant_t(),
			_variant_t(),
			_variant_t()));

		CComPtr<ITaskFolder> pRootFolder;

		IFH_RET(pService->GetFolder(_bstr_t(L"\\"), &pRootFolder));

		IFH_RET(pRootFolder->DeleteTask(_bstr_t(sTaskName), 0));

		RETURN_SUCCESS;
	}

	struct CreateTaskSheduleParm
	{
		TStr taskName = NULL;
		TStr sPath = NULL;
		TStr sArgs = NULL;
		bool removeOld = true;
		bool fAsAdmin = true;
	};
	inline TStatus CreateTaskShedule2(CreateTaskSheduleParm& parm)
	{
		if (parm.taskName == nullptr || parm.sPath == nullptr)
		{
			RETS(SW_ERR_INVALID_PARAMETR);
		}

		CComPtr<ITaskFolder> pRootFolder;
		CComPtr<ITaskDefinition> pTask;

		// service
		{
			CComPtr<ITaskService> pService;

			IFH_RET(CoCreateInstance(
				CLSID_TaskScheduler,
				NULL,
				CLSCTX_INPROC_SERVER,
				IID_ITaskService,
				(void**)&pService));

			IFH_RET(pService->Connect(
				_variant_t(),
				_variant_t(),
				_variant_t(),
				_variant_t()));


			IFH_RET(pService->GetFolder(_bstr_t(L"\\"), &pRootFolder));

			if (parm.removeOld)
			{
				pRootFolder->DeleteTask(_bstr_t(parm.taskName), 0);
			}
			else
			{
				// TODO: error if exists
			}

			IFH_RET(pService->NewTask(0, &pTask));
		}
	
		// reginfo
		{
			CComPtr<IRegistrationInfo> pRegInfo;
			IFH_RET(pTask->get_RegistrationInfo(&pRegInfo));
			IFH_RET(pRegInfo->put_Author(L"12345"));
		}

		// principal
		{
			CComPtr<IPrincipal> pPrincipal;
			IFH_RET(pTask->get_Principal(&pPrincipal));
			IFH_RET(pPrincipal->put_LogonType(TASK_LOGON_INTERACTIVE_TOKEN));
			if (parm.fAsAdmin)
			{
				IFH_RET(pPrincipal->put_RunLevel(TASK_RUNLEVEL_HIGHEST));
			}
		}
	
		// settings
		{
			CComPtr<ITaskSettings> pSettings;
			IFH_RET(pTask->get_Settings(&pSettings));

			IFH_RET(pSettings->put_StartWhenAvailable(VARIANT_TRUE));
			IFH_RET(pSettings->put_AllowDemandStart(VARIANT_TRUE));

			IFH_RET(pSettings->put_DisallowStartIfOnBatteries(VARIANT_FALSE));
			IFH_RET(pSettings->put_StopIfGoingOnBatteries(VARIANT_FALSE));

			IFH_RET(pSettings->put_ExecutionTimeLimit(TEXT("PT0S")));
			IFH_RET(pSettings->put_AllowHardTerminate(VARIANT_FALSE));

			IFH_RET(pSettings->put_RunOnlyIfNetworkAvailable(VARIANT_FALSE));
			//SW_HRESULT_RET(pSettings->put_DeleteExpiredTaskAfter(NULL));
			//SW_HRESULT_RET(pSettings->put_RestartCount(0));

			IFH_RET(pSettings->put_Priority(6));
		}
	
		// triggers
		{
			CComPtr<ITriggerCollection> pTriggerCollection;
			IFH_RET(pTask->get_Triggers(&pTriggerCollection));

			CComPtr<ITrigger> pTrigger;
			IFH_RET(pTriggerCollection->Create(TASK_TRIGGER_LOGON, &pTrigger));
			pTriggerCollection.Release();

			CComPtr<ILogonTrigger> pLogonTrigger;
			IFH_RET(pTrigger->QueryInterface(
				IID_ILogonTrigger,
				(void**)&pLogonTrigger));
			pTrigger.Release();

			IFH_RET(pLogonTrigger->put_Id(_bstr_t(L"Trigger1")));
		}
	

		// actions
		{
			CComPtr<IActionCollection> pActionCollection;
			IFH_RET(pTask->get_Actions(&pActionCollection));

			CComPtr<IAction> pAction;
			IFH_RET(pActionCollection->Create(TASK_ACTION_EXEC, &pAction));

			CComPtr<IExecAction> pExecAction;
			IFH_RET(pAction->QueryInterface(
				IID_IExecAction, (void**)&pExecAction));

			IFH_RET(pExecAction->put_Path(_bstr_t(parm.sPath)));

			if (parm.sArgs)
			{
				IFH_RET(pExecAction->put_Arguments(_bstr_t(parm.sArgs)));
			}
		}
	
		// create task
		{
			CComPtr<IRegisteredTask> pRegisteredTask;
			IFH_RET(pRootFolder->RegisterTaskDefinition(
				_bstr_t(parm.taskName),
				pTask,
				TASK_CREATE,
				_variant_t(),
				_variant_t(),
				TASK_LOGON_INTERACTIVE_TOKEN,
				_variant_t(L""),
				&pRegisteredTask));
		}
	
		RETURN_SUCCESS;
	}
}