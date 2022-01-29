#include "stdafx.h"

#include "Settings.h"
//#include "LuaConfig.h"
#include "utils/parsecfg.h"

#include "extern/rapidjson/document.h"

using namespace rapidjson;

TStatus UserConf::Load2() {

    std::wstring sCurFolder;
    IFS_RET(GetPath(sCurFolder, PATH_TYPE_SELF_FOLDER, GetSelfBit()));
    std::wstring res = sCurFolder + L"config.json";

    if (!FileUtils::IsFileExists(res.c_str())) {
        RETURN_SUCCESS;
    }

    std::wifstream ifs(res);
    std::wstring content((std::istreambuf_iterator<wchar_t>(ifs)), (std::istreambuf_iterator<wchar_t>()));

    GenericDocument<UTF16<>> doc;
    using TVal = GenericValue<UTF16<>>;
    ParseResult ok = doc.Parse<kParseCommentsFlag | kParseTrailingCommasFlag>(content.c_str());
    if (ok.IsError()) {
        LOG_INFO_1(L"json parse error");
        RETURN_SUCCESS;
    }
    TVal& s  = doc[L"ll"];
    if (s.IsInt()) {
        ll = s.GetInt();
    }
    s        = doc[L"disableInPrograms"];
    if (s.IsArray()) {
        for (auto& elem : s.GetArray()) {
            disableInProcess.push_back(elem.GetString());
        }
    }



    RETURN_SUCCESS;
}


void SettingsGui::GenerateListHK()
{
    auto AddHotKey = [&](HotKeyType type, CHotKeySet& set) {
        set.hkId          = type;
        set.name          = HotKeyTypeName(type);
        hotkeysList[type] = set;
    };

    {
        CHotKeySet set;
        set.def2           = CHotKey(VK_PAUSE);
        set.def            = CHotKey(VK_F24);
        set.fUseDef        = true;
        set.fNeedSavedWord = true;
        AddHotKey(hk_RevertLastWord, set);
    }

    //{
    //	CHotKeySet set;
    //	set.def = CHotKey(VK_LSHIFT, VK_RSHIFT);
    //	AddHotKey(hk_ChangeLayoutCycle, set);
    //}

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
        set.def2           = CHotKey(VK_SHIFT, VK_PAUSE);
        set.def            = CHotKey(VK_SHIFT, VK_F24);
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

    //{
    //	CHotKeySet set;
    //	set.fNeedSavedWord = true;
    //	AddHotKey(hk_RevertLastWord_CustomLang, set);

    //}

    //{
    //	CHotKeySet set;
    //	set.fNeedSavedWord = true;
    //	AddHotKey(hk_RevertCycle_CustomLang, set);

    //}

    {
        CHotKeySet set;
        set.def2    = CHotKey(VK_PAUSE);
        set.def     = CHotKey(VK_F24);
        set.fUseDef = true;
        AddHotKey(hk_RevertSel, set);
    }

    //{
    //	CHotKeySet set;
    //	set.def2 = CHotKey(VK_SHIFT, VK_SCROLL);
    //	set.def = CHotKey(VK_F23);
    //	set.fFillFromDef = true;
    //	AddHotKey(hk_ChangeCase, set);
    //}

    //{
    //	CHotKeySet set;
    //	set.def = CHotKey(VK_SHIFT, VK_CONTROL, VK_F24);
    //	set.def2 = CHotKey(VK_F23);
    //	AddHotKey(hk_EmulCopyNoFormat, set);
    //}

    //{
    //	CHotKeySet set;
    //	set.def = CHotKey(VK_SHIFT, VK_CONTROL, VK_F24);
    //	set.def2 = CHotKey(VK_F23);
    //	AddHotKey(hk_EmulCopyWithFormat, set);
    //}

    //{
    //	CHotKeySet set;
    //	set.def = CHotKey(VK_F24).SetHold(true);
    //	set.fFillFromDef = true;
    //	AddHotKey(hk_RevertRecentTyped, set);
    //}

    //{
    //	CHotKeySet set;
    //	set.fGui = false;
    //	AddHotKey(hk_ChangeTextCase, set);
    //}

    for (auto& it : hotkeysList) {
        auto& hk = it.second;
        if (hk.fUseDef) {
            hk.key = hk.def;
        }
    }
}

TStatus SettingsGui::Load()
{
    SettingsGui loc;
    IFS_LOG(loc.LoadAutoSettings());
    *this = loc;

	RETURN_SUCCESS;
}

