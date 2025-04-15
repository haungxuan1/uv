#include "pch.h"
#include "StMatch.h"
#include "PQMsg.h"

IMPLEMENT_DYNAMIC(CStMatch, CWnd)

BEGIN_MESSAGE_MAP(CStMatch, CWnd)
	ON_WM_PAINT()
	ON_MESSAGE(PQM_FEATURE_RESET_PARAM, OnTempParam)
END_MESSAGE_MAP()

CStMatch::CStMatch()
	: m_nOx(0), m_nOy(0), m_dScale{ DBL_MAX, DBL_MAX, DBL_MAX }
{
	m_lpMold[0] = new cv::Mat(0, 0, CV_8UC1);

	m_lpMold[1] = new cv::Mat(0, 0, CV_8UC1);

	m_lpTemp[0] = new cv::Mat(0, 0, CV_8UC1);

	m_lpTemp[1] = new cv::Mat(0, 0, CV_8UC1);
}

CStMatch::~CStMatch()
{
	if (m_lpMold[0]) delete m_lpMold[0];
	if (m_lpMold[1]) delete m_lpMold[1];
	if (m_lpTemp[0]) delete m_lpTemp[0];
	if (m_lpTemp[1]) delete m_lpTemp[1];
}

bool CStMatch::GetTempParam(cv::Mat& cTemp, int& ox, int& oy)
{
	if (m_lpTemp[0]->empty()) return false;

	ox = m_nOx; oy = m_nOy;

	cTemp = m_lpTemp[0]->clone();

	return (!cTemp.empty());
}

void CStMatch::OnSyncMatchRange(
	const int& ox, const int& oy,
	cv::Mat &cMold,
	int &x1, int &y1, int &w1, int &h1,
	cv::Mat& cTemp,
	int &x2, int &y2, int &w2, int &h2)
{
	// 1 为源 2 为模板
	const auto &cx1 = cMold.cols, &cx2 = cTemp.cols;
	const auto &cy1 = cMold.rows, &cy2 = cTemp.rows;

	x1 = std::max(0.0, cx1 * 0.5 - cx2 * 0.5 + ox);
	y1 = std::max(0.0, cy1 * 0.5 - cy2 * 0.5 - oy);
	w1 = (cMold.empty() ? INT_MAX : (cMold.cols - x1));
	h1 = (cMold.empty() ? INT_MAX : (cMold.rows - y1));

	x2 = std::abs(std::min(0.0, cx1 * 0.5 - cx2 * 0.5 + ox));
	y2 = std::abs(std::min(0.0, cy1 * 0.5 - cy2 * 0.5 - oy));
	w2 = (cTemp.empty() ? INT_MAX : (cTemp.cols - x2));
	h2 = (cTemp.empty() ? INT_MAX : (cTemp.rows - y2));

	w1 = w2 = std::min(w1, w2);
	h1 = h2 = std::min(h1, h2);
}

void CStMatch::OnSyncTaskParam(
	cv::Mat& cTempTask,
	cv::Mat& cViewTask,
	cv::Mat& cTemp,
	cv::Mat& cView,
	int& x, int& y, int& w, int& h)
{
	if (cTemp.empty() || cView.empty()) return;

	double sx = (double)cView.cols / cTemp.cols;

	double sy = (double)cView.rows / cTemp.rows;

	double dScale = std::max(sx, sy); // 两者相等, double精度问题这里取min

	int x2 = (double)x / dScale, y2 = (double)y / dScale, w2 = (double)w / dScale, h2 = (double)h / dScale;

	cViewTask = cView(cv::Rect(x, y, w, h)).clone();

	cTempTask = cTemp(cv::Rect(x2, y2, w2, h2)).clone();
}

