#include <iostream>
#include <vector>
#include <string>
#include <regex>

inline auto ParseSnippet(const std::string& input) {

	struct Token {
		std::string text;
		bool inBrackets;
	};

	//std::string input = "Начало @@(внутри 1) середина @@(внутри 2) конец";

	// Регулярное выражение ищет конструкцию @@(...)
	// [0] - все совпадение, [1] - содержимое внутри скобок
	std::regex pattern(R"(@@\((.*?)\))");

	std::vector<Token> result;

	auto it = std::sregex_iterator(input.begin(), input.end(), pattern);
	auto end = std::sregex_iterator();

	size_t lastPos = 0;
	for (; it != end; ++it) {
		std::smatch match = *it;

		// 1. Текст ПЕРЕД скобками (если он есть)
		if (match.position() > lastPos) {
			result.push_back({ input.substr(lastPos, match.position() - lastPos), false });
		}

		// 2. Текст ВНУТРИ скобок (первая захваченная группа)
		result.push_back({ match.str(1), true });

		lastPos = match.position() + match.length();
	}

	// 3. Остаток строки после последнего совпадения
	if (lastPos < input.length()) {
		result.push_back({ input.substr(lastPos), false });
	}

	// Вывод результата
	//for (const auto& t : result) {
	//	std::cout << "[" << (t.inBrackets ? "БЫЛО В СКОБКАХ" : "ПРОСТО ТЕКСТ") << "]: "
	//		<< t.text << std::endl;
	//}

	return result;
}
