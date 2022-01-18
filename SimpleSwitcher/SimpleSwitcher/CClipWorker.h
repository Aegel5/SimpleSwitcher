#pragma once

#include "CMainWorker.h"

enum ClipMode
{
	ClipMode_GetClipString,
	ClipMode_ClipClearFormat,
	ClipMode_SavePrevData,
	ClipMode_RestoreClipData,
	ClipMode_InsertData,
};

enum EClipRequest
{
	CLR_NONE = 0,

	CLR_GET_FROM_CLIP,
	CLR_hk_COPY,
	CLR_hk_INSERT,
};

struct TClipMessage
{
	ClipMode mode;

	union
	{
		EClipRequest request;
	};
};

class CClipWorker
{
private:
	std::mutex  mtxClipboardData;
	std::wstring m_sClipData;
	std::wstring m_clipboardSave;
	ThreadQueue::CThreadQueue<TClipMessage> m_queueClip;

	TStatus GetFromClipBoardOur(std::wstring& data)
	{
		LOG_INFO_1(L"1..");

		HANDLE hData = GetClipboardData(CF_UNICODETEXT);
		IFNULL(hData) RETW();

		LOG_INFO_1(L"2..");

		LPVOID lockSrc = GlobalLock(hData);
		IFNULL(lockSrc) RETW();

		TCHAR* sTextSrc = (TCHAR*)lockSrc;
		data = sTextSrc;

		BOOL res = GlobalUnlock(lockSrc);
		DWORD dwerr = GetLastError();

		LOG_INFO_1(L"unlock res=%u, err=%u", res, dwerr);

		RETURN_SUCCESS;

	}
	TStatus OpenAndGetFromClipBoardOur(std::wstring& data)
	{
		CAutoClipBoard clip;
		IFS_RET(clip.Open(gdata().hWndMonitor));
		IFS_RET(GetFromClipBoardOur(data));

		RETURN_SUCCESS;
	}

	TStatus SaveClipboardText(std::wstring& data)
	{
		LOG_INFO_1(L"SaveClipboardText");

		data.clear();

		if (!IsClipboardFormatAvailable(CF_UNICODETEXT))
		{
			RETURN_SUCCESS;
		}

		CAutoClipBoard clip;
		IFS_RET(clip.Open(gdata().hWndMonitor));

		IFS_RET(GetFromClipBoardOur(data));

		RETURN_SUCCESS;
	}

	TStatus ClipboardClearFormat()
	{
		LOG_INFO_1(L"ClipboardClearFormat");

		if (!IsClipboardFormatAvailable(CF_UNICODETEXT))
		{
			RETURN_SUCCESS;
		}

		std::wstring data;

		CAutoClipBoard clip;
		IFS_RET(clip.Open(gdata().hWndMonitor));

		UINT format = 0;
		bool fFound = false;

		while (1)
		{
			format = EnumClipboardFormats(format);
			LOG_INFO_1(L"Found format %u", format);
			if (format == 0)
			{

				if (GetLastError() != ERROR_SUCCESS)
				{
					IFW_LOG(FALSE);
				}
				break;
			}
			if (format >= 40000)
			{
				fFound = true;
			}
		}

		LOG_INFO_1(L"fFound=%u", fFound);

		if (!fFound)
		{
			LOG_INFO_1(L"Skip clear format because format not found");
			RETURN_SUCCESS;
		}

		IFS_RET(GetFromClipBoardOur(data));

		if (data.length() == 0)
		{
			LOG_INFO_1(L"skip empty data");
			RETURN_SUCCESS;
		}

		//std::wstring f100 = data.substr(0, 1000);

		////LOG_INFO_1(
		////	L"data: %s\n, f100: %s\n, m_sLastClipFormatFree: %s", 
		////	data.c_str(), 
		////	f100.c_str(),
		////	m_sLastClipFormatFree.c_str());

		//if (f100 == m_sLastClipFormatFree)
		//{
		//	LOG_INFO_1(L"Skip clear format for same");
		//	RETURN_SUCCESS;
		//}
		//m_sLastClipFormatFree = data.substr(0, 1000);

		IFS_RET(PutToClipBoardOur(data));


		//if (statPut != SW_ERR_SUCCESS)
		//{
		//	LOG_INFO_1(L"try another...");
		//	CAutoClipBoard clip;
		//	IFS_RET(clip.Open(CommonDataGlobal().hWndMonitor));
		//	IFS_RET(PutToClipBoardOur(data));
		//}

		RETURN_SUCCESS;
	}

