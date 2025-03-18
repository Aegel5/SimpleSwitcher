#pragma once

//#include "swlocal.h"

using TKeyCode = WORD;

enum {
	VK_E_1 = 0x31,
	VK_E_2 = 0x32,
	VK_E_3 = 0x33,
	VK_E_CTX_MENU = 0x5D
};

class HotKeyNames
{
	std::vector<TStr> vkMap{ 256, nullptr };
	std::map <std::wstring , TKeyCode > mapCode;
	void Set(TKeyCode k, TStr s)
	{
		if (k >= vkMap.size()) {
			LOG_WARN(L"can't set code");
			return;
		}
		vkMap[k] = s;
	}
public:
	
	HotKeyNames()
	{
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

		Set(0x5D, L"MENU");

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

		GenerateMap();
	}

	static HotKeyNames& Global()
	{
		static HotKeyNames hkNames;
		return hkNames;
	}

	std::map<TKeyCode, std::wstring> unknownNames;

	TStr GetName(TKeyCode k)
	{
		if (k >= vkMap.size() || vkMap[k] == nullptr) {
			return L"UNKNOWN";
		}

		return vkMap[k];
	}

	void GenerateMap()
	{
		if (!mapCode.empty())
			return;
		for(int i = 0; i < vkMap.size(); ++i)
		{
			TStr sName = vkMap[i];
			if (!sName) {
				std::wostringstream ss;
				ss << L"KEY_" << std::hex << i;
				unknownNames[i] = ss.str(); // холдер.
				sName = unknownNames[i].c_str();
				vkMap[i] = sName;
			}
			std::wstring sNameLower = sName;
			Str_Utils::ToLower(sNameLower);
			mapCode[sNameLower] = i;
		}

		//mapCode[L"f24"] = VK_F24;
	}

	TKeyCode GetCode(TStr s) // need lower
	{
		if (!s)
			return 0;

		auto it = mapCode.find(s);
		if (it == mapCode.end())
			return 0;
		return it->second;
	}

};

