#include "stdafx.h"
//#include "decent_utils.h"
#include "noname.h" 
#include "Settings.h"
#include "CoreWorker.h"
#include "SwAutostart.h"
//#include "decent_tray.h"

#include "decent_gui.h"

#include "tools/accessibil.h"



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

    Minimal_Show = wxID_HIGHEST + 1,

    Minimal_SetLay1,
};
//void onExit(wxCommandEvent& event) {
//    return;
//}
class MyTray : public wxTaskBarIcon {
    std::vector<HKL> lays;

public:

    //MyTray() {
    //    //Connect(Minimal_Quit, wxMouseEventHandler(MyTray::onExit), NULL, this);
    //}
    //~MyTray() {
    //    return;
    //}

    void AddLay(HKL lay) {
        lays.push_back(lay);
    }

    HKL LayById(int id) {
        return lays[id - Minimal_SetLay1];
    }

    virtual wxMenu* CreatePopupMenu() override {

        auto menu = new wxMenu();

        menu->Append(Minimal_Show, _("Show"));
        menu->Append(Minimal_Quit, _("Exit"));
        menu->AppendSeparator();
        for (int i = 0; i < lays.size(); i++) {
            menu->Append(Minimal_SetLay1 + i, Utils::GetNameForHKL(lays[i]));
        }

        return menu;
    }
};




class MainWnd : public MyFrame4
{
public:
    MainWnd() : MyFrame4(nullptr)
    {
        try {
            g_guiHandle = GetHandle();

            icon = wxIcon("appicon");
            SetIcon(icon);

            Bind(wxEVT_CLOSE_WINDOW, &MainWnd::onExitReqest, this);

            SetTitle(std::format(L"{} {}{}", GetTitle().t_str(), SW_VERSION, Utils::IsSelfElevated() ? L" Administrator" : L""));
            SetWindowStyleFlag(wxMINIMIZE_BOX | wxCLOSE_BOX | wxCAPTION | wxRESIZE_BORDER);

            m_staticTextBuildDate->SetLabelText(std::format(L"Built on '{}'", _SW_ADD_STR_UT(__DATE__)));


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

            updateAutoStart();
            FillCombo();
            updateLayFilter();

            updateCapsTab();
            handleDisableAccess();
            UpdateUiLang();

            if (myTray.IsAvailable()) {
                trayTooltip = L"SimpleSwitcher";
                trayTooltip += L" ";
                trayTooltip += SW_VERSION;
                myTray.SetIcon(icon, trayTooltip);
                myTray.Bind(wxEVT_MENU, &MainWnd::onExit, this, Minimal_Quit);
                myTray.Bind(wxEVT_MENU, &MainWnd::onShow, this, Minimal_Show);
                myTray.Bind(wxEVT_TASKBAR_LEFT_DCLICK, &MainWnd::onShow2, this);
                for (int i = 0; i < all_lay_size; i++) {
                    myTray.AddLay(all_lays[i]);
                    myTray.Bind(wxEVT_MENU, &MainWnd::onSetLay, this, Minimal_SetLay1 + i);
                }
            }

            if (startOk()) {
                coreWork.Start();
            }
            updateEnable();
        }
        catch (std::exception& e) {
            wxMessageBox(_("Error while initing main wnd: ") + e.what());
        }
        inited = true;
    }

private:
    //wxDECLARE_EVENT_TABLE();
    CoreWorker coreWork;
    //DecentTray tray;
    MyTray myTray;
    wxString trayTooltip;
    wxIcon icon;
    //wxIcon trayIcon;
    bool exitRequest = false;
    bool inited = false;

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
        auto key = conf_get()->GetHk(type).key();
        elem->SetValue(key.ToString());

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

    std::map<wxString, wxBitmapBundle> flags_map;
    std::set<wxString> flags_miss;

    wxBitmapBundle GetFlag2(const wxString& _name) {

        auto name16 = _name + L"16";
        auto name32 = _name + L"32";

        static bool inited = false;

        if (!inited) {
            ::wxInitAllImageHandlers();
            inited = true;
        }

        auto it = flags_map.find(name16);
        if (it != flags_map.end()) return it->second;

        auto it2 = flags_miss.find(name16);
        if (it2 != flags_miss.end()) return {};

        if (FindResource(0, name16, RT_RCDATA) == nullptr || FindResource(0, name32, RT_RCDATA) == nullptr) {
            flags_miss.insert(name16);
            return {};
        }

        wxVector<wxBitmap> bitmaps;
        bitmaps.push_back(wxBitmap(name16, wxBITMAP_TYPE_PNG_RESOURCE));
        bitmaps.push_back(wxBitmap(name32, wxBITMAP_TYPE_PNG_RESOURCE));
        flags_map.emplace(name16, wxBitmapBundle::FromBitmaps(bitmaps));

        return flags_map[name16];
    }

