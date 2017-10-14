#pragma once

#include <map>
#include <mutex>
#include <set>

namespace AVX {
namespace Debug {
namespace TestTime {

class Counter
{
private:

	using TKey = const wchar_t*;
	using TTid = unsigned long;
	using TCounter = unsigned long long;
	using TPrinter = std::function<void(const wchar_t*)>;

	struct KeyData
	{
		TCounter start = 0;
		//TCounter end = 0;

		TKey name = 0;
		TKey parent = 0;
		int stackCount = 0;

		TCounter totalDelt = 0;
		int totalCount = 0;

		std::set<TKey> childs;

	};

	struct DataThread
	{
		TKey curParent = 0;
		std::map<TKey, KeyData> startTimes;
	};

	struct SharedData
	{
		//std::map<TKey, DataKey> countersData;
		std::map<TTid, DataThread> threadsData;
		DWORD lastPrintTime = 0;
		std::mutex mtxData;
	};

	static SharedData& Shared()
	{
		static SharedData sharedData;
		return sharedData;
	}
	static TCounter GetTotal(KeyData& keyData)
	{
		TCounter total = keyData.totalDelt;

		if (keyData.start)
		{
			// yet not finished
			total += CalcDelt(keyData.start, GetCurCounter());
		}
		return total;
	}
	static long double GetPercent(TCounter t, TCounter tall)
	{
		long double percent = 100;
		if (tall != 0)
		{
			percent = (long double)t / tall * 100;
		}
		return percent;
	}
	static void PrintNode(KeyData& node, int depth, TPrinter printer, KeyData& parent, KeyData& rootParent)
	{
		TKey name = node.name;

		TCounter nodeTime = GetTotal(node);
		TCounter parentTime = GetTotal(parent);
		TCounter rootParentTime = GetTotal(rootParent);

		std::wstring sSpace(depth * 2, L' ');

		wchar_t sBuf[512];
		swprintf_s(sBuf,
			L"%s%s time=%.2lf (%.2lf%%) root(%.2lf%%) count=%u",
			sSpace.c_str(),
			name,
			ConvertToMs(nodeTime),
			GetPercent(nodeTime, parentTime),
			GetPercent(nodeTime, rootParentTime),
			(ULONG)node.totalCount);

		printer(sBuf);
	}
	static void PrintTree(KeyData& node, int depth, TPrinter printer, DataThread& thdata, KeyData& parent, KeyData& rootParent)
	{
		PrintNode(node, depth, printer, parent, rootParent);

		for (TKey k : node.childs)
		{
			KeyData& child = thdata.startTimes[k];
			PrintTree(child, depth + 1, printer, thdata, node, rootParent);
		}
	}
	static void printStats(TPrinter printer)
	{

		printer(L"");
		for (auto& it : Shared().threadsData)
		{
			TTid tid = it.first;
			DataThread& thdata = it.second;

			{
				wchar_t sBuf[512];
				swprintf_s(sBuf, L"[TestTime TID=%u]", tid);
				printer(sBuf);
			}

			KeyData& root = thdata.startTimes[NULL];
			for (TKey k : root.childs)
			{
				KeyData& child = thdata.startTimes[k];
				PrintTree(child, 1, printer, thdata, child, child);
			}
		}
		printer(L"");
		int k = 0;
	}

	static TCounter CalcDelt(TCounter nStart, TCounter nStop)
	{
		if (nStop < nStart)
		{
			return (TCounter(-1) - nStart) + nStop;
		}
		return nStop - nStart;
	}

	static long double ConvertToMs(TCounter val)
	{
		static TCounter qwFreq = 0;
		if (qwFreq == 0)
		{
			LARGE_INTEGER val;
			QueryPerformanceFrequency(&val);
			qwFreq = val.QuadPart;
		}
		return (long double)val * 1000 / qwFreq;
	}

	static TCounter GetCurCounter()
	{
		LARGE_INTEGER val;
		QueryPerformanceCounter(&val);
		return val.QuadPart;
	}

private:
	TKey m_name = 0;
	TKey m_PrevParentName = 0;
public:

	// TODO: string between printer
	static void PrintReport(TPrinter printer, int interval = 0)
	{
		std::unique_lock<std::mutex> lock(Shared().mtxData);
		int printDelt = GetTickCount() - Shared().lastPrintTime;
		if (interval == 0 || printDelt >= interval)
		{
			auto saved = Shared().lastPrintTime;
			Shared().lastPrintTime = GetTickCount();
			if (interval == 0 || saved != 0)
			{
				printStats(printer);
			}
		}
	}

	Counter(TKey sName) : m_name(sName)
	{
		std::unique_lock<std::mutex> lock(Shared().mtxData);
		DataThread& thdata = Shared().threadsData[GetCurrentThreadId()];

		auto res = thdata.startTimes.insert(std::make_pair(sName, KeyData()));
		KeyData& keyData = res.first->second;

		if (res.second)
		{
			keyData.parent = thdata.curParent;
			keyData.name = m_name;

			// set child for parent. create root if need.
			auto resParent = thdata.startTimes.insert(std::make_pair(thdata.curParent, KeyData()));
			resParent.first->second.childs.insert(m_name);
		}

		if (keyData.stackCount == 0)
		{
			keyData.start = GetCurCounter();
			//keyData.end = 0;
		}

		keyData.stackCount += 1;
		keyData.totalCount += 1;

		m_PrevParentName = thdata.curParent;
		thdata.curParent = m_name;
	}

	~Counter()
	{
		std::unique_lock<std::mutex> lock(Shared().mtxData);

		DataThread& thdata = Shared().threadsData[GetCurrentThreadId()];
		KeyData& keyData = thdata.startTimes[m_name];
		keyData.stackCount -= 1;
		//keyData.end = GetCurCounter();

		thdata.curParent = m_PrevParentName;

		if (keyData.stackCount == 0)
		{
			keyData.totalDelt += CalcDelt(keyData.start, GetCurCounter());
			keyData.start = 0;
			//keyData.end = 0;
		}
	}
};


}
}
}
