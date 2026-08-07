	// 	inline TStatus LoadIcoFromFile(const char* file, ImageImpl* image) {

	// 		// возьмем самую большую что есть. todo - брать все размеры.
	// 		CAutoHIcon hIcon = (HICON)LoadImage(NULL, StrUtils::Convert(file).c_str(), IMAGE_ICON, 256, 256, LR_LOADFROMFILE); 
	// 		IFW_RET(hIcon.IsInvalid());

	// 		// Получаем HBITMAP из HICON
	// 		ICONINFO iconInfo;
	// 		IFW_RET(GetIconInfo(hIcon, &iconInfo));
	// 		CAutoHBitmap hBitmap = iconInfo.hbmColor;
	// 		CAutoHBitmap hBitmapMask = iconInfo.hbmMask;

	// 		BITMAP bmp;
	// 		IFW_RET(GetObject(hBitmap, sizeof(BITMAP), &bmp));

	// 		image->width = bmp.bmWidth;
	// 		image->height = bmp.bmHeight;
	// 		image->channels = 4;
	// 		image->data = new BYTE[bmp.bmWidthBytes * bmp.bmHeight];
	// 		image->is_our_memory = true;

	// 		auto outData = image->data;

	// 		// Получаем данные из HBITMAP
	// 		HDC hdc = GetDC(NULL);
	// 		GetDIBits(hdc, hBitmap, 0, bmp.bmHeight, outData, (BITMAPINFO*)&bmp, DIB_RGB_COLORS);
	// 		ReleaseDC(NULL, hdc);

	// 		// Преобразуем данные в формат RGBA
	// 		for (int y = 0; y < bmp.bmHeight; ++y) {
	// 			for (int x = 0; x < bmp.bmWidth; ++x) {
	// 				BYTE* pixel = outData + (y * bmp.bmWidthBytes) + (x * 3); // RGB
	// 				BYTE r = pixel[2];
	// 				BYTE g = pixel[1];
	// 				BYTE b = pixel[0];
	// 				BYTE a = 255; // Устанавливаем альфа-канал в 255 (непрозрачный)

	// 				// Записываем в выходной массив в формате RGBA
	// 				(outData)[(y * bmp.bmWidth + x) * 4 + 0] = r; // R
	// 				(outData)[(y * bmp.bmWidth + x) * 4 + 1] = g; // G
	// 				(outData)[(y * bmp.bmWidth + x) * 4 + 2] = b; // B
	// 				(outData)[(y * bmp.bmWidth + x) * 4 + 3] = a; // A
	// 			}
	// 		}

	// 		RETURN_SUCCESS;
	// 	}