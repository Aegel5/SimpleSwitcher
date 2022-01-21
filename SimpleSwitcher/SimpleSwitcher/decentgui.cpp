#include "stdafx.h"

#include "decent_utils.h"

#include "gen_ui/noname.h" 

#include "Settings.h"



SW_NAMESPACE(SwGui)
extern bool ChangeHotKey2(HotKeyType type, HWND hwnd);
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
public:
    // ctor(s)
    MainWnd():MyFrame4(nullptr) {
        SetupToHotCtrl(m_textLastword, hk_RevertLastWord);
        SetupToHotCtrl(m_textSeveralWords, hk_RevertCycle);
        SetupToHotCtrl(m_textSelected, hk_RevertSel);
    }

    void onExit(wxCommandEvent& event) override {
        Close(true);
    }

    void onEnable(wxCommandEvent& event) override {
        auto cur = m_checkBoxEnable->IsChecked();
    }



};

bool MyApp::OnInit()
{
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