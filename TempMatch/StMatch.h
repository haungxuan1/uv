#pragma once
#include "GDIDBuffer.h"
#include "opencv.hpp" 

class CStMatch : public CWnd, public CGDIDBuffer
{
	DECLARE_DYNAMIC(CStMatch)

public:
	CStMatch();
	virtual ~CStMatch();

public:
	bool GetTempParam(cv::Mat &cTemp, int& ox, int& oy);

private:
	virtual void OnTask(
		int ox, int oy, int ox2, int oy2,
		cv::Mat* lpMoldTemp, cv::Mat* lpMoldView, 
		cv::Mat* lpTempTemp, cv::Mat* lpTempView) = 0;

private:
	void OnSyncMatchRange(
		const int &ox, const int &oy,
		cv::Mat& cMold,
		int& x1, int& y1, int& w1, int& h1,
		cv::Mat& cTemp,
		int& x2, int& y2, int& w2, int& h2);

	void OnSyncTaskParam(
		cv::Mat& cTempTask,
		cv::Mat& cViewTask,
		cv::Mat& cTemp,
		cv::Mat& cView,
		int& x, int& y, int& w, int& h);

protected:
	void OnSyncMatched(
		int &ox, int &oy, int& ox2, int& oy2,
		cv::Mat* lpMoldTemp, cv::Mat* lpMoldView, 
		cv::Mat* lpTempTemp, cv::Mat* lpTempView, double &dSame);
	void OnSyncCanvas(cv::Mat* lpMatrix, double &dScale);
	void OnSyncView(cv::Mat* lpMatrix, cv::Mat &cView);

protected:
	void OnDrawMold(CDC& dc, unsigned char*& lpPixel, const long& biWidth);
	void OnDrawTemp(CDC& dc, unsigned char*& lpPixel, const long& biWidth);
	void OnOwnerDraw(CDC& dc, unsigned char*& lpPixel, const long& biWidth);

protected:
	double m_dScale[3]; // 0 canvas | 1 mold | 2 temp
	cv::Mat* m_lpMold[2];
	cv::Mat* m_lpTemp[2];
	cv::Mat m_cMold;
	cv::Mat m_cTemp;
	int m_nOx;
	int m_nOy;

private:
	afx_msg void OnPaint();
	LRESULT OnTempParam(WPARAM wapram, LPARAM lparam);

	DECLARE_MESSAGE_MAP()
};