LRESULT CStMatch::OnTempParam(WPARAM wapram, LPARAM lparam)
{
	std::tuple<int, int>* lpParam = (std::tuple<int, int> *)lparam;

	std::tuple<
		std::tuple<cv::Mat*, cv::Mat*>*,
		std::tuple<cv::Mat*, cv::Mat*>*>* lpMatrix = (std::tuple<
		std::tuple<cv::Mat*, cv::Mat*>*,
		std::tuple<cv::Mat*, cv::Mat*>*>*)wapram;

	const auto& ox = std::get<0>(*lpParam);

	const auto& oy = std::get<1>(*lpParam);

	auto& lpMold = std::get<0>(*lpMatrix);

	auto& lpTemp = std::get<1>(*lpMatrix);

	auto& lpMoldTemp = std::get<0>(*lpMold);

	auto& lpMoldView = std::get<1>(*lpMold);

	auto& lpTempTemp = std::get<0>(*lpTemp);

	auto& lpTempView = std::get<1>(*lpTemp);

	// 求交集
	// 1 为源 2 为模板
	int x1 = 0, y1 = 0, w1 = 0, h1 = 0;

	int x2 = 0, y2 = 0, w2 = 0, h2 = 0;

	// 模板交集
	OnSyncMatchRange(
		ox, oy,
		*lpMoldView,
		x1, y1, w1, h1,
		*lpTempView,
		x2, y2, w2, h2); // 如果不存在交集，则必然无法产生模板图，此时下方崩溃，提示？直接返回？待定

	// 交集相对于源视图中心点偏移
	int ox2 = x1 + w1 * 0.5;

	int oy2 = y1 + h1 * 0.5;

	cv::Mat* lpTMoldView = new cv::Mat(), * lpTMoldTemp = new cv::Mat(), * lpTTempView = new cv::Mat(), * lpTTempTemp = new cv::Mat();

	OnSyncTaskParam(*lpTMoldTemp, *lpTMoldView, *lpMoldTemp, *lpMoldView, x1, y1, w1, h1);

	OnSyncTaskParam(*lpTTempTemp, *lpTTempView, *lpTempTemp, *lpTempView, x2, y2, w2, h2);

	OnTask(ox, oy, ox2, oy2, lpTMoldTemp, lpTMoldView, lpTTempTemp, lpTTempView);

	delete lpMoldView; delete lpMoldTemp;
	
	delete lpTempView; delete lpTempTemp;

	delete lpParam; delete lpMatrix;

	return 0;
}

void CStMatch::OnSyncMatched(
	int& ox, int& oy, int& ox2, int& oy2,
	cv::Mat* lpMoldTemp, cv::Mat* lpMoldView, 
	cv::Mat* lpTempTemp, cv::Mat* lpTempView, 
	double& dSame)
{
	delete m_lpMold[0]; m_lpMold[0] = lpMoldTemp;
	delete m_lpMold[1]; m_lpMold[1] = lpMoldView;
	delete m_lpTemp[0]; m_lpTemp[0] = lpTempTemp;
	delete m_lpTemp[1]; m_lpTemp[1] = lpTempView;

	m_nOx = ox2; m_nOy = oy2;

	OnSyncCanvas(m_lpMold[1], m_dScale[1]);

	OnSyncCanvas(m_lpTemp[1], m_dScale[2]);

	m_dScale[0] = (m_dScale[1] != 0.0 ? 
		(m_dScale[2] != 0.0 ?
			(std::min(m_dScale[1], m_dScale[2])): m_dScale[1]): m_dScale[2]);

	OnSyncView(m_lpMold[1], m_cMold);

	OnSyncView(m_lpTemp[1], m_cTemp);

	Invalidate();

	// 抛出评分
	::PostMessage(GetParent()->GetSafeHwnd(), 
		PQM_TEMP_MATCHED, 
		(WPARAM)new double(dSame), (LPARAM)new std::tuple<int, int>(ox, oy));
}

