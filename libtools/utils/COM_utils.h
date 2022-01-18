#pragma once
#include "Objbase.h"

#include <comdef.h>
#define _WIN32_DCOM

MAKE_AUTO_CLEANUP(BSTR, ::SysFreeString, NULL, CAuto_Bstr)

namespace COM
{
	inline TStatus COMInitCommonSecurity()
	{
		IFH_RET(CoInitializeSecurity(
			NULL,
			-1,
			NULL,
			NULL,
			RPC_C_AUTHN_LEVEL_PKT_PRIVACY,
			RPC_C_IMP_LEVEL_IMPERSONATE,
			NULL,
			0,
			NULL));

		RETURN_SUCCESS;
	}

	class CAutoCOMInitialize
	{
	public:
		CAutoCOMInitialize()
		{
		}
		TStatus Init()
		{
			if (m_init)
				RETURN_SUCCESS;
			IFH_RET(CoInitializeEx(0, COINIT_MULTITHREADED));
			IFS_LOG(COM::COMInitCommonSecurity());
			m_init = true;
			RETURN_SUCCESS;
		}
		~CAutoCOMInitialize()
		{
			if (m_init)
			{
				CoUninitialize();
			}
		}
	private:
		bool m_init = false;
	};


}



