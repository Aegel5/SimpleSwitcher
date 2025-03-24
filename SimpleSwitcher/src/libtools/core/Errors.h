#pragma once
#include <simple_enum/simple_enum.hpp>

enum struct TStatus
{
	SW_ERR_SUCCESS                          = 0,
	SW_ERR_UNKNOWN                          ,
	SW_ERR_UNSUPPORTED                      ,
	SW_ERR_INVALID_PARAMETR                 ,
	SW_ERR_PROCESS_NOT_STOPPED              ,
	SW_ERR_WINAPI                           ,
	SW_ERR_CRT                              ,
	SW_ERR_NOT_FOUND                        ,
	SW_ERR_TIMEOUT                          ,
	SW_ERR_TOO_MANY_ELEMENTS                ,
	SW_ERR_CANT_WAIT_MUTEX                  ,
	SW_ERR_GUI_ERROR                        ,
	SW_ERR_HRESULT                          ,
	SW_ERR_LSTATUS                          ,
	SW_ERR_WAIT_PROCESS                     ,
	SW_ERR_WND_NOT_FOUND,
	SW_ERR_PROTOCOL_VER,
	SW_ERR_BUFFER_TOO_SMALL,
	SW_ERR_ALREADY_RUN,
	SW_ERR_LUA_ERROR,
	SW_ERR_ERRNO,
	SW_ERR_BAD_INTERNAL_STATE,
	SW_ERR_NO_MEMORY,
	SW_ERR_JSON,

	SW_ERR_LAST_ENUM,
};
using enum TStatus;

consteval auto adl_enum_bounds(TStatus) -> simple_enum::adl_info<TStatus> {
	return { TStatus::SW_ERR_SUCCESS, TStatus::SW_ERR_LAST_ENUM }; // Assumes my_enum satisfies enum_concept
}











