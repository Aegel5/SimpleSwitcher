#pragma once



namespace Utils
{
	template <typename T> inline void RemoveAt(T& cont, int i) {
		cont.erase(cont.begin() + i);
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
		Str_Utils::ToLower(sPath);
		auto last = wcsrchr(sBuff, L'\\');
		if (last == NULL)
		{
			return SW_ERR_INVALID_PARAMETR;
		}
		sName = last + 1;
		Str_Utils::ToLower(sName);

		RETURN_SUCCESS;
	}

	inline TStatus IsProductHas(const TChar* sPath, const TChar* sStr, bool& fRes)
	{
		fRes = false;

		// allocate a block of memory for the version info
		DWORD dummy;
		DWORD dwSize = GetFileVersionInfoSize(sPath, &dummy);
		IFW_RET(dwSize != 0);


		std::vector<BYTE> data(dwSize);
		IFW_RET(GetFileVersionInfo(sPath, NULL, dwSize, &data[0]));

		struct LANGANDCODEPAGE {
			WORD wLanguage;
			WORD wCodePage;
		} *lpTranslate;

		UINT cbTranslate = 0;

		IFW_RET(VerQueryValue(
			&data[0],
			TEXT("\\VarFileInfo\\Translation"),
			(LPVOID*)&lpTranslate,
			&cbTranslate));

		LPVOID pvProductName = NULL;
		unsigned int iProductNameLen = 0;

		for (UINT i = 0; i < (cbTranslate / sizeof(struct LANGANDCODEPAGE)); i++)
		{
			TChar sResPath[0x100];
			sResPath[0] = 0;

			if(swprintf_s(sResPath, L"\\StringFileInfo\\%04x%04x\\ProductName",
				lpTranslate[i].wLanguage,
				lpTranslate[i].wCodePage) < 0)
				RET_ERRNO();

			// Retrieve file description for language and code page "i". 
			VerQueryValue(
				&data[0],
				sResPath,
				&pvProductName,
				&iProductNameLen);

			TCHAR* sInfo = (TCHAR*)pvProductName;
			std::wstring sProdName;
			sProdName = sInfo;

			Str_Utils::ToLower(sProdName);
			if (sProdName.find(sStr) != std::wstring::npos)
			{
				fRes = true;
			}
			int k = 0;
		}


		RETURN_SUCCESS;
	}

    bool is_in(auto&& first, auto&&... t) { return ((first == t) || ...); }
    bool is_all(auto&& first, auto&&... t) { return ((first == t) && ...); }
}






