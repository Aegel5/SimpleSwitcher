#pragma once

using TKeyCode = WORD;

enum {
	VKE_1 = 0x31,
	VKE_2 = 0x32,
	VKE_3 = 0x33,
	VKE_V = 0x56,
	VKE_C = 67,
	VKE_BREAK = VK_PAUSE,
	VKE_ALT = VK_MENU,

	VKE_MAX = 2048,
	VKE_WIN = VKE_MAX - 1,
};

namespace _internal {
	class HotKeyNames {
		UStr vkMap[VKE_MAX] = { 0 };
		std::map <std::string, TKeyCode, std::less<> > mapCode;
		void Set(TKeyCode k, UStr s) {
			if (k >= std::ssize(vkMap)) {
				LOG_WARN("can't set code");
				return;
			}
			vkMap[k] = s;
			std::string sNameLower = s;
			StrUtils::ToLowerUnsafe(sNameLower);
			mapCode[std::move(sNameLower)] = k;
		}
	public:
		HotKeyNames() {
			Set(0x41, "A");
			Set(0x42, "B");
			Set(0x43, "C");
			Set(0x44, "D");
			Set(0x45, "E");
			Set(0x46, "F");
			Set(0x47, "G");
			Set(0x48, "H");
			Set(0x49, "I");
			Set(0x4A, "J");
			Set(0x4B, "K");
			Set(0x4C, "");
			Set(0x4D, "M");
			Set(0x4E, "N");
			Set(0x4F, "O");
			Set(0x50, "P");
			Set(0x51, "Q");
			Set(0x52, "R");
			Set(0x53, "S");
			Set(0x54, "T");
			Set(0x55, "U");
			Set(0x56, "V");
			Set(0x57, "W");
			Set(0x58, "X");
			Set(0x59, "Y");
			Set(0x5A, "Z");
			Set(0x30, "0");
			Set(0x31, "1");
			Set(0x32, "2");
			Set(0x33, "3");
			Set(0x34, "4");
			Set(0x35, "5");
			Set(0x36, "6");
			Set(0x37, "7");
			Set(0x38, "8");
			Set(0x39, "9");

			Set(VK_SHIFT, "Shift");
			Set(VK_LSHIFT, "LShift");
			Set(VK_RSHIFT, "RShift");
			Set(VK_CONTROL, "Ctrl");
			Set(VK_LCONTROL, "LCtrl");
			Set(VK_RCONTROL, "RCtrl");
			Set(VK_MENU, "Alt");
			Set(VK_LMENU, "LAlt");
			Set(VK_RMENU, "RAlt");
			Set(VK_LWIN, "LWin");
			Set(VK_RWIN, "RWin");
			Set(VKE_WIN, "Win");
			Set(VK_APPS, "Apps");
			Set(VK_ESCAPE, "Esc");

			Set(VK_NUMPAD0, "NUMPAD0");
			Set(VK_NUMPAD1, "NUMPAD1");
			Set(VK_NUMPAD2, "NUMPAD2");
			Set(VK_NUMPAD3, "NUMPAD3");
			Set(VK_NUMPAD4, "NUMPAD4");
			Set(VK_NUMPAD5, "NUMPAD5");
			Set(VK_NUMPAD6, "NUMPAD6");
			Set(VK_NUMPAD7, "NUMPAD7");
			Set(VK_NUMPAD8, "NUMPAD8");
			Set(VK_NUMPAD9, "NUMPAD9");
			Set(VK_MULTIPLY, "MULTIPLY");
			Set(VK_ADD, "ADD");
			Set(VK_SEPARATOR, "SEPARATOR");
			Set(VK_SUBTRACT, "SUBTRACT");
			Set(VK_DECIMAL, "DECIMA");
			Set(VK_DIVIDE, "DIVIDE");
			Set(VK_F1, "F1");
			Set(VK_F2, "F2");
			Set(VK_F3, "F3");
			Set(VK_F4, "F4");
			Set(VK_F5, "F5");
			Set(VK_F6, "F6");
			Set(VK_F7, "F7");
			Set(VK_F8, "F8");
			Set(VK_F9, "F9");
			Set(VK_F10, "F10");
			Set(VK_F11, "F11");
			Set(VK_F12, "F12");
			Set(VK_F13, "F13");
			Set(VK_F14, "F14");
			Set(VK_F15, "F15");
			Set(VK_F16, "F16");
			Set(VK_F17, "F17");
			Set(VK_F18, "F18");
			Set(VK_F19, "F19");
			Set(VK_F20, "F20");
			Set(VK_F21, "F21");
			Set(VK_F22, "F22");
			Set(VK_F23, "F23");
			Set(VK_F24, "F24");

			Set(VK_INSERT, "Insert");
			Set(VK_DELETE, "Delete");
			Set(VK_HOME, "Home");
			Set(VK_END, "End");
			Set(VK_NEXT, "PageDown");
			Set(VK_PRIOR, "PageUp");
			Set(VK_PAUSE, "Break");
			Set(VK_PRINT, "PrintScreen");
			Set(VK_SNAPSHOT, vkMap[VK_PRINT]);
			Set(VK_CANCEL, "Cancel");
			Set(VK_SCROLL, "ScrollLock");
			Set(VK_CAPITAL, "CapsLock");
			Set(VK_NUMLOCK, "NumLock");
			Set(VK_SPACE, "Space");
			Set(VK_BACK, "Backspace");
			Set(VK_RETURN, "Enter");
			Set(VK_TAB, "Tab");

			Set(VK_OEM_4, "OEM_4");
			Set(VK_OEM_5, "OEM_5");
			Set(VK_OEM_6, "OEM_6");
			Set(VK_OEM_7, "OEM_7");
			Set(VK_OEM_8, "OEM_8");

			Set(VK_OEM_AX, "OEM_AX");
			Set(VK_OEM_102, "OEM_102");

			Set(VK_OEM_1, "OEM_1");
			Set(VK_OEM_PLUS, "OEM_PLUS");
			Set(VK_OEM_COMMA, "OEM_COMMA");
			Set(VK_OEM_MINUS, "OEM_MINUS");
			Set(VK_OEM_PERIOD, "OEM_PERIOD");
			Set(VK_OEM_2, "OEM_2");
			Set(VK_OEM_3, "OEM_3");

			Set(VK_BROWSER_BACK, "BROWSER_BACK");
			Set(VK_BROWSER_FORWARD, "BROWSER_FORWARD");
			Set(VK_BROWSER_REFRESH, "BROWSER_REFRESH");
			Set(VK_BROWSER_STOP, "BROWSER_STOP");
			Set(VK_BROWSER_SEARCH, "BROWSER_SEARCH");
			Set(VK_BROWSER_FAVORITES, "BROWSER_FAVORITES");
			Set(VK_BROWSER_HOME, "BROWSER_HOME");
			Set(VK_VOLUME_MUTE, "VOLUME_MUTE");
			Set(VK_VOLUME_DOWN, "VOLUME_DOWN");
			Set(VK_VOLUME_UP, "VOLUME_UP");
			Set(VK_MEDIA_NEXT_TRACK, "MEDIA_NEXT_TRACK");
			Set(VK_MEDIA_PREV_TRACK, "MEDIA_PREV_TRACK");
			Set(VK_MEDIA_STOP, "MEDIA_STOP");
			Set(VK_MEDIA_PLAY_PAUSE, "MEDIA_PLAY_PAUSE");
			Set(VK_LAUNCH_MAIL, "LAUNCH_MAI");
			Set(VK_LAUNCH_MEDIA_SELECT, "LAUNCH_MEDIA_SELECT");
			Set(VK_LAUNCH_APP1, "LAUNCH_APP1");
			Set(VK_LAUNCH_APP2, "LAUNCH_APP2");
		}

		static HotKeyNames& Global() {
			static HotKeyNames hkNames;
			return hkNames;
		}

	public: UStr GetName(TKeyCode k) {
		if (k >= std::ssize(vkMap))	return nullptr;
		return vkMap[k];
	}
	TKeyCode GetCode(UStr s) { // need lower
		if (!s)	return 0;

		auto it = mapCode.find(s);
		if (it != mapCode.end())
			return it->second;

		return 0;
	}

	};
}
