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

bool MyApp::OnInit()
{
    // call the base class initialization method, currently it only parses a
    // few common command-line options but it could be do more in the future
    //if (!wxApp::OnInit())
    //    return false;

    SetLogLevel(setsgui.fDbgMode ? LOG_LEVEL_1 : LOG_LEVEL_0);

    bool notExists = false;

    auto err = LoadConfig(setsgui, &notExists);

    IFS_LOG(err);

    if (notExists) {
        IFS_LOG(Save());
    }


    //if (err != SW_ERR_SUCCESS) {
    //    wxMessageBox("Error reading config");
    //}
    SetLogLevel2(setsgui.fDbgMode ? setsgui.logLevel : LOG_LEVEL_0);

    IFS_LOG(autoCom.Init());
    //Initlll();

    setlocale(LC_ALL, "en_US.utf8");

    bool show = true;
    for (int i = 0; i < wxApp::argc; i++) {
        auto cur = wxApp::argv[i];
        if (cur == c_sArgAutostart) {
            show = false;
            break;
        }
    }

    StartMainGui(show, err != SW_ERR_SUCCESS);

    // success: wxApp::OnRun() will be called which will enter the main message
    // loop and the application will run. If we returned false here, the
    // application would exit immediately.
    return true;
}