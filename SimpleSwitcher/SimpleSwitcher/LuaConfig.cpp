#include "stdafx.h"

#include "Settings.h"
#include "LuaConfig.h"

#include "luawrap/luawrap.h"


TStatus LuaLoadConf(UserConf& conf)
{
	std::wstring sFilePath;
	IFS_RET(GetPath(sFilePath, PATH_TYPE_SELF_FOLDER, GetSelfBit()));
	sFilePath += L"config.lua";
	if (!FileUtils::IsFileExists(sFilePath.c_str()))
	{
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

	RETURN_SUCCESS;
}
