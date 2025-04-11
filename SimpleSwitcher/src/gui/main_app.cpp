#include "sw-base.h"

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

namespace {
    TStatus update_cur_dir() {
        std::wstring dir;
        IFS_RET(Utils::GetPath_folder_noLower(dir));
        IFW_RET(SetCurrentDirectory(dir.c_str()));
        RETURN_SUCCESS;
    }
}

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

        SetLogLevel(Utils::IsDebug() ? LOG_LEVEL_2 : LOG_LEVEL_0);

        __g_config.reset(new ProgramConfig());
        auto errLoadConf = LoadConfig(*__g_config);
        if (errLoadConf != TStatus::SW_ERR_SUCCESS) {
            IFS_LOG(errLoadConf);
        }
        else {
            if (__g_config->config_version != SW_VERSION) {
                __g_config->config_version = SW_VERSION;
                SaveConfigWith([](auto cfg) {}); // пересохраним конфиг, чтобы туда добавились все последние настройки, которые заполнены по-умолчанию.
            }
        }

        IFS_LOG(autoCom.Init());

        setlocale(LC_ALL, "en_US.utf8");

        LOG_ANY(L"Start program {}", SW_VERSION);


        // init ui
        if (conf_get_unsafe()->uiLang_ != wxLANGUAGE_ENGLISH) {

            wxTranslations* const trans = new wxTranslations();
            wxTranslations::Set(trans);
            trans->SetLoader(new wxResourceTranslationsLoader());
            trans->SetLanguage((wxLanguage)conf_get_unsafe()->uiLang_);
            trans->AddCatalog("lang");
        }

        IFS_LOG(update_cur_dir());

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