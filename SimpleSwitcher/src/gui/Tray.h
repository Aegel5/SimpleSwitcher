#pragma once 

enum
{
    // menu items
    Minimal_Quit = wxID_EXIT,

    // it is important for the id corresponding to the "About" command to have
    // this standard value as otherwise it won't be handled properly under Mac
    // (where it is special and put into the "Apple" menu)
    Minimal_About = wxID_ABOUT,

    Minimal_Show = wxID_HIGHEST + 1,

    Minimal_Activate,

    Minimal_SetLay_1, // last
};

class MainWnd;

class MyTray : public wxTaskBarIcon {

    wxString trayTooltip;
    MainWnd* parent = 0;

public:

    wxIcon standart_icon;

    void ResetIcon(const auto& newIcon) {
        SetIcon(newIcon, trayTooltip);
    }

    void Init(MainWnd* p) {

        parent = p;

        if (!IsAvailable())
            return;

        trayTooltip = L"SimpleSwitcher";
        trayTooltip += L" ";
        trayTooltip += SW_VERSION;
        standart_icon = wxIcon("appicon");
        ResetIcon(standart_icon);

        Bind(wxEVT_MENU, [this](wxCommandEvent& event) {
            auto id = event.GetId();
            if (id < Minimal_SetLay_1) {
                event.Skip();
                return;
            }
            auto info = conf_get_unsafe()->layouts_info.GetLayoutIndex(id - Minimal_SetLay_1);
            if (info != nullptr) {
                auto lay = info->layout;
                Worker()->PostMsg([lay](auto& w) {w.SetNewLay(lay);});
            }
            });
    }
    virtual wxMenu* CreatePopupMenu() override;
};

