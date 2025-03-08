





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
    const TChar* gui_text = _(L"unknown").wc_str();

};


struct LayoutInfo {
    HKL layout = 0;
    bool enabled = true;
    CHotKey WinHotKey;
    CHotKeyList hotkey;
};

struct RunProgramInfo {
    wxString path;
    wxString args;
    CHotKeyList hotkey;
    bool elevated = false;
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