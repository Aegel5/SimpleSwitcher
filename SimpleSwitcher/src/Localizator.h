
class Localizator {

	struct MOFile {

		bool isLittleEndian = true;
		//std::map<std::string, std::string, std::less<>> translations;
		std::map<UStr, UStr, decltype([](const auto& s1, const auto& s2) {return std::strcmp(s1, s2) < 0; }) > translations;

		uint32_t readUInt32(const uint8_t*& ptr) {
			uint32_t value = 0;
			if (isLittleEndian) {
				value = (uint32_t(ptr[0])) |
					(uint32_t(ptr[1]) << 8) |
					(uint32_t(ptr[2]) << 16) |
					(uint32_t(ptr[3]) << 24);
			}
			else {
				value = (uint32_t(ptr[3])) |
					(uint32_t(ptr[2]) << 8) |
					(uint32_t(ptr[1]) << 16) |
					(uint32_t(ptr[0]) << 24);
			}
			ptr += 4; // Увеличиваем указатель на 4 байта
			return value;
		}

		bool load(const uint8_t* data, size_t size) {
			if (size <= 24) return false;
			const uint8_t* ptr = data;

			// Читаем magic number
			uint32_t magic = readUInt32(ptr);

			// Проверяем порядок байт
			if (magic == 0x950412de) {
				isLittleEndian = true;
			}
			else if (magic == 0xde120495) {
				isLittleEndian = false;
			}
			else {
				std::cerr << "Invalid magic number: " << std::hex << magic << "\n";
				return false;
			}

			// Читаем версию
			uint32_t version = readUInt32(ptr);
			if (version != 0) {
				std::cerr << "Unsupported version: " << version << "\n";
				return false;
			}

			// Читаем количество строк и смещения таблиц
			uint32_t numStrings = readUInt32(ptr);
			uint32_t origTableOffset = readUInt32(ptr);
			uint32_t transTableOffset = readUInt32(ptr);

			// Пропускаем hash table offset и size
			auto hash_sz = readUInt32(ptr); // hash size
			readUInt32(ptr); // hash offset

			// Читаем таблицу оригинальных строк
			struct Entry { uint32_t length; uint32_t offset; };

			ptr = data + origTableOffset; // Переходим к таблице оригинальных строк

			std::vector<Entry> origEntries(numStrings);

			for (uint32_t i = 0; i < numStrings; ++i) {
				origEntries[i].length = readUInt32(ptr);
				origEntries[i].offset = readUInt32(ptr);
			}

			// Читаем таблицу переводов
			ptr = data + transTableOffset; // Переходим к таблице переводов

			std::vector<Entry> transEntries(numStrings);

			for (uint32_t i = 0; i < numStrings; ++i) {
				transEntries[i].length = readUInt32(ptr);
				transEntries[i].offset = readUInt32(ptr);
			}


			for (uint32_t i = 0; i < numStrings; ++i) {
				// Читаем оригинальную строку
				auto orig = (char*)data + origEntries[i].offset;
				auto trans = (char*)data + transEntries[i].offset;

				if (*orig != 0 && *trans != 0) {
					//string trans_with_id = std::format("{}###{}", trans, orig);
					//translations.try_emplace((string)orig, std::move(trans_with_id));
					//translations.try_emplace((string)orig, trans);
					translations.try_emplace(orig, trans);
				}
			}
			return true;
		}
	} mo;
public:
	UStr Translate(UStr s) {
		auto it = mo.translations.find(s);
		if (it == mo.translations.end()) return s;
		return it->second;
	}
	void Reinit(UStr lang) {
		mo.translations.clear();
		string key = std::format("lang_{}", lang);
		auto res = WinUtils::GetResource(StrUtils::Convert(key).c_str());
		if (res.empty()) return;
		mo.load((uint8_t*)res.data(), res.size());

	}
};

namespace loc_details {
	inline Localizator g_loc;
	inline bool do_reinit = false;
}

inline UStr LOC(UStr s) { return loc_details::g_loc.Translate(s); }
