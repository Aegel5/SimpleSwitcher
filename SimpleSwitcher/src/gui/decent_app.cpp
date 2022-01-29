#include "stdafx.h"

#include "Settings.h"

extern void StartMainGui(bool show);

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

bool MyApp::OnInit()
{
    //if (Utils::ProcSingleton(c_mtxSingltonGui))
    //{
    //    LOG_INFO_1(L"Gui already running.Exit");
    //    return false;
    //}

    SetLogLevel(setsgui.fDbgMode ? LOG_LEVEL_1 : LOG_LEVEL_0);
    IFS_LOG(setsgui.Load());
    IFS_LOG(u_conf.Load2());
    if (setsgui.fDbgMode && u_conf.ll != 0) {
        LOG_INFO_1(L"Set ll %u", u_conf.ll);
        SetLogLevel((TLogLevel)u_conf.ll);
    }

    COM::CAutoCOMInitialize autoCom;
    IFS_LOG(autoCom.Init());
    //auto res = Init();
    //IFS_LOG(res);
    //if (res != SW_ERR_SUCCESS)
    //    return false;

    // call the base class initialization method, currently it only parses a
    // few common command-line options but it could be do more in the future
    if (!wxApp::OnInit())
        return false;

    bool show = true;
    for (int i = 0; i < wxApp::argc; i++) {
        auto cur = wxApp::argv[i];
        if (cur == c_sArgAutostart) {
            show = false;
            break;
        }
    }

    StartMainGui(show);

    // success: wxApp::OnRun() will be called which will enter the main message
    // loop and the application will run. If we returned false here, the
    // application would exit immediately.
    return true;
}