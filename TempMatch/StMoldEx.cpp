#include "pch.h"
#include "StMoldEx.h"
#include "PQMsg.h"
#include "PQSetting.h"
#include <algorithm>

IMPLEMENT_DYNAMIC(CStMoldEx, CStMold)

BEGIN_MESSAGE_MAP(CStMoldEx, CStMold)
	ON_WM_PAINT()
	ON_MESSAGE(PQM_FEATURE_DEFAULT, OnFeatureDefault)
	ON_MESSAGE(PQM_FEATURE_ZOOM, OnFeatureZoom)
	ON_MESSAGE(PQM_FEATURE_FRAME_CHANGED, OnFeatureFrame)
	ON_MESSAGE(PQM_FEATURE_RESET_PARAM, OnFeatureParam)
END_MESSAGE_MAP()

CStMoldEx::CStMoldEx()
	:m_nOx{ 0, 0, 0 }, m_nOy{ 0, 0, 0 }, 
	m_nCx{ 0, 0, 0 }, m_nCy{ 0, 0, 0 }, 
	m_dScale{ 0.0, 0.0 }
{

}

CStMoldEx::~CStMoldEx()
{

}

bool CStMoldEx::GetMoldParam(int& x, int& y, std::wstring& strMold, cv::Mat& cTemp)
{
	if (m_cMatrix[eMT_View].empty()) return false;

	strMold = m_strMold; 

	auto& iView = m_cMatrix[eMT_View], &iTemp = m_cMatrix[eMT_Temp], &iRange = m_cMatrix[eMT_Range], & iSrc = m_cMatrix[eMT_Src];

	const auto &cx1 = m_cMatrix[eMT_View].cols, &cy1 = m_cMatrix[eMT_View].rows;

	const auto& cx2 = m_cMatrix[ePP_Src2Range].cols, & cy2 = m_cMatrix[ePP_Src2Range].rows;

	x = (double)x / m_dScale[ePP_View];

	y = (double)y / m_dScale[ePP_View]; // λ��ģ��λ��

	x += m_nOx[ePP_Range2Temp];

	y += m_nOy[ePP_Range2Temp]; // λ��ѡ������λ��

	x += m_nOx[ePP_Src2Range];

	y += m_nOy[ePP_Src2Range]; // λ��ѡ��ԭʼλ��

	x = x - cx2 * 0.5; y = cy2 * 0.5 - y; // ��ԭʼͼ���ĵ�Ϊ����ԭ��

	return true;
}

void CStMoldEx::GetViewOs(int& ox, int& oy)
{
	const auto& iView = m_cMatrix[eMT_View];

	const auto& iCanvas = m_rtCanvas;

	auto w1 = iView.cols, h1 = iView.rows;

	auto w2 = iCanvas.Width(), h2 = iCanvas.Height();

	ox = iCanvas.left + (w2 - w1) * 0.5 + w1 * 0.5;

	oy = iCanvas.top + (h2 - h1) * 0.5 + h1 * 0.5/*���Ͻ�ת���ĵ�*/;
}

std::tuple<cv::Mat*, cv::Mat*>* CStMoldEx::GetView()
{
	std::tuple<cv::Mat*, cv::Mat*>* lpView = new std::tuple<cv::Mat*, cv::Mat*>(new cv::Mat(), new cv::Mat());

	*std::get<0>(*lpView) = m_cMatrix[eMT_Temp].clone();

	*std::get<1>(*lpView) = m_cMatrix[eMT_View].clone();

	return lpView;
}

void CStMoldEx::Sync2Parent()
{
	std::tuple<int, int, int, int, double>* lpParam = 
		new std::tuple<int, int, int, int, double>
		(m_nOx[ePP_Range2Temp], m_nOy[ePP_Range2Temp], 
			m_nCx[ePP_Range2Temp], m_nCy[ePP_Range2Temp], m_dScale[ePP_View]);

	CWnd* lpParent = GetParent();

	::PostMessage(lpParent->GetSafeHwnd(), PQM_MOLD_PARAM, 0, (LPARAM)lpParam);
}

void CStMoldEx::OnSyncMold(std::wstring strMold)
{
	cv::Mat cTif;

	if (!PQSetting::Tif2Mat(strMold, cTif)){ /*AfxMessageBox(L"����tif�ļ�ʧ��")*/; return; }

	m_strMold = strMold;

	cvtColor(cTif, m_cMatrix[eMT_Src], cv::COLOR_RGBA2BGRA); // rgba->bgra

	OnReset();
}

void CStMoldEx::OnReset()
{
	OnClearRangeParam();

	OnClearTempParam();

	OnClearViewParam();

	Invalidate();

	Sync2Parent();
}

