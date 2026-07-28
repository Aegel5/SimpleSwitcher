
#include "TrayIcon.h"



inline TStatus update_cur_dir() { 
	std::wstring dir;
	IFS_RET(PathUtils::GetPath_folder_noLower(dir));
	IFW_RET(SetCurrentDirectory(dir.c_str()));
	RETURN_SUCCESS;
}


extern void StartGui();
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {

	setlocale(LC_ALL, "en_US.utf8");

	SetLogLevel(Utils::IsDebug() ? LOG_LEVEL_2 : LOG_LEVEL_DISABLE);

	if (!cfg_details::ReloadGuiConfig()) {
		MessageBox(
			NULL,
			L"Bad config",
			L"Error",
			MB_OK | MB_ICONERROR | MB_TASKMODAL
		);
		return 1;
	}

	IFS_LOG(update_cur_dir());
	LOG_ANY("Start program {}", GET_SW_VERSION());

	COM::CAutoCOMInitialize autoCom;
	IFS_LOG(autoCom.Init());

	CMainWorker::Inst().Init();

	g_isAura = std::filesystem::is_directory(PathUtils::GetPath_folder_noLower2() / "UI_Skins");
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

	CoreWorker core;

	StartGui();

	LOG_ANY("program exit");

	return 0;

}
