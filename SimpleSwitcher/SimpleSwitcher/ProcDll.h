#pragma once

//
//struct HotKeyHold
//{
//	CAutoHotKey autoHot;
//	CHotKey saved;
//};
//struct HotKeyHolders
//{
//	HotKeyHold holders[hk_MAX];
//};
//TStatus SetHotKey(CHotKey key, CAutoHotKey& autokey, int idHotKey)
//{
//	if(key.IsEmpty())
//	{
//		autokey.Cleanup();
//	}
//	else
//	{
//		UINT fsModifiers = 0;
//		std::wstring s;
//		key.ToString(s);
//		LOG_INFO_1(L"Set hotkey %s for %s", s.c_str(), HotKeyTypeName((HotKeyType)idHotKey));
//		if (key.HasMod(VK_MENU))
//			fsModifiers |= MOD_ALT;
//		if (key.HasMod(VK_CONTROL))
//			fsModifiers |= MOD_CONTROL;
//		if (key.HasMod(VK_SHIFT))
//			fsModifiers |= MOD_SHIFT;
//		IFW_RET(autokey.Register(gdata().hWndMonitor, idHotKey, fsModifiers, key.ValueKey()));
//	}
//	RETURN_SUCCESS;
//}
//TStatus ResetAllHotKey(HotKeyHolders& keys)
//{
//	for(auto& it : SettingsGlobal().hotkeysList)
//	{
//		auto hkId = it.first;
//		CHotKeySet& set = it.second;
//
//		HotKeyHold& hold = keys.holders[hkId];
//
//		if(hold.saved != set.key)
//		{
//			TStatus stat = SetHotKey(set.key, hold.autoHot, hkId);
//			if (SW_SUCCESS(stat))
//			{
//				hold.saved = set.key;
//			}
//		}
//	}
//
//	RETURN_SUCCESS;
//}


// ------------------------
// Hook dll


// ------------------------
// Hook dll x64
//CAutoProcMonitor monitor64(c_sClassName64, c_sArgHook64, SW_BIT_64, false);
//if (bit == SW_BIT_32 && IsWindows64())
//{
//	IFS_LOG(monitor64.EnsureStarted(SW_ADMIN_SELF));
//}

//HotKeyHolders hotKeys;
//gdata().hotkeyHolders = &hotKeys;
//if (gdata().curModeBit == SW_BIT_32)
//{
//	IFS_RET(ResetAllHotKey(hotKeys));
//}


struct HookDllHandles
{
	CAutoHHOOK hHookGetMessage;
	CAutoHHOOK hHookCallWndProc;
	CAutoHHOOK hHookKeyboard;
};

TStatus CheckFilesEquals1(const TChar* s1, const TChar* s2, bool& fRes)
{
	//DWORD HeaderSum1;
	//DWORD CheckSum1;

	//DWORD res = MapFileAndCheckSum(s1, &HeaderSum1, &CheckSum1);

	fRes = false;

	CAutoHandle2 h1 = CreateFile(
		s1,
		GENERIC_READ,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);
	IFW_RET(h1.IsValid());

	CAutoHandle2 h2 = CreateFile(
		s2,
		GENERIC_READ,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);
	IFW_RET(h2.IsValid());

	LARGE_INTEGER size1;
	IFW_RET(GetFileSizeEx(h1, &size1));

	LARGE_INTEGER size2;
	IFW_RET(GetFileSizeEx(h2, &size2));

	if (size1.QuadPart != size2.QuadPart)
	{
		RETURN_SUCCESS;
	}

	FILETIME cr1;
	IFW_RET(GetFileTime(h1, &cr1, NULL, NULL));

	FILETIME cr2;
	IFW_RET(GetFileTime(h2, &cr2, NULL, NULL));

	if (cr1.dwLowDateTime != cr2.dwLowDateTime || cr1.dwHighDateTime != cr2.dwHighDateTime)
	{
		RETURN_SUCCESS;
	}

	fRes = true;

	RETURN_SUCCESS;
}
TStatus CopyToWorkBin(std::wstring& sPathDllNew, TSWBit bit)
{
	std::wstring sPathDll;
	GetPath(sPathDll, PATH_TYPE_DLL_NAME, bit);

	GetPath(sPathDllNew, PATH_TYPE_SELF_FOLDER, bit);
	sPathDllNew += L"workbin\\";

	size_t index = sPathDll.find_last_of(L"\\");
	if (index != std::string::npos)
	{
		sPathDllNew += sPathDll.substr(index + 1);
	}

	if (PathFileExists(sPathDllNew.c_str()))
	{
		bool fEquals = false;
		IFS_RET(CheckFilesEquals2(sPathDll.c_str(), sPathDllNew.c_str(), fEquals));

		if (fEquals)
		{
			RETURN_SUCCESS;
		}
		else
		{
			std::wstring sPathDllNewOld = sPathDllNew;

			FILETIME ft;
			GetSystemTimeAsFileTime(&ft);
			ULARGE_INTEGER li;
			li.LowPart = ft.dwLowDateTime;
			li.HighPart = ft.dwHighDateTime;
			TCHAR sTime[0x100];
			swprintf_s(sTime, L"_%I64u.old", li.QuadPart);

			sPathDllNewOld += sTime;

			IFW_RET(MoveFileEx(sPathDllNew.c_str(), sPathDllNewOld.c_str(), 0));
			DeleteFileW(sPathDllNewOld.c_str());
			IFW_RET(CopyFile(sPathDll.c_str(), sPathDllNew.c_str(), FALSE));
		}
	}
	else
	{
		IFW_RET(CopyFile(sPathDll.c_str(), sPathDllNew.c_str(), FALSE));
	}

	RETURN_SUCCESS;
}

