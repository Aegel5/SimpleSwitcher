#pragma once

class EnableHodler {

	class MtxHolder {

		std::atomic<HANDLE> mtxGui = NULL;

	public:
		bool is_taked() { // only this thread safe!!
			return mtxGui != NULL;
		}
		bool take() {
			if (is_taked())
				return true;
			mtxGui = CreateMutex(NULL, FALSE, L"E629E6ED27B64671");
			if (GetLastError() != ERROR_SUCCESS) {
				if (mtxGui != NULL && mtxGui != INVALID_HANDLE_VALUE) {
					CloseHandle(mtxGui);
					mtxGui = NULL;
				}
			}
			return is_taked();
		}
		void clear() {
			if (is_taked()) {
				CloseHandle(mtxGui);
				mtxGui = NULL;
			}
		}
		~MtxHolder() {
			clear();
		}
	} mtx;
	std::mutex mtx2;
public:
	bool IsEnabled() {
		return mtx.is_taked();
	}
	bool TryToggle() { return TryEnable(!IsEnabled()); }
	bool TryEnable(bool val = true) {
		std::unique_lock<std::mutex> _lock(mtx2);
		if (!val) {
			mtx.clear();
			return true;
		}
		return mtx.take();
	}
};
