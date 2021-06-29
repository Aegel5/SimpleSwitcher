#include "stdafx.h"

#include "Settings.h"
#include "LuaConfig.h"

#include "luawrap/luawrap.h"

TStatus CustomLangListFromStr(std::vector<HKL>& lst, const std::wstring& str)
{
	HKL lays[20] = { 0 };
	auto sz = GetKeyboardLayoutList(SW_ARRAY_SIZE(lays), lays);
	std::map<tstring, HKL> mp;
	for (int i = 0; i < sz; i++) {
		mp[Utils::GetNameForHKL(lays[i])] = lays[i];
	}

	std::vector<std::wstring> strs;
	Str_Utils::Split2(str, strs, L", ", true);

	std::set<HKL> st;

	for (size_t i = 0; i < strs.size(); ++i)
	{
		auto& cur = strs[i];
		auto it = mp.find(cur);
		if (it != mp.end()) {
			st.insert(it->second);
		}
	}

	lst.clear();
	for (auto& e : st) {
		lst.push_back(e);
	}

	RETURN_SUCCESS;
}


TStatus LuaLoadConf(UserConf& conf)
{
	std::wstring sFilePath;
	IFS_RET(GetPath(sFilePath, PATH_TYPE_SELF_FOLDER, GetSelfBit()));
	sFilePath += L"config.lua";
	if (!FileUtils::IsFileExists(sFilePath.c_str()))
	{
		LOG_INFO_1(L"Config %s not found", sFilePath.c_str());
		return SW_ERR_SUCCESS;
	}
	CAutoLuaState luaState = luaL_newstate();
	if (!luaState)
		IFS_RET(SW_ERR_LUA_ERROR);
	//luaL_openlibs(luaState);
	std::string sFilePathA;
	IFS_RET(Str_Utils::WideToUtf8(sFilePath.c_str(), sFilePathA));
	LUASTAT_RET(luaL_dofile(luaState, sFilePathA.c_str()), L"luamsg=%S", lua_tostring(luaState, -1));

	IFS_RET(LuaLoadLowStringList(luaState, conf.disableInProcess, "disableInProgram"));
	IFS_RET(LuaLoadLowStringList(luaState, conf.altModePrg, "useAlternateModeInProgram"));

	LuaGetInt(luaState, "ll", conf.ll);
	LuaGetInt(luaState, "msDelayAfterCtrlC", conf.msDelayAfterCtrlC);
	LuaGetBool(luaState, "useAlternateModeByDefault", conf.fUseAltMode);

	LuaGetString(luaState, "hk_ChangeTextCase", conf.s_hk_ChangeTextCase);

	tstring revert_cutomlang;
	LuaGetString(luaState, "CustomLang_List", revert_cutomlang);
	Str_Utils::trim(revert_cutomlang);

	if (!revert_cutomlang.empty()) {
		IFS_RET(CustomLangListFromStr(SettingsGlobal().revert_customLangList, revert_cutomlang));

		auto parseKey = [&](const char* sName, CHotKey& keydest) {

			keydest.Clear();

			tstring key;
			if (!LuaGetStringLowTrim(luaState, sName, key)) {
				RETURN_SUCCESS;
			}

			CHotKey key2;
			IFS_RET(key2.FromString(key));

			keydest = key2;

			RETURN_SUCCESS;
		};

		IFS_RET(parseKey("CustomLang_RevertLastWorld", SettingsGlobal().GetHk(hk_RevertLastWord_CustomLang).key));
		IFS_RET(parseKey("CustomLang_RevertCycle", SettingsGlobal().GetHk(hk_RevertCycle_CustomLang).key));

	}

	RETURN_SUCCESS;
}
