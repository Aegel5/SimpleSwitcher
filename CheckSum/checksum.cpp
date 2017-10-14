// CheckSumSha256.cpp: определяет точку входа для консольного приложения.
//

#include "stdafx.h"
#include "sha256.h"
#include "md5.h"
#include <fstream>    
#include <string>

int printStdErr()
{
	auto err = errno;
	char buf[256];
	*buf = 0;
	strerror_s(buf, err);
	printf(buf);

	return err;
}
static const wchar_t* c_md5 = L"md5";
static const wchar_t* c_sha256 = L"sha256";
static const wchar_t* c_toFile = L"toFile";
static const wchar_t* c_stdout = L"stdout";
int _tmain(int argc, _TCHAR* argv[])
{
	if (argc < 3)
	{
		wprintf(L"Usage: checksum.exe [%s,%s] pathToFile [%s,%s]", c_md5, c_sha256, c_toFile, c_stdout);
		return 1;
	}

	std::wstring type = argv[1];

	std::wstring sPath = argv[2];

	std::ifstream ifs(sPath, std::ifstream::binary);
	if (!ifs.is_open())
	{
		return printStdErr();
	}

	std::string content(
		(std::istreambuf_iterator<char>(ifs)),
		(std::istreambuf_iterator<char>())
		);

	std::string resCheckSum;
	if (type == c_md5)
	{
		resCheckSum = md5(content);
	}
	else if (type == c_sha256)
	{
		resCheckSum = sha256(content);
	}
	else
	{
		wprintf(L"Unknown type %s", type);
		return 1;
	}

	std::wstring resCheckSumW(resCheckSum.begin(), resCheckSum.end());
	if (argc >= 3 && wcscmp(argv[3], c_toFile) == 0)
	{
		std::wstring sPathOut = sPath;
		std::wstring sName;

		auto pos = sPathOut.find_last_of(L"\\");
		if (pos != std::wstring::npos)
		{
			sName = sPathOut.substr(pos + 1);
		}

		pos = sPathOut.find_last_of(L".");
		if (pos != std::wstring::npos)
		{
			sPathOut.erase(pos);
		}
		sPathOut += L".";
		sPathOut += type;

		std::wofstream file(sPathOut);
		if (!file.is_open())
		{
			return printStdErr();
		}

		file << resCheckSumW;
		if (!sName.empty())
		{
			file << L" *";
			file << sName;
		}
		printf("File %S was created", sPathOut.c_str());
	}
	else
	{
		printf(resCheckSum.c_str());
	}



	return 0;
}

