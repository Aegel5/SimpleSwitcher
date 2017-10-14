#pragma once

extern "C"
{
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}

struct LuaStatErrDescr
{
	int res;
	LuaStatErrDescr(int r) : res(r) {}
	void Log()
	{
		__SW_LOG_FORMAT__(L"LuaStatus=%d", res);
	}
	bool IsError() const { return res != 0; }
	operator bool() const { return IsError(); }
	TStatus ToTStatus() { return SW_ERR_LUA_ERROR; }
};

#define LUASTAT_RET(X, ...) _SW_ERR_RET(LuaStatErrDescr, X, __VA_ARGS__)
#define LUASTAT_LOG(X, ...) _SW_ERR_LOG(LuaStatErrDescr, X, __VA_ARGS__)
#define LUARET(X, ...) __RET_ERR(LuaStatErrDescr, X, __VA_ARGS__)

MAKE_AUTO_CLEANUP(lua_State*, ::lua_close, NULL, CAutoLuaState);

TStatus LuaGetStringStat(lua_State* luaState, const char* sName, std::wstring& data)
{
	int type = lua_getglobal(luaState, sName);
	if (type == LUA_TSTRING)
	{
		const char* utf8Val = lua_tostring(luaState, -1);
		IFS_RET(Str_Utils::Utf8ToWide(utf8Val, data));
	}
	lua_pop(luaState, 1);

	RETURN_SUCCESS;
}


bool LuaGetInt(lua_State* luaState, const char* sName, int& val)
{
	bool found = false;
	int type = lua_getglobal(luaState, sName);
	if (type == LUA_TNUMBER)
	{
		val = (int)lua_tointeger(luaState, -1);
		found = true;
	}
	lua_pop(luaState, 1);

	return found;
}
bool LuaGetBool(lua_State* luaState, const char* sName, bool& val)
{
	int valInt;
	if (LuaGetInt(luaState, sName, valInt))
	{
		val = valInt ? true : false;
		return true;
	}
	return false;
}

bool LuaGetString(lua_State* luaState, const char* sName, std::wstring& data)
{
	auto stat = LuaGetStringStat(luaState, sName, data);
	IFS_LOG(stat);

	return SW_SUCCESS(stat);
}

TStatus LuaLoadLowStringList(lua_State* luaState, std::vector<std::wstring>& lst, TAStr name)
{
	std::wstring sVal;
	if (LuaGetString(luaState, name, sVal))
	{
		IFS_RET(Str_Utils::Split(sVal, lst, L','));
		for (auto& v : lst)
		{
			Str_Utils::trim(v);
			Str_Utils::ToLower(v);
		}
	}
	RETURN_SUCCESS;
}

