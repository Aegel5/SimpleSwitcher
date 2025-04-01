#include "sw-base.h"
#include "Settings.h"
#include "extern/json.hpp"

using json = nlohmann::json;


void SettingsGui::GenerateListHK()
{
    auto AddHotKey = [&](HotKeyType type, CHotKeySet& set) {
        set.hkId          = type;
        hotkeysList.push_back(set);
    };

    {
        CHotKeySet set;
        set.def_list = { CHotKey(VK_CAPITAL), CHotKey(VK_PAUSE), CHotKey(VK_F24) };
        set.fUseDef = true;
        set.gui_text = _(L"Change layout for last word").wc_str();
        AddHotKey(hk_RevertLastWord, set);
    }

    {
        CHotKeySet set;
        set.def_list = { CHotKey(VK_SHIFT, VK_CAPITAL), CHotKey(VK_SHIFT, VK_PAUSE), CHotKey(VK_SHIFT, VK_F24) };
        set.fUseDef = true;
        set.gui_text = _(L"Change layout for last several words").wc_str();
        AddHotKey(hk_RevertSeveralWords, set);
    }

    {
        CHotKeySet set;
        set.def_list = { CHotKey(VK_SHIFT, VK_CAPITAL), CHotKey(VK_SHIFT, VK_PAUSE), CHotKey(VK_LSHIFT).SetDouble() };
        set.gui_text = _(L"Change layout for all recent text").wc_str();
        AddHotKey(hk_RevertAllRecentText, set);
    }

    {
        CHotKeySet set;
        set.def_list = { CHotKey{VK_CONTROL, VK_CAPITAL}, CHotKey{VK_CONTROL, VK_CAPITAL}.SetDouble(), CHotKey{VK_CONTROL, VKE_BREAK} };
        set.fUseDef = true;
        set.gui_text = _(L"Change layout for selected text").wc_str();
        AddHotKey(hk_RevertSelelected, set);
    }

    {
        CHotKeySet set;
        set.def_list = { CHotKey(VKE_WIN, VK_SPACE), CHotKey(VK_LCONTROL,VK_LWIN), CHotKey(VK_SHIFT).SetDouble() };
        set.gui_text = _(L"Cycle change layout").wc_str();
        AddHotKey(hk_CycleSwitchLayout, set);
    }


    {
        CHotKeySet set;
        set.def_list = { CHotKey(VKE_WIN, VK_CAPITAL), CHotKey(VK_CONTROL, VK_F24) };
        set.gui_text = _(L"Generate CapsLock").wc_str();
        AddHotKey(hk_EmulateCapsLock, set);
    }

    {
        CHotKeySet set;
        set.def_list = { CHotKey(VK_SCROLL), CHotKey(VK_PAUSE), CHotKey(VK_F23) };
        set.fUseDef = true;
        set.gui_text = _(L"Selected text to UPPER/lower case").wc_str();
        AddHotKey(hk_toUpperSelected, set);
    }

    {
        CHotKeySet set;
        set.def_list = { CHotKey(VKE_WIN, VK_F8) };
        set.fUseDef = true;
        set.gui_text = _(L"Enable/Disable").wc_str();
        AddHotKey(hk_ToggleEnabled, set);
    }

    {
        CHotKeySet set;
        set.def_list = { CHotKey(VKE_WIN, VK_SHIFT).SetDouble(), CHotKey(VK_SHIFT).SetDouble() };
        set.fUseDef = true;
        set.gui_text = _(L"Show main window").wc_str();
        AddHotKey(hk_ShowMainWindow, set);
    }

    for (auto& it : hotkeysList) {
        if (it.fUseDef) {
            it.keys.key() = it.def_list[0];
        }
    }
}

//namespace ns {

void to_json(json& j, const HKL& p) {
    char sBuf[30] = {0};
    sprintf(sBuf, "0x%I64X", (uint64_t)p);
    j = sBuf;
}

void from_json(const json& j, HKL& p) {
    if (j.is_string()) {
        Str_Utils::StrToUInt64_2(j.get_ref<const std::string&>(), p);
    } else {
        p = 0;
    }
}

void to_json(json& j, const wxString& p) {
    j = p.c_str();

}

