#include "pch.h"
#include "Helper.h"
#include <opencv2/core.hpp>

using namespace cv;
using namespace std;

void Density(Mat& cImage, Mat& cDensity, int uKernel)
{
	Mat cIntegral, cPadding;

	cDensity = Mat::zeros(cImage.rows, cImage.cols, CV_32SC1);

	copyMakeBorder(cImage, cPadding, uKernel, uKernel, uKernel, uKernel, BORDER_CONSTANT, Scalar(0));

	integral(cPadding, cIntegral, CV_32S);

	int* lpDensity = (int*)cDensity.data;

	int* lpIntegral = (int*)cIntegral.data;

	uint64 x1 = 0, x2 = 0, y1 = 0, y2 = 0;

	for (uint64 y = 0; y < cImage.rows; ++y)
	{
		for (uint64 x = 0; x < cImage.cols; ++x)
		{
			x1 = x + 1; x2 = x + 2ull * uKernel + 1;

			y1 = y + 1; y2 = y + 2ull * uKernel + 1;

			lpDensity[y * cDensity.cols + x] =
				lpIntegral[y1 * cIntegral.cols + x1] +
				lpIntegral[y2 * cIntegral.cols + x2] -
				lpIntegral[y1 * cIntegral.cols + x2] -
				lpIntegral[y2 * cIntegral.cols + x1];
		}
	}
}

void PixelDensity(Mat& cImage, Mat& cDensity, int uKernel)
{
	Mat cWinDensity;

	cDensity = Mat::zeros(cImage.rows, cImage.cols, CV_8UC1);

	Density(cImage, cWinDensity, uKernel);

	double dMin = 0.0, dMax = 0.0;

	minMaxLoc(cWinDensity, &dMin, &dMax);

	int nTh = round(dMax * 0.9);

	uchar* lpDensity = cDensity.data;

	int* lpWinDensity = (int*)cWinDensity.data;

	for (uint64 y = 0; y < cImage.rows; ++y)
	{
		for (uint64 x = 0; x < cImage.cols; ++x)
		{
			uint64 idx = y * cImage.cols + x;

			lpDensity[idx] = (nTh < lpWinDensity[idx] ? 0xff : 0);
		}
	}
}

void MaxConnected(Mat& cConnected, Rect& rtMax)
{
	rtMax = Rect(0, 0, 1, 1); // 1*1 防止触发 CV_DbgAssert

	auto lpConnected = (int*)cConnected.data;

	for (int y = 1; y < cConnected.rows; ++y)
	{
		auto& w = lpConnected[y * 5 + CC_STAT_WIDTH];

		auto& h = lpConnected[y * 5 + CC_STAT_HEIGHT];

		if (w * h <= rtMax.area()) continue;

		rtMax.x = lpConnected[y * 5 + CC_STAT_LEFT];

		rtMax.y = lpConnected[y * 5 + CC_STAT_TOP];

		rtMax.width = w;

		rtMax.height = h;
	}
}

// 当参照物为二维码时
// 定位参照物：
//	1. 二值化
//	2. 转为密度图,将低密度部分进行丢弃
//	3. 腐蚀
//	4. 膨胀
//	5. 求取联通区域
//	6. 选取最大联通区域
// 二维码位置 = 最大联通区域 * 权值
void QEncodePos(std::array<int, 5> vParam, Mat& cImage, Mat& cQEncode, Rect& rtQencode)
{
	Mat cCache, cDensity; 

	GaussianBlur(cImage, cCache, cv::Size(vParam[0], vParam[0]), 0, 0);

	threshold(cCache, cCache, vParam[1], 255, 1);

	PixelDensity(cCache, cDensity, vParam[2]);

	Mat kErode = Mat::ones(vParam[3], vParam[3], CV_8UC1);

	Mat kDilate = Mat::ones(vParam[4], vParam[4], CV_8UC1);

	erode(cDensity, cCache, kErode);

	dilate(cCache, cCache, kDilate);

	Mat labels = Mat::zeros(cCache.rows, cCache.cols, CV_32S), stats, centroids;

	connectedComponentsWithStats(cCache, labels, stats, centroids);

	MaxConnected(stats, rtQencode);

	cQEncode = cImage(rtQencode).clone();
}