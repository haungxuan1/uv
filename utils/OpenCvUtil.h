#pragma once
#include <afxwin.h>
#include <iostream>

using namespace std;

class OpenCvUtil
{
public:
	static void rotate(const Mat& tSrcMat, Mat& tDstMat, double dAngle, unsigned char ucFillVal);
	static Mat resize_ext(const Mat &tImg, Size tSize, double dX=0, double dY=0, int iInterpolation=INTER_AREA);

private:
	OpenCvUtil();
	~OpenCvUtil();
};
