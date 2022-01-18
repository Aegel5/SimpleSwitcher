//#pragma once
//
//#include "utils/ShareMemory.h"
//
//static const TUInt16 c_curVersionProtocol = 55;
//static const TChar c_sShareMtxName[] = L"Local\\SW_MTX_F62F91AD49184EEA";
//static const TChar c_sMtxSendInput[] = L"Local\\SW_MTX_434ab219ff012eaa";
//static const TChar c_sShareMapName[] = L"Local\\SW_MAP_D7271A8C4FA32C1E";
//
//
//
//
//struct TSharedData
//{
//	TUInt16 ver;
//
//	TUInt64 hwndGui;
//	TUInt64 hwndMonitor32;
//
//	SendData sendData;
//
//	TUInt8 fMonitor32;
//	TUInt8 fMonitor64;
//};
//
//struct SwShared
//{
//	TStatus Init(bool open = false)
//	{
//		if(m_init)
//			RETURN_SUCCESS;
//
//		m_mtxSync = CreateMutex(NULL, false, c_sShareMtxName);
//		IFW_RET(m_mtxSync.IsValid());
//
//		if(open)
//		{
//			bool exist;
//			IFS_RET(m_buf.Init(c_sShareMapName, exist));
//
//			if (!exist)
//				IFS_RET(SW_ERR_NOT_FOUND);
//			if(m_buf.Get().ver != c_curVersionProtocol)
//				IFS_RET(SW_ERR_PROTOCOL_VER, L"Protocol ver %d (our) != %d (exists)", c_curVersionProtocol, m_buf.Get().ver);
//
//		}
//		else
//		{
//			CAutoWinMutexWaiter w(m_mtxSync);
//
//			bool exist;
//			IFS_RET(m_buf.Init(c_sShareMapName, exist));
//			if (exist)
//			{
//				if (m_buf.Get().ver != c_curVersionProtocol)
//				{
//					IFS_RET(SW_ERR_PROTOCOL_VER, L"Protocol ver %d (our) != %d (exists)", c_curVersionProtocol, m_buf.Get().ver);
//				}
//			}
//			else
//			{
//				SwZeroMemory(m_buf.Get());
//				m_buf.Get().ver = c_curVersionProtocol;
//			}
//		}
//
//		m_init = true;
//
//		RETURN_SUCCESS;	
//	}
//
//	static SwShared& Global()
//	{
//		static SwShared sh;
//		return sh;
//	}
//
//	//static Ss
//
//	CShareMemory<TSharedData> m_buf;
//	CAutoHandle m_mtxSync;
//private:
//	bool m_init = false;
//};
//
//inline HANDLE  G_SwSharedMtx()
//{
//	return SwShared::Global().m_mtxSync;
//}
//inline TSharedData&  G_SwSharedBuf()
//{
//	return SwShared::Global().m_buf.Get();
//}