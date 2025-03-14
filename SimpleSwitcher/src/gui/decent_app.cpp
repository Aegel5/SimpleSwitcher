﻿#include "stdafx.h"

#include "gui/decent_gui.h"

#include "CMainWorker.h"

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

        auto conf = __g_config.get();

        SetLogLevel(Utils::IsDebug() ? LOG_LEVEL_1 : LOG_LEVEL_0);

        auto errLoadConf = LoadConfig(*conf);
        IFS_LOG(errLoadConf);

        IFS_LOG(autoCom.Init());

        setlocale(LC_ALL, "en_US.utf8");

        if (conf->config_version != SW_VERSION) {
            conf->config_version = SW_VERSION;
            IFS_LOG(Save()); // пересохраним конфиг, чтобы туда добавились все последние настройки, которые заполнены по-умолчанию.
        }

        // init ui
        if (conf->uiLang == SettingsGui::UiLang::rus) {

            wxTranslations* const trans = new wxTranslations();
            wxTranslations::Set(trans);
            trans->SetLoader(new wxResourceTranslationsLoader());
            trans->SetLanguage(wxLANGUAGE_RUSSIAN);
            trans->AddCatalog("lang");
        }

        // Запускаем воркера, теперь он будет работать всегда, даже когда программа выключена, чтобы указатели всегда были валидны.
        static CMainWorker mainWorker;
        gdata().mainWorker = &mainWorker;
        //IFS_LOG(Worker()->ReStart());

        StartMainGui(!is_autostart, errLoadConf != SW_ERR_SUCCESS);
    }
    catch (std::exception& e) {
        wxMessageBox(_("Error while initing app: ") + e.what());
        return false;
    }

    // success: wxApp::OnRun() will be called which will enter the main message
    // loop and the application will run. If we returned false here, the
    // application would exit immediately.
    return true;
}