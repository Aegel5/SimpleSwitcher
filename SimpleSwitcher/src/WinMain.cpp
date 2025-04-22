#include "TrayIcon.h"

inline TStatus update_cur_dir() {
	std::wstring dir;
	IFS_RET(Utils::GetPath_folder_noLower(dir));
	IFW_RET(SetCurrentDirectory(dir.c_str()));
	RETURN_SUCCESS;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {

	{
		std::string cmdLine(lpCmdLine);
		g_autostart = cmdLine.find("/autostart") != std::string::npos;

		SetLogLevel(Utils::IsDebug() ? LOG_LEVEL_2 : LOG_LEVEL_DISABLE);
		__g_config.reset(new ProgramConfig());
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
		setlocale(LC_ALL, "en_US.utf8");
		IFS_LOG(update_cur_dir());
		LOG_ANY("Start program {}", SW_VERSION);
	}

	auto hwnd = WinUtils::CreateMsgWin(L"SimpleSwitcher_MainThread");
	IFW_LOG(hwnd != NULL);

	IFW_LOG(AddClipboardFormatListener(hwnd));

	TrayIcon trayIcon;

	trayIcon.Update();

	CoreWorker core;

	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0)>0) {
		auto mesg = msg.message;

		if (mesg == WM_CLIPBOARDUPDATE) {
			Worker()->PostMsg([](auto w) {w->CliboardChanged(); });
			continue;
		}

		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

}
