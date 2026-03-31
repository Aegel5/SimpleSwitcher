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
	bool Open(CAutoClipBoard& clip) {
		auto res = clip.Open();
		IFS_LOG(res);
		return res == TStatus::SW_ERR_SUCCESS;
	}
	TStatus Open2(CAutoClipBoard& clip) {
		return clip.Open();
	}

// ----------------- Backup и восстановление

private:
	// Карта: ID формата -> бинарные данные
	std::map<UINT, std::vector<std::uint8_t>> m_backup;
	const size_t MAX_TOTAL_SIZE = 600 * 1024;

public:
	bool HasBackup() {
		return !m_backup.empty();
	}
	void ClearBackup() {
		m_backup.clear();
	}
	bool BackupCurrent() {
		ClearBackup();

		CAutoClipBoard clip;
		// Предполагаю, что Open() возвращает true при успехе. 
		// Если Open() возвращает 0 при успехе (как WinAPI), проверьте условие!
		if (!Open(clip)) return false;

		size_t currentTotalSize = 0;
		UINT format = 0;

		static UINT f1 = RegisterClipboardFormatW(CFSTR_PREFERREDDROPEFFECT);
		static UINT f2 = RegisterClipboardFormatW(CFSTR_SHELLIDLIST);

		// Перебираем все доступные форматы в буфере
		while ((format = EnumClipboardFormats(format)) != 0) {
			if (!Utils::is_in(format,
				CF_UNICODETEXT,
				CF_HDROP,
				CF_DIB,
				f1,
				f2
			)) {
				continue; // скипаем мусор.
			}
			HANDLE hData = GetClipboardData(format);
			if (!hData) continue;

			size_t dataSize = GlobalSize(hData);

			// Проверяем, не выходим ли за лимит
			if (dataSize == 0 || (currentTotalSize + dataSize) > MAX_TOTAL_SIZE) {
				continue;
			}

			if (void* pData = GlobalLock(hData)) {
				m_backup[format].assign(
					static_cast<uint8_t*>(pData),
					static_cast<uint8_t*>(pData) + dataSize
				);

				currentTotalSize += dataSize;
				GlobalUnlock(hData);
			}
		}

		return !m_backup.empty();
	}
	bool Restore() {

		if (m_backup.empty()) return false;

		CAutoClipBoard clip;
		if (!Open(clip)) return false;

		// Очистка обязательна перед записью своих форматов
		if (!EmptyClipboard()) return false;

		bool success = true;
		for (const auto& [format, data] : m_backup) {

			if (data.empty()) continue; // Пропуск пустых данных

			// Выделяем глобальную память для Windows
			HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, data.size());
			if (!hMem) {
				success = false;
				continue;
			}

			// Копируем данные в выделенную память
			if (void* pDest = GlobalLock(hMem)) {
				std::memcpy(pDest, data.data(), data.size());
				GlobalUnlock(hMem);

				// Передаем владение памятью системе
				if (!SetClipboardData(format, hMem)) {
					GlobalFree(hMem); // Освобождаем только если Set failed
					success = false;
				}
			}
			else {
				GlobalFree(hMem);
				success = false;
			}
		}

		ClearBackup();
		return success;
	}

// ------------------ Остальное

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
		IFS_RET(Open2(clip));
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

		LOG_ANY(L"unlock PUT res={}, err={}", res, dwerr);
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
        IFS_RET(Open2(clip));
        IFS_RET(PutToClipBoardOur(data));

        RETURN_SUCCESS;
    }

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
        IFS_RET(Open2(clip));

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

};


