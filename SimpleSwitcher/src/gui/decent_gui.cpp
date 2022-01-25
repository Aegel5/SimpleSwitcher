#include "stdafx.h"
//#include "decent_utils.h"
#include "gen_ui/noname.h" 
#include "Settings.h"
#include "CoreWorker.h"
#include "SwAutostart.h"
//#include "decent_tray.h"

#include <wx/taskbar.h>

extern bool ChangeHotKey(wxFrame* frame, HotKeyType type);

enum
{
    // menu items
    Minimal_Quit = wxID_EXIT,

    // it is important for the id corresponding to the "About" command to have
    // this standard value as otherwise it won't be handled properly under Mac
    // (where it is special and put into the "Apple" menu)
    Minimal_About = wxID_ABOUT,

    Minimal_Show = wxID_HIGHEST + 1
};
//void onExit(wxCommandEvent& event) {
//    return;
//}
class MyTray : public wxTaskBarIcon {
public:
    MyTray() {
        //Connect(Minimal_Quit, wxMouseEventHandler(MyTray::onExit), NULL, this);

    }
    ~MyTray() {
        return;
    }

    virtual wxMenu* CreatePopupMenu() override {
        auto menu = new wxMenu();
        menu->Append(Minimal_Show, "Show");
        menu->Append(Minimal_Quit, "Exit");
        return menu;
    }
};




class MainWnd : public MyFrame4
{
private:
    //wxDECLARE_EVENT_TABLE();
    CoreWorker coreWork;
    //DecentTray tray;
    MyTray myTray;
    wxIcon icon;

    void SetupToHotCtrl(wxTextCtrl* elem, HotKeyType type) {
        elem->SetClientData((void*)type);
        elem->SetEditable(false);
        elem->Connect(wxEVT_LEFT_DCLICK, wxMouseEventHandler(MainWnd::onHotKeyChange), NULL, this);
    }

