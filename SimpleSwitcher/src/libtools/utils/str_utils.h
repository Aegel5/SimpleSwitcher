#pragma once

#include <sstream>
#include <algorithm>
#include <functional>
#include <cwctype>

namespace Str_Utils
{
	inline bool isLetter(wchar_t symb) {
		return std::iswalpha(symb);
	}
	inline bool isDigit(wchar_t symb) {
		return std::iswdigit(symb);
	}
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

	inline bool IsStartWith(const char* str, const char* pref)
	{
		return strncmp(str, pref, strlen(pref)) == 0;
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


	inline bool ToInt(const std::string& str, auto& res) {
		const char* val = str.c_str();
		int base = 10;
		if (Str_Utils::IsStartWith(val, "0x")) {
			val += 2;
			base = 16;
		}

		//if (std::from_chars(str, str + std::wcslen(str), res, base)) {
		//	return true;
		//}

		try {
			res = (DECLTYPE_DECAY(res))std::stoull(val, 0, base);
			return true;
		}
		catch (std::exception) {
		}
		return false;
	}

	inline bool ToInt(const TChar* str, auto& val, int base = 0) {
		if (Str_Utils::IsStartWith(str, L"0x")) {
			str += 2;
			base = 16;
		}
		if (base == 0) 
			base = 10;
		try {
			val = std::stoull(str, 0, base);
			return true;
		}
		catch (std::exception) {
		}
		return false;
	}

	inline void ToLower(std::wstring& str)
	{
		std::transform(str.begin(), str.end(), str.begin(), ::tolower);
	}
	inline void ToLower(std::string& str)
	{
		std::transform(str.begin(), str.end(), str.begin(), ::tolower);
	}
	inline void ToUpper(std::wstring& str)
	{
		std::transform(str.begin(), str.end(), str.begin(), ::toupper);
	}
	typedef std::vector<std::wstring> TVectStr;
	inline TVectStr Split(SView str, wchar_t delim, bool skipEmpty = true)
	{
		TVectStr res;
		std::wispanstream data(str);

		std::wstring line;
		while (std::getline(data, line, delim))
		{
			if (skipEmpty && line.empty())
				continue;;
			res.push_back(line);
		}

		return res;
	}

	inline bool replaceAll(std::wstring & s, SView search, SView replace) {
		bool found = false;
		size_t pos = 0;
		while ((pos = s.find(search, pos)) != std::string::npos) {
			found = true;
			s.replace(pos, search.length(), replace);
			pos += replace.length();
		}
		return found;
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

	//inline TStatus Utf8ToWide(const char* utf8, std::wstring& wide)
	//{
	//	if (*utf8 == 0)
	//	{
	//		RETURN_SUCCESS;
	//	}
	//	TChar buf[0x1000];
	//	int res = MultiByteToWideChar(
	//		CP_UTF8,
	//		0,
	//		utf8,
	//		-1,
	//		buf,
	//		std::ssize(buf)
	//		);

	//	IFW_RET(res != 0);

	//	wide = buf;

	//	RETURN_SUCCESS;
	//}
	//inline TStatus Utf8ToWide(const std::string& utf8, std::wstring& wide)
	//{
	//	return Utf8ToWide(utf8.c_str(), wide);
	//}
	//inline TStatus WideToUtf8(const TChar* wide, std::string& utf)
	//{
	//	if (*wide == 0)
	//	{
	//		RETURN_SUCCESS;
	//	}
	//	char buf[0x1000];
	//	int res = WideCharToMultiByte(
	//		CP_UTF8,
	//		0,
	//		wide,
	//		-1,
	//		buf,
	//		std::ssize(buf),
	//		NULL,
	//		NULL);

	//	IFW_RET(res != 0);

	//	utf = buf;

	//	RETURN_SUCCESS;

	//}


}