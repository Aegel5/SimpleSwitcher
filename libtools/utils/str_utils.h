#pragma once

#include <sstream>
#include <algorithm>
#include <functional>

namespace Str_Utils
{
	// trim from start (in place)
	static inline void trim_int(std::wstring &s, TStr chars, bool fLeft, bool fRight) 
	{
		auto foundNoSpace = [&chars](wchar_t c) -> bool
		{ 
			if (chars)
			{
				return wcschr(chars, c) == NULL;
			}
			return !std::isspace(c);
		};
		if (fLeft)
		{
			s.erase(
				s.begin(),
				std::find_if(
					s.begin(),
					s.end(),
					foundNoSpace
				)
			);
		}
		if (fRight)
		{
			s.erase(
				std::find_if(
					s.rbegin(),
					s.rend(),
					foundNoSpace
				).base(),
				s.end()
			);
		}
	}

	// trim from both ends (in place)
	static inline void trim(std::wstring &s, TStr chars = NULL)
	{
		trim_int(s, chars, true, true);
	}



	inline bool IsStartWith(const TChar* str, const TChar* pref)
	{
		return wcsncmp(str, pref, wcslen(pref)) == 0;
	}

	inline bool StrToBool(std::wstring& str, bool& res)
	{
		if (str == L"true")
		{
			res = true;
			return true;
		}
		else if (str == L"false")
		{
			res = false;
			return true;
		}
		else
		{
			// integer
			try
			{
				res = std::stoi(str) ? true : false;
				return true;
			}
			catch (std::exception)
			{
				return false;
			}
		}
	}

	inline bool IsEqualCI(TStr s1, TStr s2)
	{
		return _wcsicmp(s1, s2) == 0;
	}

	inline bool IsEqual(TStr s1, TStr s2)
	{
		return wcscmp(s1, s2) == 0;
	}

	template<class T>
	inline bool StrToUInt64(std::wstring& str, T& res)
	{
		const TChar* val = str.c_str();
		int base = 10;
		if (Str_Utils::IsStartWith(val, L"0x"))
		{
			val += 2;
			base = 16;
		}

		try
		{
			res = (T)std::stoull(val, 0, base);
			return true;
		}
		catch (std::exception)
		{
			return false;
		}
	}

	inline void ToLower(std::wstring& str)
	{
		std::transform(str.begin(), str.end(), str.begin(), ::tolower);
	}
	inline void ToUpper(std::wstring& str)
	{
		std::transform(str.begin(), str.end(), str.begin(), ::toupper);
	}
	typedef std::vector<std::wstring> TVectStr;
	inline TStatus Split(const TChar* str, TVectStr& res, wchar_t delim, bool skipEmpty = true)
	{
		std::wstringstream data(str);

		std::wstring line;
		while (std::getline(data, line, delim))
		{
			if (skipEmpty && line.empty())
				continue;;
			res.push_back(line);
		}

		RETURN_SUCCESS;
	}
	inline TStatus Split(const std::wstring& str, TVectStr& res, wchar_t delim, bool skipEmpty = true)
	{
		return Split(str.c_str(), res, delim, skipEmpty);
	}

	template < class ContainerT >
	void Split2(
		const std::wstring& str, 
		ContainerT& tokens,
		const std::wstring& delimiters = " ", 
		bool trimEmpty = false)
	{
		std::wstring::size_type pos, lastPos = 0, length = str.length();

		using value_type = typename ContainerT::value_type;
		using size_type = typename ContainerT::size_type;

		while (lastPos < length + 1)
		{
			pos = str.find_first_of(delimiters, lastPos);
			if (pos == std::string::npos)
			{
				pos = length;
			}

			if (pos != lastPos || !trimEmpty)
				tokens.push_back(value_type(str.data() + lastPos,
				(size_type)pos - lastPos));

			lastPos = pos + 1;
		}
	}
	//inline TStatus Trim(std::wstring& str)
	//{

	//}
	inline TStatus Utf8ToWide(const char* utf8, std::wstring& wide)
	{
		if (*utf8 == 0)
		{
			RETURN_SUCCESS;
		}
		TChar buf[0x1000];
		int res = MultiByteToWideChar(
			CP_UTF8,
			0,
			utf8,
			-1,
			buf,
			SW_ARRAY_SIZE(buf)
			);

		IFW_RET(res != 0);

		wide = buf;

		RETURN_SUCCESS;
	}
	inline TStatus WideToUtf8(const TChar* wide, std::string& utf)
	{
		if (*wide == 0)
		{
			RETURN_SUCCESS;
		}
		char buf[0x1000];
		int res = WideCharToMultiByte(
			CP_UTF8,
			0,
			wide,
			-1,
			buf,
			SW_ARRAY_SIZE(buf),
			NULL,
			NULL);

		IFW_RET(res != 0);

		utf = buf;

		RETURN_SUCCESS;

	}

	inline void ParseSlashes(std::wstring& str, std::wstring& res)
	{
		//std::wstring res;
		auto it = str.begin();
		while (it != str.end())
		{
			auto c1 = *it++;
			if (c1 == L'\\' && it != str.end())
			{
				auto c2 = *it++;
				if (c2 == L'\\')
				{
					res += L'\\';
				}
				else if (c2 == L'n')
				{
					res += L'\n';
				}
				else if (c2 == L't')
				{
					res += L'\t';
				}
				else if (c2 == L'b')
				{
					res += L'\b';
				}
				else
				{
					// as is
					res += c1;
					res += c2;
				}
			}
			else
			{
				res += c1;
			}
		}
	}
}