class CHotKey
{
	
public:
	CHotKey() { Clear(); }
	explicit CHotKey(TKeyCode key)
	{
		Clear().Add3(key);
	}
	CHotKey(TKeyCode key1, TKeyCode key2)
	{
		Clear().Add3(key1).Add3(key2);
	}
	CHotKey(TKeyCode key1, TKeyCode key2, TKeyCode key3)
	{
		Clear().Add3(key1).Add3(key2).Add3(key3);
	}
	enum 
	{
		ADDKEY_NORMAL = 0,
		ADDKEY_CHECK_EXIST = 0b1,
		ADDKEY_ENSURE_ONE_VALUEKEY = 0b10,
		ADDKEY_CHECK_MODS = 0b100,
	};
	CHotKey& Simple_Append(TKeyCode key) {
		if (size < c_MAX)
			++size;
		for (int i = size - 1; i > 0; i--)
		{
			keys[i] = keys[i - 1];
		}
		keys[0] = key;
		return *this;
	}
	CHotKey& Add3(TKeyCode key, int flags = ADDKEY_NORMAL)
	{
		if (TestFlag(flags, ADDKEY_CHECK_EXIST)) {
			for (TKeyCode k : *this) {
				if (CompareKeys(k, key, true))
				{
					if (IsCommonMods(k) && !IsCommonMods(key))
					{
						// rewrite common key
						Remove(k);
						break;
					}
					else
					{
						// already exists
						return *this;
					}
				}
			}
		}

		if (TestFlag(flags, ADDKEY_ENSURE_ONE_VALUEKEY) && !CHotKey::IsKnownMods(key))
		{
			RemoveAllNoMods();
		}

		if (TestFlag(flags, ADDKEY_CHECK_MODS))
		{
			if (size == 0)
			{
				keys[size++] = key;
			}
			else
			{
				if (IsKnownMods(keys[0]))
				{
					InsertMods(keys[0]);
					keys[0] = key;
				}
				else
				{
					InsertMods(key);
				}
			}
		}
		else
		{
			Simple_Append(key);
		}

		return *this;
	}
	bool Remove(TKeyCode key, bool strick_modifier=true)
	{
		if(size == 0)
			return false;
		bool found = false;
		for (int i = 0; i < size; i++)
		{
			if(CompareKeys(key, keys[i], strick_modifier))
			{
				found = true;
				keys[i] = 0;
				continue;
			}
			if(found)
			{
				keys[i-1] = keys[i];
			}
		}
		if(found)
			--size;
		return found;
		
	}
	void RemoveAllNoMods()
	{
		TKeyCode k = 0;
		do
		{
			k = 0;
			for(TKeyCode cur: *this)
			{
				if(!IsKnownMods(cur))
				{
					k = cur;
					break;
				}
			}
			if(k)
			{
				Remove(k);
			}
		}while (k);
	}
	bool HasKey(TKeyCode key, bool strick_modifier) const
	{
		for(TKeyCode k : *this)
		{
			if(CompareKeys(k, key, strick_modifier))
				return true;
		}
		return false;
	}
	bool HasAnyMod()
	{
		for (int i = 0; i < size; ++i)
		{
			if(IsKnownMods(keys[i]))
				return true;
		}
		return false;
	}
	bool HasAllMod()
	{
		for (int i = 0; i < size; ++i)
		{
			if (!IsKnownMods(keys[i]))
				return false;
		}
		return true;
	}
	TKeyCode At(int i) {return keys[i];}
	bool HasMod(TKeyCode k, bool strick_modifier = false) const
	{
		if(size <= 1)
			return false;
		for(int i = 1; i < size; ++i)
		{
			if (CompareKeys(k, keys[i], strick_modifier))
				return true;
		}
		return false;
	}
	enum TCompareFlags
	{
		COMPARE_NORMAL = 0,
		COMPARE_IGNORE_ORDER_VALUEKEY = 0x1,
		//COMPARE_IGNORE_HOLD = 0x2,
		COMPARE_IGNORE_KEYUP = 0x4,
		COMPARE_STRICK_MODIFIER = 0x8,
	};
	bool Compare (const CHotKey& other, int flags = COMPARE_NORMAL) const
	{
		if(size != other.size)
			return false;
		if(size == 0)
			return false;
		//if (!TestFlag(flags, COMPARE_IGNORE_HOLD) && m_hold != other.m_hold)
		//	return false;
		if (!TestFlag(flags, COMPARE_IGNORE_KEYUP) && m_keyup != other.m_keyup)
			return false;

		bool strick_modifier = TestFlag(flags, COMPARE_STRICK_MODIFIER);

		if (TestFlag(flags, COMPARE_IGNORE_ORDER_VALUEKEY) || m_ignoreOrderValueKey || other.m_ignoreOrderValueKey)
		{
			return CompareIgnoreOrder(keys, other.keys, size, strick_modifier);
		}
		else
		{
			if (!CompareKeys(keys[0], other.keys[0], strick_modifier))
				return false;
			return CompareIgnoreOrder(keys + 1, other.keys + 1, size - 1, strick_modifier);
		}
	}
	bool IsEmpty() const {return Size() == 0;}
	TKeyCode ValueKey() const	{		return keys[0];	}
	TKeyCode* begin() {return keys;}
	TKeyCode* end() {return keys+size;}
	const TKeyCode* begin() const { return keys; }
	const TKeyCode* end() const { return keys + size; }
	TKeyCode* ModsBegin() {		return keys + 1;	}
	TKeyCode* ModsEnd() {		return keys + 1 + (size > 0 ? size-1 : 0);	}
	TUInt8 Size() const  {return size;}


	//std::string ToString2() const
	//{
	//	auto res = ToString();
	//	std::string res2;
	//	IFS_LOG(Str_Utils::WideToUtf8(res.c_str(), res2));
	//	return res2;
	//}

	std::wstring ToString() const
	{
		std::wstring s;
		if (size == 0)
			return s;

		for(int i = size - 1; i >= 0; --i)
		{
			ToString(keys[i], s);
			if(i != 0)
				s += L" + ";
		}
		//if (m_hold)
		//{
		//	s += L" #hold";
		//}
		if (m_keyup)
		{
			s += L" #up";
		}
		return s;
	}
	static std::wstring ToString(TKeyCode key)
	{
		std::wstring s;
		ToString(key, s);
		return s;
	}
	static TStr GetName(TKeyCode key) {
		return HotKeyNames::Global().GetName(key);
	}
	static void ToString(TKeyCode key, std::wstring& s)
	{
		const wchar_t* sName = HotKeyNames::Global().GetName(key);
		if (sName)
		{
			s += sName;
		}
		else
		{
			s += L"VK_";
			s += std::to_wstring(key);
		}
	}
	static bool IsKnownMods(TKeyCode key)
	{
		switch (Normalize(key))
		{
		case VK_SHIFT:
		case VK_CONTROL:
		case VK_MENU:
		case VK_LWIN:
			return true;
		}
		return false;
	}
	static bool IsRightMod(TKeyCode key)
	{
		switch (key)
		{
		case VK_RSHIFT:
		case VK_RCONTROL:
		case VK_RMENU:
		case VK_RWIN:
			return true;
		}
		return false;
	}
	CHotKey& Clear()
	{
		SwZeroMemory(*this);
		return *this;
	}

