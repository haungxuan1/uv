#pragma once
#include "opencv.hpp" 

class CMold
{
protected:
	CMold();
	virtual ~CMold();

protected:
	// 将该图进行模板(可能存在)提取
	// 灰度直方图 
	// 双峰1谷求取
	// 谷值二值化
	// 轮廓求取
	// 最大外接矩形 = 模板
	void Feature(cv::Mat& cView, cv::Mat& cTemp);
	void SelectRect(std::vector<cv::Rect> vRect, cv::Rect &rtSelected);
	void RemakeFeature(cv::Mat& cView, cv::Rect& rtFeature);
};