#include "pch.h"
#include "Helper.h"

using namespace cv;
using namespace std;

void Binarization(Mat& cImage, Mat& cBin)
{
	Mat cHistogram;

	int nThreshold = 0, nThresholdType = 0;

	Histogram(cImage, cHistogram);

	BimodalThreshold(cHistogram, nThreshold, nThresholdType);

	GaussianBlur(cImage, cImage, cv::Size(7, 7), 1.5, 0);

	threshold(cImage, cBin, nThreshold, 255, nThresholdType);
}