	TStatus PutToClipBoardOur(std::wstring& data)
	{
		IFW_RET(EmptyClipboard());

		HGLOBAL hglbCopy = GlobalAlloc(GMEM_MOVEABLE, (data.length() + 1) * sizeof(TCHAR));
		IFNULL(hglbCopy) RETW();

		LPVOID lock = GlobalLock(hglbCopy);
		IFNULL(lock) RETW();
		TCHAR* sTextDst = (TCHAR*)lock;
		memcpy(sTextDst, data.c_str(), data.length() * sizeof(TCHAR));
		sTextDst[data.length()] = 0;

		BOOL res = GlobalUnlock(lock);
		DWORD dwerr = GetLastError();
		LOG_INFO_1(L"unlock put res=%u, err=%u", res, dwerr);

		//m_clipboardOurPut = true;
		//LOG_INFO_1(L"put to buf: %s, size: %u", data.c_str(), data.length());
		IFW_RET(SetClipboardData(CF_UNICODETEXT, hglbCopy) != NULL);

		RETURN_SUCCESS;
	}

	TStatus OpenAndPutToClipBoardOur(std::wstring& data)
	{
		CAutoClipBoard clip;
		IFS_RET(clip.Open(gdata().hWndMonitor));
		IFS_RET(PutToClipBoardOur(data));

		RETURN_SUCCESS;
	}

	TStatus RestoreClipboardText()
	{
		LOG_INFO_1(L"RestoreClipboardText...");

		std::wstring data = m_clipboardSave;
		m_clipboardSave.clear();

		if (data.empty())
		{
			RETURN_SUCCESS;
		}

		IFS_RET(OpenAndPutToClipBoardOur(data));

		RETURN_SUCCESS;
	}

	void ClipboardWorker()
	{
		IFW_LOG(SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_ABOVE_NORMAL));

		TClipMessage msg;
		while (true)
		{
			if (!m_queueClip.GetMessage(msg))
			{
				break;
			}
			auto mode = msg.mode;
			if (mode == ClipMode_GetClipString)
			{
				std::wstring data;
				IFS_LOG(OpenAndGetFromClipBoardOur(data));

				LOG_INFO_2(L"buf: %s\n, len: %u", data.c_str(), data.length());

				if (data.length() == 0)
				{
					LOG_INFO_1(L"Skip empty buffer");
				}
				else if (data.length() > 1000)
				{
					LOG_INFO_1(L"Skip length > 1000");
				}
				else
				{
					{
						std::unique_lock<std::mutex> lock(mtxClipboardData);
						m_sClipData = data;
					}

					Worker()->PostMsg(HWORKER_GetClipStringCallback);
					//PostMessage(CommonDataGlobal().hWndMonitor, WM_ClipDataArrave, 0, 0);
				}
			}
			else if (mode == ClipMode_SavePrevData)
			{
				std::wstring data;
				IFS_LOG(OpenAndGetFromClipBoardOur(data));

				m_clipboardSave = data;

				Worker()->PostMsgW(HWORKER_SavePrevDataCallback, (WPARAM)msg.request);
			}
			else if (mode == ClipMode_RestoreClipData)
			{
				IFS_LOG(RestoreClipboardText());
			}
			else if (mode == ClipMode_ClipClearFormat)
			{
				IFS_LOG(ClipboardClearFormat());
			}
			else if (mode == ClipMode_InsertData)
			{
				std::wstring data;
				{
					std::unique_lock<std::mutex> lock(mtxClipboardData);
					data = m_sClipData;
				}

				IFS_LOG(OpenAndPutToClipBoardOur(data));
			}
		}
		LOG_INFO_1(L"Exit clip worker");
	}
public:
	void GetData(tstring& data)
	{
		std::unique_lock<std::mutex> lock(mtxClipboardData);
		data = m_sClipData;
		m_sClipData.clear();
	}
	void SetData(tstring& data)
	{
		std::unique_lock<std::mutex> lock(mtxClipboardData);
		m_sClipData = data;
	}
	TStatus Init()
	{
		m_queueClip.StartWorker(std::bind(&CClipWorker::ClipboardWorker, this));
		RETURN_SUCCESS;
	}
	void PostMsg(ClipMode mode)
	{
		TClipMessage msg;
		msg.mode = mode;
		m_queueClip.PostMsg(msg);
	}
	void PostMsg(ClipMode mode, EClipRequest request)
	{
		TClipMessage msg;
		msg.mode = mode;
		msg.request = request;
		m_queueClip.PostMsg(msg);
	}
};


