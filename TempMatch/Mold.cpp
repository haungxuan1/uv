#include "pch.h"
#include "Mold.h"
#include "Helper.h"

CMold::CMold()
{

}

CMold::~CMold()
{

}

void CMold::Feature(cv::Mat& cView, cv::Mat& cTemp)
{
	Mat cHist;

	Rect rtFeature;

	vector<Rect> vBounding;

	int nThreshold = 0, nThresholdType = 0;

	Histogram(cView, cHist);

	BimodalThreshold(cHist, nThreshold, nThresholdType);

//	BoundingRect(cView, nThreshold, nThresholdType, vBounding);

	SelectRect(vBounding, rtFeature);

	RemakeFeature(cView, rtFeature);

	cTemp = cView(rtFeature).clone();
}

void CMold::SelectRect(vector<Rect> vRect, Rect& rtSelected)
{
	for (auto& iRect : vRect)
		rtSelected = (rtSelected.area() < iRect.area() ? iRect : rtSelected);
}

void CMold::RemakeFeature(cv::Mat& cView, cv::Rect& rtFeature)
{
	rtFeature.x -= rtFeature.width * 0.125; // 0.25 * 0.5
	rtFeature.y -= rtFeature.height * 0.125;
	rtFeature.width *= 1.25;
	rtFeature.height *= 1.25;

	int r = rtFeature.x + rtFeature.width;
	int b = rtFeature.y + rtFeature.height;

	r = min(cView.cols, r);
	b = min(cView.rows, b);
	rtFeature.x = max(0, rtFeature.x);
	rtFeature.y = max(0, rtFeature.y);
	rtFeature.width = r - rtFeature.x;
	rtFeature.height = b - rtFeature.y;
}