
#include "TrayIcon.h"
#include "GuiWorker.h"


inline TStatus update_cur_dir() {
	std::wstring dir;
	IFS_RET(Utils::GetPath_folder_noLower(dir));
	IFW_RET(SetCurrentDirectory(dir.c_str()));
	RETURN_SUCCESS;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {


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

	ApplyAcessebil();

	auto hwnd = WinUtils::CreateMsgWin(L"SimpleSwitcher_MainThread");
	IFW_LOG(hwnd != NULL);
	if (hwnd == 0) return 1;
	g_guiHandle = hwnd;
	IFW_LOG(AddClipboardFormatListener(hwnd));

	if (IsAdminOk()) {
		if (Utils::IsDebug() && !g_enabled.TryEnable()) {
			auto hk = conf_get_unsafe()->GetHk(hk_ToggleEnabled).keys.key();
			for (auto& it : hk) if (it == VKE_WIN) it = VK_LWIN;
			InputSender::SendHotKey(hk);
			Sleep(50);
		}
		g_enabled.TryEnable();
	}

	GuiWorker gui;

	if (std::string{ lpCmdLine }.find("/autostart") == std::string::npos) 
		gui.Start();

	TrayIcon trayIcon;

	CoreWorker core;

	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0)>0) {
		switch (msg.message) {
		case WM_CLIPBOARDUPDATE: {
			Worker()->PostMsg([](auto w) {w->CliboardChanged(); });
			break;
		}
		case WM_LayNotif: {
			trayIcon.Update((HKL)msg.wParam);
			break;
		}
		case WM_ShowWindow: {
			gui.Start();
			break;
		}
		default: {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			break;
		}

		}
	}

	return 0;

}
