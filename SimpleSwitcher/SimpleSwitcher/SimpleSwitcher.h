#pragma once

#include "CAutoProcMonitor.h"

//class Hooker;
struct HookHandles;
struct HotKeyHolders;
class CMainWorker;


struct CommonData
{
	HINSTANCE hInst;
	TSWBit curModeBit;
	HWND hWndMonitor;
	//HWND hWndGuiMain;
	//Hooker* hooker = NULL;
	CMainWorker* mainWorker = nullptr;
	HotKeyHolders* hotkeyHolders = NULL;
	//CThreadQueue* hookerWorker = NULL;
	//HookHandles* hookHandles = NULL;
	CAutoProcMonitor procMonitor;
	CommonData() : procMonitor(c_sClassName32, c_sArgHook32, SW_BIT_32, false) {}
	static CommonData& Global()
	{
		static CommonData data;
		return data;
	}
};

inline CommonData& gdata() { return CommonData::Global(); }
inline CMainWorker* Worker() { return gdata().mainWorker; }


TStatus StartMonitor(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ int       nCmdShow,
	TSWBit bit);


TStatus ResetAllHotKey(HotKeyHolders& keys);








