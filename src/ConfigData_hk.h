enum HotKeyType : TUInt32 {
	hk_NULL,

	hk_RevertLastWord,
	hk_RevertSeveralWords,
	hk_RevertAllRecentText,
	hk_RevertSelelected,
	hk_EmulateCapsLock,
	hk_CycleSwitchLayout,
	hk_EmulateScrollLock,
	hk_toUpperSelected,
	hk_ToggleEnabled,
	hk_ShowMainWindow,
	hk_ShowRemainderWnd,
	hk_InsertWithoutFormat,

	hk_hotkeys_end,

	hk_SetLayout_flag = 1 << 30,
	hk_RunProgram_flag = 1 << 29,
};

consteval auto adl_enum_bounds(HotKeyType) -> simple_enum::adl_info<HotKeyType> {
	return { HotKeyType::hk_NULL, HotKeyType::hk_hotkeys_end }; // Assumes my_enum satisfies enum_concept
}

inline bool IsNeedSavedWords(HotKeyType hk_type) {
	return (Utils::is_in(hk_type, hk_RevertLastWord, hk_RevertSeveralWords, hk_RevertAllRecentText));
}

inline std::string_view HotKeyTypeName(HotKeyType hk_type) { return simple_enum::enum_name(hk_type); }


inline std::generator<CHotKey> GetHk_Defaults(HotKeyType hk) {
	switch (hk) {
	case hk_RevertLastWord:
		co_yield CHotKey(VK_CAPITAL);
		co_yield CHotKey(VK_PAUSE);
		co_yield CHotKey(VK_F24);
		co_return;
	case hk_RevertSeveralWords:
		co_yield CHotKey(VK_SHIFT, VK_CAPITAL);
		co_yield CHotKey(VK_SHIFT, VK_PAUSE);
		co_yield CHotKey(VK_SHIFT, VK_F24);
		co_return;
	case hk_RevertAllRecentText:
		co_yield CHotKey{ VK_CONTROL, VK_CAPITAL };
		co_yield CHotKey(VK_CONTROL, VK_F24);
		co_return;
	case hk_RevertSelelected:
		co_yield CHotKey{ VK_CONTROL, VK_CAPITAL };
		co_yield CHotKey(VK_CONTROL, VK_F24);
		co_return;
	case hk_CycleSwitchLayout:
		co_yield CHotKey(VKE_WIN, VK_SPACE);
		co_yield CHotKey(VK_LCONTROL, VK_LWIN);
		co_yield CHotKey(VK_SHIFT).SetDouble();
		co_return;
	case hk_toUpperSelected:
		co_yield CHotKey(VK_SCROLL);
		co_yield CHotKey(VK_PAUSE);
		co_return;
	case hk_ToggleEnabled:
		co_yield CHotKey(VKE_WIN, VK_F8);
		co_return;
	case hk_ShowMainWindow:
		co_yield CHotKey(VKE_WIN, VK_SHIFT).SetKeyup();
		co_yield CHotKey(VK_SHIFT).SetDouble();
		co_return;
	case hk_ShowRemainderWnd:
		co_yield CHotKey(VKE_BREAK);
		co_return;
	case hk_InsertWithoutFormat:
		co_yield CHotKey(VK_CONTROL, VK_SHIFT, VKE_V);
		co_return;
	case hk_EmulateCapsLock:
		co_yield CHotKey(VKE_ALT, VK_F24);
		co_return;
	}
}

inline const char* GetGuiTextForHk(HotKeyType hk) {
	switch (hk) {
	case hk_RevertLastWord: return LOC("Change layout for last word");
	case hk_RevertSeveralWords: return LOC("Change layout for last several words");
	case hk_RevertAllRecentText: return LOC("Change layout for all recent text");
	case hk_RevertSelelected: return LOC("Change layout for selected text");
	case hk_CycleSwitchLayout: return  LOC("Cyclic layout change");
	case hk_EmulateCapsLock: return LOC("Generate CapsLock");
	case hk_toUpperSelected: return LOC("Selected text to UPPER/lower");
	case hk_ToggleEnabled: return LOC("Enable/Disable");
	case hk_ShowMainWindow: return LOC("Show/hide main window");
	case hk_ShowRemainderWnd: return LOC("Show/hide Reminder");
	case hk_InsertWithoutFormat: return LOC("Paste text without formatting");
	}
	return "Error";
}
