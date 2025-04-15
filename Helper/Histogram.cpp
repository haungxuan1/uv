#include "pch.h"
#include "Helper.h"

using namespace cv;
using namespace std;

void Histogram(Mat& cGray, Mat& cHistogram)
{
	cHistogram = Mat::zeros(1, 256, CV_32SC1);

	for (uint64 y = 0; y < cGray.rows; ++y)
	{
		for (uint64 x = 0; x < cGray.cols; ++x)
		{
			uint64 idx = y * cGray.cols + x;

			auto& iGray = cGray.data[idx];

			auto lpData = (int*)cHistogram.data;

			++lpData[iGray];
		}
	}
}

void HistogramView(Mat& cHistogram, cv::Mat& cImHistogtam, int nThreshold)
{
	cImHistogtam = cv::Mat::zeros(100, 256, CV_8UC3);

	Mat cHisView = Mat::zeros(1, 256, CV_32FC1);

	auto iGray = (int*)cHistogram.data;

	auto iView = (float*)cHisView.data;

	int nMax = (*max_element(iGray, iGray + 256));

	int nMin = (*min_element(iGray, iGray + 256));

	for (int i = 0; i < 256; ++i)
	{
		iView[i] = (float)(iGray[i] - nMin) / (nMax - nMin);
	}

	for (int i = 1; i < 256; ++i)
	{
		cv::line(cImHistogtam,
			cv::Point(i - 1, (1.0 - iView[i - 1]) * 100.0),
			cv::Point(i, (1.0 - iView[i - 1]) * 100.0),
			cv::Scalar(0xff, 0xff, 0xff));
	}
	cv::line(cImHistogtam,
		cv::Point(nThreshold, 0),
		cv::Point(nThreshold, 100),
		cv::Scalar(0, 0xff, 0));
}