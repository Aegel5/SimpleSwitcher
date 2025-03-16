#pragma once
#include <fstream>
#include <clocale>
#include "Errors.h"
#include <mutex>

enum TLogLevel
{
	LOG_LEVEL_0 = 0,  // No log
	LOG_LEVEL_1 = 1,
	LOG_LEVEL_2 = 2,
	LOG_LEVEL_3 = 3,
	LOG_LEVEL_4 = 4,

	LOG_LEVEL_ERROR = LOG_LEVEL_1,
};


class SwLogger
{
public:
	static SwLogger&  Get() 
	{
		static SwLogger logger;
		return logger;
	}
	void VFormat(const TChar* format, va_list args)
	{
		if(LazyOpen())
        {
			vfwprintf_s(m_fp, format, args);
        }
	}
	void Append(const TChar* data)
	{
		if (LazyOpen())
		{
			fputws(data, m_fp);
		}
	}
	void Format(const TChar* Format, ...)
	{
		va_list alist;
		va_start(alist, Format);
		VFormat(Format, alist);
		va_end(alist);
	}
	void Flash()
	{
		if (m_fp)
			fflush(m_fp);
	}
	void EndLineFlash()
	{
		if(LazyOpen())
		{
			fwprintf_s(m_fp, L"\n");
			fflush(m_fp);
		}
	}
	~SwLogger()
	{
		if(m_fp)
		{
			fclose(m_fp);
		}
	}
	void SetLogLevel(TLogLevel logLevel)
	{
		m_logLevel = logLevel;
	}
	TLogLevel GetLogLevel()
	{
		return m_logLevel;
	}
	std::mutex& Mtx()
	{
		return m_mtxLog;
	}
private:
	FILE* LazyOpen()
	{
		if (!m_fp)
		{
			if (!m_tryOpen)
			{
				m_tryOpen = true;

				static const size_t nSize = 0x1000;
				std::unique_ptr<TChar[]> buf(new TChar[nSize]);
				TChar* sFolder = buf.get();
				if (!sFolder)
				{
					return m_fp;
				}
				
				if (!GetModuleFileNameEx(GetCurrentProcess(), GetCurrentModule(), sFolder, nSize))
					return NULL;
				TChar* sLast = wcsrchr(sFolder, L'\\');
				if (sLast)
					*sLast = 0;
				wcscat_s(sFolder, nSize, L"\\log");
				CreateDirectory(sFolder, NULL);
				TChar base[512];
				base[0] = 0;
				GetModuleBaseName(GetCurrentProcess(), NULL, base, SW_ARRAY_SIZE(base));

				auto path = std::format(L"{}\\{}.log", sFolder, base);
				//TChar sLogPath[0x1000];
				//sLogPath[0] = 0;
				//if (swprintf_s(sLogPath, L"%s\\%s(%d)_%u.log", sFolder, base, GetCurrentProcessId(), GetTick()) == -1)
				//{
				//	return m_fp;
				//}
				m_fp = _wfsopen(path.c_str(), L"wt, ccs=UTF-8", _SH_DENYNO);
			}
		}
		return m_fp;
	}
	std::mutex m_mtxLog;
	TLogLevel m_logLevel = LOG_LEVEL_0;
	FILE* m_fp = NULL;
	bool m_tryOpen = false;
	SwLogger()	{}

};


inline SwLogger& SwLoggerGlobal() {return SwLogger::Get();}

inline TLogLevel GetLogLevel() { return SwLoggerGlobal().GetLogLevel(); }
inline void SetLogLevel(TLogLevel logLevel) { SwLoggerGlobal().SetLogLevel(logLevel); }


inline void __SW_LOG_FORMAT_V__(const TChar* format, va_list alist)
{
	SwLoggerGlobal().VFormat(format, alist);
}
inline void __SW_LOG_TIME()
{
	SYSTEMTIME ST;
	::GetLocalTime(&ST);
	SwLoggerGlobal().Format(
		L"%02u.%02u|%02u:%02u:%02u.%03u|%05u ",
		(TUInt32)ST.wDay, 
		(TUInt32)ST.wMonth,
		(TUInt32)ST.wHour, 
		(TUInt32)ST.wMinute, 
		(TUInt32)ST.wSecond, 
		(TUInt32)ST.wMilliseconds,
		GetCurrentThreadId());
}

inline void __SW_LOG_FORMAT__(const TChar* Format, ...)
{
	va_list alist;
	va_start(alist, Format);
	__SW_LOG_FORMAT_V__(Format, alist);
	va_end(alist);
}

inline void SW_LOG_INFO(const TChar* Format, ...)
{
	std::unique_lock<std::mutex> _lock(SwLoggerGlobal().Mtx());

	__SW_LOG_TIME();
	va_list alist;
	va_start(alist, Format);
	SwLoggerGlobal().VFormat(Format, alist);
	va_end(alist);
	SwLoggerGlobal().EndLineFlash();
}


