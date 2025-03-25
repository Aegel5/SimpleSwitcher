#pragma once
#include <Shellapi.h>
#include <atlbase.h>
#include <Shlobj.h>

namespace procstart
{
	namespace internal {
		inline HRESULT FindDesktopFolderView(REFIID riid, void** ppv)
		{
			HRESULT hr;
			CComPtr<IShellWindows> spShellWindows;
			hr = spShellWindows.CoCreateInstance(CLSID_ShellWindows);
			if (FAILED(hr))
				return hr;

			CComVariant vtLoc{ 0 };    // 0 = CSIDL_DESKTOP
			CComVariant vtEmpty;
			long lhwnd = 0;
			CComPtr<IDispatch> spdisp;
			hr = spShellWindows->FindWindowSW(&vtLoc, &vtEmpty, SWC_DESKTOP, &lhwnd, SWFO_NEEDDISPATCH, &spdisp);
			if (FAILED(hr))
				return hr;

			CComQIPtr<IServiceProvider> spProv{ spdisp };
			if (!spProv)
				return E_NOINTERFACE;

			CComPtr<IShellBrowser> spBrowser;
			hr = spProv->QueryService(SID_STopLevelBrowser, IID_PPV_ARGS(&spBrowser));
			if (FAILED(hr))
				return hr;

			CComPtr<IShellView> spView;
			hr = spBrowser->QueryActiveShellView(&spView);
			if (FAILED(hr))
				return hr;

			return spView->QueryInterface(riid, ppv);
		}


		inline HRESULT GetDesktopAutomationObject(REFIID riid, void** ppv)
		{
			HRESULT hr;
			CComPtr<IShellView> spsv;
			hr = FindDesktopFolderView(IID_PPV_ARGS(&spsv));
			if (FAILED(hr))
				return hr;
			if (!spsv)
				return E_NOINTERFACE;

			CComPtr<IDispatch> spdispView;
			hr = spsv->GetItemObject(SVGIO_BACKGROUND, IID_PPV_ARGS(&spdispView));
			if (FAILED(hr))
				return hr;

			return spdispView->QueryInterface(riid, ppv);
		}


		inline HRESULT ShellExecuteFromExplorer(PCWSTR pszFile, PCWSTR pszParameters, PCWSTR pszDirectory, PCWSTR pszOperation, int nShowCmd)
		{
			HRESULT hr;
			CComPtr<IShellFolderViewDual> spFolderView;
			hr = GetDesktopAutomationObject(IID_PPV_ARGS(&spFolderView));
			if (FAILED(hr))
				return hr;

			CComPtr<IDispatch> spdispShell;
			hr = spFolderView->get_Application(&spdispShell);
			if (FAILED(hr))
				return hr;

			CComQIPtr<IShellDispatch2> spdispShell2{ spdispShell };
			if (!spdispShell2)
				return E_NOINTERFACE;

			// without this, the launched app is not moved to the foreground
			AllowSetForegroundWindow(ASFW_ANY);

			return spdispShell2->ShellExecute(
				CComBSTR{ pszFile },
				CComVariant{ pszParameters ? pszParameters : L"" },
				CComVariant{ pszDirectory ? pszDirectory : L"" },
				CComVariant{ pszOperation ? pszOperation : L"" },
				CComVariant{ nShowCmd });
		}
	}

	enum CreateProcFunc
	{
		SW_CREATEPROC_DEFAULT = 0,
		SW_CREATEPROC_AS_USER,
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

	inline TStatus SwCreateProcess(CreateProcessParm& parm, CAutoHandle& hProc)
	{
		static const size_t nSizeExe = 0x1000;
		std::unique_ptr<TChar[]> bufExe(new TChar[nSizeExe]);
		if(!bufExe.get()) return SW_ERR_NO_MEMORY;
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

		LOG_ANY(L"Try create new process path={}, args={}, mode={}", sExe, args, (int)parm.mode);

		if (selfElevated && parm.admin == SW_ADMIN_OFF) {

			if (parm.mode != SW_CREATEPROC_SHELLEXE)
				return SW_ERR_UNSUPPORTED;

			IFH_RET(internal::ShellExecuteFromExplorer(sExe, args, NULL, NULL, 0));

			RETURN_SUCCESS;
		}


		if (parm.mode == SW_CREATEPROC_SHELLEXE)
		{
			SHELLEXECUTEINFO shExInfo{};
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
			RETURN_SUCCESS;
		}


		STARTUPINFO         siStartupInfo;
		PROCESS_INFORMATION piProcessInfo;

		ZeroMemory(&siStartupInfo, sizeof(siStartupInfo));
		ZeroMemory(&piProcessInfo, sizeof(piProcessInfo));

		siStartupInfo.cb = sizeof(siStartupInfo);
		if (parm.mode == SW_CREATEPROC_AS_USER)	{

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
		else 
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

		IFW_RET(Res, L"Cant create proc %s %s", sExe, args);

		CloseHandle(piProcessInfo.hThread);
		hProc = piProcessInfo.hProcess;

		RETURN_SUCCESS;
	}
}