void CStMatch::OnSyncCanvas(cv::Mat* lpMatrix, double& dScale)
{
	double sx = (lpMatrix->empty() ? 0.0 : ((double)m_rtCanvas.Width() / lpMatrix->cols));

	double sy = (lpMatrix->empty() ? 0.0 : ((double)m_rtCanvas.Height() / lpMatrix->rows));

	dScale = std::min(sx, sy);
}

void CStMatch::OnSyncView(cv::Mat* lpMatrix, cv::Mat& cView)
{
	if (lpMatrix->empty()) return;

	cv::resize(*lpMatrix, cView,
		cv::Size(lpMatrix->cols * m_dScale[0], lpMatrix->rows * m_dScale[0]));
}

void CStMatch::OnDrawMold(CDC& dc, unsigned char*& lpPixel, const long& biWidth)
{
	if (m_cMold.empty()) return;

	auto w1 = m_rtCanvas.Width(), h1 = m_rtCanvas.Height();

	auto& w2 = m_cMold.cols, & h2 = m_cMold.rows; auto& data = m_cMold.data;

	int ox = (w1 - w2) * 0.5;

	int oy = (h1 - h2) * 0.5;

	for (unsigned long long y = 0; y < h2; ++y)
	{
		for (unsigned long long x = 0; x < w2; ++x)
		{
			unsigned long long uMatIndex = (y * w2 + x) * 4; // BGRA

			unsigned long long uCanIndex = ((y + oy) * biWidth + x + ox) * 4;

			lpPixel[uCanIndex + 0] = data[uMatIndex + 0]; // b

			lpPixel[uCanIndex + 1] = data[uMatIndex + 1]; // g

			lpPixel[uCanIndex + 2] = data[uMatIndex + 2]; // r

			lpPixel[uCanIndex + 3] = 0xff;
		}
	}
	lpPixel = UpdatePixel(lpPixel);
}

void CStMatch::OnDrawTemp(CDC& dc, unsigned char*& lpPixel, const long& biWidth)
{
	if (m_cTemp.empty()) return;

	auto w1 = m_rtCanvas.Width(), h1 = m_rtCanvas.Height();

	auto& w2 = m_cTemp.cols, & h2 = m_cTemp.rows; auto& data = m_cTemp.data;

	int ox = (w1 - w2) * 0.5;

	int oy = (h1 - h2) * 0.5;

	unsigned long long x1 = 0, y1 = 0, x2 = 0, y2 = 0;

	for (y2 = std::max(0, -oy); y2 < h2; ++y2)
	{
		y1 = y2 + oy;

		for (x2 = std::max(0, -ox); x2 < w2; ++x2)
		{
			x1 = x2 + ox;

			unsigned long long uMatIndex = (y2 * w2 + x2) * 4; // brga

			unsigned long long uCanIndex = (y1 * biWidth + x1) * 4;

			double alpha2 = (double)data[uMatIndex + 3] / 0xff; auto alpha1 = 1.0 - alpha2;

			lpPixel[uCanIndex + 0] = data[uMatIndex + 0] * alpha2 + lpPixel[uCanIndex + 0] * alpha1;

			lpPixel[uCanIndex + 1] = data[uMatIndex + 1] * alpha2 + lpPixel[uCanIndex + 1] * alpha1; // b

			lpPixel[uCanIndex + 2] = data[uMatIndex + 2] * alpha2 + lpPixel[uCanIndex + 2] * alpha1;

			lpPixel[uCanIndex + 3] = 0xff;
		}
	}
	lpPixel = UpdatePixel(lpPixel);
}

void CStMatch::OnOwnerDraw(CDC& dc, unsigned char*& lpPixel, const long& biWidth)
{
	OnDrawMold(dc, lpPixel, biWidth);

	OnDrawTemp(dc, lpPixel, biWidth);
}

void CStMatch::OnPaint()
{
	CPaintDC dc(this);

	CGDIDBuffer::OnDraw2(&dc,
		std::bind(
			&CStMatch::OnOwnerDraw,
			this, 
			std::placeholders::_1, 
			std::placeholders::_2, 
			std::placeholders::_3));
}