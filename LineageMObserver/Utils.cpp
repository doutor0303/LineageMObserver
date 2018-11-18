#include "stdafx.h"
#include "Utils.h"

Utils::Utils()
{
}


Utils::~Utils()
{
}


bool Utils::executeShell(const wchar_t *file, const wchar_t *parm)
{
	// TODO: 여기에 구현 코드 추가.
	SHELLEXECUTEINFO execinfo;
	DWORD ExitCode;

	// 실행을 위해 구조체 세트
	ZeroMemory(&execinfo, sizeof(execinfo));
	execinfo.cbSize = sizeof(execinfo);
	execinfo.lpVerb = _T("open");
	execinfo.lpFile = file;
	execinfo.lpParameters = parm;
	execinfo.fMask = SEE_MASK_FLAG_NO_UI | SEE_MASK_NOCLOSEPROCESS;
	execinfo.nShow = SW_HIDE;

	if (ShellExecuteEx(&execinfo) == TRUE) {
		do {
			GetExitCodeProcess(execinfo.hProcess, &ExitCode);
			Sleep(100);
		} while (ExitCode == STILL_ACTIVE);

		return true;
	}
	return false;
}


Mat Utils::hBitmapToMat(HBITMAP hBmp)
{
	// TODO: 여기에 구현 코드 추가.
	BITMAP bmp;

	int error = GetObject(hBmp, sizeof(BITMAP), (LPSTR)&bmp);
	int nChannels = bmp.bmBitsPixel == 1 ? 1 : bmp.bmBitsPixel / 8;
	int depth = bmp.bmBitsPixel == 1 ? IPL_DEPTH_1U : IPL_DEPTH_8U;

	BYTE *pBuffer = new BYTE[bmp.bmHeight*bmp.bmWidth*nChannels];
	GetBitmapBits(hBmp, bmp.bmHeight*bmp.bmWidth*nChannels, pBuffer);
	
	Mat Channel4Mat(bmp.bmHeight, bmp.bmWidth, CV_8UC4, pBuffer);
	delete pBuffer;

	Mat Channel3Mat(bmp.bmHeight, bmp.bmWidth, CV_8UC3);
	cvtColor(Channel4Mat, Channel3Mat, CV_BGRA2BGR);
	
//	imshow("name", Channel4Mat);

	return Channel3Mat;
}


int Utils::SaveFromHBitmapToBmp(HBITMAP hBitmap, LPCTSTR lpszFileName)
{
	// TODO: 여기에 구현 코드 추가.
	HDC hDC;
	int iBits;
	WORD wBitCount;
	DWORD dwPaletteSize = 0, dwBmBitsSize = 0, dwDIBSize = 0, dwWritten = 0;
	BITMAP Bitmap;
	BITMAPFILEHEADER bmfHdr;
	BITMAPINFOHEADER bi;
	LPBITMAPINFOHEADER lpbi;
	HANDLE fh, hDib, hPal, hOldPal = NULL;

	hDC = ::CreateDC(_T("DISPLAY"), NULL, NULL, NULL);
	iBits = ::GetDeviceCaps(hDC, BITSPIXEL) * ::GetDeviceCaps(hDC, PLANES);
	::DeleteDC(hDC);
	if (iBits <= 1)
		wBitCount = 1;
	else if (iBits <= 4)
		wBitCount = 4;
	else if (iBits <= 8)
		wBitCount = 8;
	else
		wBitCount = 24;
	GetObject(hBitmap, sizeof(Bitmap), (LPSTR)&Bitmap);
	bi.biSize = sizeof(BITMAPINFOHEADER);
	bi.biWidth = Bitmap.bmWidth;
	bi.biHeight = Bitmap.bmHeight;
	bi.biPlanes = 1;
	bi.biBitCount = wBitCount;
	bi.biCompression = BI_RGB;
	bi.biSizeImage = 0;
	bi.biXPelsPerMeter = 0;
	bi.biYPelsPerMeter = 0;
	bi.biClrImportant = 0;
	bi.biClrUsed = 0;
	dwBmBitsSize = ((Bitmap.bmWidth * wBitCount + 31) / 32) * 4 * Bitmap.bmHeight;

	hDib = GlobalAlloc(GHND, dwBmBitsSize + dwPaletteSize + sizeof(BITMAPINFOHEADER));
	lpbi = (LPBITMAPINFOHEADER)GlobalLock(hDib);
	*lpbi = bi;

	hPal = GetStockObject(DEFAULT_PALETTE);
	if (hPal)
	{
		hDC = ::GetDC(NULL);
		hOldPal = ::SelectPalette(hDC, (HPALETTE)hPal, FALSE);
		::RealizePalette(hDC);
	}


	GetDIBits(hDC, hBitmap, 0, (UINT)Bitmap.bmHeight, (LPSTR)lpbi + sizeof(BITMAPINFOHEADER)
		+ dwPaletteSize, (BITMAPINFO *)lpbi, DIB_RGB_COLORS);

	if (hOldPal)
	{
		::SelectPalette(hDC, (HPALETTE)hOldPal, TRUE);
		::RealizePalette(hDC);
		::ReleaseDC(NULL, hDC);
	}

	fh = CreateFile(lpszFileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL);

	if (fh == INVALID_HANDLE_VALUE)
		return FALSE;

	bmfHdr.bfType = 0x4D42; // "BM"
	dwDIBSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + dwPaletteSize + dwBmBitsSize;
	bmfHdr.bfSize = dwDIBSize;
	bmfHdr.bfReserved1 = 0;
	bmfHdr.bfReserved2 = 0;
	bmfHdr.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER) + (DWORD)sizeof(BITMAPINFOHEADER) + dwPaletteSize;

	WriteFile(fh, (LPSTR)&bmfHdr, sizeof(BITMAPFILEHEADER), &dwWritten, NULL);

	WriteFile(fh, (LPSTR)lpbi, dwDIBSize, &dwWritten, NULL);
	GlobalUnlock(hDib);
	GlobalFree(hDib);
	CloseHandle(fh);

	return 0;
}

int Utils::SaveFromHBitmapToJpg(HBITMAP hBitmap, LPCTSTR lpszFileName)
{
	// TODO: 여기에 구현 코드 추가.
	CImage img;
	
	img.Attach(hBitmap);
	img.Save(lpszFileName);
	img.Detach();
	img.Destroy();

	return 0;
}

int Utils::SaveFromHBitmapToPng(HBITMAP hBitmap, LPCTSTR lpszFileName)
{
	// TODO: 여기에 구현 코드 추가.
	CImage img;

	img.Attach(hBitmap);
	img.Save(lpszFileName);
	img.Detach();
	img.Destroy();

	return 0;
}

int Utils::SavePNGFromBitmapUsingOpenCV(HBITMAP bitmap)
{
	// TODO: 여기에 구현 코드 추가.
	return 0;
}

BITMAPINFOHEADER Utils::convertFromHBitmapToBi(HBITMAP hBitmap)
{
	HDC hDC;
	BITMAP Bitmap;
	BITMAPINFOHEADER bi;

	GetObject(hBitmap, sizeof(Bitmap), (LPSTR)&Bitmap);
	bi.biSize = sizeof(BITMAPINFOHEADER);
	bi.biWidth = Bitmap.bmWidth;
	bi.biHeight = Bitmap.bmHeight;
	bi.biPlanes = 1;
	bi.biBitCount = 24;
	bi.biCompression = BI_RGB;

	// TODO: 여기에 구현 코드 추가.
	return bi;
}
