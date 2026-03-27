#pragma once

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
		if (size < c_MAX) ++size;
		keys[size - 1] = key;
		return *this;
	}
	CHotKey& Add(TKeyCode key, int flags = ADDKEY_NORMAL) {

		if (TestFlag(flags, ADDKEY_CHECK_EXIST)) {
			for (TKeyCode k : *this) {
				if (CompareKeys(k, key, !TestFlag(flags, ADDKEY_NO_STRICK_MODS_CHECK))) {
					if (IsCommonMods(k) && !IsCommonMods(key)) {
						// rewrite common key
						Remove(k);
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
			Remove_if([](auto k) { return !IsKnownMods(k); });
		}

		Simple_Append(key);
		return *this;
	}

	bool Remove(TKeyCode key, bool strick_modifier = true) {
		return Remove_if([key, strick_modifier](auto k) { return CompareKeys(key, k, strick_modifier); }) != 0;
	}
	bool HasKey(TKeyCode key, bool strick_modifier) const {
		for (TKeyCode k : *this) {
			if (CompareKeys(k, key, strick_modifier))
				return true;
		}
		return false;
	}
	//TKeyCode At(int i) { return keys[i]; }
	bool HasMod(TKeyCode k, bool strick_modifier = false) const {
		for (int i = 0; i < size-1; i++) {
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

		if (!CompareKeys(key(), other.key(), strick_modifier))
			return false;
		return CompareIgnoreOrder(keys, other.keys, size - 1, strick_modifier);
	}
	bool IsEmpty() const { return Size() == 0; }
	TKeyCode ValueKey() const { return size == 0 ? 0 : key(); }

	auto* begin(this auto&& self) { return self.keys; }
	auto* end(this auto&& self) { return self.keys + self.size; }
	//auto* ModsBegin(this auto&& self) { return self.keys + 1; }
	//auto* ModsEnd(this auto&& self) { return self.size <= 1 ? self.ModsBegin() : self.end(); }
	int Size() const { return size; }

	std::string ToString() const {
		std::string s;
		if (size == 0)
			return s;

		for (int i = 0; i < size; ++i) {
			AddToString(keys[i], s);
			if (i != size-1)
				s += " + ";
		}
		if (m_keyup) {
			s += " #up";
		}
		if (m_double_press) {
			s += " #double";
		}
		return s;
	}
	static std::string ToString(TKeyCode key) {
		std::string s;
		AddToString(key, s);
		return s;
	}
	CHotKey& Clear() { SwZeroMemory(*this);	return *this; }
	CHotKey& SetKeyup(bool val = true) { m_keyup = val;	return *this; }
	bool GetKeyup() const { return m_keyup; }
	static CHotKey FromString(UView s) {
		CHotKey key;
		if (s.empty())
			return key;
		auto sElems = StrUtils::Split(s, '+');
		for (auto& sCur : sElems) {
			StrUtils::ToLowerUnsafe(sCur);
			if (StrUtils::replaceAll(sCur, "#up", "")) { key.SetKeyup(true); }
			if (StrUtils::replaceAll(sCur, "#double", "")) { key.SetDouble(true); }
			StrUtils::Trim(sCur);
			TKeyCode kCur = _internal::HotKeyNames::Global().GetCode(sCur.c_str());
			if (kCur == 0) {
				auto split = StrUtils::Split(sCur, L'_');
				if (split.size() == 2) {
					StrUtils::ToInt(split[1], kCur, 16);
				}
			}
			if (kCur == 0) {
				key.Clear();
				LOG_WARN("Not found keycode for {}", sCur);
				return key;
			}
			key.Simple_Append(kCur);
		}
		return key;
	}
	bool Has_left_right() const { return std::any_of(begin(), end(), [](auto v) {return v != Normalize(v); }); }
	bool IsDouble() const { return m_double_press; }
	auto& SetDouble(bool val = true) { m_double_press = val; return *this; }
	bool IsEnabled() const { return !m_disabled; }
	void SetEnabled(bool val = true) { m_disabled = !val; }
	static bool IsKnownMods(TKeyCode key) { return Utils::is_in(Normalize(key), VK_SHIFT, VK_CONTROL, VK_MENU, VKE_WIN); }
	static bool IsRightMod(TKeyCode key) { return Utils::is_in(key, VK_RSHIFT, VK_RCONTROL, VK_RMENU, VK_RWIN); }
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
private: 
	TKeyCode key() const { return keys[size - 1]; }
	static void AddToString(TKeyCode key, std::string& s) {
		const char* sName = _internal::HotKeyNames::Global().GetName(key);
		if (sName) {
			s += sName;
		}
		else {
			s += std::format("VK_{:x}", key);
		}
	}

	bool operator== (const CHotKey& other) = delete;
	bool operator!= (const CHotKey& other) = delete;
	//void NormalizeAll() { for (auto& it : *this) it = Normalize(it); }


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
	//void InsertMods(TKeyCode key) {
	//	if (size < c_MAX)
	//		size++;
	//	keys[size-1] = key;
	//}


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

	int Remove_if(auto&& pred) {
		if (size == 0) return 0;

		// 1. Стандартный алгоритм: сдвигает "выживших" влево, сохраняя порядок.
		// Возвращает итератор на начало "хвоста" из удаленных элементов.
		TKeyCode* new_end = std::remove_if(keys, keys + size, pred);

		// 2. Считаем, сколько удалили
		int new_size = static_cast<int>(new_end - keys);
		int removed_count = size - new_size;

		// 3. Обнуляем "хвост" (опционально, но полезно для безопасности)
		for (int i = new_size; i < size; i++) {
			keys[i] = 0;
		}

		// 4. Обновляем логический размер
		size = new_size;

		return removed_count;
	}

//public: TStatus RegisterHk(CAutoHotKeyRegister& registor, HWND hwnd, int id) {
//		registor.Cleanup();
//		if (IsEmpty()) RETURN_SUCCESS;
//		UINT mods = MOD_NOREPEAT;
//		UINT vk = ValueKey();
//		for (TKeyCode* k = ModsBegin(); k != ModsEnd(); ++k) {
//			auto norm = Normalize(*k);
//			auto cur = *k;
//			if (cur == VK_SHIFT) {
//				mods |= MOD_SHIFT;
//			}
//			else if (cur == VK_CONTROL) {
//				mods |= MOD_CONTROL;
//			}
//			else if (cur == VK_MENU) {
//				mods |= MOD_ALT;
//			}
//			else if (cur == VKE_WIN) {
//				mods |= MOD_WIN;
//			}
//			else {
//				return SW_ERR_UNSUPPORTED;
//			}
//		}
//		IFW_RET(registor.Register(hwnd, id, mods, vk));
//		RETURN_SUCCESS;
//	}

private:
	static constexpr int c_MAX = 5;
	struct {
		uint8_t m_keyup : 1 {};
		uint8_t m_double_press : 1 {};
		uint8_t m_disabled : 1 {};
	};
	int8_t size = 0;
	TKeyCode keys[c_MAX] = { 0 };
};

static_assert(sizeof(CHotKey) == 12);

