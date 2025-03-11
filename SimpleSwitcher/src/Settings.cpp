#include "stdafx.h"
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
        AddHotKey(hk_RevertCycle, set);
    }

    {
        CHotKeySet set;
        set.def_list = { {VK_LMENU, VK_CAPITAL}, CHotKey(VK_LMENU, VK_PAUSE), CHotKey(VK_PAUSE), CHotKey(VK_LMENU,VK_F24) };
        set.fUseDef = true;
        set.gui_text = _(L"Change layout for selected text").wc_str();
        AddHotKey(hk_RevertSel, set);
    }

    {
        CHotKeySet set;
        set.def_list = { CHotKey(VK_LWIN), CHotKey(VK_E_CTX_MENU), CHotKey(VK_LCONTROL, VK_SPACE) };
        set.gui_text = _(L"Cycle change layout").wc_str();
        AddHotKey(hk_CycleCustomLang, set);
    }


    {
        CHotKeySet set;
        set.def_list = { CHotKey(VK_CONTROL, VK_CAPITAL), CHotKey(VK_CONTROL, VK_F24) };
        set.fUseDef = true;
        set.gui_text = _(L"Generate CapsLock").wc_str();
        AddHotKey(hk_CapsGenerate, set);
    }

    {
        CHotKeySet set;
        set.def_list = { CHotKey(VK_SCROLL), CHotKey(VK_PAUSE), CHotKey(VK_F23) };
        set.gui_text = _(L"Selected text to UPPER/lower case").wc_str();
        AddHotKey(hk_toUpperSelected, set);
    }

    {
        CHotKeySet set;
        set.def_list = { CHotKey(VK_LMENU, VK_SHIFT), CHotKey(VK_CONTROL, VK_SHIFT) };
        set.gui_text = _(L"Cycle change layout (win hotkey)").wc_str();
        set.fUseDef = true;
        AddHotKey(hk_CycleLang_win_hotkey, set);
    }

    for (auto& it : hotkeysList) {
        if (it.fUseDef) {
            it.keys.key() = it.def_list[0];
        }
    }
}

//namespace ns {

void to_json(json& j, const HKL& p) {
    char sBuf[30];
    sprintf(sBuf, "0x%I64X", (uint64_t)p);
    std::string s = sBuf;
    j = s;
}

void from_json(const json& j, HKL& p) {
    if (j.is_string()) {
        auto s = j.get<std::string>();
        Str_Utils::StrToUInt64_2(s, p);
    } else {
        p = 0;
    }
}

void to_json(json& j, const CHotKey& p) {
    j             = p.ToString().c_str();
}

void from_json(const json& j, CHotKey& p) {
    if (j.is_string()) {
        IFS_LOG(p.FromString(j.get<std::string>()));
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

void to_json(json& j, const wxString& p) {
    j = p.c_str();

}

void from_json(const json& j, wxString& p) {
    p.Clear();
    if (j.is_string()) {
        p = j.get<std::string>();
    }
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
    showFlags,
    disableInPrograms,
    logLevel,
    uiLang,
    AllowRemoteKeys,
    AlternativeLayoutChange,
    config_version,
    layouts_info, 
    fixRAlt,
    fixRAlt_lay_,
    run_programs
    )
    


TStatus LoadConfig(SettingsGui& gui) {
    try {


        std::wstring path_old;
        std::wstring path_new;
        IFS_RET(GetPath_folder_noLower(path_old));
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



TStatus Save2(const SettingsGui& gui) {
    try {
        std::wstring path;
        IFS_RET(GetPath_Conf(path));

        json data = gui;
        json hk_json;

        for (auto& elem : gui.hotkeysList) {
            auto hk = elem.hkId;
            auto key = HotKeyTypeName(hk);
            hk_json[key] = elem.keys.keys;
        }

        data["hotkeys"] = hk_json;

        std::stringstream o;
        // std::ofstream o(res);
        o << std::setw(4) << data << std::endl;

        auto json = o.str();

        std::ofstream outp(path);
        outp << json;

    } catch (std::exception& e) {
        return SW_ERR_JSON;
    }

    RETURN_SUCCESS;
}
