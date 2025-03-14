﻿#include "stdafx.h"

#include "noname.h" 
#include "Settings.h"
#include "CoreWorker.h"
#include "SwAutostart.h"
#include "decent_gui.h"

#include "FloatPanel.h"

#include "utils/accessibil.h"

#include <wx/taskbar.h>

#include "wxUtils.h"

#include "Tray.h"

using namespace WxUtils;

extern bool ChangeHotKey2(wxFrame* frame, CHotKeySet set, CHotKey& key);

namespace {
    void ShowNeedAdmin(const wxString& expl=L"") {
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
}

class MainWnd : public MyFrame4
{
    std::vector< FloatPanel*> floatPanels;
public:
    MainWnd() : MyFrame4(nullptr)
    {
        try {

            g_guiHandle = GetHandle();



            Bind(wxEVT_CLOSE_WINDOW, &MainWnd::onExitReqest, this);
            SetTitle(std::format(
                L"{} {}{}{}", 
                GetTitle().t_str(), 
                SW_VERSION, 
                Utils::IsSelfElevated() ? L" Administrator" : L"",
                Utils::IsDebug()?L" DEBUG": L""));
            SetWindowStyleFlag(wxMINIMIZE_BOX | wxCLOSE_BOX | wxCAPTION 
                | wxRESIZE_BORDER
            );

            auto sz = GetSize();
            sz.x = std::max((double)sz.x, sz.x * 1.15);
            SetSize(sz);


            m_staticTextBuildDate->SetLabelText(std::format(L"Built on '{}'", _SW_ADD_STR_UT(__DATE__)));

            m_hyperlink11->SetLabel(GetPath_Conf());
            m_hyperlink11->SetURL(std::format(L"file://{}", GetPath_Conf()));

            BindButtom(m_buttonReloadConfig, []() {
                auto conf = ConfPtr(new SettingsGui());
                auto errLoadConf = LoadConfig(*conf);
                IFS_LOG(errLoadConf);
                if (errLoadConf) {
                    wxMessageBox(_("Error reading config"));
                }
                else {
                    conf_set(conf);
                }
                });

            BindButtom(m_buttonAddPanel, [this]() {
                auto wnd = new FloatPanel(this);
                floatPanels.push_back(wnd);
                wnd->Show(true);
                });

            SyncLayouts();

            m_notebook2->SetSelection(0);

            BindCheckbox(m_checkDebuglog, []() {return conf_get()->IsNeedDebug(); }, [](bool val) {
                SetLogLevel_info(val? conf_get()->logLevel : LOG_LEVEL_0);
                });

            BindCheckbox(m_checkBoxFixRAlt, []() {return conf_get()->fixRAlt; }, [](bool val) {
                auto conf = conf_copy();
                conf->fixRAlt = val;
                conf_set(conf);
                });
            m_checkBoxFixRAlt->SetLabelText(m_checkBoxFixRAlt->GetLabelText() + L" \"" + Utils::GetNameForHKL(conf_get()->fixRAlt_lay_)+"\"");

            BindCheckbox(m_checkBoxPrevent, []() {return conf_get()->EnableKeyLoggerDefence; }, [](bool val) {
                auto conf = conf_copy();
                conf->EnableKeyLoggerDefence = val;
                conf_set(conf);
             });

            BindCheckbox(m_checkBoxAlterantiveLayoutChange, []() {return conf_get()->AlternativeLayoutChange; }, [this](bool val) {
                auto conf = conf_copy();
                conf->AlternativeLayoutChange = val;
                conf_set(conf);
                FillLayoutsInfo();
                FillHotkeysInfo();
                });

            BindCheckbox(m_checkBoxAllowInjected, []() {return conf_get()->AllowRemoteKeys; }, [](bool val) {
                auto conf = conf_copy();
                conf->AllowRemoteKeys = val;
                conf_set(conf);
                });

            BindCheckbox(m_checkBoxClearForm, []() {return conf_get()->fClipboardClearFormat; }, [](bool val) {
                auto conf = conf_copy();
                conf->fClipboardClearFormat = val;
                conf_set(conf);
                });

            BindCheckbox(m_checkBoxWorkInAdmin, []() {return conf_get()->isMonitorAdmin; }, [this](bool val) {
                auto conf = conf_copy();
                conf->isMonitorAdmin = val;
                conf_set(conf);
                updateAutoStart();
                updateEnable();
                });

            BindChoice(m_comboUiLang, [](wxChoice* elem) {
                    elem->Clear();
                    elem->AppendString(_("Russian"));
                    elem->AppendString(_("English"));
                    elem->SetSelection((int)conf_get()->uiLang);
                }, [](wxChoice* elem) {
                    auto conf = conf_copy();
                    conf->uiLang = (SettingsGui::UiLang)elem->GetSelection();
                    conf_set(conf);
                    wxMessageBox(_("Need restart program"));
                    });

            updateBools();

            updateAutoStart();

            FillHotkeysInfo();
            FillLayoutsInfo();

            updateCapsTab();
            handleDisableAccess();

            myTray.Init();
            myTray.Bind(wxEVT_MENU, &MainWnd::onExit, this, Minimal_Quit);
            myTray.Bind(wxEVT_MENU, [this](auto& evt) {
                ForceShow(this);
                }, Minimal_Show);
            myTray.Bind(wxEVT_TASKBAR_LEFT_DCLICK, [this](auto& evt) {
                ForceShow(this);
                });

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


    void onExitReqest(wxCloseEvent& event) {

        if (event.CanVeto() && !exitRequest) {
            Hide();
            event.Veto();
            return;
        }

        LOG_ANY(L"destroy GUI");
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

    void SetLay(HKL newLayout) {

        std::wstring name = L"appicon";

        WORD langid = LOWORD(newLayout);

        TCHAR buf[512];
        buf[0] = 0;

        int flag = LOCALE_SNAME;
        int len = GetLocaleInfo(MAKELCID(langid, SORT_DEFAULT), flag, buf, SW_ARRAY_SIZE(buf));
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
            bndl = myTray.standart_icon;
        }

        myTray.ResetIcon(bndl);
        for (auto& it : floatPanels) {
            it->SetFlag(bndl);
        }
    }

   virtual WXLRESULT MSWWindowProc(WXUINT nMsg, WXWPARAM wParam, WXLPARAM lParam) override {

        if (nMsg == WM_LayNotif) {

            if (!inited) 
                return TRUE;

            if (!conf_get()->showFlags)
                return TRUE;

            SetLay((HKL)wParam);

            return TRUE;
        }

        return MyFrame4::MSWWindowProc(nMsg, wParam, lParam);
   }

    void updateBools() {
        auto conf = conf_get();
        m_checkBoxDisablAcc->SetValue(conf->disableAccessebility);
        m_checkBoxShowFlags->SetValue(conf->showFlags);
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
            myTray.ResetIcon(myTray.standart_icon);
        } else {
            Worker()->PostMsg(HWORKER_Getcurlay);
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

        auto& lays = conf_get()->layouts_info.info;
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
                conf->layouts_info.info[row].hotkey.key() = newkey;
                conf_set(conf);
                FillLayoutsInfo();
            }
        }
        if (col == 0) {
            auto conf = conf_copy();
            conf->layouts_info.info[row].enabled ^= 1;
            //conf->Update_hk_from_layouts();
            conf_set(conf);
            FillLayoutsInfo();
        }
        if (col == 2) {
            CHotKey newkey;
            CHotKeySet set;
            set.def_list = {
                CHotKey(VK_LMENU, VK_SHIFT, 0x31)
                ,CHotKey(VK_LMENU, VK_SHIFT, 0x32)
                ,CHotKey(VK_LMENU, VK_SHIFT, 0x33)
                ,CHotKey(VK_CONTROL, VK_SHIFT, 0x31)
                ,CHotKey(VK_CONTROL, VK_SHIFT, 0x32)
                ,CHotKey(VK_CONTROL, VK_SHIFT, 0x33)
            };
            set.keys.key() = data.win_hotkey;
            if (ChangeHotKey2(this, set, newkey)) {
                auto conf = conf_copy();
                conf->layouts_info.info[row].win_hotkey = newkey;
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

    void FillHotkeysInfo() {

        ClearGrid(m_gridHotKeys);

        
        for (int i = -1; const auto& it : conf_get()->hotkeysList) {
            i++;
            if (it.hkId == hk_CycleLang_win_hotkey&& !conf_get()->AlternativeLayoutChange) break;
            m_gridHotKeys->AppendRows();
            m_gridHotKeys->SetRowLabelValue(i, it.gui_text);
            m_gridHotKeys->SetCellValue(i, 0, L" " + it.keys.ToString());
            if (it.hkId == hk_CycleLang_win_hotkey) {
                m_gridHotKeys->SetCellBackgroundColour(i, 0, wxColor(0xE3,0xF2,0xFD));
            }
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
        auto& info = info_copy.info;
        bool was_changes = false;

        // удалим те, которых сейчас нет в системе
        for (int i = (int)info.size() - 1; i >= 0; i--) {
            if (!has_system_layout(info[i].layout)) {
                Utils::RemoveAt(info, i);
                was_changes = true;
            }
        }

        // добавим все новые
        for (int i = 0; i < all_lay_size; i++) {
            auto cur = all_lays[i];
            if (!info_copy.HasLayout(cur)) {
                was_changes = true;
                CHotKey winhk(VK_LMENU, VK_SHIFT, VK_E_1 + info.size());
                info.push_back({ .layout = cur, .win_hotkey=winhk });
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

        ClearGrid(m_gridLayouts);

        while (m_gridLayouts->GetNumberCols() >= 3) {
            m_gridLayouts->DeleteCols(2);
        }
        if (conf_get()->AlternativeLayoutChange) {
            m_gridLayouts->AppendCols();
            m_gridLayouts->SetColLabelValue(2, L"Win hotkey");
        }

        // отобразим в gui
        for (int i = -1; const auto& it: conf_get()->layouts_info.info) {
            i++;
            auto name = Utils::GetNameForHKL(it.layout);
            m_gridLayouts->AppendRows();
            m_gridLayouts->SetRowLabelValue(i, name);
            if (it.enabled) {
                m_gridLayouts->SetCellValue(i, 0, "X");
            }
            m_gridLayouts->SetCellValue(i, 1, it.hotkey.ToString());

            if (conf_get()->AlternativeLayoutChange) {
                m_gridLayouts->SetCellValue(i, 2, it.win_hotkey.ToString());
                m_gridLayouts->SetCellBackgroundColour(i, 2, wxColor(0xE3, 0xF2, 0xFD));
            }
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

    //virtual WXLRESULT MSWWindowProc(WXUINT nMsg, WXWPARAM wParam, WXLPARAM lParam) override
    //{
    //    // Pass the messages to the original WinAPI window procedure
    //    return MyFrame4::MSWWindowProc(nMsg, wParam, lParam);
    //}
public:

    void onExit(wxCommandEvent& event) override {
        exitRequest = true;
        LOG_INFO_1(L"exit request");
        Close(true);
    }

    void onCloseToTray(wxCommandEvent& event)     { 
        Hide();
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

    if(!startOk()) {
        ShowNeedAdmin();
    }


}



