#pragma once

inline TKeyType AnalizeTyped(CHotKey key, UINT vk, UINT scan, HKL lay){

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
	case VK_CAPITAL:
	case VK_SCROLL:
	case VK_PRINT:
	case VK_NUMLOCK:
	case VK_INSERT:
		return KEYTYPE_COMMAND_NO_CLEAR;
	case VK_RETURN:
		return KEYTYPE_COMMAND_CLEAR;
	case VK_TAB:
	case VK_SPACE:
		return KEYTYPE_SPACE;
	case VK_BACK:
		return KEYTYPE_BACKSPACE;
	}

	auto curSymbol = Utils::TypedToChar(vk, scan, is_shift, lay);
	if (curSymbol == 0)
		return KEYTYPE_COMMAND_CLEAR;

	LOG_ANY_4(L"char {}", curSymbol);

	if (Str_Utils::isDigit(curSymbol)) {
		return KEYTYPE_SYMBOL; // никогда не являются разделителями
	}

	bool is_letter = Str_Utils::isLetter(curSymbol);
	bool have_changes = false;

	if (conf_get()->layouts_info.CntLayoutEnabled() <= 3) {

		// проверим возможные варианты для символа.

		for (const auto& it : conf_get()->layouts_info.EnabledLayouts()) {
			if (it != lay) {
				auto symb = Utils::TypedToChar(vk, scan, is_shift, it);
				if (symb != curSymbol) {
					have_changes = true;
				}
				if (Str_Utils::isLetter(symb)) {
					is_letter = true;
				}
			}
		}
		if (!have_changes) {
			return KEYTYPE_SPACE;
		}
		if (!is_letter) {
			return KEYTYPE_CUSTOM;
		}
		static const wxString leading(L"'\"<[{");
		if (leading.Index(curSymbol) != -1) {
			return KEYTYPE_LEADING_POSSIBLE_LETTER;
		}
		return KEYTYPE_SYMBOL;
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

		return KEYTYPE_SYMBOL;
	}

}