#include "InputSender.h"

void InputSender::Send()
{
	if (list.empty())
		return;

	for (auto& i : list)
	{
		LOG_ANY(L"SEND {} {}", TestFlag(i.ki.dwFlags, KEYEVENTF_KEYUP) ? L"UP" : L"DW", CHotKey::ToString(i.ki.wVk));
	}

	{
		InjectSkipper::LocSkipper loc;
		IFW_LOG(SendInput((UINT)list.size(), &list[0], sizeof(INPUT)) == list.size()); // синхронно вызывается наш хук.
	}

	//bool doPause = false;

	//// https://github.com/Aegel5/SimpleSwitcher/issues/53
	//if (WorkerImpl()->m_sTopProcName == L"notepad.exe" && IsWindows11OrGreater()) {
	//	// костыль для нового notepad...
	//	// не используем по дефолту, так как работаем медленнее...
	//	//doPause = true; 
	//}

	//if (doPause) {
	//	for (int i = 0; i < list.size();i+=2) {
	//		InjectSkipper::LocSkipper loc;
	//		auto res = SendInput(i+1 < list.size() ? 2 : 1, &list[i], sizeof(INPUT));
	//		IFW_LOG(res == 2);
	//		Sleep(1);
	//	}
	//}
	//else {
	//	InjectSkipper::LocSkipper loc;
	//	IFW_LOG(SendInput((UINT)list.size(), &list[0], sizeof(INPUT)) == list.size()); // синхронно вызывается наш хук.
	//}

	//return;
}
