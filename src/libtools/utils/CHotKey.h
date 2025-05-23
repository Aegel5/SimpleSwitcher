﻿#pragma once

#include "VkNames.h"

class CHotKey {

public:
	CHotKey() {}
	explicit CHotKey(TKeyCode key) {Add(key);}
	CHotKey(TKeyCode key1, TKeyCode key2) {	Add(key1).Add(key2);}
	CHotKey(TKeyCode key1, TKeyCode key2, TKeyCode key3) { Add(key1).Add(key2).Add(key3);}
	enum {
		ADDKEY_NORMAL = 0,
		ADDKEY_CHECK_EXIST = 0b1,
		ADDKEY_ENSURE_ONE_VALUEKEY = 0b10,
		ADDKEY_NO_STRICK_MODS_CHECK = 0b100,
	};
	CHotKey& Simple_Append(TKeyCode key) {
		if (size < c_MAX)
			++size;
		for (int i = size - 1; i > 0; i--) {
			keys[i] = keys[i - 1];
		}
		keys[0] = key;
		return *this;
	}
	CHotKey& Add(TKeyCode key, int flags = ADDKEY_NORMAL) {
		if (TestFlag(flags, ADDKEY_CHECK_EXIST)) {
			for (TKeyCode k : *this) {
				if (CompareKeys(k, key, !TestFlag(flags, ADDKEY_NO_STRICK_MODS_CHECK))) {
					if (IsCommonMods(k) && !IsCommonMods(key)) {
						// rewrite common key
						RemoveFirst(k);
						break;
					}
					else {
						// already exists
						return *this;
					}
				}
			}
		}

		if (TestFlag(flags, ADDKEY_ENSURE_ONE_VALUEKEY) && !CHotKey::IsKnownMods(key)) {
			RemoveAllNoMods();
		}

		Simple_Append(key);
		return *this;
	}
	bool RemoveFirst(TKeyCode key, bool strick_modifier = true) {

		if (size == 0)
			return false;

		bool found = false;

		for (int i = 0; i < size; i++) {
			if (!found){
				if (CompareKeys(key, keys[i], strick_modifier)) {
					found = true;
				}
			}
			else {
				keys[i - 1] = keys[i];
			}
		}

		if (found) {
			--size;
			keys[size] = 0;
		}
		return found;

	}
	void RemoveAllNoMods() {
		TKeyCode k = 0;
		do {
			k = 0;
			for (TKeyCode cur : *this) {
				if (!IsKnownMods(cur)) {
					k = cur;
					break;
				}
			}
			if (k) {
				RemoveFirst(k);
			}
		} while (k);
	}
	bool HasKey(TKeyCode key, bool strick_modifier) const {
		for (TKeyCode k : *this) {
			if (CompareKeys(k, key, strick_modifier))
				return true;
		}
		return false;
	}
	TKeyCode At(int i) { return keys[i]; }
	bool HasMod(TKeyCode k, bool strick_modifier = false) const {
		if (size <= 1)
			return false;
		for (int i = 1; i < size; ++i) {
			if (CompareKeys(k, keys[i], strick_modifier))
				return true;
		}
		return false;
	}
	enum TCompareFlags {
		COMPARE_NORMAL = 0,
		COMPARE_IGNORE_KEYUP = 1<<0,
		COMPARE_IGNORE_DOUBLE = 1<<1,
		COMPARE_STRICK_MODIFIER = 1<<2,
	};
	bool Compare(const CHotKey& other, int flags = COMPARE_NORMAL) const {
		if (size != other.size)
			return false;
		if (size == 0)
			return false;
		if (!TestFlag(flags, COMPARE_IGNORE_KEYUP) && m_keyup != other.m_keyup)
			return false;
		if (!TestFlag(flags, COMPARE_IGNORE_DOUBLE) && m_double_press != other.m_double_press)
			return false;

		bool strick_modifier = TestFlag(flags, COMPARE_STRICK_MODIFIER);

		if (!CompareKeys(keys[0], other.keys[0], strick_modifier))
			return false;
		return CompareIgnoreOrder(keys + 1, other.keys + 1, size - 1, strick_modifier);
	}
	bool IsEmpty() const { return Size() == 0; }
	TKeyCode ValueKey() const { return keys[0]; }

	auto* begin(this auto&& self) { return self.keys; }
	auto* end(this auto&& self) { return self.keys + self.size; }

	TKeyCode* ModsBegin() { return keys + 1; }
	TKeyCode* ModsEnd() { return keys + 1 + (size > 0 ? size - 1 : 0); }
	int Size() const { return size; }

