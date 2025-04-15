#pragma once
#include "opencv.hpp" 

class CMold
{
protected:
	CMold();
	virtual ~CMold();

protected:
	// ����ͼ����ģ��(���ܴ���)��ȡ
	// �Ҷ�ֱ��ͼ 
	// ˫��1����ȡ
	// ��ֵ��ֵ��
	// ������ȡ
	// �����Ӿ��� = ģ��
	void Feature(cv::Mat& cView, cv::Mat& cTemp);
	void SelectRect(std::vector<cv::Rect> vRect, cv::Rect &rtSelected);
	void RemakeFeature(cv::Mat& cView, cv::Rect& rtFeature);
};