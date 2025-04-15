#include "pch.h"
#include "StOverlap.h"
#include <math.h>
#include <algorithm>

IMPLEMENT_DYNAMIC(CStOverlap, CWnd)

BEGIN_MESSAGE_MAP(CStOverlap, CWnd)
	ON_WM_PAINT()
END_MESSAGE_MAP()

CStOverlap::CStOverlap()
	:m_lpLine(new cv::Mat()), m_lpLines(new cv::Mat()) // 极小的临时内存
{

}

CStOverlap::~CStOverlap()
{
	delete m_lpLine; delete m_lpLines;
}

bool CStOverlap::GetMatrix(cv::Mat*& lpMatrix)
{
	lpMatrix = new cv::Mat();

	(*lpMatrix) = m_lpLine->clone();

	return !lpMatrix->empty();
}

void CStOverlap::OnOverlap(cv::Mat* lpLine, cv::Mat* lpLines)
{
	if(!!lpLine) OnVerlap(lpLine);

	if (!!lpLines) OnVerlapEx(lpLines);
}

void CStOverlap::OnVerlap(cv::Mat* lpLine)
{
	*m_lpLine = lpLine->clone();
}

void CStOverlap::OnVerlapEx(cv::Mat* lpLines)
{
	*m_lpLines = lpLines->clone();

	double sx = (double)m_rtCanvas.Width() / m_lpLines->cols;

	double sy = (double)m_rtCanvas.Height() / m_lpLines->rows;

	double dScale = std::min(sx, sy);

	cv::resize(
		*m_lpLines,
		m_cView,
		cv::Size(m_lpLines->cols * dScale, m_lpLines->rows * dScale));

	Invalidate();
}

void CStOverlap::OnPaint()
{
	CPaintDC dc(this);

	CGDIDBuffer::OnDraw2(&dc,
		std::bind(
			&CStOverlap::OnDrawMatrix, 
			this, 
			std::placeholders::_1, 
			std::placeholders::_2, 
			std::placeholders::_3));
}

void CStOverlap::OnDrawMatrix(CDC& dc, unsigned char*& lpPixel, const long& biWidth)
{
	if (m_cView.empty()) return;

	auto w1 = m_rtCanvas.Width(), h1 = m_rtCanvas.Height();

	auto& w2 = m_cView.cols, & h2 = m_cView.rows; auto& data = m_cView.data;

	int ox = (w1 - w2) * 0.5;

	int oy = (h1 - h2) * 0.5;

	for (unsigned long long y = 0; y < h2; ++y)
	{
		for (unsigned long long x = 0; x < w2; ++x)
		{
			unsigned long long uMatIndex = (y * w2 + x) * 1; // gray

			unsigned long long uCanIndex = ((y + oy) * biWidth + x + ox) * 4;

			lpPixel[uCanIndex + 0] = data[uMatIndex + 0]; // b

			lpPixel[uCanIndex + 1] = data[uMatIndex + 0]; // g

			lpPixel[uCanIndex + 2] = data[uMatIndex + 0]; // r

			lpPixel[uCanIndex + 3] = 0xff;
		}
	}
	lpPixel = UpdatePixel(lpPixel);
}

