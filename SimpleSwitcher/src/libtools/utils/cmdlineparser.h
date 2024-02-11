#pragma once

namespace cmdlineparse
{
	using TKeyValue = std::map<tstring, tstring>;

	inline TStatus Parse(int argc, _TCHAR** argv, TKeyValue& res)
	{
		for (int i = 1; i < argc; ++i)
		{
			tstring cur = argv[i];
			Str_Utils::TVectStr splited;
			IFS_RET(Str_Utils::Split(cur, splited, '='));
			if (splited.size() > 2 || splited.size() == 0)
			{
				RETS(SW_ERR_INVALID_PARAMETR)
			}
			else if (splited.size() == 2)
			{
				res[splited[0]] = splited[1];
			}
			else
			{
				res[splited[0]] = L"";
			}
		}
		RETURN_SUCCESS;
	}
}
