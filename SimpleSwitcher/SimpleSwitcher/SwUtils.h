#pragma once
#include "utils/procstart.h"
#include "utils/getpath.h"

const static TChar c_sRegRunValue[] = SW_PROGRAM_NAME_L;
const static TChar c_sServiceName[] = SW_PROGRAM_NAME_L L"Service";



enum TStopRes
{
	STOP_RES_OK,
	STOP_RES_NOT_FOUND,
	STOP_RES_CANT_ADMIN,
	STOP_RES_NOT_WIN64,
};



inline TStatus SwCreateProcessOur(const TCHAR* sCmd, TSWBit bit, TSWAdmin admin, CAutoHandle& hProc)
{
	tstring sExe;
	IFS_RET(GetPath(sExe, PATH_TYPE_EXE_PATH, bit));

	procstart::CreateProcessParm parm;
	parm.sExe = sExe.c_str();
	parm.sCmd = sCmd;
	parm.admin = admin;
	IFS_RET(procstart::SwCreateProcess(parm, hProc));

	RETURN_SUCCESS;
}
inline TStatus SwCreateProcessOurWaitIdle(const TCHAR* sCmd, TSWBit bit, TSWAdmin admin)
{
	CAutoHandle hProc;
	IFS_RET(SwCreateProcessOur(sCmd, bit, admin, hProc));
	IF_WAITDWORD_RET(WaitForInputIdle(hProc, c_nCommonWaitProcess));

	RETURN_SUCCESS;
}
inline TStatus SwCreateProcessOurWaitFinished(const TCHAR* sCmd, TSWBit bit, TSWAdmin admin)
{
	CAutoHandle hProc;
	IFS_RET(SwCreateProcessOur(sCmd, bit, admin, hProc));
	IF_WAITDWORD_RET(WaitForSingleObject(hProc, c_nCommonWaitProcess));

	RETURN_SUCCESS;
}




TStatus CreateService();
TStatus RemoveService();

TStatus CheckService(bool& isAdmin);

inline TStatus AddRemapKey(TKeyCode s, TKeyCode v)
{
	BufScanMap buf;
	IFS_RET(buf.FromRegistry());
	buf.PutRemapKey(s, v);
	IFS_RET(buf.ToRegistry());

	RETURN_SUCCESS;
}
inline TStatus DelRemapKey(TKeyCode s)
{
	BufScanMap buf;
	IFS_RET(buf.FromRegistry());
	buf.DelRemapKey(s);
	IFS_RET(buf.ToRegistry());

	RETURN_SUCCESS;
}
enum RemapType
{
	RemapKey_CAPS,
	RemapKey_ScrollLock,
	//RemapKey_Cancel,
};
inline TStatus AddRemap(RemapType rtype)
{
	if (rtype == RemapKey_CAPS)
	{
		IFS_RET(AddRemapKey(VK_CAPITAL, VK_F24));
	}
	else if (rtype == RemapKey_ScrollLock)
	{
		IFS_RET(AddRemapKey(VK_SCROLL, VK_F23));
	}
	RETURN_SUCCESS;
}
inline TStatus DelRemap(RemapType rtype)
{
	if(rtype == RemapKey_CAPS)
	{
		IFS_RET(DelRemapKey(VK_CAPITAL));
	}
	else if (rtype == RemapKey_ScrollLock)
	{
		IFS_RET(DelRemapKey(VK_SCROLL));
	}
	RETURN_SUCCESS;
}




