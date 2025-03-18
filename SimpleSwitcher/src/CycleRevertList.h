#pragma once

// Класс текущих набранных символов, разбивает буквы на слова.

class CycleRevertList {

	static const int c_maxWordRevert = 7;
	static const int c_nMaxLettersSave = 100;
	struct CycleRevert {
		int iStartFrom = -1;
		bool fNeedLanguageChange = false;
	};
	std::deque<TKeyHookInfo> m_symbolList; // просто список всего, что сейчас набрано.
	int m_nCurrentRevertCycle = -1;

public:
	void DeleteLastSymbol() {
		if (!m_symbolList.empty())
			m_symbolList.pop_back();
		ClearGenerated();
	}
	bool HasAnySymbol() const { return !m_symbolList.empty(); }
	void Clear() {
		m_symbolList.clear();
		ClearGenerated();
	}
private: void ClearGenerated() {
	m_nCurrentRevertCycle = -1;
}

private: std::vector<CycleRevert> GenerateCycleRevertList(HotKeyType typeRevert) {
	GETCONF;
	const auto& lst = m_symbolList;
	std::vector<CycleRevert> res;
	int countWords = 0;
	if (!lst.empty()) {
		for (int i = std::ssize(lst) - 1; i >= 0; --i) {
			auto is_ending = [&](int i) {
				if (lst[i].is_end) return true;
				auto check_type = [&](auto type) {
					if (lst[i].type == type || lst[i + 1].type == type) return true;
					return false;
					};
				if (check_type(KEYTYPE_CUSTOM)) return true;
				if (typeRevert == hk_RevertLastWord && cfg->separate_ext_last_word && check_type(KEYTYPE_LETTER_OR_CUSTOM)) return true;
				if (typeRevert == hk_RevertCycle && cfg->separate_ext_several_words && check_type(KEYTYPE_LETTER_OR_CUSTOM)) return true;
				return false;
				};
			auto add = [&](int i) {
				res.push_back({ i, res.empty() });
				if (++countWords >= c_maxWordRevert)
					return true;
				return false;
				};
			if (lst[i].type != KEYTYPE_SPACE && (i == 0 || lst[i - 1].type == KEYTYPE_SPACE || is_ending(i - 1))) {
				if (add(i))
					break;
			}

		}
	}

	if (res.size() > 1) {
		res.push_back(res.back());
		res.back().fNeedLanguageChange = true;
	}

	if (res.empty()) {
		CycleRevert cycleRevert = { -1, true };
		res.push_back(cycleRevert);
	}

	return res;
}
	   struct RevertKeysData {
		   TKeyRevert keys;
		   bool needLanguageChange = false;
	   };
public: RevertKeysData FillKeyToRevert(HotKeyType typeRevert) {

	RevertKeysData keyList;
	auto& list = keyList.keys;

	CycleRevert curRevertInfo;

	if (typeRevert == hk_RevertAllRecentText) {
		keyList.needLanguageChange = m_nCurrentRevertCycle <= 0;
		m_nCurrentRevertCycle = -1;
		
		// простой алгоритм от позиции предыдущего реверта
		for (int i = std::ssize(m_symbolList) - 1; i >= 0; --i) { 
			if (i != std::ssize(m_symbolList)-1 // самый последний end не считается.
				&& m_symbolList[i].is_end) break;
			curRevertInfo.iStartFrom = i;
		}
	}
	else {

		auto separated = GenerateCycleRevertList(typeRevert);

		if (separated.empty()) {
			LOG_WARN(L"empty separated");
			return keyList;
		}

		if (m_symbolList.empty()) {
			LOG_WARN(L"empty m_symbolList");
			return keyList;
		}

		if (m_nCurrentRevertCycle < 0) {
			m_nCurrentRevertCycle = 0;
		}

		if (m_nCurrentRevertCycle >= separated.size()) {
			m_nCurrentRevertCycle = separated.size() - 1;
		}

		keyList.needLanguageChange = separated[m_nCurrentRevertCycle].fNeedLanguageChange;

		if (typeRevert == hk_RevertLastWord) {
			if (m_nCurrentRevertCycle > 0) {
				m_nCurrentRevertCycle -= 1;
				curRevertInfo = separated[m_nCurrentRevertCycle];
				m_nCurrentRevertCycle = 0;
			}
			else {
				curRevertInfo = separated[m_nCurrentRevertCycle];
				m_nCurrentRevertCycle = 1;
				if (m_nCurrentRevertCycle >= (int)separated.size())
					m_nCurrentRevertCycle = 0;
			}
		}
		else {
			curRevertInfo = separated[m_nCurrentRevertCycle];
			++m_nCurrentRevertCycle;
			if (m_nCurrentRevertCycle >= (int)separated.size())
				m_nCurrentRevertCycle = 0;
		}
	}


	if (curRevertInfo.iStartFrom == -1)
		return keyList;

	for (int i = curRevertInfo.iStartFrom; i < (int)m_symbolList.size(); ++i) {
		list.push_back(m_symbolList[i].decrypted().key());
	}

	return keyList;
}
public: void SetSeparateLast() {
	if (!m_symbolList.empty())
		m_symbolList.back().is_end = true;
}
public: void AddKeyToList(TKeyType type, CHotKey hotkey, TScanCode_Ext scan_code) {
	ClearGenerated();

	while (m_symbolList.size() >= c_nMaxLettersSave) {
		m_symbolList.pop_front();
	}

	auto vk = hotkey.ValueKey();

	TKeyHookInfo key2;

	key2.key().vk_code = vk;
	key2.key().scan_code = scan_code;
	if (hotkey.Size() == 2) {
		key2.key().shift_key = hotkey.At(1); // надеемся это shift, пока так. todo
	}
	key2.type = type;

	key2.encrypt();
	m_symbolList.push_back(key2);
}

};