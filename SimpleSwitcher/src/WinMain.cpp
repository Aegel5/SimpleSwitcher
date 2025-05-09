
#include "TrayIcon.h"
#include "utils/WinTimer.h"


inline TStatus update_cur_dir() {
	std::wstring dir;
	IFS_RET(PathUtils::GetPath_folder_noLower(dir));
	IFW_RET(SetCurrentDirectory(dir.c_str()));
	RETURN_SUCCESS;
}
extern int StartGui(bool show, bool);
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {


	SetLogLevel(Utils::IsDebug() ? LOG_LEVEL_2 : LOG_LEVEL_DISABLE);

	auto conf_ok = cfg_details::ReloadGuiConfig();

	setlocale(LC_ALL, "en_US.utf8");
	IFS_LOG(update_cur_dir());
	LOG_ANY("Start program {}", SW_VERSION);

	CMainWorker::Inst().Init();

	ApplyLocalization();
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

	StartGui(show, !conf_ok);

	LOG_ANY("program exit");

	return 0;

}