    void onHotKeyChange(wxMouseEvent& ev) {

        auto obj = wxDynamicCast(ev.GetEventObject(), wxTextCtrl);
        if (!obj)
            return;
        HotKeyType type = (HotKeyType)(TUInt32)obj->GetClientData();
        if (ChangeHotKey(this, type)) {
            auto res = setsgui.GetHk(type).key.ToString();
            obj->SetValue(res);
        }

    }
    void updateEnable() {
        if (!startOk()) {
            coreWork.Stop();
        }
        m_checkBoxEnable->SetValue(coreWork.IsStarted());
    }
    void UpdateAutostartExplain()
    {
        Startup::CheckTaskSheduleParm parm;
        parm.taskName = c_wszTaskName;
        IFS_LOG(Startup::CheckTaskShedule(parm));

        bool isHasEntry = false;
        std::wstring value;
        IFS_LOG(Startup::GetString_AutoStartUser(c_sRegRunValue, isHasEntry, value));

        std::wstring registryRes = L"none";
        std::wstring schedulRes = L"none";

        if (isHasEntry)        {
            registryRes = value;
        }
        if (parm.isTaskExists)        {
            schedulRes = parm.pathValue;
        }
        std::wstring sLabel = fmt::format(L"         Registry: {}\r\n         Scheduler: {}", registryRes, schedulRes);
        m_staticTextExplain->SetLabelText(sLabel);
    }
    HKL lay_buf[50] = { 0 };
    int lay_size = 0;
    void FillCombo() {
        m_choiceLayFilter->Clear();
        lay_size = GetKeyboardLayoutList(SW_ARRAY_SIZE(lay_buf), lay_buf);
        for (int i = 0; i < lay_size; i++) {
            auto name = Utils::GetNameForHKL(lay_buf[i]);
            m_choiceLayFilter->AppendString(name);
        }
    }
    void onLayChoice(wxCommandEvent& event) override {
        auto cur = event.GetSelection();
        auto lay = lay_buf[cur];
        for (auto& elem : setsgui.customLangList) {
            if (elem == lay)
                return;
        }
        setsgui.customLangList.push_back(lay);
        updateLayFilter();
        setsgui.SaveAndPostMsg();

    }
    void onClearFilter(wxCommandEvent& event) override {
        setsgui.customLangList.clear();
        updateLayFilter();
        setsgui.SaveAndPostMsg();
    }
    void updateLayFilter() {
        std::wstring res;
        auto& lst = setsgui.customLangList;
        for (size_t i = 0; i <lst.size(); ++i)
        {
            res += Utils::GetNameForHKL(lst[i]);
            if (i != lst.size() - 1)
            {
                res += L", ";
            }
        }
        m_textFilterLay->SetValue(res);
    }
    void updateAutoStart() {

        bool isUserAllOk = false;
        bool isUserHasTask = false;
        IFS_LOG(CheckRegRun(isUserAllOk, isUserHasTask));

        bool isAdminAllOk = false;
        bool isAdminHasTask = false;
        IFS_LOG(CheckSchedule(isAdminAllOk, isAdminHasTask));

        if (isUserHasTask)
        {
            if (!isUserAllOk || setsgui.isMonitorAdmin)
            {
                IFS_LOG(DelRegRun());
                IFS_LOG(CheckRegRun(isUserAllOk, isUserHasTask));
            }
        }

        if (isAdminHasTask && Utils::IsSelfElevated())
        {
            if (!isAdminAllOk || !setsgui.isMonitorAdmin)
            {
                IFS_LOG(DelSchedule());
                IFS_LOG(CheckSchedule(isAdminAllOk, isAdminHasTask));
            }
        }

        m_checkAddToAutoStart->SetValue(setsgui.isMonitorAdmin ? isAdminAllOk : isUserAllOk);
        UpdateAutostartExplain();

    }
    void ShowNeedAdmin() {
        wxMessageBox("Need admin rights");
    }
    bool startOk() {
        return Utils::IsSelfElevated() || !setsgui.isMonitorAdmin;
    }
    //virtual WXLRESULT MSWWindowProc(WXUINT nMsg, WXWPARAM wParam, WXLPARAM lParam) override
    //{
    //    // Pass the messages to the original WinAPI window procedure
    //    return MyFrame4::MSWWindowProc(nMsg, wParam, lParam);
    //}
public:
    // ctor(s)
    MainWnd():MyFrame4(nullptr) {
        m_notebook2->SetSelection(0);
        SetupToHotCtrl(m_textLastword, hk_RevertLastWord);
        SetupToHotCtrl(m_textSeveralWords, hk_RevertCycle);
        SetupToHotCtrl(m_textSelected, hk_RevertSel);

        if (startOk()) {
            coreWork.Start();
        }
        updateEnable();
        updateAutoStart();
        FillCombo();
        updateLayFilter();

        //tray.Init(this);
        //tray.SetEnabled(true);
        

        icon = wxIcon("appicon");
        SetIcon(icon);
        if (myTray.IsAvailable()) {
            myTray.SetIcon(icon);
            myTray.Bind(wxEVT_MENU, &MainWnd::onExit, this, Minimal_Quit); 
            myTray.Bind(wxEVT_MENU, &MainWnd::onShow, this, Minimal_Show);
            myTray.Bind(wxEVT_TASKBAR_LEFT_DCLICK, &MainWnd::onShow2, this);
        }
    }
    void onShow2(wxTaskBarIconEvent& event) {
        Show(true);
    }
    void onShow(wxCommandEvent& event) {
        Show(true);
    }
    void onCloseToTray(wxCommandEvent& event)     { 
        Hide();
    }

    void onExit(wxCommandEvent& event) override {
        Close(true);
    }

    void onWorkInAdminCheck(wxCommandEvent& event) override {
        setsgui.isMonitorAdmin = m_checkBoxWorkInAdmin->GetValue();
        setsgui.Save();
        updateAutoStart();
        updateEnable();
    }
    void onAutocheck(wxCommandEvent& event) override {
        if (setsgui.isMonitorAdmin) {
            if (Utils::IsSelfElevated())  {
                if (m_checkAddToAutoStart->GetValue()) {
                    IFS_LOG(SetSchedule());
                }
                else {
                    IFS_LOG(DelSchedule());
                }
            }
            else            {
                ShowNeedAdmin();
            }
        }
        else {
            if (m_checkAddToAutoStart->GetValue()) {
                SetRegRun();
            }
            else {
                DelRegRun();
            }
        }
        updateAutoStart();
    }

    void onEnable(wxCommandEvent& event) override {
        auto cur = m_checkBoxEnable->IsChecked();
        if (cur) {
            if (startOk()) {
                coreWork.Start();
            }
            else {
                ShowNeedAdmin();
            }
        }
        else {
            coreWork.Stop();
        }
        updateEnable();
    }
};

//wxBEGIN_EVENT_TABLE(MainWnd, MyFrame4)
//EVT_MENU(Minimal_Show, MainWnd::onExit)
//wxEND_EVENT_TABLE()



void StartMainGui(bool show) {
    MainWnd* frame = new MainWnd();

    // and show it (the frames, unlike simple controls, are not shown when
    // created initially)
    frame->Show(show);
}

//TStatus Init() {
//
//    IFS_RET(setsgui.Load());
//    RETURN_SUCCESS;
//}

