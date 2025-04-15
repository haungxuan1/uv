#include "pch.h"
#include "Helper.h"

using namespace cv;
using namespace std;

extern void MeanFileter(int*& nHis, int nSize, int nWin);

void BimodalThreshold(Mat& cHistogram, int& nThreshold, int& nThresholdType)
{
	int* fGray = (int*)cHistogram.data;

	MeanFileter(fGray, 256, 2);

	int nMax = *max_element(fGray, fGray + 256), nMin = *min_element(fGray, fGray + 256);

	int nFristPeak = max_element(fGray, fGray + 256) - fGray;

	double dTmp[256] = { 0.0 }; // second peak = max{|k-first peak|*histogram[k]}

	for (int i = 0; i < 256; ++i) 
		dTmp[i] = (double)abs(i - nFristPeak) * (double)((long long)fGray[i] - nMin) / ((long long)nMax - nMin);

	int nSecondPeak = max_element(dTmp, dTmp + 256) - dTmp;

	nThreshold = min_element(fGray + min(nFristPeak, nSecondPeak), fGray + max(nFristPeak, nSecondPeak)) - fGray;

	int nTemp = (fGray[nFristPeak] < fGray[nSecondPeak] ? nFristPeak : nSecondPeak);

	nThresholdType = (nThreshold < nTemp ? THRESH_BINARY : THRESH_BINARY_INV);
}

void BimodalThresholdEx(Mat& cHistogram, int& nThreshold, int& nThresholdType)
{
	int* fGray = (int*)cHistogram.data;

	MeanFileter(fGray, 256, 2);

	int nMax = *max_element(fGray, fGray + 256), nMin = *min_element(fGray, fGray + 256);

	int nFristPeak = max_element(fGray, fGray + 256) - fGray;

	double dTmp[256] = { 0.0 }; // second peak = max{|k-first peak|*histogram[k]}

	for (int i = 0; i < 256; ++i)
		dTmp[i] = (double)abs(i - nFristPeak) * (double)((long long)fGray[i] - nMin) / ((long long)nMax - nMin);

	int nSecondPeak = max_element(dTmp, dTmp + 256) - dTmp;

	nThreshold = min_element(fGray + min(nFristPeak, nSecondPeak), fGray + max(nFristPeak, nSecondPeak)) - fGray;

	int nSePeak = fGray[nSecondPeak], nSection = round(0.01 * nSePeak);

	int nValley = fGray[nThreshold];

	for (int i = nThreshold; 0 <= i; --i)
	{
		if (nSection < abs(fGray[i] - nValley)) break;
		
		nThreshold = i;
	}

	int nTemp = (fGray[nFristPeak] < fGray[nSecondPeak] ? nFristPeak : nSecondPeak);

	nThresholdType = (nThreshold < nTemp ? THRESH_BINARY : THRESH_BINARY_INV);
}