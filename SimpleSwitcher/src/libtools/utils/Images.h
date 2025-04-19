
#include "stb_image.h"

namespace Images {

	namespace details {
		class ImageImpl {
		public:
			unsigned char* data = 0;
			int width = 0;
			int height = 0;
			int channels = 0;
			bool IsOk() {
				return data != 0;
			}
		public:
			void clear() {
				if (data) {
					stbi_image_free(data);
					data = 0;
				}
			}
			~ImageImpl() {
				clear();
			}
		};

		class IconImageImpl {
		public:
			ImageImpl img;
			HICON hicon = 0;
			bool IsOk() {
				return hicon != 0;
			}
			~IconImageImpl() {
				if (hicon) {
					DestroyIcon(hicon);
					hicon = 0;
				}
			}
		};

		inline HICON CreateIconFromRGBA(unsigned char* pixels, int width, int height) {
			// Pixels are assumed to be in RGBA format (4 bytes per pixel)
			// Windows expects BGRA for bitmaps, so we need to convert

			// Convert RGBA to BGRA
			for (int i = 0; i < width * height; i++) {
				unsigned char r = pixels[i * 4 + 0];
				unsigned char g = pixels[i * 4 + 1];
				unsigned char b = pixels[i * 4 + 2];
				unsigned char a = pixels[i * 4 + 3];

				pixels[i * 4 + 0] = b;
				pixels[i * 4 + 1] = g;
				pixels[i * 4 + 2] = r;
				pixels[i * 4 + 3] = a;
			}

			// Create a DIB section for color bitmap
			BITMAPV5HEADER bi;
			ZeroMemory(&bi, sizeof(bi));
			bi.bV5Size = sizeof(BITMAPV5HEADER);
			bi.bV5Width = width;
			bi.bV5Height = -height; // top-down bitmap
			bi.bV5Planes = 1;
			bi.bV5BitCount = 32;
			bi.bV5Compression = BI_BITFIELDS;
			bi.bV5RedMask = 0x00FF0000;
			bi.bV5GreenMask = 0x0000FF00;
			bi.bV5BlueMask = 0x000000FF;
			bi.bV5AlphaMask = 0xFF000000;

			void* pvBitsColor = nullptr;

			HDC hdcScreen = GetDC(NULL);
			HBITMAP hbmColor = CreateDIBSection(hdcScreen, (BITMAPINFO*)&bi, DIB_RGB_COLORS, &pvBitsColor, NULL, 0);

			if (!hbmColor) {
				ReleaseDC(NULL, hdcScreen);
				return NULL;
			}

			// Copy pixel data into the bitmap
			memcpy(pvBitsColor, pixels, width * height * 4);

			// Create mask bitmap (monochrome) - all zero (no transparency mask)
			HBITMAP hbmMask = CreateBitmap(width, height, 1, 1, NULL);

			ICONINFO ii;
			ZeroMemory(&ii, sizeof(ii));

			ii.fIcon = TRUE;          // TRUE for icon, FALSE for cursor
			ii.xHotspot = 0;          // Hotspot ignored for icons
			ii.yHotspot = 0;

			ii.hbmColor = hbmColor;   // Color bitmap
			ii.hbmMask = hbmMask;     // Mask bitmap

			HICON hIcon = CreateIconIndirect(&ii);

			// Cleanup GDI objects
			DeleteObject(hbmColor);
			DeleteObject(hbmMask);

			ReleaseDC(NULL, hdcScreen);

			return hIcon;
		}
	}

	using ImageIcon = std::shared_ptr<details::IconImageImpl>;
	using Image = std::shared_ptr<details::ImageImpl>;

	inline Image LoadImageFromFile(const char* file) {
		auto image = std::make_shared<details::ImageImpl>();
		image->data = stbi_load(file, &image->width, &image->height, &image->channels, STBI_rgb_alpha);
		return image;
	}

	inline ImageIcon ImageToIconConsume(Image image) {
		auto res = std::make_shared<details::IconImageImpl>();
		res->img = *image;
		image->data = 0; // move ownership
		auto* cur = &res->img;
		if (!cur->IsOk()) return res;
		res->hicon = details::CreateIconFromRGBA(cur->data, cur->width, cur->height);
		cur->clear(); // не требуется
		return res;
	}

	inline void SetBrightness(Image image, float factor) {
		int img_size = image->width * image->height * image->channels;
		for (int i = 0; i < img_size; i++) {
			// Если есть альфа-канал (channels == 4), не трогаем его
			if (image->channels == 4 && (i % 4) == 3) continue;

			int value = static_cast<int>(image->data[i] * factor);
			image->data[i] = static_cast<unsigned char>(std::clamp(value, 0, 255));
		}
	}

	inline void SetToGray(Image image) {
		int img_size = image->width * image->height * image->channels;
		auto img = image->data;
		for (int i = 0; i < img_size; i += image->channels) {
			// Вычисляем яркость по формуле для преобразования в оттенки серого
			unsigned char gray = static_cast<unsigned char>(
				0.299f * img[i] + // Красный
				0.587f * img[i + 1] + // Зеленый
				0.114f * img[i + 2]   // Синий
				);

			// Устанавливаем значения R, G и B в одно и то же значение серого
			img[i] = gray;       // R
			img[i + 1] = gray;   // G
			img[i + 2] = gray;   // B

			// Если есть альфа-канал, оставляем его без изменений
			if (image->channels == 4) {
				img[i + 3] = img[i + 3]; // Альфа-канал остается прежним
			}
		}
	}

}