TStatus SettingsGui::LoadAutoSettings()
{
	auto sPathIni = GetPathIni();
	if (!FileUtils::IsFileExists(sPathIni.c_str()))
	{
		RETURN_SUCCESS;
	}

	ParseCfg::TSMap tsmap;
	IFS_RET(ParseCfg::ReadKeyMapFile(sPathIni, tsmap));

	// Load bools
	// --------------------------------------------------------------
//#ifdef _DEBUG
//	ParseCfg::GetBool(tsmap, L"enable", isEnabledSaved);
//#endif

	ParseCfg::GetBool(tsmap, L"monitorAdmin", isMonitorAdmin);
	//ParseCfg::GetBool(tsmap, L"addToTray", isAddToTray);
	ParseCfg::GetBool(tsmap, L"OEM2", isTryOEM2);
	//ParseCfg::GetBool(tsmap, L"closeByEsc", fCloseByEsc);
	ParseCfg::GetBool(tsmap, L"fEnableKeyLoggerDefence", fEnableKeyLoggerDefence);
	//ParseCfg::GetBool(tsmap, L"isDashSeparate", isDashSeparate);
	ParseCfg::GetBool(tsmap, L"fDbgMode", fDbgMode);
	ParseCfg::GetBool(tsmap, L"fClipboardClearFormat", fClipboardClearFormat);
    ParseCfg::GetBool(tsmap, L"disableAccessebility", disableAccessebility);

	//ParseCfg::GetInt(tsmap, L"idLang", idLang);
	LOG_INFO_1(L"Load settings...");
	if (GetLogLevel() >= 1)
	{
		for (auto& it : tsmap)
		{
			if (Str_Utils::IsStartWith(it.first.c_str(), L"hk_"))
			{
				continue;
			}
			LOG_INFO_1(L"%s=%s", it.first.c_str(), it.second.c_str());
		}
	}

	// Load customLangList
	// ---------------------------------------------------------
	std::wstring customLangListStr = ParseCfg::GetString(tsmap, L"customLangList");
	std::vector<std::wstring> vect;
	IFS_LOG(Str_Utils::Split(customLangListStr, vect, L','));
	customLangList.clear();
	for (auto& sValCur : vect)
	{
		if (!sValCur.empty())
		{
			int val;
			if (Str_Utils::StrToUInt64(sValCur, val))
			{
				customLangList.push_back((HKL)val);
			}
		}
	}
	for (int i = 0; i < SW_ARRAY_SIZE(hkl_lay); ++i)
	{
		HKL& cur = hkl_lay[i];
		std::wstring sName = L"hkl";
		sName += std::to_wstring(i);
		cur = ParseCfg::GetIntDef(tsmap, sName.c_str(), (HKL)0);
	}

	// Load hotkeys
	// ---------------------------------------
	for (auto& it : hotkeysList)
	{
		auto hkId = it.first;
		CHotKeySet& set = it.second;

		//if(!set.fGui)
		//	continue;

		std::wstring sName = L"hk_";
		sName += std::to_wstring(set.hkId);
		// try by id
		if(!ParseCfg::GetInt(tsmap, sName.c_str(), set.key.AsUInt64()))
		{
			// try by name
			ParseCfg::GetInt(tsmap, HotKeyTypeName(hkId), set.key.AsUInt64());
		}

		if (set.key.AsUInt64() != 0)
		{
			tstring keyStr;
			set.key.ToString(keyStr);
			LOG_INFO_1(L"%s: %s", HotKeyTypeName(hkId), keyStr.c_str());
		}
	}

	RETURN_SUCCESS;
}

void AddStr(std::wstring& cont, const TChar* name, const TChar* val)
{
	cont += name;
	cont += L"=";
	cont += val;
	cont += L"\n";
}
void AddStr(std::wstring& cont, const TChar* name, std::wstring& val)
{
	AddStr(cont, name, val.c_str());
}
void AddHex(std::wstring& cont, const TChar* name, TUInt64 val)
{
	TChar sBuf[20];
	swprintf_s(sBuf, L"0x%I64X", val);
	AddStr(cont, name, sBuf);
}
void AddInt(std::wstring& cont, const TChar* name, TUInt64 val)
{
	AddStr(cont, name, std::to_wstring(val).c_str());
}
void AddBool(std::wstring& cont, const TChar* name, bool val)
{
	AddInt(cont, name, TUInt64(val));
}
void SettingsGui::Save()
{
	std::wstring cont;

	//AddBool(cont, L"enable", isEnabledSaved);
	AddBool(cont, L"monitorAdmin", isMonitorAdmin);
	//AddBool(cont, L"addToTray", isAddToTray);  
	AddBool(cont, L"OEM2", isTryOEM2);  
	//AddBool(cont, L"closeByEsc", fCloseByEsc);
	AddBool(cont, L"fEnableKeyLoggerDefence", fEnableKeyLoggerDefence);
	AddBool(cont, L"fDbgMode", fDbgMode); 
	AddBool(cont, L"fClipboardClearFormat", fClipboardClearFormat); 
	AddBool(cont, L"disableAccessebility", disableAccessebility); 
	//AddBool(cont, L"isDashSeparate", isDashSeparate); 
	//AddInt(cont, L"idLang", idLang);
	//AddBool(cont, L"fHookDll", fHookDll);

	std::wstring customLangListStr;
	for (auto k : customLangList)
	{
		customLangListStr += std::to_wstring((TUInt64)k);
		customLangListStr += L",";
	}
	if (!customLangListStr.empty())
	{
		AddStr(cont, L"customLangList", customLangListStr);
	}
	for (int i = 0; i < SW_ARRAY_SIZE(hkl_lay); ++i)
	{
		HKL& cur = hkl_lay[i];
		if (cur)
		{
			std::wstring sName = L"hkl" + std::to_wstring(i);
			AddHex(cont, sName.c_str(), (TUInt64)cur);
		}
	}

	//SettingsGui defaults;

	for (auto& it : hotkeysList)
	{
		auto hkId = it.first;
		auto& info = it.second;

		//if(!info.fGui)
		//	continue;

		//if (!info.key.CompareRaw(defaults.hotkeysList[info.hkId].key)) // нет смысла сохранять def
		{
			TChar sBuf[100];
            swprintf_s(sBuf, L"0x%I64X -- %s=%s", info.key.AsUInt64(), HotKeyTypeName(info.hkId), info.key.ToString().c_str());
            AddStr(cont,  (L"hk_" + std::to_wstring(hkId)).c_str(), sBuf);
			//AddHex(cont, HotKeyTypeName(hkId), info.key.AsUInt64());
		}
	}

	std::wofstream file(GetPathIni());
	file << cont;
}
