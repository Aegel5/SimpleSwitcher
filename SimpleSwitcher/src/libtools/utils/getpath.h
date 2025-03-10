#pragma once

enum TPathType
{
	PATH_TYPE_SELF_FOLDER,
	PATH_TYPE_EXE_PATH,
	//PATH_TYPE_DLL_NAME,
	PATH_TYPE_EXE_FILENAME,
};

inline TStatus __GetPath(std::wstring& sPath, TPathType type, bool tolower)
{
	TCHAR buf[0x1000];
	DWORD nSize = GetModuleFileName(NULL, buf, SW_ARRAY_SIZE(buf));
	IFW_RET(nSize > 0);

	sPath = buf;
	if(tolower)
		Str_Utils::ToLower(sPath);

	if (type == PATH_TYPE_SELF_FOLDER)
	{
		size_t index = sPath.find_last_of(L"\\");
		if (index != std::string::npos)
			sPath.erase(index + 1);
		RETURN_SUCCESS;
	}


	if (type == PATH_TYPE_EXE_FILENAME)
	{
		size_t index = sPath.find_last_of(L"\\");
		if (index != std::string::npos)
			sPath = sPath.substr(index + 1);
	}



	//size_t index = sPath.rfind(L"64.exe");
	//if (index != std::string::npos)
	//	sPath.erase(index);
	//index = sPath.rfind(L".exe");
	//if (index != std::string::npos)
	//	sPath.erase(index);

	//if (type == PATH_TYPE_DLL_NAME)
	//{
	//	sPath += L"hook";
	//}

	//if (bit == SW_BIT_64)
	//{
	//	sPath += L"64";
	//}

	//if (type == PATH_TYPE_DLL_NAME)
	//{
	//	sPath += L".dll";
	//}
	//else
	//{
	//	sPath += L".exe";
	//}


	RETURN_SUCCESS;
}

//inline TStatus GetPath(std::wstring& sPath, TPathType type) {
//	return __GetPath(sPath, type, true);
//}

inline TStatus GetPath_folder_noLower(std::wstring& sPath) {
	return __GetPath(sPath, PATH_TYPE_SELF_FOLDER, false);
}

inline TStatus GetPath_exe_noLower(std::wstring& sPath) {
	return __GetPath(sPath, PATH_TYPE_EXE_PATH, false);
}

inline TStatus GetPath_fileExe_lower(std::wstring& sPath) {
	return __GetPath(sPath, PATH_TYPE_EXE_FILENAME, true);
}

inline TStatus GetPath_Conf(std::wstring& path) {
	IFS_RET(GetPath_folder_noLower(path));
	path += L"SimpleSwitcher.json";
	RETURN_SUCCESS;
}

inline std::wstring GetPath_Conf() {
	std::wstring path;
	IFS_LOG(GetPath_Conf(path));
	return path;
}