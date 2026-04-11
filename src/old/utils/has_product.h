//inline TStatus IsProductHas(const TChar* sPath, const TChar* sStr, bool& fRes)
//{
//	fRes = false;

//	// allocate a block of memory for the version info
//	DWORD dummy;
//	DWORD dwSize = GetFileVersionInfoSize(sPath, &dummy);
//	IFW_RET(dwSize != 0);


//	std::vector<BYTE> data(dwSize);
//	IFW_RET(GetFileVersionInfo(sPath, NULL, dwSize, &data[0]));

//	struct LANGANDCODEPAGE {
//		WORD wLanguage;
//		WORD wCodePage;
//	} *lpTranslate;

//	UINT cbTranslate = 0;

//	IFW_RET(VerQueryValue(
//		&data[0],
//		TEXT("\\VarFileInfo\\Translation"),
//		(LPVOID*)&lpTranslate,
//		&cbTranslate));

//	LPVOID pvProductName = NULL;
//	unsigned int iProductNameLen = 0;

//	for (UINT i = 0; i < (cbTranslate / sizeof(struct LANGANDCODEPAGE)); i++)
//	{
//		TChar sResPath[0x100];
//		sResPath[0] = 0;

//		if(swprintf_s(sResPath, L"\\StringFileInfo\\%04x%04x\\ProductName",
//			lpTranslate[i].wLanguage,
//			lpTranslate[i].wCodePage) < 0)
//			RET_ERRNO();

//		// Retrieve file description for language and code page "i". 
//		VerQueryValue(
//			&data[0],
//			sResPath,
//			&pvProductName,
//			&iProductNameLen);

//		TCHAR* sInfo = (TCHAR*)pvProductName;
//		std::wstring sProdName;
//		sProdName = sInfo;

//		StrUtils::ToLower(sProdName);
//		if (sProdName.find(sStr) != std::wstring::npos)
//		{
//			fRes = true;
//		}
//		int k = 0;
//	}


//	RETURN_SUCCESS;
//}
