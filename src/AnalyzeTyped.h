#pragma once

// todo: разделить на 2 функции 1: препроцессинг (символ или очистка) 2: реал аналайзинг (может быть lazy).
inline TKeyType AnalizeTyped(const CHotKey& key, UINT vk, const TScanCode_Ext& scan, const auto& get_lay, TKeyBaseInfo& data){

	auto count_keys = std::ranges::count_if(key, [](auto key) {
		return !CHotKey::IsKnownMods(key);
		});

	if (count_keys == 0) return KEYTYPE_COMMAND_NO_CLEAR;

	auto size = key.Size() - count_keys + 1;

	bool is_shift = key.HasMod(VK_SHIFT);

	if (size > 2) {
		return KEYTYPE_COMMAND_CLEAR;
	}

	if (size == 2) {
		if (!is_shift)
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

	if (vk >= VK_F1 && vk <= VK_F24) {
		return KEYTYPE_COMMAND_CLEAR;
	}

	GETCONF;

	HKL lay = get_lay();

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
	if (StrUtils::isDigit(curSymbol)) {
		return KEYTYPE_LETTER_OR_SPACE; 
	}

	if (cfg->separate_ext_mode == SeparateExtMode::Disabled) {
		return Utils::is_in(curSymbol, '\n', ' ', '\t', '\r') ? KEYTYPE_SPACE : KEYTYPE_LETTER;
	}

	if (cfg->layouts_info.CntLayoutEnabled() <= 3) {

		auto is_letter = [&cfg](wchar_t c) {return cfg->treat_as_letters.contains(c) || StrUtils::isLetter(c); };

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
			if (curSymbol == L'-') {
				data.space_on_extended = true; // '-' всегда является пробелом, если extended separation
			}
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

		if (StrUtils::IsSpace(curSymbol)) 
			return KEYTYPE_SPACE;

		if (vk == VK_OEM_2) {
			return KEYTYPE_CUSTOM;
		}

		return KEYTYPE_LETTER;
	}

}
