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
	static bool hBitmapToMat(HBITMAP hBmp, Mat &img);
	int SavefileFromBitmap(HBITMAP hBitmap, LPCTSTR lpszFileName);
	int SavePNGFromBitmapUsingOpenCV(HBITMAP bitmap);
};

