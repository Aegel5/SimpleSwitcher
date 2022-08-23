#include "stdafx.h"
//#include "decent_utils.h"
#include "noname.h" 
#include "Settings.h"
#include "CoreWorker.h"
#include "SwAutostart.h"
//#include "decent_tray.h"

#include "decent_gui.h"

#include "tools/accessibil.h"

#include "ver.h"

#include <wx/taskbar.h>

extern bool ChangeHotKey(wxFrame* frame, HotKeyType type, CHotKey& key);

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
public:
    MainWnd() : MyFrame4(nullptr)
    {
        g_guiHandle = GetHandle();

        icon = wxIcon("appicon");
        //trayIcon = icon;
        SetIcon(icon);
        if (myTray.IsAvailable()) {
            myTray.SetIcon(icon);
            myTray.Bind(wxEVT_MENU, &MainWnd::onExit, this, Minimal_Quit);
            myTray.Bind(wxEVT_MENU, &MainWnd::onShow, this, Minimal_Show);
            myTray.Bind(wxEVT_TASKBAR_LEFT_DCLICK, &MainWnd::onShow2, this);
        }

        Bind(wxEVT_CLOSE_WINDOW, &MainWnd::onExitReqest, this);

        SetTitle(fmt::format(L"{} {}{}", GetTitle(), SW_VERSION, Utils::IsSelfElevated() ? L" Administrator" : L""));
        SetWindowStyleFlag(wxMINIMIZE_BOX | wxCLOSE_BOX | wxCAPTION | wxRESIZE_BORDER);

        m_staticTextBuildDate->SetLabelText(fmt::format(L"Built on '{}'", _SW_ADD_STR_UT(__DATE__)));

        m_notebook2->SetSelection(0);

        BindHotCtrl(m_textLastword, hk_RevertLastWord);
        BindHotCtrl(m_textSeveralWords, hk_RevertCycle);
        BindHotCtrl(m_textSelected, hk_RevertSel);
        BindHotCtrl(m_textCycleLay, hk_CycleCustomLang);
        BindHotCtrl(m_textSetlay1, hk_ChangeSetLayout_1);
        BindHotCtrl(m_textSetlay2, hk_ChangeSetLayout_2);
        BindHotCtrl(m_textSetlay3, hk_ChangeSetLayout_3);
        BindHotCtrl(m_textcapsgen, hk_CapsGenerate);
        BindHotCtrl(m_text_sel_toupper, hk_toUpperSelected);

        updateBools();

        if (startOk()) {
            coreWork.Start();
        }
        updateEnable();
        updateAutoStart();
        FillCombo();
        updateLayFilter();

        updateCapsTab();
        handleDisableAccess();


    }

