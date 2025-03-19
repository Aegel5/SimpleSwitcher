#pragma once

inline TKeyType AnalizeTyped(const CHotKey& key, UINT vk, const TScanCode_Ext& scan, HKL lay){

	bool is_shift = key.HasMod(VK_SHIFT);

	if (key.Size() == 0) {
		return KEYTYPE_COMMAND_NO_CLEAR;
	}
	else if (key.Size() > 2) {
		return KEYTYPE_COMMAND_CLEAR;
	}
	else if (key.Size() == 2) {
		if (!is_shift || CHotKey::IsKnownMods(vk))
			return KEYTYPE_COMMAND_CLEAR;
	}

	switch (vk) {

	case VK_TAB:
	case VK_SPACE:
		return KEYTYPE_SPACE;

	case VK_BACK:
		return KEYTYPE_BACKSPACE;

	case VK_RETURN:
	case VK_END:
	case VK_HOME:
	case VK_NEXT:
	case VK_PRIOR:
	case VK_DELETE:
	case VK_LEFT:
	case VK_RIGHT:
	case VK_DOWN:
	case VK_UP:
	case VK_ESCAPE:
		return KEYTYPE_COMMAND_CLEAR;

	case VK_CAPITAL:
	case VK_SCROLL:
	case VK_PRINT:
	case VK_NUMLOCK:
	case VK_INSERT:
		return KEYTYPE_COMMAND_NO_CLEAR;
	}

	GETCONF;

	bool isBadMapping = true; // numpad problem
	if (scan.to_vk_or_def(lay) == vk) {
		isBadMapping = false;
	}
	else {
		for (auto l2 : cfg->layouts_info.EnabledLayouts()) {
			if (l2 != lay) {
				if (scan.to_vk_or_def(l2) == vk) isBadMapping = false;
				break;
			}
		}
	}
	if (isBadMapping) {
		LOG_ANY(L"bad mapping");
	}

	auto curSymbol = Utils::VkCodeToChar(isBadMapping ? vk : scan.to_vk_or_def(lay,vk), scan.scan, is_shift, lay);
	if (curSymbol == 0)
		return KEYTYPE_COMMAND_CLEAR;
	if(curSymbol == -1)
		return KEYTYPE_LETTER; // dead symbol

	LOG_ANY_4(L"char {}", curSymbol);
	if (Str_Utils::isDigit(curSymbol)) {
		return KEYTYPE_LETTER_OR_SPACE; 
	}

	if (cfg->layouts_info.CntLayoutEnabled() <= 3) {

		auto is_letter = [](wchar_t c) {return Utils::is_in(c, L'-', L'_') || Str_Utils::isLetter(c); };

		bool have_letter = is_letter(curSymbol);
		bool have_custom = !have_letter;
		bool have_changes = false;

		// проверим возможные варианты для символа.

		for (const auto& it : cfg->layouts_info.EnabledLayouts()) {
			if (it != lay) {
				auto symb = Utils::VkCodeToChar(isBadMapping ? vk : scan.to_vk_or_def(it,vk), scan.scan, is_shift, it);
				if (symb == -1) {
					LOG_WARN(L"dead symbol on other layout");
					return KEYTYPE_LETTER;
				}
				if (symb == 0) {
					LOG_WARN(L"bad symbol on other layout");
					return KEYTYPE_LETTER;
				}
				if (symb != curSymbol) {
					have_changes = true;
				}
				if (is_letter(symb)) {
					have_letter = true;
				}
				else {
					have_custom = true;
				}
			}
		}
		if (!have_changes) {
			return have_letter ? KEYTYPE_LETTER_OR_SPACE : KEYTYPE_SPACE;
		}
		if (!have_letter) {
			return KEYTYPE_CUSTOM;
		}
		if (have_custom) {
			return KEYTYPE_LETTER_OR_CUSTOM;
		}
		return KEYTYPE_LETTER;
	}
	else {

		// old simple algo

		static const wxString spaces(L" \t-=+*()%!");
		if (spaces.Index(curSymbol) != -1) {
			return KEYTYPE_SPACE;
		}

		if (vk == VK_OEM_2) {
			return KEYTYPE_CUSTOM;
		}

		return KEYTYPE_LETTER;
	}

}