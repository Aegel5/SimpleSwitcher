#pragma once
#include "Wincodec.h"

namespace gui_tools
{

	// Creates a stream object initialized with the data from an executable resource.
	inline IStream * CreateStreamOnResource(LPCTSTR lpName, LPCTSTR lpType)
	{
		// initialize return value
		IStream * ipStream = NULL;

		// find the resource
		HRSRC hrsrc = FindResource(NULL, lpName, lpType);
		if (hrsrc == NULL)
			goto Return;

		// load the resource
		DWORD dwResourceSize = SizeofResource(NULL, hrsrc);
		HGLOBAL hglbImage = LoadResource(NULL, hrsrc);
		if (hglbImage == NULL)
			goto Return;

		// lock the resource, getting a pointer to its data
		LPVOID pvSourceResourceData = LockResource(hglbImage);
		if (pvSourceResourceData == NULL)
			goto Return;

		// allocate memory to hold the resource data
		HGLOBAL hgblResourceData = GlobalAlloc(GMEM_MOVEABLE, dwResourceSize);
		if (hgblResourceData == NULL)
			goto Return;

		// get a pointer to the allocated memory
		LPVOID pvResourceData = GlobalLock(hgblResourceData);
		if (pvResourceData == NULL)
			goto FreeData;

		// copy the data from the resource to the new memory block
		CopyMemory(pvResourceData, pvSourceResourceData, dwResourceSize);
		GlobalUnlock(hgblResourceData);

		// create a stream on the HGLOBAL containing the data
		if (SUCCEEDED(CreateStreamOnHGlobal(hgblResourceData, TRUE, &ipStream)))
			goto Return;

	FreeData:
		// couldn't create stream; free the memory
		GlobalFree(hgblResourceData);

	Return:
		// no need to unlock or free the resource
		return ipStream;
	}

	// Loads a PNG image from the specified stream (using Windows Imaging Component).
	inline IWICBitmapSource * LoadBitmapFromStream(IStream * ipImageStream)
	{
		// initialize return value
		IWICBitmapSource * ipBitmap = NULL;

		// load WIC's PNG decoder
		IWICBitmapDecoder * ipDecoder = NULL;
		if (FAILED(CoCreateInstance(CLSID_WICPngDecoder, NULL, CLSCTX_INPROC_SERVER, __uuidof(ipDecoder), reinterpret_cast<void**>(&ipDecoder))))
			goto Return;

		// load the PNG
		if (FAILED(ipDecoder->Initialize(ipImageStream, WICDecodeMetadataCacheOnLoad)))
			goto ReleaseDecoder;

		// check for the presence of the first frame in the bitmap
		UINT nFrameCount = 0;
		if (FAILED(ipDecoder->GetFrameCount(&nFrameCount)) || nFrameCount != 1)
			goto ReleaseDecoder;

		// load the first frame (i.e., the image)
		IWICBitmapFrameDecode * ipFrame = NULL;
		if (FAILED(ipDecoder->GetFrame(0, &ipFrame)))
			goto ReleaseDecoder;

		// convert the image to 32bpp BGRA format with pre-multiplied alpha
		//   (it may not be stored in that format natively in the PNG resource,
		//   but we need this format to create the DIB to use on-screen)
		WICConvertBitmapSource(GUID_WICPixelFormat32bppPBGRA, ipFrame, &ipBitmap);
		ipFrame->Release();

	ReleaseDecoder:
		ipDecoder->Release();
	Return:
		return ipBitmap;
	}

	inline HBITMAP CreateHBITMAP(IWICBitmapSource * ipBitmap)
	{
		// initialize return value
		HBITMAP hbmp = NULL;

		// get image attributes and check for valid image
		UINT width = 0;
		UINT height = 0;
		if (FAILED(ipBitmap->GetSize(&width, &height)) || width == 0 || height == 0)
			goto Return;

		// prepare structure giving bitmap information (negative height indicates a top-down DIB)
		BITMAPINFO bminfo;
		ZeroMemory(&bminfo, sizeof(bminfo));
		bminfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		bminfo.bmiHeader.biWidth = width;
		bminfo.bmiHeader.biHeight = -((LONG)height);
		bminfo.bmiHeader.biPlanes = 1;
		bminfo.bmiHeader.biBitCount = 32;
		bminfo.bmiHeader.biCompression = BI_RGB;

		// create a DIB section that can hold the image
		void * pvImageBits = NULL;
		HDC hdcScreen = GetDC(NULL);
		hbmp = CreateDIBSection(hdcScreen, &bminfo, DIB_RGB_COLORS, &pvImageBits, NULL, 0);
		ReleaseDC(NULL, hdcScreen);
		if (hbmp == NULL)
			goto Return;

		// extract the image into the HBITMAP
		const UINT cbStride = width * 4;
		const UINT cbImage = cbStride * height;
		if (FAILED(ipBitmap->CopyPixels(NULL, cbStride, cbImage, static_cast<BYTE *>(pvImageBits))))
		{
			// couldn't extract image; delete HBITMAP
			DeleteObject(hbmp);
			hbmp = NULL;
		}

	Return:
		return hbmp;
	}

	inline HBITMAP LoadSplashImage(LPCTSTR lpName, LPCTSTR lpType)
	{
		HBITMAP hbmpSplash = NULL;

		// load the PNG image data into a stream
		IStream * ipImageStream = CreateStreamOnResource(lpName, lpType);
		if (ipImageStream == NULL)
			goto Return;

		// load the bitmap with WIC
		IWICBitmapSource * ipBitmap = LoadBitmapFromStream(ipImageStream);
		if (ipBitmap == NULL)
			goto ReleaseStream;

		// create a HBITMAP containing the image
		hbmpSplash = CreateHBITMAP(ipBitmap);
		ipBitmap->Release();

	ReleaseStream:
		ipImageStream->Release();
	Return:
		return hbmpSplash;
	}
}