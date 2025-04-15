#pragma once
#include "GDIDBuffer.h"
#include "opencv.hpp" 

class CStOverlap : public CWnd, public CGDIDBuffer
{
	DECLARE_DYNAMIC(CStOverlap)

public: 
	CStOverlap();
	virtual ~CStOverlap();

public:
	void OnOverlap(cv::Mat* lpLine, cv::Mat* lpLines);
	bool GetMatrix(cv::Mat*& lpMatrix);

private:
	void OnVerlap(cv::Mat* lpLine);
	void OnVerlapEx(cv::Mat* lpLines);
	void OnDrawMatrix(CDC& dc, unsigned char*& lpPixel, const long& biWidth);

private:
	cv::Mat m_cView;
	cv::Mat* m_lpLine;
	cv::Mat* m_lpLines;

private:
	afx_msg void OnPaint();

	DECLARE_MESSAGE_MAP()
};

