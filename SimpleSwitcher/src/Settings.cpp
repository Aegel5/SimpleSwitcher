#include "stdafx.h"

#include "Settings.h"
//#include "LuaConfig.h"
//#include "utils/parsecfg.h"

//#include "extern/rapidjson/document.h"

#include "extern/json.hpp"

using json = nlohmann::json;

//using namespace rapidjson;

//TStatus UserConf::Load2() {
//
//
//
//    RETURN_SUCCESS;
//}


void SettingsGui::GenerateListHK()
{
    auto AddHotKey = [&](HotKeyType type, CHotKeySet& set) {
        set.hkId          = type;
        hotkeysList.push_back(set);
    };

    {
        CHotKeySet set;
        set.def_list = { CHotKey(VK_PAUSE), CHotKey(VK_CAPITAL), CHotKey(VK_F24) };
        set.fUseDef        = true;
        set.fNeedSavedWord = true;
        set.gui_text = _(L"Change layout for last word").wc_str();
        AddHotKey(hk_RevertLastWord, set);
    }

    {
        CHotKeySet set;
        set.def_list = { CHotKey(VK_SHIFT, VK_PAUSE), CHotKey(VK_SHIFT, VK_CAPITAL), CHotKey(VK_SHIFT, VK_F24) };
        set.fUseDef = true;
        set.fNeedSavedWord = true;
        set.gui_text = _(L"Change layout for last several words").wc_str();
        AddHotKey(hk_RevertCycle, set);
    }

    {
        CHotKeySet set;
        set.def_list = { CHotKey(VK_PAUSE), CHotKey(VK_CAPITAL), CHotKey(VK_F24) };
        set.fUseDef = true;
        set.gui_text = _(L"Change layout for selected text").wc_str();
        AddHotKey(hk_RevertSel, set);
    }

    {
        CHotKeySet set;
        set.def_list = { CHotKey(VK_LSHIFT, VK_RSHIFT), CHotKey(VK_LCONTROL, VK_SPACE).SetLeftRightMode() };
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
        set.def_list = { CHotKey(VK_SCROLL), CHotKey(VK_F23) };
        set.gui_text = _(L"Selected text to UPPER/lower case").wc_str();
        AddHotKey(hk_toUpperSelected, set);
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
    j             = p.ToString2();
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
void to_json(json& j, const LayoutInfo& p) {
    j["enabled"] = p.enabled;
    j["hotkey"] = p.hotkey.keys;
    j["win_hotkey"] = p.WinHotKey;
    j["layout"] = p.layout;
}

void from_json(const json& j, LayoutInfo& p) {
    if (j.is_object() && j.contains("layout")) {
        if (j.contains("enabled"))
            p.enabled = j["enabled"].get<bool>();
        if (j.contains("hotkey"))
            p.hotkey = j["hotkey"];
        if(j.contains("win_hotkey"))
            p.WinHotKey = j["win_hotkey"];
        p.layout = j["layout"].get<HKL>();
    }
}



NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(
    SettingsGui,
    isMonitorAdmin,
    fDbgMode,
    fClipboardClearFormat,
    fEnableKeyLoggerDefence,
    disableAccessebility,
    showFlags,
    disableInPrograms,
    logLevel,
    uiLang,
    AllowRemoteKeys,
    AlternativeLayoutChange,
    SystemLayoutChange,
    config_version,
    layouts_info
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

        for (auto& elem : gui.disableInPrograms) {
            std::wstring s;
            Str_Utils::Utf8ToWide(elem, s);
            Str_Utils::ToLower(s);
            gui.__disableInPrograms.insert(s);
        }

        gui.Update_hk_from_layouts();

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
            if (!TestFlag(hk, hk_SetLayout_flag)) {
                auto key = HotKeyTypeName(hk);
                hk_json[key] = elem.keys.keys;
            }
        }

        data["hotkeys"] = hk_json;

        std::stringstream o;
        // std::ofstream o(res);
        o << std::setw(4) << data << std::endl;

        auto json = o.str();

        auto insert_after = [&](const char* s, const char* comm) {
            auto it = json.find(s);
            if (it != std::string::npos) {
                it = json.find("\n", it);
                if (it != -1)
                    json.insert(it, comm);
            }
        };

        insert_after("\"disableInPrograms", " // example [\"game1.exe\"]");

        std::ofstream outp(path);
        outp << json;

    } catch (std::exception& e) {
        return SW_ERR_JSON;
    }

    RETURN_SUCCESS;
}
