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

	inline std::wstring GetNameForHKL_simple(HKL hkl)
	{
		WORD langid = LOWORD(hkl);

		TCHAR buf[512];
		buf[0] = 0;

		int flag = IsWindowsVistaOrGreater() ? LOCALE_SNAME : LOCALE_SLANGUAGE;
		int len = GetLocaleInfo(MAKELCID(langid, SORT_DEFAULT), flag, buf, std::ssize(buf));
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
			IFS_RET(SW_ERR_WND_NOT_FOUND, L"Cant found focused window");
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

	inline TStatus FoundEmulateHotKey(CHotKey& key)
	{
		key = CHotKey(VK_LMENU, VK_LSHIFT);

		HKEY hKey = 0;
		IF_LSTATUS_RET(RegOpenKeyExW(HKEY_CURRENT_USER, L"Keyboard Layout\\Toggle", 0, KEY_READ, &hKey));

		DWORD dataType = REG_SZ;
		TChar sBuf[0x100];
		DWORD sSize = 0x100;
		LONG nError = ::RegQueryValueExW(
			hKey,
			L"Hotkey",
			0,
			&dataType,
			(PBYTE)sBuf,
			&sSize);

		if (nError == ERROR_SUCCESS)
		{
			if (wcscmp(sBuf, L"2") == 0)
			{
				key = CHotKey(VK_LCONTROL, VK_LSHIFT);
			}
		}

		RegCloseKey(hKey);

		RETURN_SUCCESS;
	}



	inline wchar_t VkCodeToChar(UINT vk, UINT scan, bool is_shift, HKL lay) { // -1 for dead char.

		BYTE keyState[256] = { 0 };
		keyState[VK_SHIFT] =is_shift ? 0x80 : 0;
		TCHAR sBufKey[0x10] = { 0 };
		int res = ToUnicodeEx(vk, scan, keyState, sBufKey, ARRAYSIZE(sBufKey), 0, lay);
		if (res < 0) {
			return -1;
		}
		if (res >= 1) {
			return sBufKey[0];
		}
		return 0;
	}

}






