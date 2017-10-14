#pragma once


namespace ParseCfg
{
	template<bool DEF>
	class SettingBool
	{
		bool m_val;
	public:
		SettingBool()
		{
			m_val = DEF;
		}
		operator bool() const { return m_val; }
	};

	namespace _Int
	{
		inline void DelLuaComment(std::wstring& line)
		{
			auto res = line.find(L"--");
			if (res != std::wstring::npos)
			{
				line.erase(res);
			}
		}
	}
	typedef std::map<std::wstring, std::wstring> TSMap;
	inline TStatus ReadKeyMapFile(std::wstring& sPath, TSMap& res)
	{

		std::wifstream ifs(sPath);
		IF_ERRNO_RET(errno);

		std::wstring content(
			(std::istreambuf_iterator<wchar_t>(ifs)),
			(std::istreambuf_iterator<wchar_t>())
		);

		Str_Utils::TVectStr vect;
		IFS_RET(Str_Utils::Split(content, vect, L'\n'));

		for (auto& line : vect)
		{
			_Int::DelLuaComment(line);

			Str_Utils::TVectStr vect2;
			IFS_RET(Str_Utils::Split(line, vect2, L'='));

			if (vect2.size() != 2)
			{
				continue;
			}
			auto& key = vect2[0];
			auto& value = vect2[1];
			Str_Utils::trim(key);
			Str_Utils::trim(value);
			res[key] = value;
		}


		RETURN_SUCCESS;

	}
	bool GetBool(TSMap& map, const TChar* name, bool& res)
	{
		auto it = map.find(name);
		if (it == map.end())
		{
			return false;
		}

		if (Str_Utils::StrToBool(it->second, res))
		{
			return true;
		}

		return false;
	}

	bool GetBoolDef(TSMap& map, const TChar* name, bool def)
	{
		bool res;
		if (GetBool(map, name, res))
		{
			return res;
		}

		return def;
	}
	template<class T>
	bool GetInt(ParseCfg::TSMap& map, const TChar* name, T& res)
	{
		auto it = map.find(name);
		if (it == map.end())
		{
			return false;
		}

		if (!Str_Utils::StrToUInt64(it->second, res))
		{
			res = (T)0;
			LOG_INFO_1(L"[WARN] Can't convert to int %s", it->second.c_str());
		}

		return true;
	}

	template<class T>
	T GetIntDef(ParseCfg::TSMap& map, const TChar* name, T def = 0)
	{
		T val;
		if (GetInt(map, name, val))
		{
			return val;
		}

		return def;
	}

	std::wstring GetString(ParseCfg::TSMap& map, const TChar* name)
	{
		auto it = map.find(name);
		if (it == map.end())
		{
			return L"";
		}
		return it->second;
	}
}