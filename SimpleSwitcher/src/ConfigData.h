
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


    CHotKey& key() {
        return keys[0];
    }

    const CHotKey& key() const {
        return keys[0];
    }

    std::wstring ToString() const {
        std::wstring res;
        for (int i = -1; const auto & it : keys) {
            i++;
            if (i != 0) {
                res += L", ";
            }
            res += it.ToString();
        }
        return res;
    }
};

struct CHotKeySet
{
    std::vector<CHotKey> def_list;
    bool fUseDef = false;
    HotKeyType hkId = hk_NULL;
    CHotKeyList keys;

};


struct LayoutInfo {
    HKL layout = 0;
    bool enabled = true;
    CHotKey win_hotkey;
    CHotKeyList hotkey;
};

struct RunProgramInfo {
    wstring path;
    wstring args;
    CHotKeyList hotkey;
    bool elevated = false;
};


struct LayoutInfoList {

    std::vector<LayoutInfo> info;

    bool AllLayoutEnabled() const {
        return std::ranges::all_of(info, [](const auto& x) {return x.enabled; });
    }
    std::generator<HKL> EnabledLayouts() const {
        for (const auto& it : info) {
            if (it.enabled) co_yield it.layout;
        }
    }
    int CntLayoutEnabled() const {
        int cnt = 0;
        for (const auto& it : EnabledLayouts()) cnt++;
        return cnt;
    }
    HKL NextEnabledLayout(HKL lay) const {
        bool found = false;
        for (int i = 0; i < 2; i++) {
            for (const auto& it : info) {
                if (it.layout == lay)
                    found = true;
                else {
                    if (found && it.enabled)
                        return it.layout;
                }
            }
        }
        // не нашли?
        return 0;
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
