inline bool IsWindows64() {
	BOOL bIsWow64 = FALSE;

	typedef BOOL(APIENTRY* LPFN_ISWOW64PROCESS)
		(HANDLE, PBOOL);

	LPFN_ISWOW64PROCESS fnIsWow64Process;

	HMODULE module = GetModuleHandleA("kernel32");
	if (!module) {
		return bIsWow64 != FALSE;
	}
	const char funcName[] = "IsWow64Process";
	fnIsWow64Process = (LPFN_ISWOW64PROCESS)GetProcAddress(module, funcName);

	if (NULL != fnIsWow64Process) {
		if (!fnIsWow64Process(GetCurrentProcess(), &bIsWow64)) {
			return false;
		}
	}
	return bIsWow64 != FALSE;
}
