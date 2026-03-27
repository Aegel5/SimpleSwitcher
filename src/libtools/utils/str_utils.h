#pragma once

#include <sstream>
#include <algorithm>
#include <functional>
#include <cwctype>

namespace StrUtils
{
	inline void Trim(std::string& str) {
		const char* whitespaces = " \t\n\r\f\v"; // Все стандартные пробельные символы

		// 1. Убираем с конца
		size_t last = str.find_last_not_of(whitespaces);
		if (last == std::string::npos) {
			str.clear(); // Строка состоит только из пробелов
			return;
		}
		str.erase(last + 1);

		// 2. Убираем с начала
		size_t first = str.find_first_not_of(whitespaces);
		if (first != 0) {
			str.erase(0, first);
		}
	}

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


	inline bool ToInt(const std::string& str, auto& res, int base_ = 0) {
		const char* val = str.c_str();
		int base = 10;
		if (StrUtils::IsStartWith(val, "0x")) {
			val += 2;
			base = 16;
		}

		if (base_ != 0) base = base_;

		//if (std::from_chars(str, str + std::wcslen(str), res, base)) {
		//	return true;
		//}

		try {
			res = (TYPE_OF(res))std::stoull(val, 0, base);
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

	inline void ToLowerUnsafe(std::string& str) {
		// Работает быстро, но только для английских букв A-Z
		for (auto& c : str) c = (char)tolower((unsigned char)c);
	}

	inline void ToLower(std::wstring& str)	{
		std::transform(str.begin(), str.end(), str.begin(), [](wchar_t c) { return std::towlower(c); });
	}
	inline void ToUpper(std::wstring& str) {
		std::transform(str.begin(), str.end(), str.begin(), [](wchar_t c) { return std::towupper(c); });
	}

	// utf8 - incorrect todo lib
	//inline void ToLower(std::string& str) {
	//	std::transform(str.begin(), str.end(), str.begin(), [](unsigned char c) { return std::tolower(c); });
	//}
	//inline void ToUpper(std::string& str) {
	//	std::transform(str.begin(), str.end(), str.begin(), [](unsigned char c) { return std::toupper(c); });
	//}


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

	inline auto Split(UView str, char delim, bool skipEmpty = true) {
		std::vector<std::string> res;
		std::ispanstream data(str);

		std::string line;
		while (std::getline(data, line, delim)) {
			if (skipEmpty && line.empty())
				continue;;
			res.push_back(line);
		}

		return res;
	}

	inline bool replaceAll(std::string & s, UView search, UView replace) {
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

	// Из string в wstring (UTF-8 -> UTF-16)
	inline std::wstring Convert(std::string_view str) {
		if (str.empty()) return {};
		int size_needed = MultiByteToWideChar(CP_UTF8, 0, str.data(), (int)str.size(), NULL, 0);
		std::wstring wstrTo(size_needed, 0);
		MultiByteToWideChar(CP_UTF8, 0, str.data(), (int)str.size(), &wstrTo[0], size_needed);
		return wstrTo;
	}

	// Из wstring в string (UTF-16 -> UTF-8)
	inline std::string Convert(std::wstring_view wstr) {
		if (wstr.empty()) return {};
		int size_needed = WideCharToMultiByte(CP_UTF8, 0, wstr.data(), (int)wstr.size(), NULL, 0, NULL, NULL);
		std::string strTo(size_needed, 0);
		WideCharToMultiByte(CP_UTF8, 0, wstr.data(), (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);
		return strTo;
	}

	inline bool IsSpace(wchar_t c) {
		static const wstring spaces(L" \t-=+*()%!\n\r");
		return spaces.find(c) != -1;
	}

	inline void Sprintf(auto& buf, auto&&... args) { // todo array
		snprintf(buf, std::ssize(buf), FORWARD(args)...);
	}

	template<typename... Args>
	inline void FormatTo(auto& buf, const std::format_string<Args...> s, Args&&... v) { // todo array
		auto sz = std::size(buf);
		if (sz == 0) return;
		auto res = std::format_to_n(buf, sz-1, s, FORWARD(v)...);
		*res.out = 0;
	}

	inline UView GetLine(const string& s) {
		auto i = s.find("\n");
		if (i == -1) return { s.c_str(), s.length() };
		return { s.c_str(), (size_t)i };
	}
	inline bool EqualsCI(const std::string& a, const std::string& b) {
		// Быстрый путь: если длины не совпадают — строки не равны
		if (a.size() != b.size()) return false;

		// Используем std::equal с кастомным предикатом
		return std::equal(
			a.begin(), a.end(),
			b.begin(),
			[](char c1, char c2) {
				return std::tolower(static_cast<unsigned char>(c1)) == std::tolower(static_cast<unsigned char>(c2));
			}
		);
	}

}