private:
    //wxDECLARE_EVENT_TABLE();
    CoreWorker coreWork;
    //DecentTray tray;
    MyTray myTray;
    wxIcon icon;
    //wxIcon trayIcon;
    bool exitRequest = false;

    int getChildIndex(wxWindow* obj) {
        auto par = obj->GetParent();
        for (int i = 0; i < par->GetChildren().size(); i++) {
            if (par->GetChildren()[i] == obj)
                return i;
        }
        return -1;
    }

    void BindHotCtrl(wxTextCtrl* elem, HotKeyType type) {

        elem->SetClientData((void*)type);

        elem->SetEditable(false);
        elem->SetValue(setsgui.GetHk(type).key().ToString());

        //auto sizer = elem->GetSizer();
        //auto size = sizer->GetSize();
        //return;
        auto par  = elem->GetParent();
        //auto sizer = wxDynamicCast(par, wxBoxSizer);
        auto* btn = wxDynamicCast(par->GetChildren()[getChildIndex(elem)+1], wxButton);
        if (btn == nullptr) {
            return;
        }
        btn->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(MainWnd::onHotKeyChange_btn), NULL, this);


        //elem->Connect(wxEVT_LEFT_DCLICK, wxMouseEventHandler(MainWnd::onHotKeyChange), NULL, this);
        //elem->Connect(wxEVT_RIGHT_DOWN, wxMouseEventHandler(MainWnd::onHotKeyChange), NULL, this);

    }

    void onExitReqest(wxCloseEvent& event) {

        if (event.CanVeto() && !exitRequest) {
            Hide();
            event.Veto();
            return;
        }

        Destroy(); // you may also do:  event.Skip();
                   // since the default event handler does call Destroy(), too
    }

    std::map<std::string, wxIcon> icons;

   virtual WXLRESULT MSWWindowProc(WXUINT nMsg, WXWPARAM wParam, WXLPARAM lParam) override {

        if (nMsg == WM_LayNotif) {

            if (!setsgui.showFlags)
                return TRUE;

            HKL newLayout = (HKL)wParam;
            LOG_INFO_1(L"mainguid new layout: 0x%x", newLayout);


            std::string name = "appicon";

            WORD langid = LOWORD(newLayout);

            TCHAR buf[512];
            buf[0] = 0;

            int flag = LOCALE_ICOUNTRY;
            int len  = GetLocaleInfo(MAKELCID(langid, SORT_DEFAULT), flag, buf, SW_ARRAY_SIZE(buf));
            IFW_LOG(len != 0);


            if (Str_Utils::IsEqual(buf, L"1")) { // todo get country
                name = "flag_us";
            } else if (Str_Utils::IsEqual(buf, L"7")) {
                name = "flag_ru";
            } else {
                int k = 0;
            }

            auto it = icons.find(name);
            if (it == icons.end()) {
                icons.emplace(name, wxIcon(name));
                it     = icons.find(name);
            } 

            myTray.SetIcon(it->second);

            return TRUE;
        }

        return MyFrame4::MSWWindowProc(nMsg, wParam, lParam);
   }

    void updateBools() {
        m_checkBoxWorkInAdmin->SetValue(setsgui.isMonitorAdmin);
        m_checkBoxClearForm->SetValue(setsgui.fClipboardClearFormat);
        m_checkBoxKeyDef->SetValue(setsgui.fEnableKeyLoggerDefence);
        m_checkBoxDisablAcc->SetValue(setsgui.disableAccessebility);
        m_checkDebuglog->SetValue(setsgui.fDbgMode);
        m_checkBoxShowFlags->SetValue(setsgui.showFlags);
    }

    virtual void onEnableLog(wxCommandEvent& event)
    {
        setsgui.fDbgMode = event.IsChecked();
        SetLogLevel2(setsgui.fDbgMode ? setsgui.logLevel : LOG_LEVEL_0);
        SaveAndPostMsg();
    }
    virtual void onPrevent(wxCommandEvent& event){
        setsgui.fEnableKeyLoggerDefence = event.IsChecked();
        SaveAndPostMsg();
    }
    virtual void onClearFormat(wxCommandEvent& event) {
        setsgui.fClipboardClearFormat = event.IsChecked();
        SaveAndPostMsg();
    }

    void handleDisableAccess() {
        if (setsgui.disableAccessebility) {
            AllowAccessibilityShortcutKeys(false);
        }
    }
    virtual void onShowFlags(wxCommandEvent& event) {
        setsgui.showFlags = event.IsChecked();
        SaveAndPostMsg();

        if (!setsgui.showFlags) {
            myTray.SetIcon(icon);
        } else {
            GetCurLayRequest();
        }

    }
    virtual void onDisableAccessebl(wxCommandEvent& event) {
        setsgui.disableAccessebility = event.IsChecked();
        SaveAndPostMsg();

        handleDisableAccess();
    }

    void updateCapsTab()
    {
        BufScanMap remap;
        IFS_LOG(remap.FromRegistry());

        TKeyCode caps;

        remap.GetRemapedKey(VK_CAPITAL, caps);
        m_checkcapsrem->SetValue(caps == VK_F24);

        remap.GetRemapedKey(VK_SCROLL, caps);
        m_check_scrollremap->SetValue(caps == VK_F23);

        auto caption = [](DWORD sc, DWORD vc) {
            return fmt::format(L"sc: {}, vk: {}, '{}'", sc, vc, HotKeyNames::Global().GetName(vc));
        };

        m_listBoxRemap->Clear();
        for (auto iter = remap.GetIter(); !iter.IsEnd(); ++iter) {
            auto [sc, vc]   = iter.curElemSrc();
            auto [sc2, vc2] = iter.curElemDst();
            m_listBoxRemap->Append(fmt::format(L"{} -> {}", caption(sc, vc), caption(sc2, vc2)));
        }
    }
    virtual void onRemapCaps(wxCommandEvent& event)
    {
        auto obj = event.GetEventObject();
        auto check = wxDynamicCast(obj, wxCheckBox);
        if (check == nullptr)
            return;

        if (!Utils::IsSelfElevated()) {
            check->SetValue(!check->GetValue());
            ShowNeedAdmin();
            return;
        }

        BufScanMap remap;
        IFS_LOG(remap.FromRegistry());

        bool showmsg = false;
        if (check->GetValue()) {
            if(check == m_checkcapsrem){
                remap.PutRemapKey(VK_CAPITAL, VK_F24);
            } else {
                remap.PutRemapKey(VK_SCROLL, VK_F23);
            }
            showmsg = true;

        } else {
            remap.DelRemapKey(check == m_checkcapsrem ? VK_CAPITAL : VK_SCROLL);
        }
        IFS_LOG(remap.ToRegistry());

        updateCapsTab();

        if(showmsg)
            wxMessageBox(_("Will be applied after PC reboot"));
    }

    void onHotKeyChange_btn(wxCommandEvent& ev) {
        auto btn      = wxDynamicCast(ev.GetEventObject(), wxButton);
        auto* edit_bx = wxDynamicCast(btn->GetParent()->GetChildren()[getChildIndex(btn)-1], wxTextCtrl);
        onHotKey_ForEditBox(edit_bx);
    }

    void onHotKey_ForEditBox(wxTextCtrl* obj) {
        if (!obj)
            return;
        HotKeyType type = (HotKeyType)(TUInt32)obj->GetClientData();
        CHotKey newkey;
        if (ChangeHotKey(this, type, newkey)) {
            setsgui.hotkeysList[type].key() = newkey;
            SaveAndPostMsg();
            auto res = setsgui.GetHk(type).key().ToString();
            obj->SetValue(res);
        }
    }

    void onHotKeyChange(wxMouseEvent& ev) {

        auto obj = wxDynamicCast(ev.GetEventObject(), wxTextCtrl);
        onHotKey_ForEditBox(obj);

        //ev.Skip();

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
    wxChoice* getByIndex(int i) {
        if (i == 0)            return m_choiceset1;
        if(i == 1)            return m_choiceset2;
        if (i == 2)
            return m_choiceset3;
        return nullptr;
    }
    void FillCombo() {
        m_choiceLayFilter->Clear();
        for (int i = 0; i < 3; i++) {
            getByIndex(i)->Clear();
        }
        lay_size = GetKeyboardLayoutList(SW_ARRAY_SIZE(lay_buf), lay_buf);
        for (int i = 0; i < lay_size; i++) {
            auto name = Utils::GetNameForHKL(lay_buf[i]);
            m_choiceLayFilter->AppendString(name);

            for (int i = 0; i < 3; i++) {
                getByIndex(i)->AppendString(name);
            }
        }

        for (int i = 0; i < 3; i++) {
            auto cur = setsgui.hkl_lay[i];
            for (int j = 0; j < lay_size; j++) {
                if (lay_buf[j] == cur) {
                    getByIndex(i)->SetSelection(j);
                    break;
                }
            }
        }
    }
    void onLayChoice(wxCommandEvent& event) override {

        auto obj = wxDynamicCast(event.GetEventObject(), wxChoice);
        if (!obj)
            return;
        auto cur = event.GetSelection();
        auto lay = lay_buf[cur];

        if (obj == m_choiceLayFilter) {
            for (auto& elem : setsgui.customLangList) {
                if (elem == lay)
                    return;
            }
            setsgui.customLangList.push_back(lay);
            updateLayFilter();
            SaveAndPostMsg();
        } else {
            if (obj == m_choiceset1) {
                setsgui.hkl_lay[0] = lay;
            } else if (obj == m_choiceset2) {
                setsgui.hkl_lay[1] = lay;
            } else {
                setsgui.hkl_lay[2] = lay;
            }
            SaveAndPostMsg();
        }
    }
    void onClearFilter(wxCommandEvent& event) override {
        setsgui.customLangList.clear();
        updateLayFilter();
        SaveAndPostMsg();
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

    void ensureAuto(bool enable) {
        if (setsgui.isMonitorAdmin) {

            IFS_LOG(DelRegRun());

            if (enable) {
                IFS_LOG(SetSchedule());
            } else {
                IFS_LOG(DelSchedule());
            }

        } else {

            bool isAdminAllOk   = false;
            bool isAdminHasTask = false;
            IFS_LOG(CheckSchedule(isAdminAllOk, isAdminHasTask));
            if (isAdminHasTask) {
                if (Utils::IsSelfElevated()) {
                    IFS_LOG(DelSchedule());
                } else {
                    ShowNeedAdmin("delete old task");
                    return; // exit because can't delete old
                }
            }

            if (enable) {
                IFS_LOG(SetRegRun());
            } else {
                IFS_LOG(DelRegRun());
            }
        }
    }

    void updateAutoStart() {

        bool isUserAllOk = false;
        bool isUserHasTask = false;
        IFS_LOG(CheckRegRun(isUserAllOk, isUserHasTask));

        bool isAdminAllOk = false;
        bool isAdminHasTask = false;
        IFS_LOG(CheckSchedule(isAdminAllOk, isAdminHasTask));

        m_checkAddToAutoStart->SetValue(setsgui.isMonitorAdmin ? isAdminAllOk && !isUserHasTask
                                                               : isUserAllOk && !isAdminHasTask);
        UpdateAutostartExplain();

    }
    void ShowNeedAdmin(const char* expl = nullptr) {
        wxString ms(_("Need admin rights"));
        if (expl != nullptr) {
            ms += " for \"";
            ms += expl;
            ms += "\"";
        }
        wxMessageBox(ms);
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
        exitRequest = true;
        LOG_INFO_1(L"exit request");
        Close(true);
    }

    void onWorkInAdminCheck(wxCommandEvent& event) override {
        setsgui.isMonitorAdmin = m_checkBoxWorkInAdmin->GetValue();
        Save();
        updateAutoStart();
        updateEnable();
    }
    void onAutocheck(wxCommandEvent& event) override {
        if (setsgui.isMonitorAdmin && !Utils::IsSelfElevated()) {
            m_checkAddToAutoStart->SetValue(!m_checkAddToAutoStart->GetValue());
            ShowNeedAdmin();
            return;
        }
        ensureAuto(m_checkAddToAutoStart->GetValue());
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



void StartMainGui(bool show, bool conf_err_msg) {

    MainWnd* frame = new MainWnd();

    // and show it (the frames, unlike simple controls, are not shown when
    // created initially)
    frame->Show(show);

    if (conf_err_msg) {
        wxMessageBox(_("Error reading config"));
    }


}

//TStatus Init() {
//
//    IFS_RET(setsgui.Load());
//    RETURN_SUCCESS;
//}

