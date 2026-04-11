#pragma once



namespace Utils
{
	template <typename T> inline void RemoveAt(T& cont, int i) {
		cont.erase(cont.begin() + i);
	}

	template <typename T> inline void RemoveFirst(T& v, auto&& element_to_remove) {
		auto it = std::find(v.begin(), v.end(), element_to_remove);
		// Если нашли — удаляем
		if (it != v.end()) {
			v.erase(it);
		}
	}

	inline TStatus IsElevated(HANDLE hProc, bool& res)
	{
		res = false;

		if (!IsWindowsVistaOrGreater())
			RETURN_SUCCESS;

		if (!hProc)
			return SW_ERR_INVALID_PARAMETR;

		CAutoHandle hToken;
		IFW_RET(OpenProcessToken(hProc, TOKEN_QUERY, &hToken));

		TOKEN_ELEVATION Elevation;
		DWORD cbSize = sizeof(TOKEN_ELEVATION);
		IFW_RET(GetTokenInformation(hToken, TokenElevation, &Elevation, sizeof(Elevation), &cbSize));
		res = Elevation.TokenIsElevated != 0;

		RETURN_SUCCESS;
	}

	inline bool IsElevated(HANDLE hProc)
	{
		bool fRet;
		IFS_LOG(IsElevated(hProc, fRet));

		return fRet;
	}

	inline bool IsElevatedByPid(DWORD procId)
	{
		if (!IsWindowsVistaOrGreater())
			return false;

		CAutoHandle hProc = OpenProcess(
			PROCESS_QUERY_LIMITED_INFORMATION,
			FALSE,
			procId);
		if (hProc.IsValid())
			return IsElevated(hProc);
		else
		{
			IFW_LOG(FALSE);
			return false;
		}
	}

	consteval inline bool IsDebug() {
#ifdef _DEBUG
		return true;
#endif
		return false;
	}

	inline bool IsSelfElevated()
	{
		return IsElevated(GetCurrentProcess());
	}

	inline bool ProcSingleton(const TChar* mtxName)
	{
		HANDLE mtxGui = CreateMutex(NULL, FALSE, mtxName);
		if (GetLastError() == ERROR_ALREADY_EXISTS)
		{
			return true;
		}
		else if (GetLastError() != ERROR_SUCCESS)
		{
			IFW_LOG(FALSE);
		}

		return false;
	}

	inline TStatus GetProcLowerNameByPid(DWORD pid, std::wstring& sPath, std::wstring& sName)
	{
		sPath.clear();
		sName.clear();
		if (pid == 0) 
			return SW_ERR_INVALID_PARAMETR;
		CAutoHandle hProc = OpenProcess(IsWindowsVistaOrGreater() ? PROCESS_QUERY_LIMITED_INFORMATION : PROCESS_QUERY_INFORMATION, FALSE, pid);
		IFW_RET(hProc.IsValid());
		TCHAR sBuff[0x1000];
		IFW_RET(GetProcessImageFileName(hProc, sBuff, ARRAYSIZE(sBuff)) > 0);
		sPath = sBuff;
		StrUtils::ToLower(sPath);
		auto last = wcsrchr(sBuff, L'\\');
		if (last == NULL)
		{
			return SW_ERR_INVALID_PARAMETR;
		}
		sName = last + 1;
		StrUtils::ToLower(sName);

		RETURN_SUCCESS;
	}

    bool is_in(auto&& first, auto&&... t) { return ((first == t) || ...); }
    bool is_all(auto&& first, auto&&... t) { return ((first == t) && ...); }

}






