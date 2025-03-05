

enum HotKeyType : TUInt32
{
    hk_NULL,

    hk_RevertLastWord,
    hk_RevertCycle,
    hk_RevertSel,
    hk_CapsGenerate,
    hk_CycleCustomLang,
    hk_CycleLang_win_hotkey,
    hk_ScrollGenerate,
    hk_toUpperSelected,

    hk_SetLayout_flag = 0b100000000000,
};

inline const char* HotKeyTypeName(HotKeyType hk_type)
{
    switch (hk_type)
    {
    case hk_RevertLastWord:	return "hk_RevertLastWord";
    case hk_RevertCycle: return "hk_RevertSeveralWords";
    case hk_RevertSel: return "hk_RevertSelelected";
    case hk_CapsGenerate:return "hk_EmulateCapsLock";
    case hk_CycleCustomLang:return "hk_CycleSwitchLayout";
    case hk_CycleLang_win_hotkey:return "hk_CycleLang_win_hotkey";
    case hk_ScrollGenerate:return "hk_EmulateScrollLock";
    case hk_toUpperSelected:    return "hk_toUpperSelected";
    default: return "hk_Unknown";
    }
}

struct CHotKeyList {

    std::vector<CHotKey> keys{ 1 };
    bool HasKey(CHotKey ktest, CHotKey::TCompareFlags flags) const {
        for (const auto& k : keys) {
            if (ktest.Compare(k, flags))
                return true;
        }
        return false;
    }

    bool Empty() {
        for (const auto& k : keys) {
            if (!k.IsEmpty())
                return false;
        }
        return true;
    }

    bool HasKey_skipkeyup(CHotKey ktest, CHotKey::TCompareFlags flags) const {
        for (const auto& k : keys) {
            if (!k.GetKeyup() && ktest.Compare(k, flags))
                return true;
        }
        return false;
    }

    CHotKey& key() {
        return keys[0];
    }

    const CHotKey& key() const {
        return keys[0];
    }
};

struct CHotKeySet
{
    std::vector<CHotKey> def_list;
    bool fNeedSavedWord = false;
    bool fUseDef = false;
    HotKeyType hkId = hk_NULL;
    CHotKeyList keys;
    const TChar* gui_text = _(L"unknown").wc_str();

};


struct LayoutInfo {
    HKL layout = 0;
    bool enabled = true;
    CHotKey WinHotKey;
    CHotKeyList hotkey;
};


struct LayoutInfoList {

    std::vector<LayoutInfo> info;

    bool AllLayoutEnabled() const {
        for (const auto& it : info) {
            if (!it.enabled) return false;
        }
        return true;
    }
    bool HasLayout(HKL lay) const {
        return GetLayoutInfo(lay) != nullptr;
    }

    const LayoutInfo* GetLayoutInfo(HKL lay) const {
        for (const auto& it : info) {
            if (it.layout == lay)
                return &it;
        }
        return nullptr;
    }
    const LayoutInfo* GetLayoutIndex(int i) const {
        if (i >= info.size()) return nullptr;
        return &info[i];
    }
};