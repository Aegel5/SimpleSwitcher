#pragma once

namespace Startup
{
	namespace Int
	{
		inline auto BuildCmdLine(std::wstring_view sPath, std::wstring_view sArgs) {
			if (sArgs.empty()) {
				return StrUtils::MakeFormatArray(L"\"{}\"", sPath);
			}

			return StrUtils::MakeFormatArray(L"\"{}\" {}", sPath, sArgs);
		}
	}
	inline TStatus GetString_AutoStartUser(TStr keyName, bool& exist, std::wstring& value)
	{
		exist = false;
		value.clear();

		CAutoCloseHKey hg;
		IF_LSTATUS_RET(RegOpenKeyEx(
			HKEY_CURRENT_USER,
			L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run",
			0,
			KEY_ALL_ACCESS,
			&hg));
		DWORD type = 0;
		wchar_t data[0x1000];
		DWORD size = std::ssize(data) - sizeof(wchar_t);
		auto res = RegQueryValueEx(hg, keyName, 0, &type, (PBYTE)data, &size);
		if (res != ERROR_SUCCESS)
		{
			if (res == ERROR_NOT_FOUND)
			{
				IF_LSTATUS_LOG(res);
			}
			RETURN_SUCCESS;
		}
		exist = true;

		DWORD len = size / sizeof(wchar_t);
		data[len] = 0;

		if (type != REG_SZ)
		{
			RETURN_SUCCESS;
		}

		value = data;

		RETURN_SUCCESS;
	}
	inline TStatus CheckAutoStartUser(bool& isPathEquals, bool& isHasEntry, TStr keyName, TStr sPath, TStr sArgs)
	{
		isPathEquals = false;
		isHasEntry = false;

		std::wstring value;
		IFS_RET(GetString_AutoStartUser(keyName, isHasEntry, value));

		auto cmdLine = Int::BuildCmdLine(sPath, sArgs);

		isPathEquals = StrUtils::IsEqualCI(cmdLine.data(), value.c_str());

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
		auto cmdLine = Int::BuildCmdLine(sPath, sArgs);

		DWORD nSizeInBytes = (DWORD)(cmdLine.size() + 1) * sizeof(TCHAR);
		IF_LSTATUS_RET(RegSetValueEx(hg, keyName, 0, REG_SZ, (PBYTE)cmdLine.data(), nSizeInBytes));

		RETURN_SUCCESS;
	}
}