void CStMoldEx::OnRange()
{
	auto& iSrc = m_cMatrix[eMT_Src],
		& iRange = m_cMatrix[eMT_Range],
		& iCur = m_cMatrix[eMT_Temp],
		& iView = m_cMatrix[eMT_View];

	// ������� = ģ�����
	m_cMatrix[eMT_Range] = m_cMatrix[eMT_Temp];

	m_nOx[ePP_Src2Range] += m_nOx[ePP_Range2Temp];

	m_nOy[ePP_Src2Range] += m_nOy[ePP_Range2Temp];

	m_nCx[ePP_Src2Range] = m_cMatrix[eMT_Range].cols;

	m_nCy[ePP_Src2Range] = m_cMatrix[eMT_Range].rows;

	m_nOx[ePP_Range2Temp] = 0;

	m_nOy[ePP_Range2Temp] = 0;

	m_nCx[ePP_Range2Temp] = m_cMatrix[eMT_Temp].cols;

	m_nCy[ePP_Range2Temp] = m_cMatrix[eMT_Temp].rows;

	Invalidate();

	Sync2Parent();
}

void CStMoldEx::OnSave()
{
	m_nOx[ePP_Backup] = m_nOx[ePP_Range2Temp];

	m_nOy[ePP_Backup] = m_nOy[ePP_Range2Temp];

	m_nCx[ePP_Backup] = m_nCx[ePP_Range2Temp];

	m_nCy[ePP_Backup] = m_nCy[ePP_Range2Temp];

	m_dScale[ePP_Backup] = m_dScale[ePP_View];
}

void CStMoldEx::OnRecover()
{
	m_nOx[ePP_Range2Temp] = m_nOx[ePP_Backup];

	m_nOy[ePP_Range2Temp] = m_nOy[ePP_Backup];

	m_nCx[ePP_Range2Temp] = m_nCx[ePP_Backup];

	m_nCy[ePP_Range2Temp] = m_nCy[ePP_Backup];

	m_dScale[ePP_View] = m_dScale[ePP_Backup];

	FeatureFrame vFrame{
	m_nOx[ePP_Range2Temp],
	m_nOy[ePP_Range2Temp],
	m_nOx[ePP_Range2Temp] + m_nCx[ePP_Range2Temp],
	m_nOy[ePP_Range2Temp] + m_nCy[ePP_Range2Temp] };

	Range2Cur(vFrame);

	Cur2View(true);

	Invalidate();

	Sync2Parent();
}

void CStMoldEx::OnClearRangeParam()
{
	m_cMatrix[eMT_Range] = m_cMatrix[eMT_Src];

	m_nOx[ePP_Src2Range] = 0;

	m_nOy[ePP_Src2Range] = 0;

	m_nCx[ePP_Src2Range] = m_cMatrix[eMT_Range].cols;

	m_nCy[ePP_Src2Range] = m_cMatrix[eMT_Range].rows;
}

void CStMoldEx::OnClearTempParam()
{
	m_cMatrix[eMT_Temp] = m_cMatrix[eMT_Range];

	m_nOx[ePP_Range2Temp] = 0;

	m_nOy[ePP_Range2Temp] = 0;

	m_nCx[ePP_Range2Temp] = m_cMatrix[eMT_Temp].cols;

	m_nCy[ePP_Range2Temp] = m_cMatrix[eMT_Temp].rows;
}

void CStMoldEx::OnClearViewParam()
{
	double sx = (double)m_rtCanvas.Width() / m_cMatrix[eMT_Temp].cols;

	double sy = (double)m_rtCanvas.Height() / m_cMatrix[eMT_Temp].rows;

	m_dScale[ePP_View] = std::min(sx, sy);

	cv::resize(
		m_cMatrix[eMT_Temp],
		m_cMatrix[eMT_View],
		cv::Size(m_nCx[ePP_Range2Temp] * m_dScale[ePP_View], m_nCy[ePP_Range2Temp] * m_dScale[ePP_View]));
}

LRESULT CStMoldEx::OnFeatureFrame(WPARAM wapram, LPARAM lparam)
{
	FeatureFrame* lpParam = (FeatureFrame*)lparam;

	m_vFeatureFrame = (*lpParam);

	delete lpParam;

	Invalidate();

	return 0;
}

LRESULT CStMoldEx::OnFeatureDefault(WPARAM wapram, LPARAM lparam)
{
	// ��ǰ���� = �������
	// ��ͼ���� = ��ǰ���� * scale
	if (m_cMatrix[eMT_Range].empty()) return 0;

	OnClearTempParam();

	OnClearViewParam();

	Invalidate();

	Sync2Parent();

	return 0;
}

