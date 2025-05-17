namespace details {
	inline HK_Description descr[hk_hotkeys_end];
	inline Init() {
		descr[hk_RevertLastWord] = {
			.fUseDef = true,
			.def_list = { CHotKey(VK_CAPITAL), CHotKey(VK_PAUSE) },
		};
		descr[hk_RevertAllRecentText] = {
			.fUseDef = true,
			.def_list = { CHotKey{VK_CONTROL, VK_CAPITAL}},
		};
		descr[hk_RevertSelelected] = {
			.fUseDef = true,
			.def_list = { CHotKey{VK_CONTROL, VK_CAPITAL} },
		};
		descr[hk_CycleSwitchLayout] = {
				.def_list = { CHotKey(VKE_WIN, VK_SPACE), CHotKey(VK_LCONTROL,VK_LWIN), CHotKey(VK_SHIFT).SetDouble() },
		};
		descr[hk_toUpperSelected] = {
				.def_list = { CHotKey(VK_SCROLL), CHotKey(VK_PAUSE) },
		};
		descr[hk_ToggleEnabled] = {
				.fUseDef = true,
				.def_list = { CHotKey(VKE_WIN, VK_F8) },
		};
		descr[hk_ShowMainWindow] = {
				.fUseDef = true,
				.def_list = { CHotKey(VKE_WIN, VK_SHIFT).SetKeyup(), CHotKey(VK_SHIFT).SetDouble() },
		};
		descr[hk_ShowRemainderWnd] = {
				.def_list = { CHotKey(VKE_BREAK) },
		};
		descr[hk_InsertWithoutFormat] = {
				.def_list = { CHotKey(VK_CONTROL, VK_SHIFT, VKE_V) },
		};
	}
}
