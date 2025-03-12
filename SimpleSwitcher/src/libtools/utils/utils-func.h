#pragma once

#include "Msctf.h"
#include "atlbase.h"

struct TopWndInfo {

	HKL lay = 0;

	HWND hwnd_default = nullptr;
	DWORD threadid_default = 0;
	DWORD pid_default = 0;

	HWND hwnd_top = nullptr;
	DWORD threadid_top = 0;
	DWORD pid_top = 0;
};

namespace Utils
{
	template <typename T> inline void RemoveAt(T& cont, int i) {
		cont.erase(cont.begin() + i);
	}
	inline std::wstring GetNameForHKL_simple(HKL hkl)
	{
		WORD langid = LOWORD(hkl);

		TCHAR buf[512];
		buf[0] = 0;

		int flag = IsWindowsVistaOrGreater() ? LOCALE_SNAME : LOCALE_SLANGUAGE;
		int len = GetLocaleInfo(MAKELCID(langid, SORT_DEFAULT), flag, buf, SW_ARRAY_SIZE(buf));
		IFW_LOG(len != 0);

		if (len == 0) {
			return L"Unknown";
		}

		return buf;
	}

	inline std::wstring GetNameForHKL_Unique(HKL hkl)
	{
		std::wstringstream stream;
		stream << GetNameForHKL_simple(hkl) << " (0x" << std::hex << TUInt64(hkl) << ")";
		return stream.str();
	}

	inline std::wstring GetNameForHKL(HKL hkl)
	{
		return GetNameForHKL_Unique(hkl);
	}



	inline TStatus IsElevated(HANDLE hProc, bool& res)
	{
		res = false;

		if (!IsWindowsVistaOrGreater())
			RETURN_SUCCESS;

		if (!hProc)
			RETS(SW_ERR_INVALID_PARAMETR);

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

	inline bool IsDebug() {
#ifdef _DEBUG
		return true;
#endif
		return false;
	}

	inline bool IsSelfElevated()
	{
		return IsElevated(GetCurrentProcess());
	}

	inline TStatus GetFocusWindowByAttach(HWND& hwnd, HWND top)
	{
		DWORD procId;
		DWORD activeThreadId = GetWindowThreadProcessId(top, &procId);
		IFW_RET(AttachThreadInput(activeThreadId, GetCurrentThreadId(), TRUE));
		hwnd = GetFocus();
		IFW_RET(AttachThreadInput(activeThreadId, GetCurrentThreadId(), FALSE));

		RETURN_SUCCESS;
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
			RETS(SW_ERR_INVALID_PARAMETR);
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

			IFNEG(swprintf_s(sResPath, L"\\StringFileInfo\\%04x%04x\\ProductName",
				lpTranslate[i].wLanguage,
				lpTranslate[i].wCodePage))
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

			LOG_INFO_2(L"productName is %s", sInfo);

			Str_Utils::ToLower(sProdName);
			if (sProdName.find(sStr) != std::wstring::npos)
			{
				fRes = true;
			}
			int k = 0;
		}


		RETURN_SUCCESS;
	}

	inline TStatus GetFocusWindow(HWND& hwndFocused)
	{
		hwndFocused = NULL;
		GUITHREADINFO gui {};
		gui.cbSize = sizeof(gui);
		IFW_LOG(GetGUIThreadInfo(0, &gui));

		hwndFocused = gui.hwndFocus;

		if (hwndFocused == NULL)
			hwndFocused = GetForegroundWindow();

		if (hwndFocused == NULL)
		{
			RETS(SW_ERR_WND_NOT_FOUND, L"Cant found focused window");
		}

		RETURN_SUCCESS;

	}

	inline void SetLayPost(HWND hwnd, HKL lay) {
		LOG_ANY(L"post WM_INPUTLANGCHANGEREQUEST {:x}", (ULONGLONG)lay);
		PostMessage(hwnd, WM_INPUTLANGCHANGEREQUEST, 0, (LPARAM)lay);
	}