LRESULT CStMoldEx::OnFeatureZoom(WPARAM wapram, LPARAM lparam)
{
	FeatureFrame* lpParam = (FeatureFrame*)lparam;
	FeatureFrame vFrame = (*lpParam);
	delete lpParam;
	if (m_cMatrix[eMT_Src].empty()) return 0;

	auto& l = std::get<eFFT_l>(vFrame),
		& t = std::get<eFFT_t>(vFrame),
		& r = std::get<eFFT_r>(vFrame),
		& b = std::get<eFFT_b>(vFrame);

	auto& iSrc = m_cMatrix[eMT_Src],
		& iRange = m_cMatrix[eMT_Range],
		& iCur = m_cMatrix[eMT_Temp],
		& iView = m_cMatrix[eMT_View];

	Client2Canvas(vFrame);
	Canvas2View(vFrame);
	l = std::max(0, l); t = std::max(0, t); r = std::min(iView.cols, r); b = std::min(iView.rows, b);
	cv::Mat cViewTmp = iView.clone();
	cv::rectangle(cViewTmp, cv::Point(l, t), cv::Point(r, b), cv::Scalar(0xff));

	View2Cur(vFrame);
	cv::Mat cCurTmp = iCur.clone();
	cv::rectangle(cCurTmp, cv::Point(l, t), cv::Point(r, b), cv::Scalar(0xff));

	Cur2Range(vFrame);
	cv::Mat cRangeTmp = iRange.clone();
	cv::rectangle(cRangeTmp, cv::Point(l, t), cv::Point(r, b), cv::Scalar(0xff));

	Range2Cur(vFrame);
	Cur2View(false);

	Invalidate();
	Sync2Parent();

	return 0;
}

LRESULT CStMoldEx::OnFeatureParam(WPARAM wapram, LPARAM lparam)
{
	std::tuple<int, int, int, int, double>* lpParam = (std::tuple<int, int, int, int, double>*)lparam;
	std::tuple<int, int, int, int, double> vParam = (*lpParam); 
	delete lpParam;
	if (m_cMatrix[eMT_Src].empty()) return 0;

	// ��¼��ǰ������ʧ����ָ�����
	BackupOrRestore(true);

	if (!IsValidInput(vParam)) { BackupOrRestore(false); Sync2Parent(); return 0; }

	FeatureFrame vFrame{
		std::get<0>(vParam),
		std::get<1>(vParam),
		std::get<0>(vParam) + std::get<2>(vParam),
		std::get<1>(vParam) + std::get<3>(vParam)};

	m_dScale[ePP_View] = std::get<4>(vParam);

	Range2Cur(vFrame);

	Cur2View(true);

	Invalidate();

	Sync2Parent();

	return 0;
}

bool CStMoldEx::IsValidInput(std::tuple<int, int, int, int, double>& vFrame)
{
	const auto& x = std::get<0>(vFrame);
	const auto& y = std::get<1>(vFrame);
	const auto& cx = std::get<2>(vFrame);
	const auto& cy = std::get<3>(vFrame);

	return ((0 <= x) &&
			(0 <= y) &&
			(cx <= m_cMatrix[eMT_Range].cols) &&
			(cy <= m_cMatrix[eMT_Range].rows));
}

void CStMoldEx::BackupOrRestore(bool bBackup)
{
	static double dAngle = 0.0;

	static int ox = 0, oy = 0, cx = 0, cy = 0;

	ox = (bBackup ? m_nOx[ePP_Range2Temp] : ox);
	oy = (bBackup ? m_nOy[ePP_Range2Temp] : oy);
	cx = (bBackup ? m_nCx[ePP_Range2Temp] : cx);
	cy = (bBackup ? m_nCy[ePP_Range2Temp] : cy);
	dAngle = (bBackup ? m_dScale[ePP_View] : dAngle);

	m_nOx[ePP_Range2Temp] = ox;
	m_nOy[ePP_Range2Temp] = oy;
	m_nCx[ePP_Range2Temp] = cx;
	m_nCy[ePP_Range2Temp] = cy;
	m_dScale[ePP_View] = dAngle;
}

void CStMoldEx::Client2Canvas(FeatureFrame& vFrame)
{
	// Client = Canvas
}

void CStMoldEx::Canvas2View(FeatureFrame& vFrame)
{
	auto& l = std::get<eFFT_l>(vFrame),
		& t = std::get<eFFT_t>(vFrame),
		& r = std::get<eFFT_r>(vFrame),
		& b = std::get<eFFT_b>(vFrame);

	auto& iView = m_cMatrix[eMT_View];

	const auto& iCanvas = m_rtCanvas;

	int ox = (iCanvas.Width() - iView.cols) * 0.5;

	int oy = (iCanvas.Height() - iView.rows) * 0.5;

	l -= ox; r -= ox;

	t -= oy; b -= oy;
}

