#include "stdafx.h"
#include "SimpleSwitcher.h"
#include "Settings.h"
#include "SwAutostart.h"
#include "SwGui.h"
#include "WinMainParameters.h"
#include "SimpleSwitcherQt.h"
#include <QtWidgets/QApplication>

using namespace WinMainParameters;


//TStatus MonitorOn(TSWAdmin admin);

TStatus HandleAutostart(HINSTANCE hInstance, HINSTANCE hPrevInstance);

TStatus StartGuiCheck64(int argc, char* argv[])
{
	if (IsSelf64())
	{
		CAutoHandle hProc32;
		IFS_RET(SwCreateProcessOur(0, SW_BIT_32, SW_ADMIN_SELF, hProc32));
	}
	else
	{
		if (argv != nullptr)
		{
			SimpleSwitcherQt w;
			w.show();
			auto res = QApplication::exec();
			LOG_INFO_1(L"End qt application with: %d", res);
			
		}
		else
		{
			IFS_RET(SwGui::StartGui(true));
		}
	}

	RETURN_SUCCESS;
}



TStatus MainInt(HINSTANCE hInstance, HINSTANCE hPrevInstance, int nCmdShow, int argc, char* argv[])
{
	gdata().hInst = hInstance;

	IFS_LOG(SettingsGlobal().Load());

	auto args = QCoreApplication::arguments();

	LOG_INFO_1(L"Start %s. Elevated=%d. Version=%s", 
		args.join(" ").toStdWString().c_str(), Utils::IsSelfElevated(), SW_VERSION_L);

	if (args.count() > 1)
	{
		auto cmd = args[1].toStdString();

		if (cmd == c_sArgHook32)
		{
			IFS_RET(StartMonitor(hInstance, hPrevInstance, nCmdShow, SW_BIT_32));
		}
		else if (cmd == c_sArgHook64)
		{
			IFS_RET(StartMonitor(hInstance, hPrevInstance, nCmdShow, SW_BIT_64));
		}
		else if (cmd == c_sArgStarter)
		{
			gdata().procMonitor.EnsureStarted(SW_ADMIN_SELF);
		}
		else if (cmd == c_sArgAutostart)
		{
			HandleAutostart(hInstance, hPrevInstance);

		}
		else if (cmd == c_sArgSchedulerON || cmd == c_sArgShedulerOFF)
		{
			COM::CAutoCOMInitialize autoCom;
			IFS_RET(autoCom.Init());
			if (cmd == c_sArgSchedulerON)
			{
				IFS_RET(SetSchedule());
			}
			else
			{
				IFS_RET(DelSchedule());
			}
		}
		else if (cmd == c_sArgCapsRemapAdd)
		{
			if (args.size() != 3)
			{
				IFS_RET(SW_ERR_UNKNOWN);
			}
			int type = args[2].toInt();
			IFS_LOG(AddRemap(RemapType(type)));
		}
		else if (cmd == c_sArgCapsRemapRemove)
		{
			if (args.size() != 3)
			{
				IFS_RET(SW_ERR_UNKNOWN);
			}
			int type = args[2].toInt();
			IFS_LOG(DelRemap(RemapType(type)));
		}
		else if (cmd == "/?" || cmd == "-h" || cmd == "--help")
		{
			const char* sHelp =
				"Command line options:\n"
				"/? - help\n"
				"/autostart - start program minimized (as on windows startup)\n";
			MessageBoxA(NULL, sHelp, SW_PROGRAM_NAME " " SW_VERSION, MB_OK);

		}
		else
		{
			IFS_RET(StartGuiCheck64(argc, argv));
		}
	}
	else
	{	
		IFS_RET(StartGuiCheck64(argc, argv));
	}

	RETURN_SUCCESS;
}

//#include "SimpleSwitcherQt.h"
//#include <QtWidgets/QApplication>
//int main(int argc, char* argv[])
//{
//	QApplication a(argc, argv);
//	SimpleSwitcherQt w;
//	w.show();
//	return a.exec();
//}

int APIENTRY Main1(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ int       nCmdShow,
	int argc, 
	char* argv[])
{
	UNREFERENCED_PARAMETER(hPrevInstance);


	TStatus status;
	status = MainInt(hInstance, hPrevInstance, nCmdShow, argc, argv);
	IFS_LOG(status);
	LOG_INFO_1(L"Exit process");

	return status;
}

//int APIENTRY _tWinMain(
//	_In_ HINSTANCE hInstance,
//	_In_opt_ HINSTANCE hPrevInstance,
//	_In_ LPTSTR    lpCmdLine,
//	_In_ int       nCmdShow)
//{
//	return Main1(hInstance, hPrevInstance, lpCmdLine, nCmdShow, 0, nullptr);
//}
//#pragma execution_character_set("utf-8")

int main(int argc, char* argv[])
{
	QApplication a(argc, argv);

	HINSTANCE hInstance = GetHInstance();
	HINSTANCE hPrevInstance = GetHPrevInstance();
	int nCmdShow = GetNCmdShow();

	return Main1(hInstance, hPrevInstance, nCmdShow, argc, argv);
	
}

TStatus HandleAutostart(HINSTANCE hInstance, HINSTANCE hPrevInstance)
{
	//SettingsGlobal().Load();

	TSWAdmin admin = SW_ADMIN_SELF;

	//if (IsSelfElevated() && !SettingsGlobal().isMonitorAdmin)
	//{
	//	admin = SW_ADMIN_OFF;
	//}

	IFS_LOG(gdata().procMonitor.EnsureStarted(admin));

	if (SettingsGlobal().IsAddToTray())
	{
		IFS_LOG(SwGui::StartGui(false));
	}

	RETURN_SUCCESS;
}

