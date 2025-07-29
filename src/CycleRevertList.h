#pragma once

// Класс текущих набранных символов, разбивает буквы на слова.

class CycleRevertList {

	static const int c_maxWordRevert = 7;
	static const int c_nMaxLettersSave = 90;
	std::deque<TKeyHookInfo> m_symbolList; // просто список всего, что сейчас набрано.
	static const int c_lastCorrectedInf = 9999999;
	int iLastCorrected = c_lastCorrectedInf;
	TimePoint lastadd;

public:
	void DeleteLastSymbol() {
		if (!m_symbolList.empty()) {
			bool move_last = m_symbolList.back().is_last_revert;
			m_symbolList.pop_back();
			if (move_last && !m_symbolList.empty())
				m_symbolList.back().is_last_revert = true;
		}
		ClearGenerated();
	}
	bool HasAnySymbol() const { return !m_symbolList.empty(); }
	void Clear() {
		if (!m_symbolList.empty()) {
			LOG_ANY(L"ClearsKeys {}", m_symbolList.size());
			m_symbolList.clear();
		}
		ClearGenerated();
	}
	void ClearByTimer() {
		if (lastadd.DeltToNow() >= 10min) {
			Clear();
		}
	}
private: void ClearGenerated() {
	iLastCorrected = c_lastCorrectedInf;
}

private: std::vector<int> GenerateWords(HotKeyType typeRevert) {

	// сначала объеденим все одинаковые сохраняя индексы старта слов.

	struct ZippData { int i; const TKeyHookInfo* p; TKeyType type; bool is_last_revert = false; };
	std::vector<ZippData> zipped;
	zipped.reserve(8);
	{
		TKeyHookInfo stumb;
		TKeyHookInfo* prev = &stumb;
		for (int i = 0; i < m_symbolList.size(); prev = &m_symbolList[i], i++) {
			const auto& cur = m_symbolList[i];
			auto type = cur.type;

			if (prev->is_last_revert) { 
				zipped.emplace_back(i, &cur, type, true);
				continue;
			}

			auto check = [&]() -> bool {

				if (cur.key == prev->key) { // одинаковый клавиши - всегда обрабатываются одинаково.
					return false;
				}

				if(Utils::is_in(type, KEYTYPE_CUSTOM, KEYTYPE_LETTER_OR_CUSTOM)) return true;

				return type != prev->type;
				};
			if (check()) {
				zipped.emplace_back(i, &cur, type);
			}
		}
	}

	// по сути, все уже готово, осталось лишь решить вопрос possible letter / letter.

	std::vector<int> words; // индексы старта слов.

	GETCONF;
	const auto can_separate_posible =
		(typeRevert == hk_RevertLastWord && cfg->separate_ext_mode == 2)
		|| (typeRevert == hk_RevertSeveralWords && cfg->separate_ext_mode > 0);


	for (int i = -1; auto & it : zipped) {
		i++;
		auto check = [&]() -> bool {
			// it.type должен иметь актуальный тип, так как используется на следующих итерациях.
			if (can_separate_posible && it.p->data.space_on_extended) {
				it.type = KEYTYPE_SPACE;
			}
			if (it.type == KEYTYPE_LETTER_OR_SPACE) {
				// не разделяем слово без надобности.
				it.type = (i > 0 && i < std::ssize(zipped) - 1 && Utils::is_all(KEYTYPE_LETTER, zipped[i - 1].type, zipped[i + 1].type))
					? KEYTYPE_LETTER
					: KEYTYPE_SPACE;
			}
			if (it.type == KEYTYPE_SPACE) return false;
			if (it.type == KEYTYPE_LETTER_OR_CUSTOM) {
				bool separate = can_separate_posible;
				if (separate) {
					// выделаем только если слева или справа space
					separate = i == 0 || i + 1 >= zipped.size() 
						|| Utils::is_in(KEYTYPE_SPACE, zipped[i - 1].type, zipped[i + 1].type)
						|| Utils::is_in(KEYTYPE_CUSTOM, zipped[i - 1].type, zipped[i + 1].type)
						;
				}

				it.type = separate ? KEYTYPE_CUSTOM : KEYTYPE_LETTER; // теперь можем определить тип
			}
			if (it.is_last_revert) { // форсируем разделение.
				return true;
			}
			if (it.type == KEYTYPE_LETTER) {
				return i == 0 || zipped[i - 1].type != KEYTYPE_LETTER;
			}
			return true; // custom
		};
		if (check()) {
			words.push_back(it.i);
		}

	}

	std::reverse(words.begin(), words.end());

	if (words.size() > c_maxWordRevert) {
		words.resize(c_maxWordRevert);
	}

	std::reverse(words.begin(), words.end());

	return words;

}


struct RevertKeysData {
	TKeyRevert keys;
	bool needLanguageChange = false;
};

public: RevertKeysData FillKeyToRevert(HotKeyType typeRevert) {

	RevertKeysData keyList;

	if (m_symbolList.empty()) {
		LOG_WARN(L"empty m_symbolList");
		return keyList;
	}

	auto words = GenerateWords(typeRevert);

	if (words.empty()) { // например одни пробелы были введены.
		return keyList;
	}

	/*
* Новый экспериментальный алгоритм:
* храним индекс первой буквы последнего изменнного слова.
* если запрос на изменение последнего слова - просто меняем последнее слово.
* если запрос "изменить все" - меняем все.
* если запрос "несколько слов" - то ищем слово, начинающееся с индекса < сохраненного, если такого нет - то меняем последнее слово.
*/
	int prev_correct = iLastCorrected;

	if (typeRevert == hk_RevertAllRecentText) {
		keyList.needLanguageChange = true;
		iLastCorrected = words[0];
		for (int i = ssize(m_symbolList) - 2; i >= 0; --i) {
			if (m_symbolList[i].is_last_revert) {
				iLastCorrected = i+1;
				break;
			}
		}
	}
	else if (typeRevert == hk_RevertLastWord) {
		keyList.needLanguageChange = true;
		iLastCorrected = words[words.size() - 1];
	}
	else { // несколько слов
		if (words[0] >= iLastCorrected) {
			iLastCorrected = c_lastCorrectedInf; // все слова уже изменили, сбрасываем на начало.
		}
		keyList.needLanguageChange = iLastCorrected == c_lastCorrectedInf;
		for (int i = words.size() - 1; i >= 0; i--) {
			if (words[i] < iLastCorrected) {
				iLastCorrected = words[i];
				break;
			}
		}
	}


	for (int i = iLastCorrected; i < ssize(m_symbolList); ++i) {
		keyList.keys.push_back(m_symbolList[i].key);
	}

	if (prev_correct == iLastCorrected) {
		iLastCorrected = c_lastCorrectedInf; // происходит отмена предыдущего реверт, сбрасываемся на начало.
	}

	return keyList;
}
public: void SetSeparateLast() {
	if (!m_symbolList.empty())
		m_symbolList.back().is_last_revert = true;
}
public: void AddKeyToList(TKeyType type, TKeyTypeData data, TScanCode_Ext scan_code, bool is_shift, TKeyCode vk = 0) {
	ClearGenerated();

	lastadd.SetNow();

	while (m_symbolList.size() >= c_nMaxLettersSave) {
		m_symbolList.pop_front();
	}

	TKeyHookInfo key;
	if (vk != 0) {
		key.key.vk_code = vk;
	}
	else {
		key.key.scan_code = scan_code;
	}
	if (is_shift) 
		key.key.shift_key = VK_LSHIFT;
	key.type = type;
	key.data = data;

	m_symbolList.push_back(key);
}

};
