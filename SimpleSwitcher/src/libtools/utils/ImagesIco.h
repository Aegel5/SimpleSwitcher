//#include <iostream>
//#include <fstream>
//#include <vector>
//
//#pragma pack(push, 1)
//struct IcoHeader {
//	uint16_t reserved; // всегда 0
//	uint16_t type;     // всегда 1 для ICO
//	uint16_t count;    // количество изображений
//};
//
//struct IcoImageHeader {
//	uint8_t width;     // ширина
//	uint8_t height;    // высота
//	uint8_t colors;    // количество цветов
//	uint8_t reserved;  // резерв
//	uint16_t planes;   // количество цветовых плоскостей
//	uint16_t bitCount; // количество бит на пиксель
//	uint32_t size;     // размер изображения в байтах
//	uint32_t offset;   // смещение изображения в файле
//};
//#pragma pack(pop)
//
//void printIconSizes(const std::string& filename) {
//	std::ifstream file(filename, std::ios::binary);
//
//	if (!file) {
//		std::cerr << "Не удалось открыть файл." << std::endl;
//		return;
//	}
//
//	IcoHeader icoHeader;
//	file.read(reinterpret_cast<char*>(&icoHeader), sizeof(icoHeader));
//
//	if (icoHeader.type != 1) {
//		std::cerr << "Неправильный формат файла." << std::endl;
//		return;
//	}
//
//	std::vector<IcoImageHeader> imageHeaders(icoHeader.count);
//
//	for (size_t i = 0; i < icoHeader.count; ++i) {
//		file.read(reinterpret_cast<char*>(&imageHeaders[i]), sizeof(IcoImageHeader));
//
//		// Выводим размеры иконок
//		int width = imageHeaders[i].width == 0 ? 256 : imageHeaders[i].width;
//		int height = imageHeaders[i].height == 0 ? 256 : imageHeaders[i].height;
//
//		std::cout << "Иконка " << i + 1 << ": " << width << "x" << height << std::endl;
//	}
//}
//
//int main() {
//	printIconSizes("path_to_your_icon_file.ico");
//	return 0;
//}
