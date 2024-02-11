#pragma once

inline HMODULE GetCurrentModule()
{
	// NB: XP+ solution!
	HMODULE hModule = NULL;
	GetModuleHandleEx(
		GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS,
		(LPCTSTR)GetCurrentModule,
		&hModule);

	return hModule;
}

inline bool IsWindowsVersionOrGreater(WORD wMajorVersion, WORD wMinorVersion, WORD wServicePackMajor)
{
	OSVERSIONINFOEXW osvi = { sizeof(osvi), 0, 0, 0, 0,{ 0 }, 0, 0 };
	DWORDLONG        const dwlConditionMask = VerSetConditionMask(
		VerSetConditionMask(
			VerSetConditionMask(
				0, VER_MAJORVERSION, VER_GREATER_EQUAL),
			VER_MINORVERSION, VER_GREATER_EQUAL),
		VER_SERVICEPACKMAJOR, VER_GREATER_EQUAL);

	osvi.dwMajorVersion = wMajorVersion;
	osvi.dwMinorVersion = wMinorVersion;
	osvi.wServicePackMajor = wServicePackMajor;

	return VerifyVersionInfoW(&osvi, VER_MAJORVERSION | VER_MINORVERSION | VER_SERVICEPACKMAJOR, dwlConditionMask) != FALSE;
}

inline bool
IsWindowsVistaOrGreater()
{
	return IsWindowsVersionOrGreater(HIBYTE(_WIN32_WINNT_VISTA), LOBYTE(_WIN32_WINNT_VISTA), 0);
}

#define ____WIN32_WINNT_WIN10                  0x0A00
inline bool
IsWindows10OrGreater()
{

	//OSVERSIONINFO osvi;
	//ZeroMemory(&osvi, sizeof(OSVERSIONINFO));
	//osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	//GetVersionEx(&osvi);
	//return osvi.dwMajorVersion >= 10;

	return IsWindowsVersionOrGreater(HIBYTE(____WIN32_WINNT_WIN10), LOBYTE(____WIN32_WINNT_WIN10), 0);
}

//inline bool IsWindows10OrGreater2()
//{
//	OSVERSIONINFO osvi;
//	ZeroMemory(&osvi, sizeof(OSVERSIONINFO));
//	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
//	GetVersionEx(&osvi);
//
//	if (osvi.dwMajorVersion == 6)
//	{
//		if (osvi.dwMinorVersion > 3)
//		{
//			return true;
//		}
//	}
//	else if (osvi.dwMajorVersion > 6)
//	{
//		return true;
//	}
//
//	return false;
//}


inline bool IsWindows64()
{
	BOOL bIsWow64 = FALSE;

	typedef BOOL(APIENTRY *LPFN_ISWOW64PROCESS)
		(HANDLE, PBOOL);

	LPFN_ISWOW64PROCESS fnIsWow64Process;

	HMODULE module = GetModuleHandleA("kernel32");
	if (!module)
	{
		return bIsWow64 != FALSE;
	}
	const char funcName[] = "IsWow64Process";
	fnIsWow64Process = (LPFN_ISWOW64PROCESS)GetProcAddress(module, funcName);

	if (NULL != fnIsWow64Process)
	{
		if (!fnIsWow64Process(GetCurrentProcess(), &bIsWow64))
		{
			return false;
		}
	}
	return bIsWow64 != FALSE;
}