void from_json(const json& j, wxString& p) {
    p.Clear();
    if (j.is_string()) {
        p = j.get_ref<const std::string&>();
    }
}

void to_json(json& j, const CHotKey& p) {
    j             = p.ToString().c_str();
}


void from_json(const json& j, CHotKey& p) {
    if (j.is_string()) {
        wxString str = j;
        p.FromString(str.wc_str());
    }
}
void to_json(json& j, const CHotKeyList& p) {
    j = p.keys;
}

void from_json(const json& j, CHotKeyList& p) {
    if (j.is_array()) {
        p.keys = j;
    }
}

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(
    LayoutInfo,
    layout,
    enabled,
    win_hotkey,
    hotkey
)

void to_json(json& j, const LayoutInfoList& p) {
    j = p.info;
}

void from_json(const json& j, LayoutInfoList& p) {
    j.get_to(p.info);
}




NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(
    RunProgramInfo,
    path,
    args,
    elevated,
    hotkey
)


NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(
    SettingsGui,
    isMonitorAdmin,
    force_DbgMode,
    fClipboardClearFormat,
    EnableKeyLoggerDefence,
    disableAccessebility,
    flagsSet,
    disableInPrograms,
    logLevel,
    uiLang,
    AllowRemoteKeys_,
    AlternativeLayoutChange,
    config_version,
    layouts_info, 
    fixRAlt,
    fixRAlt_lay_,
    run_programs,
    separate_ext_last_word,
    separate_ext_several_words,
    quick_press_ms,
    win_hotkey_cycle_lang
    )
    


TStatus LoadConfig(SettingsGui& gui) {
    try {


        std::wstring path_old;
        std::wstring path_new;
        IFS_RET(Utils::GetPath_folder_noLower(path_old));
        path_new = path_old;
        path_old += L"conf.json";
        path_new += L"SimpleSwitcher.json";


        if (!FileUtils::IsFileExists(path_new.c_str())) {
            if (FileUtils::IsFileExists(path_old.c_str())) {
                FileUtils::RenameFile(path_old.c_str(), path_new.c_str());
            }
            else {
                // конфиг файла еще нет - считаем что загружены конфигом по-умолчанию.
                RETURN_SUCCESS;
            }
        }

        std::ifstream ifs(path_new);

        json data = json::parse(ifs, nullptr, true, true);

        gui = data.get<SettingsGui>();

        auto& arr = data["hotkeys"];

        if (arr.is_object()) {
            for (auto& elem : gui.hotkeysList) {
                auto key = HotKeyTypeName(elem.hkId);
                if (key.empty()) continue;
                auto it  = arr.find(key);
                if (it != arr.end()) {
                    auto& obj = it.value();
                    if (obj.is_array()) {
                        elem.keys.keys = obj;
                        if (elem.keys.keys.empty()) {
                            elem.keys.keys.resize(1);
                        }
                    }
                }
            }
        }

        gui.NormalizePaths();

        SetLogLevel_info(gui.IsNeedDebug() ? gui.logLevel : LOG_LEVEL_0);

    } catch (std::exception& e) {
        return SW_ERR_JSON;
    }


    RETURN_SUCCESS;
}



TStatus _Save_conf(const SettingsGui& gui) {
    try {
        std::wstring path;
        IFS_RET(GetPath_Conf(path));

        json data = gui;
        json hk_json;

        for (auto& elem : gui.hotkeysList) {
            auto hk = elem.hkId;
            auto key = HotKeyTypeName(hk);
            if (key.empty()) continue;
            hk_json[key] = elem.keys.keys;
        }

        data["hotkeys"] = hk_json;

        std::stringstream o;
        // std::ofstream o(res);
        o << std::setw(4) << data << std::endl;

        auto json = o.str();
        std::regex regex("\\[(\\ |\\n|\\r|\\t)*");
        json = std::regex_replace(json, regex, "[ ");
        std::regex regex2("(\\ |\\n|\\r|\\t)*\\]");
        json = std::regex_replace(json, regex2, " ]");

        std::ofstream outp(path);
        outp << json;

    } catch (std::exception& e) {
        return SW_ERR_JSON;
    }

    RETURN_SUCCESS;
}
