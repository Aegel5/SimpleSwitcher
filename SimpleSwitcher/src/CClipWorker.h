#pragma once

enum EClipRequest
{
	CLRMY_NONE = 0,

	CLRMY_GET_FROM_CLIP,
	CLRMY_hk_COPY,
	CLRMY_hk_INSERT,
    CLRMY_hk_RESTORE,
};


class CClipWorker
{
private:


	TStatus GetFromClipBoardOur(std::wstring& data)
	{
		LOG_ANY(L"1..");

		HANDLE hData = GetClipboardData(CF_UNICODETEXT);
		IFW_RET(hData != NULL);

		LOG_ANY(L"2..");

		LPVOID lockSrc = GlobalLock(hData);
		IFW_RET(lockSrc != NULL);

		TCHAR* sTextSrc = (TCHAR*)lockSrc;
		data = sTextSrc;

		BOOL res = GlobalUnlock(lockSrc);
		DWORD dwerr = GetLastError();

		LOG_ANY(L"unlock res={}, err={}", res, dwerr);
		LOG_ANY_4(L"getted data. text='{}'", data);

		RETURN_SUCCESS;

	}
	TStatus OpenAndGetFromClipBoardOur(std::wstring& data)
	{
        if (!IsClipboardFormatAvailable(CF_UNICODETEXT)) {
			LOG_ANY(L"skip no unicodetext");
            RETURN_SUCCESS;
        }
		CAutoClipBoard clip;
		IFS_RET(OpenClipboard(clip));
		IFS_RET(GetFromClipBoardOur(data));

		RETURN_SUCCESS;
	}




	TStatus PutToClipBoardOur(std::wstring& data)
	{
		IFW_RET(EmptyClipboard());

		HGLOBAL hglbCopy = GlobalAlloc(GMEM_MOVEABLE, (data.length() + 1) * sizeof(TCHAR));
		IFW_RET(hglbCopy != NULL);

		LPVOID lock = GlobalLock(hglbCopy);
		IFW_RET(lock != NULL);

		TCHAR* sTextDst = (TCHAR*)lock;
		memcpy(sTextDst, data.c_str(), data.length() * sizeof(TCHAR));
		sTextDst[data.length()] = 0;

		BOOL res = GlobalUnlock(lock);
		DWORD dwerr = GetLastError();

		LOG_ANY(L"unlock put res={}, err={}", res, dwerr);
		LOG_ANY_4(L"putted text data='{}'",sTextDst);

		//m_clipboardOurPut = true;
		//LOG_ANY(L"put to buf: %s, size: %u", data.c_str(), data.length());
		IFW_RET(SetClipboardData(CF_UNICODETEXT, hglbCopy) != NULL);

		RETURN_SUCCESS;
	}

	    TStatus OpenAndPutToClipBoardOur(std::wstring& data) {
        if (data.empty())
            RETURN_SUCCESS;

        CAutoClipBoard clip;
        IFS_RET(OpenClipboard(clip));
        IFS_RET(PutToClipBoardOur(data));

        RETURN_SUCCESS;
    }





	//void ClipboardWorker()
	//{
		//IFW_LOG(SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_ABOVE_NORMAL));

		//TClipMessage msg;
		//while (true)
		//{
		//	if (!m_queueClip.GetMessage(msg))
		//	{
		//		break;
		//	}
		//	auto mode = msg.mode;
		//	if (mode == ClipMode_GetClipString)
		//	{
		//		std::wstring data;
		//		IFS_LOG(OpenAndGetFromClipBoardOur(data));

		//		LOG_INFO_2(L"buf: %s\n, len: %u", data.c_str(), data.length());