#define LOG_INFO_4(...) {if(GetLogLevel() >= LOG_LEVEL_4){SW_LOG_INFO(__VA_ARGS__);}}
#define LOG_INFO_3(...) {if(GetLogLevel() >= LOG_LEVEL_3){SW_LOG_INFO(__VA_ARGS__);}}
#define LOG_INFO_2(...) {if(GetLogLevel() >= LOG_LEVEL_2){SW_LOG_INFO(__VA_ARGS__);}}
#define LOG_INFO_1(...) {if(GetLogLevel() >= LOG_LEVEL_1){SW_LOG_INFO(__VA_ARGS__);}}

inline void __LOG_LINE(const TChar* s) {
	std::unique_lock<std::mutex> _lock(SwLoggerGlobal().Mtx());
	__SW_LOG_TIME();
	SwLoggerGlobal().Append(s);
	SwLoggerGlobal().EndLineFlash();
}

void __LOG_LINE_FORMAT(const auto& s, auto&&... v) {
	auto res = std::vformat(s.get(), std::make_wformat_args(v...));
	__LOG_LINE(res.c_str()); 
}

template<typename... Args> inline void LOG_ANY(const std::wformat_string<Args...> s, Args&&... v)
{ if (GetLogLevel() >= LOG_LEVEL_2) { __LOG_LINE_FORMAT(s, v...); } }

template<typename... Args> inline void LOG_ANY_4(const std::wformat_string<Args...> s, Args&&... v) 	{
 if (GetLogLevel() >= LOG_LEVEL_4) { __LOG_LINE_FORMAT(s, v...); } }

template<typename... Args> inline void LOG_WARN(const std::wformat_string<Args...> s, Args&&... v){
	if (GetLogLevel() >= LOG_LEVEL_1) {
		std::unique_lock<std::mutex> _lock(SwLoggerGlobal().Mtx());
		__SW_LOG_TIME();
		SwLoggerGlobal().Append(L"[WARN] ");
		SwLoggerGlobal().Append(std::vformat(s.get(), std::make_wformat_args(v...)).c_str());
		SwLoggerGlobal().EndLineFlash();
	}
}

inline void SetLogLevel_info(TLogLevel logLevel){
	SetLogLevel(logLevel);
	LOG_ANY(L"Log level now {}", (int)logLevel);
}

#define RETURN_SUCCESS {return SW_ERR_SUCCESS; }

class WinErrBOOL
{
	bool m_res = false;
	DWORD m_dwErr;
public:
	WinErrBOOL(BOOL r) : m_res(r) {}
	WinErrBOOL(bool r) : m_res(r) {}
    WinErrBOOL() {}
	void SetError(DWORD err) {m_dwErr = err;}
	void Log()	
	{
		__SW_LOG_FORMAT__(L"WinErr=%d ", m_dwErr);

		CAutoWinMem lpMsgBuf;
		FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL,
			m_dwErr,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			(LPTSTR)&lpMsgBuf,
			0, NULL);
		TChar* sMessage = (TChar*)lpMsgBuf.get();
		__SW_LOG_FORMAT__(L"%s", sMessage);

	}
	operator bool() 
	{
		if (m_res)
			return false;
		m_dwErr = GetLastError();
		return true; 
	}
	TStatus ToTStatus()	{	return SW_ERR_WINAPI;	}
};

struct WinErrDwordWait
{
	DWORD res;
	DWORD dwErr;
	WinErrDwordWait(DWORD r) : res(r)  {}
	void Log()	{
		CAutoWinMem lpMsgBuf;
		DWORD dwErr = GetLastError();
		FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER |
			FORMAT_MESSAGE_FROM_SYSTEM |
			FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL,
			dwErr,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			(LPTSTR)&lpMsgBuf,
			0, NULL);
		TChar* sMessage = (TChar*)lpMsgBuf.get();
		__SW_LOG_FORMAT__(L"Wait result=%d LastErr=%d %s", res, dwErr, sMessage);
	 }
	operator bool() 
	{ 
		if (res == WAIT_OBJECT_0)
			return false;
		dwErr = GetLastError();
		return true;
	}
	TStatus ToTStatus()	{ return SW_ERR_WAIT_PROCESS; }
};

struct SwErrTStatus
{
	TStatus res;
	SwErrTStatus(TStatus r): res(r) {}
	void Log()	{ __SW_LOG_FORMAT__(L"TStatus=%s(%d)", res >= SW_ARRAY_SIZE(c_StatusNames) ? L"NO_INFO" : c_StatusNames[res], res); }
	operator bool() const { return res!= SW_ERR_SUCCESS; }
	TStatus ToTStatus()	{ return res; }
};



