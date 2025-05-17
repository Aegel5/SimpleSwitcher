
#include "stb_image.h"
#include <d3d11.h>

extern ID3D11Device* g_pd3dDevice;

namespace Images {

	namespace details {
		class ImageImpl { // todo add unique_ptr
		public:
			unsigned char* data = 0;
			int width = 0;
			int height = 0;
			int channels = 0;
			bool is_our_memory = false;
			bool IsOk() {
				return data != 0;
			}
		public:
			void clear() {
				if (data) {
					if(is_our_memory) delete[] data;
					else stbi_image_free(data);
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

		class TextureImpl {
		public:
			ImageImpl img;
			ID3D11ShaderResourceView* pTexture = 0;
			bool IsOk() {
				return pTexture != 0;
			}
			void clear() {
				if (pTexture) {
					pTexture->Release();
					pTexture = 0;
				}
			}
			~TextureImpl() {
				clear();
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
		inline ID3D11ShaderResourceView* LoadShader(unsigned char* image_data, int image_width, int image_height) {

			// Create texture
			D3D11_TEXTURE2D_DESC desc;
			ZeroMemory(&desc, sizeof(desc));
			desc.Width = image_width;
			desc.Height = image_height;
			desc.MipLevels = 1;
			desc.ArraySize = 1;
			desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			desc.SampleDesc.Count = 1;
			desc.Usage = D3D11_USAGE_DEFAULT;
			desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
			desc.CPUAccessFlags = 0;

			ID3D11Texture2D* pTexture = NULL;
			D3D11_SUBRESOURCE_DATA subResource;
			subResource.pSysMem = image_data;
			subResource.SysMemPitch = desc.Width * 4;
			subResource.SysMemSlicePitch = 0;
			g_pd3dDevice->CreateTexture2D(&desc, &subResource, &pTexture);

			// Create texture view
			D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
			ZeroMemory(&srvDesc, sizeof(srvDesc));
			srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			srvDesc.Texture2D.MipLevels = desc.MipLevels;
			srvDesc.Texture2D.MostDetailedMip = 0;
			ID3D11ShaderResourceView* res = 0;
			g_pd3dDevice->CreateShaderResourceView(pTexture, &srvDesc, &res);
			pTexture->Release();

			return res;
		}

		inline TStatus LoadIcoFromFile(const char* file, ImageImpl* image) {

			// возьмем самую большую что есть. todo - брать все размеры.
			CAutoHIcon hIcon = (HICON)LoadImage(NULL, StrUtils::Convert(file).c_str(), IMAGE_ICON, 256, 256, LR_LOADFROMFILE); 
			IFW_RET(hIcon.IsInvalid());

			// Получаем HBITMAP из HICON
			ICONINFO iconInfo;
			IFW_RET(GetIconInfo(hIcon, &iconInfo));
			CAutoHBitmap hBitmap = iconInfo.hbmColor;
			CAutoHBitmap hBitmapMask = iconInfo.hbmMask;

			BITMAP bmp;
			IFW_RET(GetObject(hBitmap, sizeof(BITMAP), &bmp));

			image->width = bmp.bmWidth;
			image->height = bmp.bmHeight;
			image->channels = 4;
			image->data = new BYTE[bmp.bmWidthBytes * bmp.bmHeight];
			image->is_our_memory = true;

			auto outData = image->data;

			// Получаем данные из HBITMAP
			HDC hdc = GetDC(NULL);
			GetDIBits(hdc, hBitmap, 0, bmp.bmHeight, outData, (BITMAPINFO*)&bmp, DIB_RGB_COLORS);
			ReleaseDC(NULL, hdc);

			// Преобразуем данные в формат RGBA
			for (int y = 0; y < bmp.bmHeight; ++y) {
				for (int x = 0; x < bmp.bmWidth; ++x) {
					BYTE* pixel = outData + (y * bmp.bmWidthBytes) + (x * 3); // RGB
					BYTE r = pixel[2];
					BYTE g = pixel[1];
					BYTE b = pixel[0];
					BYTE a = 255; // Устанавливаем альфа-канал в 255 (непрозрачный)

					// Записываем в выходной массив в формате RGBA
					(outData)[(y * bmp.bmWidth + x) * 4 + 0] = r; // R
					(outData)[(y * bmp.bmWidth + x) * 4 + 1] = g; // G
					(outData)[(y * bmp.bmWidth + x) * 4 + 2] = b; // B
					(outData)[(y * bmp.bmWidth + x) * 4 + 3] = a; // A
				}
			}

			RETURN_SUCCESS;
		}

	}

	using ImageIcon = std::shared_ptr<details::IconImageImpl>;
	using Image = std::shared_ptr<details::ImageImpl>;
	using ShaderResource = std::shared_ptr<details::TextureImpl>;

	inline Image LoadImageFromFile(const char* file) {
		Image image = MAKE_SHARED(image);
		image->data = stbi_load(file, &image->width, &image->height, &image->channels, STBI_rgb_alpha);
		return image;
	}


	inline Image ResizeBicubic(Image img, int newWidth, int newHeight) {
		int channels = img->channels;
		Image res = MAKE_SHARED(res);
		return res;
	}

	inline ImageIcon ImageToIconConsume(Image image) {
		ImageIcon res = MAKE_SHARED(res);
		res->img = *image;
		image->data = 0; // move ownership
		auto* cur = &res->img;
		if (!cur->IsOk()) return res;
		res->hicon = details::CreateIconFromRGBA(cur->data, cur->width, cur->height);
		cur->clear(); // не требуется
		return res;
	}

	inline ShaderResource ImageToShaderConsume(Image image) {
		ShaderResource res = MAKE_SHARED(res);
		res->img = *image;
		image->data = 0; // move ownership
		auto* cur = &res->img;
		if (!cur->IsOk()) return res;
		res->pTexture = details::LoadShader(cur->data, cur->width, cur->height);
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

	inline void SetAlphaFactor(Image image, float alpha_factor) {
		if (!image->IsOk()) return;
		int pixel_count = image->width * image->height;
		for (int i = 0; i < pixel_count; i++) {
			int idx = i * 4;
			// img[idx + 3] - альфа-канал
			//image->data[idx + 3] = 100;// (unsigned char)(std::clamp((int)(255 * alpha_factor), 0, 255));
			auto img = image->data;
			img[idx + 3] = (unsigned char)(img[idx + 3] * alpha_factor);
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
