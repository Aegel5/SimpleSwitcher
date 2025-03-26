#pragma once

namespace Utils {

	enum TPathType {
		PATH_TYPE_SELF_FOLDER,
		PATH_TYPE_EXE_PATH,
		//PATH_TYPE_DLL_NAME,
		PATH_TYPE_EXE_FILENAME,
	};

	inline TStatus __GetPath(std::wstring& sPath, TPathType type, bool tolower) {
		TCHAR buf[0x1000];
		DWORD nSize = GetModuleFileName(NULL, buf, std::ssize(buf));
		IFW_RET(nSize > 0);

		sPath = buf;
		if (tolower)
			Str_Utils::ToLower(sPath);

		if (type == PATH_TYPE_SELF_FOLDER) {
			size_t index = sPath.find_last_of(L"\\");
			if (index != std::string::npos)
				sPath.erase(index + 1);
			RETURN_SUCCESS;
		}


		if (type == PATH_TYPE_EXE_FILENAME) {
			size_t index = sPath.find_last_of(L"\\");
			if (index != std::string::npos)
				sPath = sPath.substr(index + 1);
		}

		RETURN_SUCCESS;
	}

	inline TStatus GetPath_folder_noLower(std::wstring& sPath) {
		return __GetPath(sPath, PATH_TYPE_SELF_FOLDER, false);
	}

	inline std::wstring GetPath_folder_noLower() {
		std::wstring path;
		IFS_LOG(GetPath_folder_noLower(path));
		return path;
	}



	inline TStatus GetPath_exe_noLower(std::wstring& sPath) {
		return __GetPath(sPath, PATH_TYPE_EXE_PATH, false);
	}

	inline TStatus GetPath_fileExe_lower(std::wstring& sPath) {
		return __GetPath(sPath, PATH_TYPE_EXE_FILENAME, true);
	}

	inline void NormalizeDelims(auto& cur) {
		std::replace(cur.begin(), cur.end(), L'/', L'\\');
	}
}