   virtual WXLRESULT MSWWindowProc(WXUINT nMsg, WXWPARAM wParam, WXLPARAM lParam) override {

        if (nMsg == WM_LayNotif) {

            if (!inited) 
                return TRUE;

            if (!conf_get()->showFlags)
                return TRUE;

            HKL newLayout = (HKL)wParam;

            std::wstring name = L"appicon";

            WORD langid = LOWORD(newLayout);

            TCHAR buf[512];
            buf[0] = 0;

            int flag = LOCALE_SNAME;
            int len  = GetLocaleInfo(MAKELCID(langid, SORT_DEFAULT), flag, buf, SW_ARRAY_SIZE(buf));
            IFW_LOG(len != 0);


            wxBitmapBundle bndl;

            auto len_str = wcslen(buf);
            if (len_str >= 2) {

                TStr name = buf + len_str - 2;
                wxString wname = name;

                LOG_INFO_1(L"mainguid new layout: 0x%x, name=%s", newLayout, name);

                bndl = GetFlag2(wname);
            }

            if (!bndl.IsOk()) {
                LOG_INFO_1(L"ERR. can't find flag for ");
                bndl = icon;
            }

            if (!myTray.SetIcon(bndl, trayTooltip)) {
                LOG_INFO_1(L"ERR. can't set icon ");
            }

            return TRUE;
        }

        return MyFrame4::MSWWindowProc(nMsg, wParam, lParam);
   }

    void updateBools() {
        auto conf = conf_get();
        m_checkBoxWorkInAdmin->SetValue(conf->isMonitorAdmin);
        m_checkBoxClearForm->SetValue(conf->fClipboardClearFormat);
        m_checkBoxKeyDef->SetValue(conf->fEnableKeyLoggerDefence);
        m_checkBoxDisablAcc->SetValue(conf->disableAccessebility);
        m_checkDebuglog->SetValue(conf->fDbgMode);
        m_checkBoxShowFlags->SetValue(conf->showFlags);
        m_checkBoxAllowInjected->SetValue(conf->AllowRemoteKeys);
        m_checkBoxAlterantiveLayoutChange->SetValue(conf->AlternativeLayoutChange);
    }

    virtual void onEnableLog(wxCommandEvent& event)
    {
        auto conf = conf_copy();
        conf->fDbgMode = event.IsChecked();
        SetLogLevel_v3(conf->fDbgMode ? conf->logLevel : LOG_LEVEL_0);
        conf_set(conf);
    }
    virtual void onPrevent(wxCommandEvent& event){
        auto conf = conf_copy();
        conf->fEnableKeyLoggerDefence = event.IsChecked();
        conf_set(conf);
    }
    virtual void onClearFormat(wxCommandEvent& event) {
        auto conf = conf_copy();
        conf->fClipboardClearFormat = event.IsChecked();
        conf_set(conf);
    }
    virtual void onAllowInject(wxCommandEvent& event) { 
        auto conf = conf_copy();
        conf->AllowRemoteKeys = event.IsChecked();
        conf_set(conf);
    }
    virtual void onAlternative(wxCommandEvent& event) {
        auto conf = conf_copy();
        conf->AlternativeLayoutChange = event.IsChecked();
        conf_set(conf);
    }

    void handleDisableAccess() {
        if (conf_get()->disableAccessebility) {
            AllowAccessibilityShortcutKeys(false);
        }
    }
    virtual void onShowFlags(wxCommandEvent& event) {
        auto conf = conf_copy();
        conf->showFlags = event.IsChecked();
        conf_set(conf);

        if (!conf_get()->showFlags) {
            myTray.SetIcon(icon, trayTooltip);
        } else {
            GetCurLayRequest();
        }

    }
    virtual void onDisableAccessebl(wxCommandEvent& event) {
        auto conf = conf_copy();
        conf->disableAccessebility = event.IsChecked();
        conf_set(conf);

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
            return std::format(L"sc: {}, vk: {}, '{}'", sc, vc, HotKeyNames::Global().GetName(vc));
        };

