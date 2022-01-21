#include "stdafx.h"

#include "decent_utils.h"

#include "gen_ui/noname.h" 

#include "Settings.h"

#include "CoreWorker.h"

#include "SwAutostart.h"



SW_NAMESPACE(SwGui)
extern bool ChangeHotKey2(HotKeyType type, HWND hwnd); // todo
SW_NAMESPACE_END

class MyApp : public wxApp
{
public:
    // override base class virtuals
    // ----------------------------

    // this one is called on application startup and is a good place for the app
    // initialization (doing it here and not in the ctor allows to have an error
    // return: if OnInit() returns false, the application terminates)
    virtual bool OnInit() wxOVERRIDE;
};

wxIMPLEMENT_APP(MyApp);

class MainWnd : public MyFrame4
{
private:
    CoreWorker coreWork;

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
        if (SwGui::ChangeHotKey2(type, nullptr)) {
            auto res = SettingsGlobal().GetHk(type).key.ToString();
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
        for (auto& elem : SettingsGlobal().customLangList) {
            if (elem == lay)
                return;
        }
        SettingsGlobal().customLangList.push_back(lay);
        updateLayFilter();
        SettingsGlobal().SaveAndPostMsg();

    }
    void onClearFilter(wxCommandEvent& event) override {
        SettingsGlobal().customLangList.clear();
        updateLayFilter();
        SettingsGlobal().SaveAndPostMsg();
    }
    void updateLayFilter() {
        std::wstring res;
        auto& lst = SettingsGlobal().customLangList;
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
            if (!isUserAllOk || SettingsGlobal().isMonitorAdmin)
            {
                IFS_LOG(DelRegRun());
                IFS_LOG(CheckRegRun(isUserAllOk, isUserHasTask));
            }
        }

        if (isAdminHasTask && Utils::IsSelfElevated())
        {
            if (!isAdminAllOk || !SettingsGlobal().isMonitorAdmin)
            {
                IFS_LOG(DelSchedule());
                IFS_LOG(CheckSchedule(isAdminAllOk, isAdminHasTask));
            }
        }

        m_checkAddToAutoStart->SetValue(SettingsGlobal().isMonitorAdmin ? isAdminAllOk : isUserAllOk);
        UpdateAutostartExplain();

    }
    void ShowNeedAdmin() {
        wxMessageBox("Need admin rights");
    }
    bool startOk() {
        return Utils::IsSelfElevated() || !SettingsGlobal().isMonitorAdmin;
    }
public:
    // ctor(s)
    MainWnd():MyFrame4(nullptr) {
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
    }


    void onExit(wxCommandEvent& event) override {
        Close(true);
    }

    void onWorkInAdminCheck(wxCommandEvent& event) override {
        SettingsGlobal().isMonitorAdmin = m_checkBoxWorkInAdmin->GetValue();
        SettingsGlobal().Save();
        updateAutoStart();
        updateEnable();
    }
    void onAutocheck(wxCommandEvent& event) override {
        if (SettingsGlobal().isMonitorAdmin) {
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

//TStatus Init() {
//
//    IFS_RET(SettingsGlobal().Load());
//    RETURN_SUCCESS;
//}

bool MyApp::OnInit()
{
    if (Utils::ProcSingleton(c_mtxSingltonGui))
    {
        LOG_INFO_1(L"Gui already running.Exit");
        return false;
    }

    IFS_LOG(SettingsGlobal().Load());
    //auto res = Init();
    //IFS_LOG(res);
    //if (res != SW_ERR_SUCCESS)
    //    return false;

    // call the base class initialization method, currently it only parses a
    // few common command-line options but it could be do more in the future
    if (!wxApp::OnInit())
        return false;

    // create the main application window
    MainWnd* frame = new MainWnd();

    // and show it (the frames, unlike simple controls, are not shown when
    // created initially)
    frame->Show(true);

    // success: wxApp::OnRun() will be called which will enter the main message
    // loop and the application will run. If we returned false here, the
    // application would exit immediately.
    return true;
}