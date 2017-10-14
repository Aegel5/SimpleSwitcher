#pragma once

#include "cmn.h"

namespace Startup
{
	namespace Int
	{
		inline TStatus BuildCmdLine(TStr sPath, TStr sArgs, std::wstring& res)
		{
			TChar buf[0x1000];
			if (sArgs != nullptr)
			{
				if (swprintf_s(buf, L"\"%s\" %s", sPath, sArgs) == -1)
				{
					RET_ERRNO();
				}
			}
			else
			{
				if (swprintf_s(buf, L"\"%s\"", sPath) == -1)
				{
					RET_ERRNO();
				}
			}
			res = buf;
			RETURN_SUCCESS;
		}
	}
	inline TStatus CheckAutoStartUser(bool& isPathEquals, bool& isHasEntry, TStr keyName, TStr sPath, TStr sArgs)
	{
		isPathEquals = false;
		isHasEntry = false;

		CAutoCloseHKey hg;
		IF_LSTATUS_RET(RegOpenKeyEx(
			HKEY_CURRENT_USER,
			L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run",
			0,
			KEY_ALL_ACCESS,
			&hg));
		DWORD type = 0;
		wchar_t data[0x1000];
		DWORD size = SW_ARRAY_SIZE(data) - sizeof(wchar_t) ;
		auto res = RegQueryValueEx(hg, keyName, 0, &type, (PBYTE)data, &size);
		if (res != ERROR_SUCCESS)
		{
			if (res == ERROR_NOT_FOUND)
			{
				IF_LSTATUS_LOG(res);
			}
			RETURN_SUCCESS;
		}
		isHasEntry = true;

		DWORD len = size / sizeof(wchar_t);
		data[len] = 0;

		if (type != REG_SZ)
		{
			RETURN_SUCCESS;
		}

		std::wstring cmdLine;
		IFS_RET(Int::BuildCmdLine(sPath, sArgs, cmdLine));

		isPathEquals = Str_Utils::IsEqualCI(cmdLine.c_str(), data);

		RETURN_SUCCESS;
	}
	inline TStatus RemoveWindowsRun(TStr keyName)
	{
		CAutoCloseHKey hg;
		IF_LSTATUS_RET(RegOpenKeyEx(
			HKEY_CURRENT_USER,
			L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run",
			0,
			KEY_ALL_ACCESS,
			&hg));
		IF_LSTATUS_RET(RegDeleteValue(hg, keyName));

		RETURN_SUCCESS;
	}
	inline TStatus AddWindowsRun(TStr keyName, TStr sPath, TStr sArgs)
	{
		CAutoCloseHKey hg;
		IF_LSTATUS_RET(RegOpenKeyEx(
			HKEY_CURRENT_USER,
			L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run",
			0,
			KEY_ALL_ACCESS,
			&hg));

		//std::wstring sPath;
		//GetPath(sPath, PATH_TYPE_EXE_PATH, SW_BIT_32);
		std::wstring cmdLine;
		IFS_RET(Int::BuildCmdLine(sPath, sArgs, cmdLine));

		DWORD nSizeInBytes = (DWORD)(cmdLine.size() + 1) * sizeof(TCHAR);
		IF_LSTATUS_RET(RegSetValueEx(hg, keyName, 0, REG_SZ, (PBYTE)cmdLine.c_str(), nSizeInBytes));

		RETURN_SUCCESS;
	}
}