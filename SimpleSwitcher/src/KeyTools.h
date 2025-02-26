#pragma once
#include "KeysStruct.h"
#include "InputSender.h"
#include "Settings.h"

//#include "Hooker.h"

using TKeyRevert = std::vector<TKeyBaseInfo>;

struct ContextRevert
{
	TKeyRevert keylist;
	HotKeyType typeRevert;
	HKL lay = 0;
	TUInt32 flags = 0;
	//tstring txtToInsert;
};

TStatus SendKeys(TKeyRevert& sendData);
TStatus SendBacks(TUInt32 count);
TStatus FunnyMoving(TKeyRevert& sendData);
TStatus SendDels(TUInt32 count);



enum CLEARMODS
{
	CLEARMODS_RESTORE,
	CLEARMODS_NORESTORE,
	CLEARMODS_NOCLEAR,
};
bool IsOurInput();
TStatus SendOurInput(InputSender& sender);
inline TStatus ClearMods()
{
	BYTE buf[256] = { 0 };
	IFW_RET(SetKeyboardState(buf));

	RETURN_SUCCESS;
}

inline TStatus RestoreMods(CHotKey key)
{
	//SW_LOG_INFO_2(L"RestoreMods for key %s", key.ToString().c_str());

	BYTE buf[256];// = {0};
	IFW_RET(GetKeyboardState(buf));

	CHotKey keyAdded;
	for (TKeyCode k : key)
	{
		if (!CHotKey::IsKnownMods(k))
			continue;
		SHORT res = GetAsyncKeyState(k);
		//SW_LOG_INFO_2(L"GetAsyncKeyState for key %s = 0x%x", CHotKey::ToString(k).c_str(), res);
		if (res & 0x8000)
		{
			if (k >= SW_ARRAY_SIZE(buf))
				IFS_RET(SW_ERR_BUFFER_TOO_SMALL);

			buf[k] &= 0x8000;
			keyAdded.Add(k);

			//SW_LOG_INFO_2(L"Add key %s to down after input", CHotKey::ToString(k).c_str());

		}
	}

	IFW_RET(SetKeyboardState(buf));

	RETURN_SUCCESS;
}
inline void AnalizeProblemByName(std::wstring& sPath, std::wstring& sProc, bool& fPostNotWork, bool& fDels)
{
	LOG_INFO_3(L"AnalizeProblemByName spath=%s, sproc=%s", sPath.c_str(), sProc.c_str());

	fPostNotWork = false;
	fDels = false;

	if (sProc == L"afterfx.exe")
	{
		fDels = true;
		fPostNotWork = true;
	}
	else if (
		sProc == L"skype.exe"
		|| sProc == L"devenv.exe"
		|| sProc == L"stikynot.exe"
		|| sProc == L"wordpad.exe"
		|| sProc == L"keepass.exe"
		|| sProc == L"conemu.exe"
		|| sProc == L"outlook.exe"
		|| sProc == L"skypeapp.exe"
		)
	{
		fPostNotWork = true;
	}
	else
	{
		//for (auto& name : u_conf.altModePrg)
		//{
		//	if (sProc == name)
		//	{
		//		fPostNotWork = true;
		//	}
		//}

		//bool fMicrosoft = false;
		//IFS_LOG(IsProductHas(sPath.c_str(), L"Microsoft", fMicrosoft));

		//if (fMicrosoft)
		//{
		//	LOG_INFO_2(L"Found microsoft");
		//	fHang = true;
		//	fPostNotWork = true;
		//}
	}
}