		//		if (data.length() == 0)
		//		{
		//			LOG_ANY(L"Skip empty buffer");
		//		}
		//		else if (data.length() > 1000)
		//		{
		//			LOG_ANY(L"Skip length > 1000");
		//		}
		//		else
		//		{
  //                  MoveToData(data);
		//			Worker()->PostMsg(HWORKER_GetClipStringCallback);
		//		}
		//	}
		//	else if (mode == ClipMode_SavePrevData)
		//	{
		//		std::wstring data;
		//		IFS_LOG(OpenAndGetFromClipBoardOur(data));
  //              MoveToData(data);
		//		Worker()->PostMsgW(HWORKER_SavePrevDataCallback, (WPARAM)msg.request);
		//	}
		//	else if (mode == ClipMode_RestoreClipData)
		//	{
  //              std::wstring data = TakeData();
  //              IFS_LOG(OpenAndPutToClipBoardOur(data));
		//	}
		//	else if (mode == ClipMode_ClipClearFormat)
		//	{
		//		IFS_LOG(ClipboardClearFormat());
		//	}
		//	//else if (mode == ClipMode_InsertData)
		//	//{
  // //             std::wstring data = TakeData();
		//	//	IFS_LOG(OpenAndPutToClipBoardOur(data));
		//	//}
		//}
		//LOG_ANY(L"Exit clip worker");
	//}
public:

    std::wstring getCurString() {
        std::wstring data;
        IFS_LOG(OpenAndGetFromClipBoardOur(data));

        LOG_ANY(L"buf: ????, len: {}", data.length());

        if (data.length() == 0) {
			LOG_ANY(L"Skip empty buffer");
        } else if (data.length() > 1000) {
			LOG_ANY(L"Skip length > 1000");
            data.clear();
        } else {
        }

        return data;
    }
    void setString(std::wstring& data) {
        IFS_LOG(OpenAndPutToClipBoardOur(data));
	}

    TStatus ClipboardClearFormat() {
		LOG_ANY(L"ClipboardClearFormat");

        if (!IsClipboardFormatAvailable(CF_UNICODETEXT)) {
            RETURN_SUCCESS;
        }

        CAutoClipBoard clip;
        IFS_RET(OpenClipboard(clip));

        UINT format = 0;
        bool fFound = false;

        while (1) {
            format = EnumClipboardFormats(format);
			LOG_ANY(L"Found format {}", format);
            if (format == 0) {
                if (GetLastError() != ERROR_SUCCESS) {
                    IFW_LOG(FALSE);
                }
                break;
            }
            if (format >= 40000) {
                fFound = true;
                break;
            }
        }

		LOG_ANY(L"fFound={}", fFound);

        if (!fFound) {
			LOG_ANY(L"Skip clear format because format not found");
            RETURN_SUCCESS;
        }

        std::wstring data;
        IFS_RET(GetFromClipBoardOur(data));

        if (data.length() == 0) {
			LOG_ANY(L"skip empty data");
            RETURN_SUCCESS;
        }

        IFS_RET(PutToClipBoardOur(data));

        RETURN_SUCCESS;
    }
 //   tstring TakeData()
	//{
 //       tstring loc;
 //       { 
	//		std::unique_lock<std::mutex> lock(mtxClipboardData);
 //           loc = std::move(m_sClipData);
	//	}
 //       return loc;
	//}
	//void MoveToData(tstring& data)
	//{
	//	std::unique_lock<std::mutex> lock(mtxClipboardData);
	//	m_sClipData = std::move(data);
	//}
	//TStatus Init()
	//{
	//	m_queueClip.StartWorker(std::bind(&CClipWorker::ClipboardWorker, this));
	//	RETURN_SUCCESS;
	//}
	//void PostMsg(ClipMode mode)
	//{
	//	TClipMessage msg;
	//	msg.mode = mode;
	//	m_queueClip.PostMsg(msg);
	//}
	//void PostMsg(ClipMode mode, EClipRequest request)
	//{
	//	TClipMessage msg;
	//	msg.mode = mode;
	//	msg.request = request;
	//	m_queueClip.PostMsg(msg);
	//}
};