void CStMoldEx::View2Cur(FeatureFrame& vFrame)
{
	auto& l = std::get<eFFT_l>(vFrame),
		& t = std::get<eFFT_t>(vFrame),
		& r = std::get<eFFT_r>(vFrame),
		& b = std::get<eFFT_b>(vFrame);

	l = (double)l / m_dScale[ePP_View];

	t = (double)t / m_dScale[ePP_View];

	r = (double)r / m_dScale[ePP_View];

	b = (double)b / m_dScale[ePP_View];
}

void CStMoldEx::Cur2Range(FeatureFrame& vFrame)
{
	auto& l = std::get<eFFT_l>(vFrame),
		& t = std::get<eFFT_t>(vFrame),
		& r = std::get<eFFT_r>(vFrame),
		& b = std::get<eFFT_b>(vFrame);

	l += m_nOx[ePP_Range2Temp]; r += m_nOx[ePP_Range2Temp];
	t += m_nOy[ePP_Range2Temp]; b += m_nOy[ePP_Range2Temp];
}

void CStMoldEx::Range2Cur(FeatureFrame& vFrame)
{
	auto& l = std::get<eFFT_l>(vFrame),
		& t = std::get<eFFT_t>(vFrame),
		& r = std::get<eFFT_r>(vFrame),
		& b = std::get<eFFT_b>(vFrame);

	auto w = r - l, h = b - t;

	m_nOx[ePP_Range2Temp] = l;
	m_nOy[ePP_Range2Temp] = t;
	m_nCx[ePP_Range2Temp] = w;
	m_nCy[ePP_Range2Temp] = h;

	cv::Rect rtRange(l, t, w, h);

	m_cMatrix[eMT_Temp] = m_cMatrix[eMT_Range](rtRange);
}

void CStMoldEx::Cur2View(bool bScale)
{
	const auto& iCanvas = m_rtCanvas;

	double w = iCanvas.Width();

	double h = iCanvas.Height();

	double sx = w / m_cMatrix[eMT_Temp].cols;

	double sy = h / m_cMatrix[eMT_Temp].rows;

	double smax = std::min(sx, sy);

	m_dScale[ePP_View] = (bScale ? std::min(smax, m_dScale[ePP_View]) : smax);

	cv::resize(
		m_cMatrix[eMT_Temp],
		m_cMatrix[eMT_View],
		cv::Size(
			m_cMatrix[eMT_Temp].cols * m_dScale[ePP_View],
			m_cMatrix[eMT_Temp].rows * m_dScale[ePP_View]));
}

void CStMoldEx::OnPaint()
{
	CPaintDC dc(this);

	CGDIDBuffer::OnDraw2(&dc,
		std::bind(&CStMoldEx::OnOwnerDraw, 
			this, 
			std::placeholders::_1, 
			std::placeholders::_2, 
			std::placeholders::_3));
}

void CStMoldEx::OnOwnerDraw(CDC& dc, unsigned char*& lpPixel, const long& biWidth)
{
	OnDrawMold(dc, lpPixel, biWidth);

	OnDrawFrame(dc, lpPixel, biWidth);
}

void CStMoldEx::OnDrawMold(CDC& dc, unsigned char*& lpPixel, const long& biWidth)
{
	if (m_cMatrix[eMT_View].empty()) return;

	auto& iView = m_cMatrix[eMT_View];

	auto& w = iView.cols, & h = iView.rows;

	int ox = (m_rtCanvas.Width() - w) * 0.5;

	int oy = (m_rtCanvas.Height() - h) * 0.5;

	for (unsigned long long y = 0; y < h; ++y)
	{
		for (unsigned long long x = 0; x < w; ++x)
		{
			unsigned long long uMatIndex = (y * w + x) * 4; // rgba

			unsigned long long uCanIndex = ((y + oy) * biWidth + x + ox) * 4;

			for (int c = 0; c < 3; ++c) // bgra
			{
				lpPixel[uCanIndex + c] = iView.data[uMatIndex + c];
			}
			lpPixel[uCanIndex + 4] = 0xff;
		}
	}
	lpPixel = UpdatePixel(lpPixel);
}

void CStMoldEx::OnDrawFrame(CDC& dc, unsigned char*& lpPixel, const long& biWidth)
{
	if (m_cMatrix[eMT_View].empty()) return;

	if (!m_bLbdown) return; // ����ѡȡ����

	const auto& l = std::get<eFFT_l>(m_vFeatureFrame);

	const auto& t = std::get<eFFT_t>(m_vFeatureFrame);

	const auto& r = std::get<eFFT_r>(m_vFeatureFrame);

	const auto& b = std::get<eFFT_b>(m_vFeatureFrame);

	POINT ptOpt[5] =
	{
		{(float)l, (float)t},
		{(float)r, (float)t},
		{(float)r, (float)b},
		{(float)l, (float)b},
		{(float)l, (float)t}
	};

	dc.Polyline(ptOpt, 5);
}