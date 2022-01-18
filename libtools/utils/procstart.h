#pragma once
#include <Shellapi.h>

namespace procstart
{
	enum CreateProcFunc
	{
		SW_CREATEPROC_DEFAULT = 0,

		SW_CREATEPROC_NORMAL,
		SW_CREATEPROC_AS_USER,
		SW_CREATEPROC_TOKEN,
		SW_CREATEPROC_SHELLEXE,
	};

	struct CreateProcessParm
	{
		CreateProcFunc mode = SW_CREATEPROC_DEFAULT;
		TStr sExe = nullptr;
		TStr sCmd = nullptr;
		TSWAdmin admin = SW_ADMIN_SELF;
		HANDLE hToken = nullptr;
		bool isHide = false;
	};

	// not worked
	inline TStatus GetUnElevatedToken(CAutoHandle& hToken)
	{
		HWND hShellWnd = GetShellWindow();
		IFNULL(hShellWnd)
		{
			RETS(SW_ERR_WND_NOT_FOUND, L"GetShellWindow() return NULL");
		}

		DWORD dwShellPID = 0;
		GetWindowThreadProcessId(hShellWnd, &dwShellPID);
		IFW_RET(dwShellPID != 0);

		CAutoHandle hShellProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, dwShellPID);
		IFW_RET(hShellProcess.IsValid());

		CAutoHandle hShellProcessToken;

		IFW_RET(OpenProcessToken(hShellProcess, TOKEN_DUPLICATE, &hShellProcessToken));

		const DWORD dwTokenRights = TOKEN_QUERY | TOKEN_ASSIGN_PRIMARY | TOKEN_DUPLICATE | TOKEN_ADJUST_DEFAULT | TOKEN_ADJUST_SESSIONID;
		IFW_RET(DuplicateTokenEx(hShellProcessToken, dwTokenRights, NULL, SecurityImpersonation, TokenPrimary, &hToken));

		RETURN_SUCCESS;
	}

	inline TStatus SwCreateProcess(CreateProcessParm& parm, CAutoHandle& hProc)
	{
		static const size_t nSizeExe = 0x1000;
		std::unique_ptr<TChar[]> bufExe(new TChar[nSizeExe]);
		IFNULL(bufExe)
			RETS(SW_ERR_NO_MEMORY);
		bufExe[0] = 0;
		TChar* sExe = bufExe.get();


		IF_ERRNO_RET(wcscpy_s(sExe, nSizeExe, parm.sExe));

		TCHAR args[0x1000];
		args[0] = 0;

		if (parm.sCmd)
		{
			IF_ERRNO_RET(wcscpy_s(args, L" "));
			IF_ERRNO_RET(wcscat_s(args, parm.sCmd));
		}


		BOOL Res = FALSE;


		bool selfElevated = Utils::IsSelfElevated();
		CAutoHandle hToken;

		LOG_INFO_1(L"Try create new process path=%s, args=%s, mode=%u", sExe, args, parm.mode);

		if (parm.mode == SW_CREATEPROC_DEFAULT)
		{
			if (IsWindowsVistaOrGreater())
			{
				if (parm.admin == SW_ADMIN_ON && !selfElevated)
				{
					parm.mode = SW_CREATEPROC_SHELLEXE;
				}
				//else if (parm.admin == SW_ADMIN_OFF && selfElevated)
				//{
				//	TStatus stat = GetUnElevatedToken(hToken);
				//	if (SW_SUCCESS(stat))
				//	{
				//		parm.hToken = hToken;
				//		parm.mode = SW_CREATEPROC_TOKEN;
				//	}
				//	else
				//	{
				//		IFS_LOG(stat);
				//	}
				//}
			}
		}

		if (parm.mode == SW_CREATEPROC_DEFAULT)
		{
			parm.mode = SW_CREATEPROC_NORMAL;
		}

		if (parm.mode == SW_CREATEPROC_SHELLEXE)
		{
			SHELLEXECUTEINFO shExInfo = { 0 };
			shExInfo.cbSize = sizeof(shExInfo);
			shExInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
			shExInfo.hwnd = 0;
			shExInfo.lpVerb = (parm.admin == SW_ADMIN_ON) ? _T("runas") : 0;
			shExInfo.lpFile = sExe;
			shExInfo.lpParameters = args;
			shExInfo.lpDirectory = 0;
			shExInfo.nShow = parm.isHide ? SW_HIDE : SW_SHOW;
			shExInfo.hInstApp = 0;

			IFW_RET(ShellExecuteEx(&shExInfo) == TRUE);

			hProc = shExInfo.hProcess;
		}
		else if (parm.mode == SW_CREATEPROC_NORMAL || parm.mode == SW_CREATEPROC_AS_USER || parm.mode == SW_CREATEPROC_TOKEN)
		{
			STARTUPINFO         siStartupInfo;
			PROCESS_INFORMATION piProcessInfo;

			ZeroMemory(&siStartupInfo, sizeof(siStartupInfo));
			ZeroMemory(&piProcessInfo, sizeof(piProcessInfo));

			siStartupInfo.cb = sizeof(siStartupInfo);
			if (parm.mode == SW_CREATEPROC_AS_USER)
			{

				Res = CreateProcessAsUser(
					parm.hToken,
					sExe,
					args,
					0,
					0,
					FALSE,
					CREATE_DEFAULT_ERROR_MODE,
					0,
					0,
					&siStartupInfo,
					&piProcessInfo);
			}
			else if (parm.mode == SW_CREATEPROC_NORMAL)
			{
				Res = CreateProcess(
					sExe,
					args,
					0,
					0,
					FALSE,
					CREATE_DEFAULT_ERROR_MODE,
					0,
					0,
					&siStartupInfo,
					&piProcessInfo);
			}
			else if (parm.mode == SW_CREATEPROC_TOKEN)
			{
				Res = WinApiInt::CreateProcessWithTokenW(
					parm.hToken,
					0,
					sExe,
					args,
					0,
					NULL,
					NULL,
					&siStartupInfo,
					&piProcessInfo);
			}
			IFW_RET(Res, L"Cant create proc %s %s", sExe, args);

			CloseHandle(piProcessInfo.hThread);
			hProc = piProcessInfo.hProcess;

		}
		RETURN_SUCCESS;
	}
}

