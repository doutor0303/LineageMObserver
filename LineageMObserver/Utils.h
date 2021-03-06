#pragma once

#include <opencv2\core\core.hpp>
#include <opencv2\opencv.hpp>
#include <opencv2\highgui\highgui.hpp>
#include <opencv2\imgproc\imgproc.hpp>

using namespace cv;

class Utils
{
public:
	Utils();
	~Utils();
	static bool executeShell(const wchar_t *file, const wchar_t *parm);
	static Mat hBitmapToMat(HBITMAP hBmp);
	static int SaveFromHBitmapToBmp(HBITMAP hBitmap, LPCTSTR lpszFileName);
	static int SavePNGFromBitmapUsingOpenCV(HBITMAP bitmap);
	static int SaveFromHBitmapToJpg(HBITMAP hBitmap, LPCTSTR lpszFileName);
	static int SaveFromHBitmapToPng(HBITMAP hBitmap, LPCTSTR lpszFileName);
	static BITMAPINFOHEADER convertFromHBitmapToBi(HBITMAP hBitmap);
	static HBITMAP Mat2HBITMAP(Mat mat);
};

