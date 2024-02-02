#include "stdafx.h"

#include "Settings.h"

#include "tools/lay_notif.h"

#include "gui/decent_gui.h"

extern void StartMainGui(bool show, bool err_msg);

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

COM::CAutoCOMInitialize autoCom;

bool MyApp::OnInit() {
    // call the base class initialization method, currently it only parses a
    // few common command-line options but it could be do more in the future

    try {

        bool is_autostart = false;
        for (int i = 0; i < wxApp::argc; i++) {
            auto cur = wxApp::argv[i];
            if (cur == c_sArgAutostart) {
                is_autostart = true;
                break;
            }
        }

        if (is_autostart) {
            Sleep(1000); // дать системе прогрузиться
        }


        SetLogLevel(g_setsgui.fDbgMode ? LOG_LEVEL_1 : LOG_LEVEL_0);

        auto errLoadConf = LoadConfig(g_setsgui, true);
        IFS_LOG(errLoadConf);

        SetLogLevel_v3(g_setsgui.fDbgMode ? g_setsgui.logLevel : LOG_LEVEL_0);

        IFS_LOG(autoCom.Init());
        //Initlll();

        setlocale(LC_ALL, "en_US.utf8");

        if (errLoadConf == SW_ERR_SUCCESS) {
            // init ui
            if (g_setsgui.uiLang == SettingsGui::UiLang::rus) {

                wxTranslations* const trans = new wxTranslations();
                wxTranslations::Set(trans);
                trans->SetLoader(new wxResourceTranslationsLoader());
                trans->SetLanguage(wxLANGUAGE_RUSSIAN);
                trans->AddCatalog("lang");
            }

            Rereg_all();
        }



        StartMainGui(!is_autostart, errLoadConf != SW_ERR_SUCCESS);
    }
    catch (std::exception& e) {
        wxMessageBox(_("Error while initing app: ") + e.what());
    }

    // success: wxApp::OnRun() will be called which will enter the main message
    // loop and the application will run. If we returned false here, the
    // application would exit immediately.
    return true;
}