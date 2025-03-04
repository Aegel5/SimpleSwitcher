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



//extern bool ChangeHotKey(wxFrame* frame, HotKeyType type, CHotKey& key);
extern bool ChangeHotKey2(wxFrame* frame, CHotKeySet set, CHotKey& key);

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
            SetWindowStyleFlag(wxMINIMIZE_BOX | wxCLOSE_BOX | wxCAPTION 
                //| wxRESIZE_BORDER
            );
            auto sz = GetSize();
            sz.x = std::max((double)sz.x, sz.x * 1.1);
            SetSize(sz);

            m_staticTextBuildDate->SetLabelText(std::format(L"Built on '{}'", _SW_ADD_STR_UT(__DATE__)));


            m_notebook2->SetSelection(0);

            BindCheckbox(m_checkBoxFixRAlt, []() {return conf_get()->fixAltCtrl; }, [](bool val) {
                auto conf = conf_copy();
                conf->fixAltCtrl = val;
                conf_set(conf);
                });

            BindCheckbox(m_checkBoxAlterantiveLayoutChange, []() {return conf_get()->AlternativeLayoutChange; }, [](bool val) {
                auto conf = conf_copy();
                conf->AlternativeLayoutChange = val;
                conf_set(conf);
                });

            BindCheckbox(m_checkBoxAllowInjected, []() {return conf_get()->AllowRemoteKeys; }, [](bool val) {
                auto conf = conf_copy();
                conf->AllowRemoteKeys = val;
                conf_set(conf);
                });

            updateBools();

            updateAutoStart();
            FillHotkeysInfo();
            FillLayoutsInfo();

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
                for (const auto& it : conf_get()->layouts_info) {
                    myTray.AddLay(it.layout);
                    myTray.Bind(wxEVT_MENU, &MainWnd::onSetLay, this, Minimal_SetLay1);
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

    void BindCheckbox(wxCheckBox* elem, auto get, auto set) {
        elem->SetValue(get());
        elem->Bind(wxEVT_COMMAND_CHECKBOX_CLICKED,
            [elem,&set](wxCommandEvent& ev) {
                set(elem->GetValue());
            }
        );

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

        if (FindResource(0, name16.wc_str(), RT_RCDATA) == nullptr || FindResource(0, name32.wc_str(), RT_RCDATA) == nullptr) {
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
    }

    virtual void onEnableLog(wxCommandEvent& event)
    {
        auto conf = conf_copy();
        conf->fDbgMode = event.IsChecked();
        if (conf->fDbgMode) {
            //conf->time_debug_log_last_enabled = std::chrono::system_clock::now();
        }
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

    void handleDisableAccess() {
        if (conf_get()->disableAccessebility) {
            AllowAccessibilityShortcutKeys(false);
        }
    }
    virtual void onShowFlags(wxCommandEvent& event) override {
        auto conf = conf_copy();
        conf->showFlags = event.IsChecked();
        conf_set(conf);

        if (!conf_get()->showFlags) {
            myTray.SetIcon(icon, trayTooltip);
        } else {
            GetCurLayRequest();
        }

    }
    virtual void onDisableAccessebl(wxCommandEvent& event) override {
        auto conf = conf_copy();
        conf->disableAccessebility = event.IsChecked();
        conf_set(conf);

        handleDisableAccess();
    }

    virtual void onHotDClick(wxGridEvent& event) override {

        int col = event.GetCol();
        int row = event.GetRow();

        auto& hotlist = conf_get()->hotkeysList;
        if (row >= hotlist.size()) return;
        auto& data = hotlist[row];


        if (col == 0) {
            CHotKey newkey;
            CHotKeySet set;
            if (ChangeHotKey2(this, data, newkey)) {
                auto conf = conf_copy();
                conf->hotkeysList[row].keys.key() = newkey;
                conf_set(conf);
                FillHotkeysInfo();
            }
        }

    }

    virtual void on_grid_lay_double(wxGridEvent& event) override {
        int col = event.GetCol();
        int row = event.GetRow();

        auto& lays = conf_get()->layouts_info;
        if (row >= lays.size()) return;
        auto& data = lays[row];

        if (col == 1) {
            CHotKey newkey;
            CHotKeySet set;
            set.def_list.push_back(CHotKey(VK_LCONTROL).SetKeyup());
            set.def_list.push_back(CHotKey(VK_RCONTROL).SetKeyup());
            set.keys = data.hotkey;
            if (ChangeHotKey2(this, set, newkey)) {
                auto conf = conf_copy();
                conf->layouts_info[row].hotkey.key() = newkey;
                conf_set(conf);
                FillLayoutsInfo();
            }
        }
        if (col == 0) {
            auto conf = conf_copy();
            conf->layouts_info[row].enabled ^= 1;
            //conf->Update_hk_from_layouts();
            conf_set(conf);
            FillLayoutsInfo();
        }
        if (col == 2) {
            CHotKey newkey;
            CHotKeySet set;
            set.def_list.push_back(CHotKey(VK_LMENU, VK_SHIFT, 0x31));
            set.def_list.push_back(CHotKey(VK_LMENU, VK_SHIFT, 0x32));
            set.def_list.push_back(CHotKey(VK_LMENU, VK_SHIFT, 0x33));
            set.keys.key() = data.WinHotKey;
            if (ChangeHotKey2(this, set, newkey)) {
                auto conf = conf_copy();
                conf->layouts_info[row].WinHotKey = newkey;
                conf_set(conf);
                FillLayoutsInfo();
            }
        }
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



    //void onHotKey_ForEditBox(wxTextCtrl* obj) {
    //    if (!obj)
    //        return;
    //    HotKeyType type = (HotKeyType)(TUInt32)obj->GetClientData();
    //    CHotKey newkey;
    //    if (ChangeHotKey(this, type, newkey)) {
    //        auto conf = conf_copy();
    //        conf->GetHk(type).keys.key() = newkey;
    //        auto res = conf->GetHk(type).keys.key().ToString();
    //        conf_set(conf);
    //        obj->SetValue(res);

    //        //Rereg_all();
    //    }
    //}

    //void onHotKeyChange(wxMouseEvent& ev) {

    //    auto obj = wxDynamicCast(ev.GetEventObject(), wxTextCtrl);
    //    onHotKey_ForEditBox(obj);

    //    //ev.Skip();

    //}

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
    void ClearGrid(wxGrid* grid) {
        if (grid->GetNumberRows() > 0)
            grid->DeleteRows(0, grid->GetNumberRows());
    }
    void FillHotkeysInfo() {

        ClearGrid(m_gridHotKeys);

        int i = -1;
        for (const auto& it : conf_get()->hotkeysList) {
            i++;
            m_gridHotKeys->AppendRows();
            m_gridHotKeys->SetRowLabelValue(i, it.gui_text);
            m_gridHotKeys->SetCellValue(i, 0, L" " + it.keys.key().ToString());
        }

        m_gridHotKeys->SetRowLabelSize(wxGRID_AUTOSIZE);
        m_gridHotKeys->AutoSizeRows();
        m_gridHotKeys->SetColSize(0, m_gridHotKeys->GetSize().x - m_gridHotKeys->GetRowLabelSize());
        //m_gridHotKeys->HideColLabels();


        //m_gridHotKeys->setcol(5, 5);

    }
    void SyncLayouts() {

        HKL all_lays[50] = { 0 };
        int all_lay_size = GetKeyboardLayoutList(SW_ARRAY_SIZE(all_lays), all_lays);
        auto has_system_layout = [&](HKL lay) {
            for (int i = 0; i < all_lay_size; i++) {
                auto cur = all_lays[i];
                if (cur == lay) return true;
            }
            return false;
            };

        auto info_copy = conf_get()->layouts_info;
        bool was_changes = false;

        // удалим те, которых сейчас нет в системе
        for (int i = (int)info_copy.size() - 1; i >= 0; i--) {
            if (!has_system_layout(info_copy[i].layout)) {
                info_copy.erase(info_copy.begin() + i);
                was_changes = true;
            }
        }

        // добавим все новые
        for (int i = 0; i < all_lay_size; i++) {
            auto cur = all_lays[i];
            if (!conf_get()->HasLayout(cur)) {
                was_changes = true;
                info_copy.push_back({ .layout = cur });
            }
        }

        if (was_changes) {
            // пересохраним если были изменения.
            auto conf = conf_copy();
            conf->layouts_info = info_copy;
            conf_set(conf);
        }

    }
    void FillLayoutsInfo() {

        SyncLayouts();

        ClearGrid(m_gridLayouts);

        // отобразим в gui
        int i = -1;
        for (const auto& it: conf_get()->layouts_info) {
            i++;
            auto name = Utils::GetNameForHKL(it.layout);
            m_gridLayouts->AppendRows();
            m_gridLayouts->SetRowLabelValue(i, name);
            if (it.enabled) {
                m_gridLayouts->SetCellValue(i, 0, "X");
            }
            m_gridLayouts->SetCellValue(i, 1, it.hotkey.key().ToString2());
            m_gridLayouts->SetCellValue(i, 2, it.WinHotKey.ToString2());
        }
        //m_gridLayouts->SetRowLabelSize(wxGRID_AUTOSIZE);
        m_gridLayouts->SetRowLabelSize(m_gridHotKeys->GetRowLabelSize());
        m_gridLayouts->AutoSizeColumns(false);
        m_gridLayouts->AutoSizeRows();
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
        //auto lay = myTray.LayById(event.GetId());
        ////ActivateKeyboardLayout(lay,0);
        //HWND hwndFocused = NULL;
        //IFS_LOG(Utils::GetFocusWindow(hwndFocused));
        //PostMessage(hwndFocused, WM_INPUTLANGCHANGEREQUEST, 0, (LPARAM)lay);
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