struct WinErrLSTATUS
{
	LSTATUS res;
	WinErrLSTATUS(LSTATUS r) : res(r) {}
	void Log()	{ __SW_LOG_FORMAT__(L"LSTATUS=%d", res); }
	bool IsError() const { return res != ERROR_SUCCESS; }
	operator bool() const { return IsError();  }
	TStatus ToTStatus()	{ return SW_ERR_WINAPI; }
};

struct ErrnoDescr
{
	errno_t res;
	ErrnoDescr(errno_t r) : res(r) {}

	void Log()	
	{
		TChar sBuf[0x100];
		*sBuf = 0;
		_wcserror_s(sBuf, res);
		__SW_LOG_FORMAT__(L"errno_t=%Iu msg=%s", res, sBuf); 
	}
	bool IsError() const { return res != 0; }
	operator bool() const { return IsError(); }
	TStatus ToTStatus()	{ return SW_ERR_ERRNO; }
};

struct WinErrHRESULT
{
	HRESULT res;
	WinErrHRESULT(HRESULT r) : res(r) {}
	void Log()	{ __SW_LOG_FORMAT__(L"HResult=%d(0x%x)", res, res); }
	operator bool() const { return FAILED(res); }
	TStatus ToTStatus()	{ return SW_ERR_HRESULT; }
};




template<class T>
inline void __Log_Err_Common(T err, const char* file, int line, const wchar_t* format = nullptr, ...)
{
    
	if(GetLogLevel() < LOG_LEVEL_ERROR)
		return;

	std::unique_lock<std::mutex> _lock(SwLoggerGlobal().Mtx());
    
	__SW_LOG_TIME();
	err.Log();
	__SW_LOG_FORMAT__(L" %S(%d) ", file, line);
	if (format)
	{
		va_list alist;
		va_start(alist, format);
		__SW_LOG_FORMAT_V__(format, alist);
		va_end(alist);
	}
	SwLoggerGlobal().EndLineFlash();
}

#define __SW_FILENAME__ (strrchr(__FILE__, '\\') != NULL ? strrchr(__FILE__, '\\') + 1 : __FILE__)

#define __LOG_ERR(ERR, ...) __Log_Err_Common(ERR, __SW_FILENAME__, __LINE__, __VA_ARGS__)
#define __RET_ERR(CLASS, X, ...) {CLASS __res = (X); __LOG_ERR(__res, __VA_ARGS__); return __res.ToTStatus(); }


#define  _SW_ERR_RET(ClassName, X, ...) {if (ClassName __res = (X)) { __LOG_ERR(__res, __VA_ARGS__); return __res.ToTStatus(); } }
#define  _SW_ERR_LOG(ClassName, X, ...) {if (ClassName __res = (X))   __LOG_ERR(__res, __VA_ARGS__); }


// API
// -----------------------------------------------------------------------------------------

#define IFNULL(X) if((X) == nullptr)
#define IFNEG(X) if((X) == -1)


#define IFW_RET(X, ...) _SW_ERR_RET(WinErrBOOL, X, __VA_ARGS__)
#define IFW_LOG(X, ...) _SW_ERR_LOG(WinErrBOOL, X, __VA_ARGS__)
#define RETW(...) {WinErrBOOL __res; __res.SetError(GetLastError()); __LOG_ERR(__res, __VA_ARGS__); return __res.ToTStatus(); }

#define IFS_RET(X, ...) _SW_ERR_RET(SwErrTStatus, X, __VA_ARGS__)
#define IFS_LOG(X, ...) _SW_ERR_LOG(SwErrTStatus, X, __VA_ARGS__)
#define RETS(STATUS, ...) __RET_ERR(SwErrTStatus, STATUS, __VA_ARGS__)

#define IF_LSTATUS_RET(X, ...) _SW_ERR_RET(WinErrLSTATUS, X, __VA_ARGS__)
#define IF_LSTATUS_LOG(X, ...) _SW_ERR_LOG(WinErrLSTATUS, X, __VA_ARGS__)

#define IF_ERRNO_RET(X, ...) _SW_ERR_RET(ErrnoDescr, X, __VA_ARGS__)
#define IF_ERRNO_LOG(X, ...) _SW_ERR_LOG(ErrnoDescr, X, __VA_ARGS__)
#define RET_ERRNO(...) __RET_ERR(ErrnoDescr, errno, __VA_ARGS__)

#define IFH_RET(X, ...) _SW_ERR_RET(WinErrHRESULT, X, __VA_ARGS__)
#define IFH_LOG(X, ...) _SW_ERR_LOG(WinErrHRESULT, X, __VA_ARGS__)

#define IF_WAITDWORD_RET(X, ...) _SW_ERR_RET(WinErrDwordWait, X, __VA_ARGS__)
#define IF_WAITDWORD_LOG(X, ...) _SW_ERR_LOG(WinErrDwordWait, X, __VA_ARGS__)
