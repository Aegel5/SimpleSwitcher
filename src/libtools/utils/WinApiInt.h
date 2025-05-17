#pragma once
#include <winternl.h>
#include <shellscalingapi.h>

namespace WinApiInt
{
	template<typename TFuncPtr>
	TFuncPtr _GetProcAddress(const char* sFnName, const char* sDllName, TFuncPtr)
	{
		HMODULE hDll = GetModuleHandleA(sDllName);
		if (hDll == NULL)
		{
			UINT oldErrMode = SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOGPFAULTERRORBOX | SEM_NOOPENFILEERRORBOX);
			hDll = LoadLibraryA(sDllName);
			SetErrorMode(oldErrMode);
		}
		if (hDll == NULL) return NULL;
		return (TFuncPtr)GetProcAddress(hDll, sFnName);
	}

#define __GETPROC_Shcore(FuncName) _GetProcAddress(#FuncName, "Shcore.dll", FuncName);

	HRESULT
	(NTAPI* const GetDpiForMonitor)
		(
			HMONITOR         hmonitor,
			MONITOR_DPI_TYPE dpiType,
			UINT* dpiX,
			UINT* dpiY
			) = __GETPROC_Shcore(GetDpiForMonitor);



}
