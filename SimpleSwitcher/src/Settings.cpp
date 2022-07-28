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
        //set.name          = HotKeyTypeName(type);
        hotkeysList[type] = set;
    };

    {
        CHotKeySet set;
        set.def           = CHotKey(VK_PAUSE);
        set.def2            = CHotKey(VK_F24);
        set.fUseDef        = true;
        set.fNeedSavedWord = true;
        AddHotKey(hk_RevertLastWord, set);
    }

    {
        CHotKeySet set;
        set.def = CHotKey().Add(VK_LCONTROL).SetLeftRightMode(true).SetKeyup(true);
        AddHotKey(hk_ChangeSetLayout_1, set);
    }

    {
        CHotKeySet set;
        set.def = CHotKey().Add(VK_RCONTROL).SetLeftRightMode(true).SetKeyup(true);
        AddHotKey(hk_ChangeSetLayout_2, set);
    }

    {
        CHotKeySet set;
        set.def = CHotKey();
        AddHotKey(hk_ChangeSetLayout_3, set);
    }

    {
        CHotKeySet set;
        set.def = CHotKey(VK_F24, VK_CONTROL);
        AddHotKey(hk_CapsGenerate, set);
    }

    {
        CHotKeySet set;
        set.def = CHotKey(VK_F23, VK_CONTROL);
        AddHotKey(hk_ScrollGenerate, set);
    }

    {
        CHotKeySet set;
        set.def           = CHotKey(VK_SHIFT, VK_PAUSE);
        set.def2            = CHotKey(VK_SHIFT, VK_F24);
        set.fUseDef        = true;
        set.fNeedSavedWord = true;
        AddHotKey(hk_RevertCycle, set);
    }

    {
        CHotKeySet set;
        set.def = CHotKey(VK_LSHIFT, VK_RSHIFT);
        set.def.SetLeftRightMode(true);
        AddHotKey(hk_CycleCustomLang, set);
    }

    {
        CHotKeySet set;
        set.def    = CHotKey(VK_PAUSE);
        set.def2     = CHotKey(VK_F24);
        set.fUseDef = true;
        AddHotKey(hk_RevertSel, set);
    }
  

    for (auto& it : hotkeysList) {
        auto& hk = it.second;
        if (hk.fUseDef) {
            hk.key() = hk.def;
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


NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(SettingsGui,
                                                isMonitorAdmin,
                                                fDbgMode,
                                                fClipboardClearFormat,
                                                fEnableKeyLoggerDefence,
                                                disableAccessebility,
                                                showFlags,
                                                disableInPrograms,
                                                customLangList,
                                                hkl_lay

    )


TStatus Load(SettingsGui& gui) {

    try {

    std::wstring sCurFolder;
    IFS_RET(GetPath(sCurFolder, PATH_TYPE_SELF_FOLDER, GetSelfBit()));
    std::wstring res = sCurFolder + L"conf.json";

    if (!FileUtils::IsFileExists(res.c_str())) {
        RETURN_SUCCESS;
    }

    std::ifstream ifs(res);


        json data = json::parse(ifs, nullptr,true, true);

        gui = data.get<SettingsGui>();

        if (gui.hkl_lay.size() != 3) {
            gui.hkl_lay.resize(3);
        }

        auto& arr = data["hotkeys"];

        if (arr.is_object()) {
            for (auto& elem : gui.hotkeysList) {
                auto key = HotKeyTypeName(elem.second.hkId);
                auto it  = arr.find(key);
                if (it != arr.end()) {
                    auto obj = it.value();
                    if (obj.is_array()) {
                        elem.second.keys = obj;
                        if (elem.second.keys.empty()) {
                            elem.second.keys.resize(1);
                        }
                    }
                }
            }
        }

        for (auto& elem : gui.disableInPrograms) {
            std::wstring s;
            Str_Utils::Utf8ToWide(elem, s);
            gui.__disableInPrograms.insert(s);
        }


    } catch (std::exception& e) {
        return SW_ERR_JSON;
    }

	RETURN_SUCCESS;
}



TStatus Save() {
    try {
        std::wstring sCurFolder;
        IFS_RET(GetPath(sCurFolder, PATH_TYPE_SELF_FOLDER, GetSelfBit()));
        std::wstring file = sCurFolder + L"conf.json";

        SettingsGui& gui = setsgui;

        json data = gui;
        json hk;

        for (auto& elem : gui.hotkeysList) {
            auto key = HotKeyTypeName(elem.second.hkId);
            hk[key]  = elem.second.keys;
        }

        data["hotkeys"] = hk;

        std::stringstream o;
        //std::ofstream o(res);
        o << std::setw(4) << data << std::endl;

        auto json = o.str();

        auto insert_after = [&](const char* s, const char* comm) {
            auto it = json.find(s);
            if (it != std::string::npos) {
                it = json.find("\n", it);
                if(it != -1)
                    json.insert(it, comm);
            }
        };

        insert_after("\"disableInPrograms",
                      " // example [\"game1.exe\"]");

        std::ofstream outp(file);
        outp << json;



    } catch (std::exception& e) {
        return SW_ERR_JSON;
    }

    RETURN_SUCCESS;

}
