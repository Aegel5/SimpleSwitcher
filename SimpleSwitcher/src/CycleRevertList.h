#pragma once

// ����� ������� ��������� ��������, ��������� ����� �� �����.

class CycleRevertList {

	static const int c_maxWordRevert = 7;
	static const int c_nMaxLettersSave = 90;
	std::deque<TKeyHookInfo> m_symbolList; // ������ ������ �����, ��� ������ �������.
	int iCurrentWord = -1;
	TScanCode_Ext last_scan;

public:
	void DeleteLastSymbol() {
		if (!m_symbolList.empty())
			m_symbolList.pop_back();
		last_scan.clear();
		ClearGenerated();
	}
	bool HasAnySymbol() const { return !m_symbolList.empty(); }
	void Clear() {
		m_symbolList.clear();
		ClearGenerated();
	}
private: void ClearGenerated() {
	iCurrentWord = -1;
}

private: std::vector<int> GenerateWords(HotKeyType typeRevert) {

	// ������� ��������� ��� ���������� �������� ������� ������ ����.

	struct ZippData { int i; TKeyType type; bool is_last_revert = false; };
	std::vector<ZippData> zipped;
	zipped.reserve(8);
	{
		TKeyHookInfo stumb;
		TKeyHookInfo* prev = &stumb;
		for (int i = 0; i < m_symbolList.size(); prev = &m_symbolList[i], i++) {
			const auto& cur = m_symbolList[i];
			auto type = cur.type;

			if (prev->is_last_revert) { 
				zipped.emplace_back(i, type, true);
				continue;
			}

			auto check = [&]() -> bool {

				if (cur.as_previous) { // ���������� ������� - ������ �������������� ���������.
					return false;
				}

				if(Utils::is_in(type, KEYTYPE_CUSTOM, KEYTYPE_LETTER_OR_CUSTOM)) return true;

				return type != prev->type;
				};
			if (check()) {
				zipped.emplace_back(i, type);
			}
		}
	}

	// �� ����, ��� ��� ������, �������� ���� ������ ������ possible letter / letter.

	std::vector<int> words; // ������� ������ ����.

	GETCONF;
	const auto can_separate_posible =
		(typeRevert == hk_RevertLastWord && cfg->separate_ext_last_word)
		|| (typeRevert == hk_RevertCycle && cfg->separate_ext_several_words);


	for (int i = -1; auto & it : zipped) {
		i++;
		auto check = [&]() -> bool {
			if (it.type == KEYTYPE_LETTER_OR_SPACE) {
				// �� ��������� ����� ��� ����������.
				it.type = (i > 0 && i < std::ssize(zipped) - 1 && Utils::is_all(KEYTYPE_LETTER, zipped[i - 1].type, zipped[i + 1].type))
					? KEYTYPE_LETTER
					: KEYTYPE_SPACE;
			}
			if (it.type == KEYTYPE_SPACE) return false;
			if (it.type == KEYTYPE_LETTER_OR_CUSTOM) {
				bool separate = can_separate_posible;
				if (separate) {
					// �������� ������ ���� ����� ��� ������ space
					separate = i == 0 || i + 1 >= zipped.size() 
						|| Utils::is_in(KEYTYPE_SPACE, zipped[i - 1].type, zipped[i + 1].type)
						|| Utils::is_in(KEYTYPE_CUSTOM, zipped[i - 1].type, zipped[i + 1].type)
						;
				}

				it.type = separate ? KEYTYPE_CUSTOM : KEYTYPE_LETTER; // ������ ����� ���������� ���
			}
			if (it.is_last_revert) { // ��������� ����������.
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

	if (words.empty()) { // �������� ���� ������� ���� �������.
		return keyList;
	}

	if (iCurrentWord >= std::ssize(words)) {
		iCurrentWord = words.size() - 1;
	}

	int iStartFrom = -1;

	keyList.needLanguageChange = iCurrentWord == -1 || iCurrentWord == words.size() - 1;

	if (typeRevert == hk_RevertAllRecentText) { 		// ������� �������� �� ������� ����������� �������
		iStartFrom = std::ssize(m_symbolList) - 1;
		for (int i = std::ssize(m_symbolList) - 2; i >= 0; --i) { 
			if (m_symbolList[i].is_last_revert) 
				break;
			iStartFrom = i;
		}
		iCurrentWord = -1;
	}
	else {

		if (typeRevert == hk_RevertLastWord || words.size() == 1) {
			keyList.needLanguageChange = true;
			iStartFrom = words[0];
			iCurrentWord = iCurrentWord == -1 ? 0 : -1;
		}
		else {
			if (iCurrentWord == words.size()-1) {
				// ������� ��������� revert ����� ������� �������� ���������
				iStartFrom = words[iCurrentWord];
				iCurrentWord = -1;
			}
			else {
				iCurrentWord++;
				iStartFrom = words[iCurrentWord];
			}
		}
	}


	for (int i = iStartFrom; i < std::ssize(m_symbolList); ++i) {
		keyList.keys.push_back(m_symbolList[i].decrypted().key());
	}

	return keyList;
}
public: void SetSeparateLast() {
	if (!m_symbolList.empty())
		m_symbolList.back().is_last_revert = true;
}
public: void AddKeyToList(TKeyType type, TScanCode_Ext scan_code, bool is_shift) {
	ClearGenerated();

	while (m_symbolList.size() >= c_nMaxLettersSave) {
		m_symbolList.pop_front();
	}

	TKeyHookInfo key;
	key.key().scan_code = scan_code;
	if (is_shift) key.key().shift_key = VK_LSHIFT;
	key.type = type;
	key.encrypt();

	if (!m_symbolList.empty()) {
		if (last_scan.scan == 0) last_scan = m_symbolList.back().decrypted().key().scan_code;
		if (last_scan == scan_code) key.as_previous = true;
	}
	last_scan = scan_code;

	m_symbolList.push_back(key);
}

};