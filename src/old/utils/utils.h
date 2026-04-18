//	inline bool ProcSingleton(const TChar* mtxName)
//	{
//#ifdef _WIN32
//		HANDLE mtxGui = CreateMutex(NULL, FALSE, mtxName);
//		if (GetLastError() == ERROR_ALREADY_EXISTS)
//		{
//			return true;
//		}
//		else if (GetLastError() != ERROR_SUCCESS)
//		{
//			IFW_LOG(FALSE);
//		}
//
//		return false;
//#else
//		// Логика для Linux через абстрактные сокеты (аналог именованного мьютекса)
//		int sock = socket(AF_UNIX, SOCK_STREAM, 0);
//		if (sock < 0) return false;
//
//		struct sockaddr_un addr;
//		memset(&addr, 0, sizeof(addr));
//		addr.sun_family = AF_UNIX;
//
//		// Первый байт 0 делает путь "абстрактным" — сокет исчезнет сам при закрытии процесса
//		strncpy(addr.sun_path + 1, mtxName, sizeof(addr.sun_path) - 2);
//
//		if (bind(sock, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
//			if (errno == EADDRINUSE) {
//				close(sock);
//				return true; // Уже запущено
//			}
//			close(sock);
//			return false;
//		}
//		// Сокет намеренно не закрываем, чтобы держать "мьютекс"
//		return false;
//#endif
//	}
