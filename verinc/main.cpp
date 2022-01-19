// CheckSumSha256.cpp: определяет точку входа для консольного приложения.
//

#include "stdafx.h"
#include <fstream>    
#include <string>
#include <vector>
#include <Windows.h>
#include "../libtools/inc_basic.h"

int printStdErr()
{
	auto err = errno;
	char buf[256];
	*buf = 0;
	strerror_s(buf, err);
	printf(buf);

	return err;
}

int _tmain(int argc, _TCHAR* argv[])
{
	if (argc < 2)
	{
		wprintf(L"Usage: verinc.exe path");
		return 1;
	}

	std::wstring sPath = argv[1];

	std::ifstream ifs(sPath);
	if (!ifs.is_open())
	{
		return printStdErr();
	}

	std::wstring cont(
		(std::istreambuf_iterator<char>(ifs)),
		(std::istreambuf_iterator<char>())
	);

	Str_Utils::TVectStr res;
	Str_Utils::Split2(cont, res, L"\n\r ", true);

	int a1, a2, a3 ,a4;
	Str_Utils::StrToUInt64(res[2], a1);
	Str_Utils::StrToUInt64(res[5], a2);
	Str_Utils::StrToUInt64(res[8], a3);
	Str_Utils::StrToUInt64(res[11], a4);

	a4++;
	if (a4 >= 10) {
		a4 = 0;
		a3++;
		if (a3 >= 10) {
			a3 = 0;
			a2++;
		}
		if (a2 >= 10) {
			a2 = 0;
			a1++;
		}
	}
	wchar_t buff[100];
	 auto f = LR"(
#define SW_VER_1    %d
#define SW_VER_2    %d
#define SW_VER_3    %d
#define SW_VER_4    %d
		)";

	 swprintf(buff, f, a1, a2, a3, a4);


	std::wofstream file(sPath);
	if (!file.is_open())
	{
		return printStdErr();
	}

	file << buff;


	return 0;
}

