#pragma once
#include "tlhelp32.h"

extern BOOL CALLBACK EnumWindowsProcMy(HWND hwnd, LPARAM lParam);

struct TopWndInfo {
    HWND hwnd      = nullptr;
    HKL lay        = 0;
    DWORD threadid = 0;
};

class ProgEnum {

    struct Info {
        DWORD pid_conhost = 0;
        HWND conhost = nullptr;
        DWORD thId        = 0;
    };

    std::map<DWORD, Info> mp;
    std::map<DWORD, Info*> mp2;
    std::vector<std::pair<DWORD, DWORD>> conh;

    ULONGLONG lastscan = 0;

public:

    void __Push(DWORD pid, HWND wnd, DWORD threadId) {
        auto it = mp2.find(pid);
        if (it != mp2.end()) {
            if (it->second->conhost == nullptr) {
                it->second->conhost = wnd;
                it->second->thId    = threadId;
            }
        }

    }

    TopWndInfo GetTopWnd2() {

        TopWndInfo res;

        HWND hwndFocused = nullptr;
        IFS_LOG(Utils::GetFocusWindow(hwndFocused));

        if (hwndFocused == nullptr)
            return res;

        DWORD pid;
        auto threadid = GetWindowThreadProcessId(hwndFocused, &pid);
        res.hwnd      = hwndFocused;

        auto it = mp.find(pid);
        if (it == mp.end() || GetTickCount64() - lastscan >= 5000*60) {
            IFS_LOG(Scan());
            lastscan   = GetTickCount64();
            it = mp.find(pid);
        }


        if (it == mp.end()) {
            return res;
        }

        if (it->second.conhost != nullptr) {
            threadid = it->second.thId;
            res.hwnd = it->second.conhost;
        }

        res.lay = GetKeyboardLayout(threadid);
        res.threadid = threadid;

        return res;

    }

    TStatus Scan() {
        mp.clear();
        mp2.clear();
        conh.clear();

        {
            PROCESSENTRY32 pe32;
            CAutoHandle2 snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
            IFW_RET(snap.IsValid());
            pe32.dwSize = sizeof(PROCESSENTRY32);
            IFW_RET(Process32First(snap, &pe32));
            do {
                mp[pe32.th32ProcessID] = {};
                if (_tcscmp(pe32.szExeFile, TEXT("conhost.exe")) == 0) {
                    conh.emplace_back(pe32.th32ProcessID, pe32.th32ParentProcessID);
                }
            } while (Process32Next(snap, &pe32));
        }

        for (auto& e : conh) {
            auto it = mp.find(e.second);
            if (it != mp.end()) {
                it->second.pid_conhost = e.first;
                mp2[e.first]           = &it->second;
            }
        }

        EnumWindows(EnumWindowsProcMy, 0);

        RETURN_SUCCESS;
    }

};

inline ProgEnum g_procEnum;

inline BOOL CALLBACK EnumWindowsProcMy(HWND hwnd, LPARAM lParam) {
    DWORD lpdwProcessId;
    auto threadId = GetWindowThreadProcessId(hwnd, &lpdwProcessId);

    if (threadId != 0) {
        g_procEnum.__Push(lpdwProcessId, hwnd, threadId);
    }
    // LOG_INFO_1(L"test: hwnd=0x%p, pid=%u", hwnd, lpdwProcessId);

    // if (lpdwProcessId == lParam) {
    //    return FALSE;
    return TRUE;
}




//inline void Test() {
//    LOG_INFO_1(L"\n");
//    EnumWindows(EnumWindowsProcMy, 0);
//}