	std::wstring ToString() const {
		std::wstring s;
		if (size == 0)
			return s;

		for (int i = size - 1; i >= 0; --i) {
			AddToString(keys[i], s);
			if (i != 0)
				s += L" + ";
		}
		if (m_keyup) {
			s += L" #up";
		}
		if (m_double_press) {
			s += L" #double";
		}
		return s;
	}
	static std::wstring ToString(TKeyCode key) {
		std::wstring s;
		AddToString(key, s);
		return s;
	}
private: static void AddToString(TKeyCode key, std::wstring& s) {
		const wchar_t* sName = _internal::HotKeyNames::Global().GetName(key);
		if (sName) {
			s += sName;
		}
		else {
			s += std::format(L"VK_{:x}", key);
		}
	}
public: 
	static bool IsKnownMods(TKeyCode key) { return Utils::is_in(Normalize(key), VK_SHIFT, VK_CONTROL, VK_MENU, VKE_WIN); }
	static bool IsRightMod(TKeyCode key) { return Utils::is_in(key, VK_RSHIFT, VK_RCONTROL, VK_RMENU, VK_RWIN); }
	CHotKey& Clear() { SwZeroMemory(*this);	return *this; }
	bool operator== (const CHotKey& other) = delete;
	bool operator!= (const CHotKey& other) = delete;
	void NormalizeAll() { for (auto& it : *this) it = Normalize(it); }

	static TKeyCode Normalize(TKeyCode key) {
		switch (key) {
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
		case VK_LWIN:
			return VKE_WIN;
		default:
			return key;
		}
	}
	CHotKey& SetKeyup(bool val = true) { m_keyup = val;	return *this; }
	bool GetKeyup() const { return m_keyup; }
	static CHotKey FromString(SView s) {
		CHotKey key;
		if (s.empty()) 
			return key;
		auto sElems = StrUtils::Split(s, L'+');
		for (auto& sCur : sElems) {
			StrUtils::ToLower(sCur);
			if (StrUtils::replaceAll(sCur, L"#up", L"")) { key.SetKeyup(true); }
			if (StrUtils::replaceAll(sCur, L"#double", L"")) { key.SetDouble(true); }
			StrUtils::trim(sCur);
			TKeyCode kCur = _internal::HotKeyNames::Global().GetCode(sCur.c_str());
			if (kCur == 0) {
				auto split = StrUtils::Split(sCur, L'_');
				if (split.size() == 2) {
					StrUtils::ToInt(split[1].c_str(), kCur, 16);
				}
			}
			if (kCur == 0) {
				key.Clear();
				LOG_WARN(L"Not found keycode for {}", sCur);
				return key;
			}
			key.Simple_Append(kCur);
		}
		return key;
	}
	bool Has_left_right() const { return std::any_of(begin(), end(), [](auto v) {return v != Normalize(v); });}

private:
	bool CompareIgnoreOrder(const TKeyCode* list1, const TKeyCode* list2, int size, bool strick_modifier) const {
		for (int i = 0; i < size; ++i) {
			TKeyCode k = list1[i];
			bool found = false;
			for (int j = 0; j < size; ++j) {
				if (CompareKeys(k, list2[j], strick_modifier)) {
					found = true;
					break;
				}
			}
			if (!found)
				return false;
		}
		return true;
	}
	void InsertMods(TKeyCode key) {
		if (size < c_MAX)
			size++;
		keys[size-1] = key;
	}


	static bool CompareKeys(TKeyCode k1, TKeyCode k2, bool strick_modifier) {

		if (strick_modifier) {
			return k1 == k2;
		}

		TKeyCode k1norm = Normalize(k1);
		TKeyCode k2norm = Normalize(k2);

		if (k1norm != k2norm)
			return false;

		if (IsCommonMods(k1) || IsCommonMods(k2))
			return true;

		return k1 == k2;
	}
	static bool IsCommonMods(TKeyCode key) { return Utils::is_in(key, VK_SHIFT, VK_CONTROL, VK_MENU, VKE_WIN); }

public: TStatus RegisterHk(CAutoHotKeyRegister& registor, HWND hwnd, int id) {
		registor.Cleanup();
		if (IsEmpty()) RETURN_SUCCESS;
		UINT mods = MOD_NOREPEAT;
		UINT vk = ValueKey();
		for (TKeyCode* k = ModsBegin(); k != ModsEnd(); ++k) {
			auto norm = Normalize(*k);
			auto cur = *k;
			if (cur == VK_SHIFT) {
				mods |= MOD_SHIFT;
			}
			else if (cur == VK_CONTROL) {
				mods |= MOD_CONTROL;
			}
			else if (cur == VK_MENU) {
				mods |= MOD_ALT;
			}
			else if (cur == VKE_WIN) {
				mods |= MOD_WIN;
			}
			else {
				return SW_ERR_UNSUPPORTED;
			}
		}
		IFW_RET(registor.Register(hwnd, id, mods, vk));
		RETURN_SUCCESS;
	}
public: bool IsDouble() const { return m_double_press; }
public: auto& SetDouble(bool val = true) { m_double_press = val; return *this; }
public: bool IsEnabled() const { return !m_disabled; }
public: void SetEnabled(bool val = true) { m_disabled = !val; }
private:
	static constexpr int c_MAX = 5;
	struct {
		TUInt8 m_keyup : 1 {};
		TUInt8 m_double_press : 1 {};
		TUInt8 m_disabled : 1 {};
	};
	TUInt8 size = 0;
	TKeyCode keys[c_MAX] = { 0 };
};
static_assert(sizeof(CHotKey) == 12);

