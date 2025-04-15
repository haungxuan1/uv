#pragma once
#include "opencv.hpp" 

namespace MatrixOverlap
{
	void Reg();

	void Unreg();

	void SetTar(cv::Mat& cTar, int nMaxFeatures);

	bool Overlap(cv::Mat &cTemp, POINT &ptTopLeft, double &dSame);
}

