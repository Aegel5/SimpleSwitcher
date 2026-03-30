#include "../Localization/Russian.h"

namespace Localization {

	inline std::vector<std::pair<uintptr_t, const char*>> lookup; // пока ищем прямо по адресу...

	inline void Reinit(UView language) {
		lookup.clear();
		std::span<const std::pair<const char*, const char*>> arr = {};
		if (language == "Russian") {
			arr = _Localization_Russian;
		}
		if (!arr.empty()){
			lookup.reserve(arr.size());
			for (const auto& [key, value] : arr) {
				// Кастуем адрес строки (указатель) в число
				lookup.emplace_back(reinterpret_cast<uintptr_t>(key), value);
			}
			std::sort(lookup.begin(), lookup.end());
		}
		// Ужимаем вектор до фактического размера (size == capacity)
		lookup.shrink_to_fit();
	}

	inline const char* find(UStr s) {
		auto key = reinterpret_cast<uintptr_t>(s);
		auto it = std::lower_bound(lookup.begin(), lookup.end(), key,
			[](const auto& pair, auto val) {
				return pair.first < val;
			});

		if (it != lookup.end() && it->first == key) {
			return it->second;
		}
		return s;
	}
}

inline UStr LOC(UStr s) { return Localization::find(s); }
