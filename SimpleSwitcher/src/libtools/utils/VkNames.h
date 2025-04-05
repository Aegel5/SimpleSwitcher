#pragma once

using TKeyCode = WORD;

enum {
	VKE_1 = 0x31,
	VKE_2 = 0x32,
	VKE_3 = 0x33,
	VKE_BREAK = VK_PAUSE,

	VKE_MAX = 2048,
	VKE_WIN = VKE_MAX - 1,
};

namespace _internal {
	class HotKeyNames {
		TStr vkMap[VKE_MAX] = { 0 };
		std::map <std::wstring, TKeyCode, std::less<> > mapCode;
		void Set(TKeyCode k, TStr s) {
			if (k >= std::ssize(vkMap)) {
				LOG_WARN(L"can't set code");
				return;
			}
			vkMap[k] = s;
			std::wstring sNameLower = s;
			Str_Utils::ToLower(sNameLower);
			mapCode[std::move(sNameLower)] = k;
		}
	public:
		HotKeyNames() {
			Set(0x41, L"A");
			Set(0x42, L"B");
			Set(0x43, L"C");
			Set(0x44, L"D");
			Set(0x45, L"E");
			Set(0x46, L"F");
			Set(0x47, L"G");
			Set(0x48, L"H");
			Set(0x49, L"I");
			Set(0x4A, L"J");
			Set(0x4B, L"K");
			Set(0x4C, L"L");
			Set(0x4D, L"M");
			Set(0x4E, L"N");
			Set(0x4F, L"O");
			Set(0x50, L"P");
			Set(0x51, L"Q");
			Set(0x52, L"R");
			Set(0x53, L"S");
			Set(0x54, L"T");
			Set(0x55, L"U");
			Set(0x56, L"V");
			Set(0x57, L"W");
			Set(0x58, L"X");
			Set(0x59, L"Y");
			Set(0x5A, L"Z");
			Set(0x30, L"0");
			Set(0x31, L"1");
			Set(0x32, L"2");
			Set(0x33, L"3");
			Set(0x34, L"4");
			Set(0x35, L"5");
			Set(0x36, L"6");
			Set(0x37, L"7");
			Set(0x38, L"8");
			Set(0x39, L"9");

			Set(VK_SHIFT, L"Shift");
			Set(VK_LSHIFT, L"LShift");
			Set(VK_RSHIFT, L"RShift");
			Set(VK_CONTROL, L"Ctrl");
			Set(VK_LCONTROL, L"LCtrl");
			Set(VK_RCONTROL, L"RCtrl");
			Set(VK_MENU, L"Alt");
			Set(VK_LMENU, L"LAlt");
			Set(VK_RMENU, L"RAlt");
			Set(VK_LWIN, L"LWin");
			Set(VK_RWIN, L"RWin");
			Set(VKE_WIN, L"Win");
			Set(VK_APPS, L"Apps");
			Set(VK_ESCAPE, L"Esc");

			Set(VK_NUMPAD0, L"NUMPAD0");
			Set(VK_NUMPAD1, L"NUMPAD1");
			Set(VK_NUMPAD2, L"NUMPAD2");
			Set(VK_NUMPAD3, L"NUMPAD3");
			Set(VK_NUMPAD4, L"NUMPAD4");
			Set(VK_NUMPAD5, L"NUMPAD5");
			Set(VK_NUMPAD6, L"NUMPAD6");
			Set(VK_NUMPAD7, L"NUMPAD7");
			Set(VK_NUMPAD8, L"NUMPAD8");
			Set(VK_NUMPAD9, L"NUMPAD9");
			Set(VK_MULTIPLY, L"MULTIPLY");
			Set(VK_ADD, L"ADD");
			Set(VK_SEPARATOR, L"SEPARATOR");
			Set(VK_SUBTRACT, L"SUBTRACT");
			Set(VK_DECIMAL, L"DECIMAL");
			Set(VK_DIVIDE, L"DIVIDE");
			Set(VK_F1, L"F1");
			Set(VK_F2, L"F2");
			Set(VK_F3, L"F3");
			Set(VK_F4, L"F4");
			Set(VK_F5, L"F5");
			Set(VK_F6, L"F6");
			Set(VK_F7, L"F7");
			Set(VK_F8, L"F8");
			Set(VK_F9, L"F9");
			Set(VK_F10, L"F10");
			Set(VK_F11, L"F11");
			Set(VK_F12, L"F12");
			Set(VK_F13, L"F13");
			Set(VK_F14, L"F14");
			Set(VK_F15, L"F15");
			Set(VK_F16, L"F16");
			Set(VK_F17, L"F17");
			Set(VK_F18, L"F18");
			Set(VK_F19, L"F19");
			Set(VK_F20, L"F20");
			Set(VK_F21, L"F21");
			Set(VK_F22, L"F22");
			Set(VK_F23, L"F23");
			Set(VK_F24, L"F24");

			Set(VK_INSERT, L"Insert");
			Set(VK_DELETE, L"Delete");
			Set(VK_HOME, L"Home");
			Set(VK_END, L"End");
			Set(VK_NEXT, L"PageDown");
			Set(VK_PRIOR, L"PageUp");
			Set(VK_PAUSE, L"Break");
			Set(VK_PRINT, L"PrintScreen");
			Set(VK_SNAPSHOT, vkMap[VK_PRINT]);
			Set(VK_CANCEL, L"Cancel");
			Set(VK_SCROLL, L"ScrollLock");
			Set(VK_CAPITAL, L"CapsLock");
			Set(VK_NUMLOCK, L"NumLock");
			Set(VK_SPACE, L"Space");
			Set(VK_BACK, L"Backspace");
			Set(VK_RETURN, L"Enter");
			Set(VK_TAB, L"Tab");

			Set(VK_OEM_4, L"OEM_4");
			Set(VK_OEM_5, L"OEM_5");
			Set(VK_OEM_6, L"OEM_6");
			Set(VK_OEM_7, L"OEM_7");
			Set(VK_OEM_8, L"OEM_8");

			Set(VK_OEM_AX, L"OEM_AX");
			Set(VK_OEM_102, L"OEM_102");

			Set(VK_OEM_1, L"OEM_1");
			Set(VK_OEM_PLUS, L"OEM_PLUS");
			Set(VK_OEM_COMMA, L"OEM_COMMA");
			Set(VK_OEM_MINUS, L"OEM_MINUS");
			Set(VK_OEM_PERIOD, L"OEM_PERIOD");
			Set(VK_OEM_2, L"OEM_2");
			Set(VK_OEM_3, L"OEM_3");

			Set(VK_BROWSER_BACK, L"BROWSER_BACK");
			Set(VK_BROWSER_FORWARD, L"BROWSER_FORWARD");
			Set(VK_BROWSER_REFRESH, L"BROWSER_REFRESH");
			Set(VK_BROWSER_STOP, L"BROWSER_STOP");
			Set(VK_BROWSER_SEARCH, L"BROWSER_SEARCH");
			Set(VK_BROWSER_FAVORITES, L"BROWSER_FAVORITES");
			Set(VK_BROWSER_HOME, L"BROWSER_HOME");
			Set(VK_VOLUME_MUTE, L"VOLUME_MUTE");
			Set(VK_VOLUME_DOWN, L"VOLUME_DOWN");
			Set(VK_VOLUME_UP, L"VOLUME_UP");
			Set(VK_MEDIA_NEXT_TRACK, L"MEDIA_NEXT_TRACK");
			Set(VK_MEDIA_PREV_TRACK, L"MEDIA_PREV_TRACK");
			Set(VK_MEDIA_STOP, L"MEDIA_STOP");
			Set(VK_MEDIA_PLAY_PAUSE, L"MEDIA_PLAY_PAUSE");
			Set(VK_LAUNCH_MAIL, L"LAUNCH_MAIL");
			Set(VK_LAUNCH_MEDIA_SELECT, L"LAUNCH_MEDIA_SELECT");
			Set(VK_LAUNCH_APP1, L"LAUNCH_APP1");
			Set(VK_LAUNCH_APP2, L"LAUNCH_APP2");
		}

		static HotKeyNames& Global() {
			static HotKeyNames hkNames;
			return hkNames;
		}

	public: TStr GetName(TKeyCode k) {
		if (k >= std::ssize(vkMap))	return nullptr;
		return vkMap[k];
	}
	TKeyCode GetCode(TStr s) { // need lower
		if (!s)	return 0;

		auto it = mapCode.find(s);
		if (it != mapCode.end())
			return it->second;

		return 0;
	}

	};
}