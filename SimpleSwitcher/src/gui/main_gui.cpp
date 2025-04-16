#include "sw-base.h"

#include "noname.h" 
#include "SwAutostart.h"
#include "FloatPanel.h"
#include "utils/accessibil.h"
#include <wx/taskbar.h>
#include <wx/custombgwin.h>
#include "wxUtils.h"
#include "Tray.h"
#include "IconManager.h"

using namespace WxUtils;


extern void StartGui2();
class TemporGuiHolder {
	std::thread thr;
public:
	void Start() {
		thr = std::thread(StartGui2);
	}
	~TemporGuiHolder() {
		g_exit = true;
		if (thr.joinable())
			thr.join();
	}
};

class MainWnd : public MyFrame4
{
public:TemporGuiHolder gui2;
private:
    IconManager iconsMan;
    Timers timers;
    CoreWorker coreWork;
    MyTray myTray;
    bool exitRequest = false;
    bool inited = false;

    static constexpr wxLanguage supportTranslations[] = { wxLANGUAGE_ENGLISH , wxLANGUAGE_RUSSIAN, wxLANGUAGE_HEBREW };


    void UpdateIcons() {
        if (conf_get_unsafe()->flagsSet == ProgramConfig::showAppIcon) {
            myTray.ResetIcon(iconsMan.Get_Standart(!g_enabled.IsEnabled()).bundle);
        }
        else {
            Worker()->PostMsg(Message_GetCurLay{ true });
        }
    }
public:

    MainWnd() : MyFrame4(nullptr)
    {
        try {

            g_guiHandle = GetHandle();

			coreWork.Start();

            timers.StartCycle(200, []() { Worker()->PostMsg(Message_GetCurLay{}); });

            Bind(wxEVT_CLOSE_WINDOW, &MainWnd::onExitReqest, this);

            myTray.Init(this);
            myTray.ResetIcon(iconsMan.Get_Standart().bundle);
            this->SetIcons(iconsMan.GetAppIcons());
            myTray.Bind(wxEVT_MENU, &MainWnd::onExit, this, Minimal_Quit);
            myTray.Bind(wxEVT_MENU, [this](auto& evt) {
				show_wnd();
                }, Minimal_Show);
            myTray.Bind(wxEVT_MENU, [this](auto& evt) {
				g_enabled.TryToggle();
                }, Minimal_Activate);
            myTray.Bind(wxEVT_TASKBAR_LEFT_DCLICK, [this](auto& evt) {
				show_wnd();
                });

			UpdateIcons();

        }
        catch (std::exception& e) {
            wxMessageBox(_("Error while initing main wnd: ") + e.what());
        }
        inited = true;
    }

private:

	void show_wnd() {
		if (g_usenewgui) {
			g_show_gui = true;
		}
		else {
			ForceShow(this);
		}
	}

    void initChoiceFlags() {
        m_choiceShowInTray->Clear();
        m_choiceShowInTray->AppendString(ProgramConfig::showAppIcon);
        m_choiceShowInTray->AppendString(ProgramConfig::showOriginalFlags);
        for (const auto& it : iconsMan.ScanFlags()) {
            m_choiceShowInTray->AppendString(it);
        }
        updateShowInTray(conf_get_unsafe()->flagsSet);
    }

    void updateShowInTray(wxString newVal) {

        bool found = false;
        for (int i = 0; i < m_choiceShowInTray->GetCount(); i++) {
            auto cur = m_choiceShowInTray->GetString(i);
            if (cur == newVal) {
                m_choiceShowInTray->SetSelection(i);
                found = true;
                break;
            }
        }
        if (!found) {
            newVal = ProgramConfig::showOriginalFlags;
            m_choiceShowInTray->SetSelection(1);
        }
        if (conf_get_unsafe()->flagsSet != newVal) {
            SaveConfigWith([&](auto cfg) {cfg->flagsSet = newVal; });
        }
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



   virtual WXLRESULT MSWWindowProc(WXUINT nMsg, WXWPARAM wParam, WXLPARAM lParam) override {

        if (nMsg == WM_LayNotif) {

            if (!inited) 
                return TRUE;

            if (conf_get_unsafe()->flagsSet == ProgramConfig::showAppIcon)
                return TRUE;

            auto info = iconsMan.Get((HKL)wParam, !g_enabled.IsEnabled());
            myTray.ResetIcon(info.bundle);
        }
        else if (nMsg == WM_ShowWindow) {
			show_wnd();
        }
        else {
            return MyFrame4::MSWWindowProc(nMsg, wParam, lParam);
        }

        return TRUE;
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
            return std::format(L"sc: {}, vk: {}, '{}'", sc, vc, CHotKey::ToString(vc));
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
            //ShowNeedAdmin("");
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

public:

    void onExit(wxCommandEvent& event) override {
        exitRequest = true;
        LOG_ANY(L"exit request");
        Close(true);
    }

    void onCloseToTray(wxCommandEvent& event)     { 
        Hide();
    }

};

wxMenu* MyTray::CreatePopupMenu()  {

    auto menu = new wxMenu();

    for (int i = -1; const auto & it : conf_get_unsafe()->layouts_info.info) {
        i++;
        menu->Append(Minimal_SetLay_1 + i, Utils::GetNameForHKL(it.layout));
    }

    menu->AppendSeparator();
    menu->Append(Minimal_Activate, g_enabled.IsEnabled() ? _("Disable") : _("Enable"));
    menu->Append(Minimal_Show, _("Show"));
    menu->Append(Minimal_Quit, _("Exit"));


    return menu;
}



void StartMainGui(bool show, bool conf_err_msg) {

    MainWnd* frame = new MainWnd();

    // and show it (the frames, unlike simple controls, are not shown when
    // created initially)
    frame->Show(show && !g_usenewgui);

	if (g_usenewgui) {
		g_show_gui = show;
		frame->gui2.Start();
	}

    //if (conf_err_msg) {
    //    wxMessageBox(_("Error reading config"));
    //}

    //if(!startOk()) {
    //    ShowNeedAdmin();
    //}


}



