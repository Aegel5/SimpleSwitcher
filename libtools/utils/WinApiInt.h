#pragma once
#include <winternl.h>

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

#define __GETPROC_NTDLL(FuncName) _GetProcAddress(#FuncName, "ntdll.dll", FuncName);
#define __GETPROC_Advapi32(FuncName) _GetProcAddress(#FuncName, "Advapi32.dll", FuncName);
#define __GETPROC_User32(FuncName) _GetProcAddress(#FuncName, "User32.dll", FuncName);
#define __GETPROC_Kernel32(FuncName) _GetProcAddress(#FuncName, "Kernel32.dll", FuncName);

	BOOL
	(NTAPI * const CreateProcessWithTokenW)
		(
			_In_        HANDLE hToken,
			_In_        DWORD dwLogonFlags,
			_In_opt_    LPCWSTR lpApplicationName,
			_Inout_opt_ LPWSTR lpCommandLine,
			_In_        DWORD dwCreationFlags,
			_In_opt_    LPVOID lpEnvironment,
			_In_opt_    LPCWSTR lpCurrentDirectory,
			_In_        LPSTARTUPINFOW lpStartupInfo,
			_Out_       LPPROCESS_INFORMATION lpProcessInformation
			) = __GETPROC_Advapi32(CreateProcessWithTokenW);

	BOOL
	(NTAPI * const AddClipboardFormatListener)
		(_In_ HWND hwnd) = __GETPROC_User32(AddClipboardFormatListener);

	NTSTATUS
	(NTAPI * const NtQuerySystemInformation)
		(
			_In_       SYSTEM_INFORMATION_CLASS SystemInformationClass,
			_Inout_    PVOID SystemInformation,
			_In_       ULONG SystemInformationLength,
			_Out_opt_  PULONG ReturnLength
			) = __GETPROC_NTDLL(NtQuerySystemInformation);

	BOOL
	(WINAPI * const ChangeWindowMessageFilterEx)
		(
			_In_         HWND hWnd,
			_In_         UINT message,
			_In_         DWORD action,
			_Inout_opt_  PCHANGEFILTERSTRUCT pChangeFilterStruct
			) = __GETPROC_User32(ChangeWindowMessageFilterEx);

	HHOOK
	(WINAPI * const SetWindowsHookEx)
		(
			_In_  int idHook,
			_In_  HOOKPROC lpfn,
			_In_  HINSTANCE hMod,
			_In_  DWORD dwThreadId
			) = __GETPROC_User32(SetWindowsHookExW);

	HWINEVENTHOOK
	(WINAPI * const SetWinEventHook)
		(
			_In_  UINT eventMin,
			_In_  UINT eventMax,
			_In_  HMODULE hmodWinEventProc,
			_In_  WINEVENTPROC lpfnWinEventProc,
			_In_  DWORD idProcess,
			_In_  DWORD idThread,
			_In_  UINT dwflags
			) = __GETPROC_User32(SetWinEventHook);

	int (WINAPI * const GetLocaleInfoEx)
		(
			_In_opt_  LPCWSTR lpLocaleName,
			_In_      LCTYPE  LCType,
			_Out_opt_ LPWSTR  lpLCData,
			_In_      int     cchData
			) = __GETPROC_Kernel32(GetLocaleInfoEx);

}