	inline TStatus SetLayByCom(HKL lay) {

		CComPtr<ITfInputProcessorProfileMgr> pProfile;
		CComPtr<ITfInputProcessorProfiles> pProfile2;


		IFH_RET(CoCreateInstance(CLSID_TF_InputProcessorProfiles,
			NULL,
			CLSCTX_INPROC_SERVER,
			IID_ITfInputProcessorProfileMgr,
			(LPVOID*)&pProfile));

		IFH_RET(CoCreateInstance(CLSID_TF_InputProcessorProfiles,
			NULL,
			CLSCTX_INPROC_SERVER,
			IID_ITfInputProcessorProfiles,
			(LPVOID*)&pProfile2));

		TF_INPUTPROCESSORPROFILE profile = {};
		IFH_RET(pProfile->GetActiveProfile(GUID_TFCAT_TIP_KEYBOARD, &profile));

		LANGID lang;
		IFH_RET(pProfile2->GetCurrentLanguage(&lang));

		CComPtr<IEnumTfInputProcessorProfiles> profs_enum;
		ULONG geted;
		IFH_RET(pProfile->EnumProfiles(0, &profs_enum));
		TF_INPUTPROCESSORPROFILE profs[100] = { 0 };
		IFH_RET(profs_enum->Next(100, profs, &geted));
		for (auto i = 0; i < geted; i++) {
			auto& cur = profs[i];
			if (cur.dwProfileType != TF_PROFILETYPE_KEYBOARDLAYOUT) continue;
			if (cur.hkl == lay) {

				IFH_RET(pProfile->ActivateProfile(TF_PROFILETYPE_KEYBOARDLAYOUT, cur.langid, CLSID_NULL, GUID_NULL, cur.hkl, TF_IPPMF_ENABLEPROFILE | TF_IPPMF_FORSESSION));
				//IFH_RET(pProfile2->ChangeCurrentLanguage(cur.langid));
				LOG_ANY(L"switch ok to {}", (int)lay);
				RETURN_SUCCESS;
			}
		}



		RETURN_SUCCESS;
	}

	inline TopWndInfo GetFocusedWndInfo() {

		TopWndInfo res;

		IFS_LOG(Utils::GetFocusWindow(res.hwnd_top));

		if (res.hwnd_top == nullptr)
			return res;

		res.hwnd_default = ImmGetDefaultIMEWnd(res.hwnd_top);

		res.threadid_default = GetWindowThreadProcessId(res.hwnd_default, &res.pid_default);
		IFW_LOG(res.threadid_default != 0);

		res.threadid_top = GetWindowThreadProcessId(res.hwnd_top, &res.pid_top);
		IFW_LOG(res.threadid_top != 0);

		res.lay = GetKeyboardLayout(res.threadid_default);

		return res;
	}

	//inline TStatus CenterWindow(HWND hwndMain, HWND hwndClient)
	//{
	//	RECT rect;
	//	IFW_RET(GetWindowRect(hwndClient, &rect));
	//	int wDlg = rect.right - rect.left;
	//	int hDlg = rect.bottom - rect.top;
	//	IFW_RET(GetWindowRect(hwndMain, &rect));
	//	int wMain = rect.right - rect.left;
	//	int hMain = rect.bottom - rect.top;
	//	int widthMain = rect.right - rect.left;
	//	int x = (wMain - wDlg) / 2;
	//	int y = (hMain - hDlg) / 2;
	//	SetWindowPos(hwndClient, 0, x + rect.left, y + rect.top, 0, 0, SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE);
	//	RETURN_SUCCESS;
	//}



	//inline TUInt64 GetBootTime()
	//{
	//	typedef struct _SYSTEM_TIME_OF_DAY_INFORMATION
	//	{
	//		LARGE_INTEGER BootTime;
	//		LARGE_INTEGER CurrentTime;
	//		LARGE_INTEGER TimeZoneBias;
	//		ULONG CurrentTimeZoneId;
	//	} SYSTEM_TIME_OF_DAY_INFORMATION, *PSYSTEM_TIME_OF_DAY_INFORMATION;

	//	SYSTEM_TIME_OF_DAY_INFORMATION SysTimeInfo;

	//	TUInt64 res = 0;

	//	__try
	//	{

	//		NTSTATUS stat = WinApiInt::NtQuerySystemInformation(
	//			SystemTimeOfDayInformation,
	//			&SysTimeInfo,
	//			sizeof(SysTimeInfo),
	//			0);
	//		if (NT_SUCCESS(stat))
	//		{
	//			res = SysTimeInfo.BootTime.QuadPart;
	//		}
	//	}
	//	__except (EXCEPTION_EXECUTE_HANDLER)
	//	{

	//	}
	//	return res;
	//}

    template <typename First, typename... T> bool is_in(First&& first, T&&... t) { return ((first == t) || ...); }
}






