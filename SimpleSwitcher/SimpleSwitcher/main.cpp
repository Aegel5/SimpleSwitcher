#include "stdafx.h"
#include "SimpleSwitcher.h"
#include "Settings.h"
#include "SwAutostart.h"
#include "SwGui.h"
#include "WinMainParameters.h"
#include "SimpleSwitcherQt.h"
#include "QtWidgets/QApplication"

using namespace WinMainParameters;


//TStatus MonitorOn(TSWAdmin admin);

TStatus HandleAutostart(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine);

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
			QApplication a(argc, argv);
			SimpleSwitcherQt w;
			w.show();
			auto res = a.exec();
			LOG_INFO_1(L"End qt application with: %d", res);
			
		}
		else
		{
			IFS_RET(SwGui::StartGui(true));
		}
	}

	RETURN_SUCCESS;
}



TStatus MainInt(LPTSTR lpCmdLine, HINSTANCE hInstance, HINSTANCE hPrevInstance, int nCmdShow, int argc, char* argv[])
{
	gdata().hInst = hInstance;

	IFS_LOG(SettingsGlobal().Load());

	LOG_INFO_1(L"Start %s. Elevated=%d. Version=%s", *lpCmdLine ? lpCmdLine : L"GUI", Utils::IsSelfElevated(), SW_VERSION_L);

	std::vector<std::wstring> strs;
	IFS_RET(Str_Utils::Split(lpCmdLine, strs, L' '));


	if (!strs.empty() && !strs[0].empty())
	{
		std::wstring& cmd = strs[0];

		if (cmd == c_sArgHook32)
		{
			IFS_RET(StartMonitor(hInstance, hPrevInstance, lpCmdLine, nCmdShow, SW_BIT_32));
		}
		else if (cmd == c_sArgHook64)
		{
			IFS_RET(StartMonitor(hInstance, hPrevInstance, lpCmdLine, nCmdShow, SW_BIT_64));
		}
		else if (cmd == c_sArgStarter)
		{
			gdata().procMonitor.EnsureStarted(SW_ADMIN_SELF);
		}
		else if (cmd == c_sArgAutostart)
		{
			HandleAutostart(hInstance, hPrevInstance, lpCmdLine);

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
			if (strs.size() != 2)
			{
				IFS_RET(SW_ERR_UNKNOWN);
			}
			int type = std::stoi(strs[1]);
			IFS_LOG(AddRemap(RemapType(type)));
		}
		else if (cmd == c_sArgCapsRemapRemove)
		{
			if (strs.size() != 2)
			{
				IFS_RET(SW_ERR_UNKNOWN);
			}
			int type = std::stoi(strs[1]);
			IFS_LOG(DelRemap(RemapType(type)));
		}
		else if (cmd == L"/?" || cmd == L"-h" || cmd == L"--help")
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
	_In_ LPTSTR    lpCmdLine,
	_In_ int       nCmdShow,
	int argc, char* argv[])
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);


	TStatus status;
	status = MainInt(lpCmdLine, hInstance, hPrevInstance, nCmdShow, argc, argv);
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

int main(int argc, char* argv[])
{

	HINSTANCE hInstance = GetHInstance();
	HINSTANCE hPrevInstance = GetHPrevInstance();
	LPWSTR lpCmdLine = GetLPCmdLine();
	int nCmdShow = GetNCmdShow();

	return Main1(hInstance, hPrevInstance, lpCmdLine, nCmdShow, argc, argv);
	
}

TStatus HandleAutostart(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine)
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

