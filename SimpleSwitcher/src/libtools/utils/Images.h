
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

	}

}
