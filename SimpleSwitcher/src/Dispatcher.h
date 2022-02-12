#pragma once

// todo удалить всю помойку.

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
	//CAutoProcMonitor procMonitor;
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
	TSWBit bit);


TStatus ResetAllHotKey(HotKeyHolders& keys);








