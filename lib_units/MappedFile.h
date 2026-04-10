#include <windows.h>
#include <string>

class MappedFile {
public:
	explicit MappedFile(const wchar_t* filename) {
		// 1. Open the file for reading
		hFile = CreateFileW(filename, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_DELETE,
			nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
		if (hFile == INVALID_HANDLE_VALUE) return;

		// 2. Get file size
		LARGE_INTEGER li;
		if (GetFileSizeEx(hFile, &li)) {
			file_size = static_cast<size_t>(li.QuadPart);
		}

		// 3. Create mapping object and map the view
		if (file_size > 0) {
			hMap = CreateFileMappingW(hFile, nullptr, PAGE_READONLY, 0, 0, nullptr);
			if (hMap) {
				base_ptr = MapViewOfFile(hMap, FILE_MAP_READ, 0, 0, 0);
			}
		}
	}

	// RAII: cleanup handles and unmap view on destruction
	~MappedFile() {
		if (base_ptr) UnmapViewOfFile(base_ptr);
		if (hMap) CloseHandle(hMap);
		if (hFile != INVALID_HANDLE_VALUE) CloseHandle(hFile);
	}

	// for debug
	size_t get_resident_size() const {
		if (!base_ptr || file_size == 0) return 0;

		SYSTEM_INFO si;
		GetSystemInfo(&si);
		size_t page_size = si.dwPageSize;
		size_t num_pages = (file_size + page_size - 1) / page_size;

		// Allocate info structures for each page
		auto info = std::make_unique<PSAPI_WORKING_SET_EX_INFORMATION[]>(num_pages);

		for (size_t i = 0; i < num_pages; ++i) {
			info[i].VirtualAddress = (void*)((size_t)base_ptr + (i * page_size));
		}

		// Query Windows for page states
		if (!QueryWorkingSetEx(GetCurrentProcess(), info.get(), (DWORD)(num_pages * sizeof(PSAPI_WORKING_SET_EX_INFORMATION)))) {
			return 0;
		}

		size_t resident_pages = 0;
		for (size_t i = 0; i < num_pages; ++i) {
			if (info[i].VirtualAttributes.Valid) { // Page is in physical RAM
				resident_pages++;
			}
		}

		return resident_pages * page_size;
	}

	// Accessors
	const void* data() const { return base_ptr; }
	size_t size() const { return file_size; }
	bool is_valid() const { return base_ptr != nullptr; }

	// Disable copy to prevent double-freeing handles
	MappedFile(const MappedFile&) = delete;
	MappedFile& operator=(const MappedFile&) = delete;

private:
	HANDLE hFile = INVALID_HANDLE_VALUE;
	HANDLE hMap = nullptr;
	void* base_ptr = nullptr;
	size_t file_size = 0;
};
