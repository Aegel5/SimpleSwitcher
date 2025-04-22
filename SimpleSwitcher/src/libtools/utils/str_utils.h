#pragma once

#include <sstream>
#include <algorithm>
#include <functional>
#include <cwctype>

namespace StrUtils
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
		if (StrUtils::IsStartWith(val, "0x")) {
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
		if (StrUtils::IsStartWith(str, L"0x")) {
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
	inline void ToUpper(std::string& str) {
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

	inline std::string Convert(TStr wstr) {
		using convert_typeX = std::codecvt_utf8<wchar_t>;
		std::wstring_convert<convert_typeX, wchar_t> converterX;
		return converterX.to_bytes(wstr);
	}
	inline std::string Convert(const std::wstring& wstr) {
		return Convert(wstr.c_str());
	}

	inline std::wstring Convert(const std::string& str) {
		using convert_typeX = std::codecvt_utf8<wchar_t>;
		std::wstring_convert<convert_typeX, wchar_t> converterX;
		return converterX.from_bytes(str);
	}

	inline bool IsSpace(wchar_t c) {
		static const wstring spaces(L" \t-=+*()%!\n\r");
		return spaces.find(c) != -1;
	}

}