//TStatus HookDll(HookHandles& handles, TSWBit bit)
//{
//	std::wstring sPathDll;
//	CopyToWorkBin(sPathDll, bit);
//
//	CAutoHMODULE hHookDll = LoadLibrary(sPathDll.c_str());
//	IFW_RET(hHookDll.IsValid());
//
//	HOOKPROC pfGetMsgProc = (HOOKPROC)GetProcAddress(hHookDll, "GetMsgProc");
//	IFW_RET(pfGetMsgProc != NULL);
//
//	HOOKPROC pfCallWndProc = (HOOKPROC)GetProcAddress(hHookDll, "CallWndProc");
//	IFW_RET(pfCallWndProc != NULL);
//
//	HOOKPROC pfKeyboardProcc = (HOOKPROC)GetProcAddress(hHookDll, "KeyboardProc");
//	IFW_RET(pfKeyboardProcc != NULL);
//
//	//handles.hHookKeyboard = SetWindowsHookEx(WH_KEYBOARD, pfKeyboardProcc, hHookDll, 0);
//	//IFW_LOG(handles.hHookKeyboard.IsValid());
//
//	//handles.hHookGetMessage = SetWindowsHookEx(WH_GETMESSAGE, pfGetMsgProc, hHookDll, 0);
//	//IFW_LOG(handles.hHookGetMessage.IsValid());
//
//	handles.hHookCallWndProc = SetWindowsHookEx(WH_CALLWNDPROC, pfCallWndProc, hHookDll, 0);
//	IFW_LOG(handles.hHookCallWndProc.IsValid());
//
//	RETURN_SUCCESS;
//}

TStatus PrepareWorkBin()
{
	std::wstring sPathBin;
	GetPath(sPathBin, PATH_TYPE_SELF_FOLDER, GetSelfBit());

	sPathBin += L"workbin\\";

	if (!CreateDirectoryW(sPathBin.c_str(), NULL))
	{
		DWORD err = GetLastError();
		if (err != ERROR_ALREADY_EXISTS)
		{
			IFW_LOG(FALSE);
		}
	}

	std::wstring sMask = sPathBin + L"*.old";

	WIN32_FIND_DATA data;
	CAutoHandleFind hFind = FindFirstFile(sMask.c_str(), &data);
	if (hFind.IsValid())
	{
		while (1)
		{
			std::wstring sPath = sPathBin + data.cFileName;
			IFW_LOG(DeleteFileW(sPath.c_str()));
			if (!FindNextFile(hFind, &data))
			{
				break;
			}
		}
	}

	RETURN_SUCCESS;
}
//TStatus GetCheckSum(const TChar* s, DWORD& CheckSum)
//{
//	CheckSum = 0;
//	DWORD HeaderSum;
//	DWORD res = MapFileAndCheckSum(s, &HeaderSum, &CheckSum);
//	if (res != CHECKSUM_SUCCESS)
//	{
//		IFS_RET(SW_ERR_UNSUPPORTED, L"checkres=%u", res);
//	}
//	RETURN_SUCCESS;
//}
