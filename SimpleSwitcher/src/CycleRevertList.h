#pragma once

#include "Encrypter.h"

// Класс текущих набранных символов, разбивает буквы на слова.

class CycleRevertList {
	static const int c_maxWordRevert = 7;
	static const int c_nMaxLettersSave = 100;
	struct CycleRevert
	{
		int nIndexWordList;
		bool fNeedLanguageChange;
	};
    std::vector<CycleRevert> m_CycleRevertList; // список, разбитый по словам.
	std::deque<TKeyHookInfo> m_symbolList; // просто список всего, что сейчас набрано.
    int m_nCurrentRevertCycle = -1;
public:
	void DeleteLastSymbol() {
		if (!m_symbolList.empty())
			m_symbolList.pop_back();
		ClearWords();
	}
	bool HasAnySymbol() { return !m_symbolList.empty(); }
	void Clear() {
		m_symbolList.clear();
		ClearWords();
	}
	void ClearWords() {
		m_CycleRevertList.clear();
		m_nCurrentRevertCycle = -1;
	}

	void GenerateCycleRevertList(){

		if (m_nCurrentRevertCycle != -1)
			return; // уже сгенерированно, актуальность мы поддерживаем

		ClearWords();

		int countWords = 0;
		if (!m_symbolList.empty()) {
			for (int i = std::ssize(m_symbolList) - 1; i >= 0; --i)	{
				auto issep = [&](int i) {return TestFlag(m_symbolList[i].keyFlags, TKeyFlags::SYMB_SEPARATE_REVERT); };
				auto add = [&](int i) {
					CycleRevert cycleRevert = { i, m_CycleRevertList.empty() };
					m_CycleRevertList.push_back(cycleRevert);
					if (++countWords >= c_maxWordRevert)
						return true;
					return false;
					};
				if (issep(i))
				{
					if (add(i))
						break;
				}
				else if (m_symbolList[i].type != KEYTYPE_SPACE && (i == 0 || m_symbolList[i - 1].type == KEYTYPE_SPACE || issep(i - 1)))
				{
					if (add(i))
						break;
				}

			}
		}

		if (m_CycleRevertList.size() > 1)
		{
			m_CycleRevertList.push_back(m_CycleRevertList.back());
			m_CycleRevertList.back().fNeedLanguageChange = true;
		}

		if (m_CycleRevertList.empty())
		{
			CycleRevert cycleRevert = { -1, true };
			m_CycleRevertList.push_back(cycleRevert);
		}

		m_nCurrentRevertCycle = 0;
	}
	struct RevertKeysData {
		TKeyRevert keys;
		bool needLanguageChange = false;
	};
	RevertKeysData FillKeyToRevert(HotKeyType typeRevert)	{

		RevertKeysData keyList;
		auto& list = keyList.keys;

		if (m_nCurrentRevertCycle < 0) {
			LOG_WARN(L"bad");
			return keyList;
		}

		if (m_nCurrentRevertCycle >= m_CycleRevertList.size()) {
			LOG_WARN(L"bad");
			return keyList;
		}

		keyList.needLanguageChange = m_CycleRevertList[m_nCurrentRevertCycle].fNeedLanguageChange;

		auto get_decrtypted = [this](int i) {
			TKeyHookInfo cur;
			cur.crypted = m_symbolList[i].crypted;
			Encrypter::Decrypt(cur);
			return cur.key();
			};

		if (m_nCurrentRevertCycle == -1)
			return keyList;
		if (m_CycleRevertList.empty())
			return keyList;

		CycleRevert curRevertInfo;

		if (typeRevert == hk_RevertLastWord)
		{
			if (m_nCurrentRevertCycle > 0)
			{
				m_nCurrentRevertCycle -= 1;
				curRevertInfo = m_CycleRevertList[m_nCurrentRevertCycle];
				m_nCurrentRevertCycle = 0;
			}
			else
			{
				curRevertInfo = m_CycleRevertList[m_nCurrentRevertCycle];
				m_nCurrentRevertCycle = 1;
				if (m_nCurrentRevertCycle >= (int)m_CycleRevertList.size())
					m_nCurrentRevertCycle = 0;
			}
		}
		else
		{
			curRevertInfo = m_CycleRevertList[m_nCurrentRevertCycle];
			++m_nCurrentRevertCycle;
			if (m_nCurrentRevertCycle >= (int)m_CycleRevertList.size())
				m_nCurrentRevertCycle = 0;
		}

		if (curRevertInfo.nIndexWordList == -1)
			return keyList;
		if (m_symbolList.empty())
			return keyList;

		for (int i = curRevertInfo.nIndexWordList; i < (int)m_symbolList.size(); ++i)
		{
			list.push_back(get_decrtypted(i));
		}

		return keyList;
	}
	void AddKeyToList(TKeyType type, CHotKey hotkey, TScanCode_Ext scan_code)
	{
		ClearWords();

		while (m_symbolList.size() >= c_nMaxLettersSave)
		{
			m_symbolList.pop_front();
		}

		TKeyHookInfo key2;

		key2.key().vk_code = hotkey.ValueKey();
		key2.key().scan_code = scan_code;
		if (hotkey.Size() == 2) {
			key2.key().shift_key = hotkey.At(1); // надеемся это shift, пока так.
		}
		key2.type = type;
		if (hotkey.ValueKey() == VK_OEM_2 && conf_get()->isTryOEM2) {
			SetFlag(key2.keyFlags, TKeyFlags::SYMB_SEPARATE_REVERT);
		}

		IFS_LOG(Encrypter::Encrypt(key2));
		m_symbolList.push_back(key2);
	}

};