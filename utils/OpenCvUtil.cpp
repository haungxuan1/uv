#include "pch.h"
#include "OpenCvUtil.h"

OpenCvUtil::OpenCvUtil()
{
}

OpenCvUtil::~OpenCvUtil()
{

}

void OpenCvUtil::rotate(const Mat& tSrcMat, Mat& tDstMat, double dAngle, unsigned char ucFillVal)
{
	Point2f pt(tSrcMat.cols / 2.0f, tSrcMat.rows / 2.0f);
	Mat r = getRotationMatrix2D(pt, dAngle, 1.0);

	//计算新的宽高
	int iWid = tSrcMat.cols;
	int iHei = tSrcMat.rows;
	double dCosa = abs(r.at<double>(0, 0));
	double dSina = abs(r.at<double>(0, 1));
	double dNewWid = dCosa * iWid + dSina * iHei;			//新宽
	double dNewHei = dSina * iWid + dCosa * iHei;			//新高

	r.at<double>(0, 2) += (dNewWid / 2 - iWid / 2);		//新中心位置
	r.at<double>(1, 2) += (dNewHei / 2 - iHei / 2);		//新中心位置

	warpAffine(tSrcMat, tDstMat, r, Size(dNewWid, dNewHei), INTER_LINEAR, 0, Scalar(ucFillVal));
}

Mat OpenCvUtil::resize_ext(const Mat &tImg, Size tSize, double dX, double dY, int iInterpolation)
{
	Mat tDstMat;

	if (tImg.channels() <= 4) {
		resize(tImg, tDstMat, tSize, 0, 0, iInterpolation);
	}
	else {
		std::vector<cv::Mat> tVecChnMat;
		cv::split(tImg, tVecChnMat);
		std::vector<cv::Mat> tVecResizeChnMat;

		for (int i = 0; i < tVecChnMat.size(); i++) {
			cv::Mat tTmpMat;

			resize(tVecChnMat[i], tTmpMat, tSize, 0, 0, iInterpolation);

			tVecResizeChnMat.push_back(tTmpMat);
		}

		cv::merge(tVecResizeChnMat, tDstMat);
	}

	return tDstMat;
}

