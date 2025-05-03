
#include "TrayIcon.h"
#include "utils/WinTimer.h"


inline TStatus update_cur_dir() {
	std::wstring dir;
	IFS_RET(PathUtils::GetPath_folder_noLower(dir));
	IFW_RET(SetCurrentDirectory(dir.c_str()));
	RETURN_SUCCESS;
}
extern int StartGui(bool show);
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {


	SetLogLevel(Utils::IsDebug() ? LOG_LEVEL_2 : LOG_LEVEL_DISABLE);

	{
		__g_config = MAKE_SHARED(__g_config);
		auto errLoadConf = LoadConfig(*__g_config);
		if (errLoadConf != TStatus::SW_ERR_SUCCESS) {
			IFS_LOG(errLoadConf);
		//ShowMessage("Error load config"); // todo check
		}
		else {
			if (__g_config->config_version != SW_VERSION) {
				__g_config->config_version = SW_VERSION;
				SaveConfigWith([](auto cfg) {}); // пересохраним конфиг, чтобы туда добавились все последние настройки, которые заполнены по-умолчанию.
			}
		}
	}

	setlocale(LC_ALL, "en_US.utf8");
	IFS_LOG(update_cur_dir());
	LOG_ANY("Start program {}", SW_VERSION);

	ApplyAcessebil();

	if (IsAdminOk()) {
		if (Utils::IsDebug() && !g_enabled.TryEnable()) {
			auto hk = conf_get_unsafe()->GetHk(hk_ToggleEnabled).keys.key();
			for (auto& it : hk) if (it == VKE_WIN) it = VK_LWIN;
			InputSender::SendHotKey(hk);
			Sleep(50);
		}
		g_enabled.TryEnable();
	}

	bool show = std::string{ lpCmdLine }.find("/autostart") == -1;

	CoreWorker core;

	StartGui(show);

	LOG_ANY("program exit");

	return 0;

}