        m_listBoxRemap->Clear();
        for (auto iter = remap.GetIter(); !iter.IsEnd(); ++iter) {
            auto [sc, vc]   = iter.curElemSrc();
            auto [sc2, vc2] = iter.curElemDst();
            m_listBoxRemap->Append(std::format(L"{} -> {}", caption(sc, vc), caption(sc2, vc2)));
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
            ShowNeedAdmin("");
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
            auto conf = conf_copy();
            conf->hotkeysList[type].key() = newkey;
            auto res = conf->GetHk(type).key().ToString();
            conf_set(conf);
            obj->SetValue(res);

            Rereg_all();
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
        try {
            Startup::CheckTaskSheduleParm parm;
            parm.taskName = c_wszTaskName;
            IFS_LOG(Startup::CheckTaskShedule(parm));

            bool isHasEntry = false;
            std::wstring value;
            IFS_LOG(Startup::GetString_AutoStartUser(c_sRegRunValue, isHasEntry, value));

            std::wstring registryRes = L"none";
            std::wstring schedulRes = L"none";

            if (isHasEntry) {
                registryRes = value;
            }
            if (parm.isTaskExists) {
                schedulRes = parm.pathValue;
            }
            std::wstring sLabel = std::format(L"         Registry: {}\r\n         Scheduler: {}", registryRes, schedulRes);
            m_staticTextExplain->SetLabelText(sLabel);
        }
        catch (std::exception& e) {
            wxMessageBox(_("Error while UpdateAutostartExplain: ") + e.what());
        }
    }
    HKL all_lays[50] = { 0 };
    int all_lay_size = 0;
    wxChoice* getByIndex(int i) {
        if (i == 0)            return m_choiceset1;
        if(i == 1)            return m_choiceset2;
        if (i == 2)
            return m_choiceset3;
        return nullptr;
    }
    void onUiSelect(wxCommandEvent& event) override {
        auto conf = conf_copy();
        conf->uiLang = (SettingsGui::UiLang)m_comboUiLang->GetSelection();
        conf_set(conf);
        wxMessageBox(_("Need restart program"));
    }
    void UpdateUiLang() {
        m_comboUiLang->Clear();
        m_comboUiLang->AppendString(_("Russian"));
        m_comboUiLang->AppendString(_("English"));
        m_comboUiLang->SetSelection((int)conf_get()->uiLang);
    }
    void FillCombo() {
        m_choiceLayFilter->Clear();
        for (int i = 0; i < 3; i++) {
            getByIndex(i)->Clear();
        }
        all_lay_size = GetKeyboardLayoutList(SW_ARRAY_SIZE(all_lays), all_lays);
        for (int i = 0; i < all_lay_size; i++) {
            auto name = Utils::GetNameForHKL(all_lays[i]);
            m_choiceLayFilter->AppendString(name);

            for (int i = 0; i < 3; i++) {
                getByIndex(i)->AppendString(name);
            }
        }

        auto conf = conf_get();
        for (int i = 0; i < 3; i++) {
            auto cur = conf->hkl_lay[i];
            for (int j = 0; j < all_lay_size; j++) {
                if (all_lays[j] == cur) {
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
        auto lay = all_lays[cur];

        auto conf = conf_copy();

        if (obj == m_choiceLayFilter) {
            for (auto& elem : conf->customLangList) {
                if (elem == lay)
                    return;
            }
            conf->customLangList.push_back(lay);
            conf_set(conf);
            updateLayFilter();
        } else {
            if (obj == m_choiceset1) {
                conf->hkl_lay[0] = lay;
            } else if (obj == m_choiceset2) {
                conf->hkl_lay[1] = lay;
            } else {
                conf->hkl_lay[2] = lay;
            }
            conf_set(conf);
        }
    }
    void onClearFilter(wxCommandEvent& event) override {
        auto conf = conf_copy();
        conf->customLangList.clear();
        conf_set(conf);
        updateLayFilter();
    }
    void updateLayFilter() {
        std::wstring res;
        auto conf = conf_get();
        auto& lst = conf->customLangList;
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
        if (conf_get()->isMonitorAdmin) {

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
                    ShowNeedAdmin(_("delete old task"));
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

        m_checkAddToAutoStart->SetValue(conf_get()->isMonitorAdmin ? isAdminAllOk && !isUserHasTask
                                                               : isUserAllOk && !isAdminHasTask);
        UpdateAutostartExplain();

    }
    void ShowNeedAdmin(const wxString& expl) {
        wxString ms(_("Need admin rights"));
        if (expl != "") {
            ms += _(" for: ");
            ms += "\"";
            ms += expl;
            ms += "\"";
        }
        wxMessageBox(ms);
    }
    bool startOk() {
        return Utils::IsSelfElevated() || !conf_get()->isMonitorAdmin;
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
    void onSetLay(wxCommandEvent& event) {
        auto lay = myTray.LayById(event.GetId());
        //ActivateKeyboardLayout(lay,0);
        HWND hwndFocused = NULL;
        IFS_LOG(Utils::GetFocusWindow(hwndFocused));
        PostMessage(hwndFocused, WM_INPUTLANGCHANGEREQUEST, 0, (LPARAM)lay);
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
        auto conf = conf_copy();
        conf->isMonitorAdmin = m_checkBoxWorkInAdmin->GetValue();
        conf_set(conf);
        updateAutoStart();
        updateEnable();
    }
    void onAutocheck(wxCommandEvent& event) override {
        if (conf_get()->isMonitorAdmin && !Utils::IsSelfElevated()) {
            m_checkAddToAutoStart->SetValue(!m_checkAddToAutoStart->GetValue());
            ShowNeedAdmin("");
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
                ShowNeedAdmin("");
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
//    IFS_RET(g_setsgui.Load());
//    RETURN_SUCCESS;
//}