	bool operator== (const CHotKey& other) = delete;
	bool operator!= (const CHotKey& other) = delete;

	void NormalizeAll() {
		for (int i = 0; i < Size(); i++)
		{
			keys[i] = Normalize(keys[i]);
		}
	}
	static TKeyCode Normalize(TKeyCode key) 
	{
		switch (key)
		{
		case VK_RSHIFT:
		case VK_LSHIFT:
			return VK_SHIFT;
		case VK_RCONTROL:
		case VK_LCONTROL:
			return VK_CONTROL;
		case VK_RMENU:
		case VK_LMENU:
			return VK_MENU;
		case VK_RWIN:
			return VK_LWIN;
		default:
			return key;
		}
	}
	CHotKey& SetKeyup(bool val=true)
	{
		m_keyup = val;
		return *this;
	}
	bool GetKeyup () const
	{
		return m_keyup;
	}
	TStatus FromString(const std::string& s)
	{

		std::wstring s2;
		IFS_RET(Str_Utils::Utf8ToWide(s.c_str(), s2));
		IFS_RET(FromString(s2));
		RETURN_SUCCESS;
	}
	TStatus FromString(const std::wstring& s)
	{
		Clear();

		if (s.empty())
			RETURN_SUCCESS;

		std::wstring ss = s;

		if (Str_Utils::replaceAll(ss, L"#up", L"")) {
			SetKeyup(true);
		}

		//if (Str_Utils::replaceAll(ss, L"#hold", L"")) {
		//	SetHold(true);
		//}

		Str_Utils::TVectStr sElems;
		Str_Utils::Split2(ss, sElems, L"+", true);
		for (auto& it : sElems)
		{
			std::wstring sCur = it;
			Str_Utils::trim(sCur);
			Str_Utils::ToLower(sCur);

			TKeyCode kCur = HotKeyNames::Global().GetCode(sCur.c_str());
			if (kCur == 0)
			{
				Clear();
				IFS_RET(SW_ERR_INVALID_PARAMETR, L"Not found keycode for %s", sCur.c_str());
			}

			Simple_Append(kCur);
		}

		RETURN_SUCCESS;
	}
	bool Has_left_right() const {
		for (size_t i = 0; i < Size(); i++)
		{
			auto cur = keys[i];
			if (Utils::is_in(keys[i], VK_LSHIFT, VK_RSHIFT, VK_LCONTROL, VK_RCONTROL, VK_LMENU, VK_RMENU, VK_LWIN, VK_RWIN)) {
				return true;
			}
		}
		return false;
	}
private:
	bool CompareIgnoreOrder(const TKeyCode* list1, const TKeyCode* list2, int size, bool strick_modifier) const
	{
		for(int i = 0; i < size; ++i)
		{
			TKeyCode k = list1[i];
			bool found = false;
			for (int j = 0; j < size; ++j)
			{
				if (CompareKeys(k, list2[j], strick_modifier))
				{
					found = true;
					break;
				}
			}
			if(!found)
				return false;
		}
		return true;
	}
	void InsertMods(TKeyCode key)
	{
		keys[size++] = key;
		if(size >= c_MAX)
			size = c_MAX - 1;
	}


	bool CompareKeys (TKeyCode k1, TKeyCode k2, bool strick_modifier) const	{

		if (strick_modifier) {
			return k1 == k2;
		}

		TKeyCode k1norm = Normalize(k1);
		TKeyCode k2norm = Normalize(k2);

		if(k1norm != k2norm)
			return false;

		if (IsCommonMods(k1) || IsCommonMods(k2))
			return true;

		return k1 == k2;
	}
	bool IsCommonMods(TKeyCode key) const
	{
		switch (key)
		{
		case VK_SHIFT:
		case VK_CONTROL:
		case VK_MENU:
			return true;
		}
		return false;
	}
	static const int c_MAX = 6;
	struct
	{
		TUInt8 m_ignoreOrderValueKey : 1;
		TUInt8 m_keyup : 1;
		//TUInt8 m_hold : 1;
	};
	TUInt8 size;
	TKeyCode keys[c_MAX];
};

inline CHotKey ParseStringHK(std::wstring& s)
{
	CHotKey hk;
	IFS_LOG(hk.FromString(s));
	return hk;
}