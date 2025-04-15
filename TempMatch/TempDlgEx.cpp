#include "pch.h"
#include "TempDlgEx.h"
#include "PQMsg.h"
#include <algorithm>
#include "PrClock/PrClock.h"

IMPLEMENT_DYNAMIC(CTempDlgEx, CTempDlg)

BEGIN_MESSAGE_MAP(CTempDlgEx, CTempDlg)
	ON_MESSAGE(PQM_FEATURE_DEFAULT, OnFeatureDefault)
	ON_MESSAGE(PQM_FEATURE_ZOOM, OnFeatureZoom)
	ON_MESSAGE(PQM_FEATURE_FRAME_CHANGED, OnFeatureFrame)
	ON_MESSAGE(PQM_FEATURE_RESET_PARAM, OnFeatureParam)
	ON_BN_CLICKED(IDC_BT_RESET, OnClickedReset)
	ON_BN_CLICKED(IDC_BT_SAVE_RANGE, OnClickedRange)
	ON_BN_CLICKED(IDC_BT_SAVE, OnClickedSave)
	ON_BN_CLICKED(IDC_BT_RECOVER, OnClickedRecover)
	ON_BN_CLICKED(IDC_BT_SYNC_OS, OnSyncOsParam)
	ON_BN_CLICKED(IDC_BT_OK, OnClickedOk)
END_MESSAGE_MAP()

CTempDlgEx::CTempDlgEx(CWnd* pParent /*=NULL*/)
	: CTempDlg(pParent), 
	m_nOx{ 0, 0 }, 
	m_nOy{ 0, 0 }, 
	m_nCx{ 0, 0 }, 
	m_nCy{ 0, 0 }, 
	m_dAngle{ 0.0, 0.0 },
	m_dScale{ 0.0, 0.0 }, m_nAlpha{ 0xff, 0xff }
{

}

CTempDlgEx::~CTempDlgEx()
{

}

void CTempDlgEx::DoDataExchange(CDataExchange* pDX)
{
	CTempDlg::DoDataExchange(pDX);

	DDX_Text(pDX, IDC_FEATURE_ET_X, m_nOx[ePP_Range2Temp]);

	DDX_Text(pDX, IDC_FEATURE_ET_Y, m_nOy[ePP_Range2Temp]);

	DDX_Text(pDX, IDC_FEATURE_ET_W, m_nCx[ePP_Range2Temp]);

	DDX_Text(pDX, IDC_FEATURE_ET_H, m_nCy[ePP_Range2Temp]);

	DDX_Text(pDX, IDC_FEATURE_ET_A, m_dAngle[ePP_Range2Temp]);

	DDX_Text(pDX, IDC_FEATURE_ET_S, m_dScale[ePP_View]);

	DDX_Text(pDX, IDC_ET_ALPHA, m_nAlpha[ePP_View]);
}

BOOL CTempDlgEx::OnInitDialog()
{
	CTempDlg::OnInitDialog();

	return TRUE;
}

void CTempDlgEx::GetViewOs(int& ox, int& oy)
{
	const auto& iView = m_cMatrix[eMT_View];

	const auto& iFeature = m_rtFeature[eRT_Feature];

	auto w1 = iView.cols, h1 = iView.rows;

	auto w2 = iFeature.Width(), h2 = iFeature.Height();

	ox = iFeature.left + (w2 - w1) * 0.5 + w1 * 0.5;

	oy = iFeature.top + (h2 - h1) * 0.5 + h1 * 0.5/*左上角转中心点*/;
}

std::tuple<cv::Mat*, cv::Mat*>* CTempDlgEx::GetView()
{
	std::tuple<cv::Mat*, cv::Mat*> *lpView = new std::tuple<cv::Mat*, cv::Mat*>(new cv::Mat(), new cv::Mat());

	*std::get<0>(*lpView) = m_cMatrix[eMT_Temp].clone();

	*std::get<1>(*lpView) = m_cMatrix[eMT_View].clone();

	return lpView;
}

void CTempDlgEx::OnClickedReset()
{
	CPrClock pr(__FUNCTIONW__);

	OnClearRangeParam();

	OnClearTempParam();

	OnClearViewParam();

	UpdateData(FALSE);

	Invalidate();

	Sync2Parent();
}

void CTempDlgEx::OnClickedRange()
{
	CPrClock pr(__FUNCTIONW__);

	auto& iSrc = m_cMatrix[eMT_Src],
		& iRange = m_cMatrix[eMT_Range],
		& iCur = m_cMatrix[eMT_Temp],
		& iView = m_cMatrix[eMT_View],
		& iCache = m_cMatrix[eMT_Cache];

	// 区域矩阵 = 模板矩阵
	m_cMatrix[eMT_Range] = m_cMatrix[eMT_Temp];

	m_nOx[ePP_Range2Temp] = 0;

	m_nOy[ePP_Range2Temp] = 0;

	m_nCx[ePP_Range2Temp] = m_cMatrix[eMT_Temp].cols;

	m_nCy[ePP_Range2Temp] = m_cMatrix[eMT_Temp].rows;

	m_dAngle[ePP_Range2Temp] = 0.0;

	UpdateData(FALSE);

	Invalidate();

	Sync2Parent();
}

void CTempDlgEx::OnClickedSave()
{
	CPrClock pr(__FUNCTIONW__);

	m_nOx[ePP_Backup] = m_nOx[ePP_Range2Temp];

	m_nOy[ePP_Backup] = m_nOy[ePP_Range2Temp];

	m_nCx[ePP_Backup] = m_nCx[ePP_Range2Temp];

	m_nCy[ePP_Backup] = m_nCy[ePP_Range2Temp];

	m_dAngle[ePP_Backup] = m_dAngle[ePP_Range2Temp];

	m_dScale[ePP_Backup] = m_dScale[ePP_View];

	m_nAlpha[ePP_Backup] = m_nAlpha[ePP_View];
}

void CTempDlgEx::OnClickedRecover()
{
	CPrClock pr(__FUNCTIONW__);

	m_nOx[ePP_Range2Temp] = m_nOx[ePP_Backup];

	m_nOy[ePP_Range2Temp] = m_nOy[ePP_Backup];

	m_nCx[ePP_Range2Temp] = m_nCx[ePP_Backup];

	m_nCy[ePP_Range2Temp] = m_nCy[ePP_Backup];

	m_dAngle[ePP_Range2Temp] = m_dAngle[ePP_Backup];

	m_dScale[ePP_View] = m_dScale[ePP_Backup];

	m_nAlpha[ePP_View] = m_nAlpha[ePP_Backup];

	FeatureFrame vFrame{
	m_nOx[ePP_Range2Temp],
	m_nOy[ePP_Range2Temp],
	m_nOx[ePP_Range2Temp] + m_nCx[ePP_Range2Temp],
	m_nOy[ePP_Range2Temp] + m_nCy[ePP_Range2Temp] };

	OnSyncAlpha();

	Range2Cur(vFrame);

	Cur2View(true);

	UpdateData(FALSE);
	Invalidate();
	Sync2Parent();
}

void CTempDlgEx::OnSyncOsParam()
{
	CPrClock pr(__FUNCTIONW__);

	CWnd* lpParent = GetParent();

	::PostMessage(lpParent->GetSafeHwnd(), PQM_FEATURE_SYNC_OS_PARAM, 0, 0);
}

void CTempDlgEx::OnClickedOk()
{
	CPrClock pr(__FUNCTIONW__);

	CWnd* lpParent = GetParent();

	::PostMessage(lpParent->GetSafeHwnd(), PQM_FEATURE_OK, 0, 0);
}

void CTempDlgEx::Sync2Parent()
{
	CPrClock pr(__FUNCTIONW__);

	CWnd* lpParent = GetParent();

	::PostMessage(lpParent->GetSafeHwnd(), PQM_FEATURE_PARAM, 0, 0);
}

void CTempDlgEx::OnSyncFeature(cv::Mat& cMatrix)
{
	CPrClock pr(__FUNCTIONW__);

	cvtColor(cMatrix, m_cMatrix[eMT_Src], cv::COLOR_GRAY2BGRA);

	OnClickedReset();
}

void CTempDlgEx::OnClearRangeParam()
{
	CPrClock pr(__FUNCTIONW__);

	m_cMatrix[eMT_Range] = m_cMatrix[eMT_Src];
}

void CTempDlgEx::OnClearTempParam()
{
	CPrClock pr(__FUNCTIONW__);

	m_cMatrix[eMT_Temp] = m_cMatrix[eMT_Range];

	m_nOx[ePP_Range2Temp] = 0;

	m_nOy[ePP_Range2Temp] = 0;

	m_nCx[ePP_Range2Temp] = m_cMatrix[eMT_Temp].cols;

	m_nCy[ePP_Range2Temp] = m_cMatrix[eMT_Temp].rows;

	m_dAngle[ePP_Range2Temp] = 0.0;	
}

void CTempDlgEx::OnClearViewParam()
{
	CPrClock pr(__FUNCTIONW__);

	double sx = (double)m_rtFeature[eRT_Feature].Width() / m_cMatrix[eMT_Temp].cols;

	double sy = (double)m_rtFeature[eRT_Feature].Height() / m_cMatrix[eMT_Temp].rows;

	m_dScale[ePP_View] = std::min(sx, sy);

	int nWIm = (double)m_nCx[ePP_Range2Temp] * m_dScale[ePP_View];

	int nHIm = (double)m_nCy[ePP_Range2Temp] * m_dScale[ePP_View];

	cv::resize(
		m_cMatrix[eMT_Temp],
		m_cMatrix[eMT_View],
		cv::Size((double)m_nCx[ePP_Range2Temp] * m_dScale[ePP_View], (double)m_nCy[ePP_Range2Temp] * m_dScale[ePP_View]));

	_cwprintf(L"view: %d, %d\r\n", m_cMatrix[eMT_View].cols, m_cMatrix[eMT_View].rows);
}

LRESULT CTempDlgEx::OnFeatureDefault(WPARAM wapram, LPARAM lparam)
{
	CPrClock pr(__FUNCTIONW__);

	// 当前矩阵 = 区域矩阵
	// 视图矩阵 = 当前矩阵 * scale
	if (m_cMatrix[eMT_Range].empty()) return 0;

	OnSyncAlpha();

	OnClearTempParam();

	OnClearViewParam();

	UpdateData(FALSE);

	Invalidate();

	Sync2Parent();

	return 0;
}

void CTempDlgEx::BackupOrRestore(bool bBackup)
{
	static double dAngle = 0.0;

	static int ox = 0, oy = 0, cx = 0, cy = 0;

	CPrClock pr(__FUNCTIONW__);

	ox = (bBackup ? m_nOx[ePP_Range2Temp] : ox);
	oy = (bBackup ? m_nOy[ePP_Range2Temp] : oy);
	cx = (bBackup ? m_nCx[ePP_Range2Temp] : cx);
	cy = (bBackup ? m_nCy[ePP_Range2Temp] : cy);
	dAngle = (bBackup ? m_dAngle[ePP_Range2Temp] : dAngle);

	m_nOx[ePP_Range2Temp] = ox;
	m_nOy[ePP_Range2Temp] = oy;
	m_nCx[ePP_Range2Temp] = cx;
	m_nCy[ePP_Range2Temp] = cy;
	m_dAngle[ePP_Range2Temp] = dAngle;
}

bool CTempDlgEx::IsValidInput()
{
	CPrClock pr(__FUNCTIONW__);

	return ((0 <= m_nOx[ePP_Range2Temp]) &&
	(0 <= m_nOy[ePP_Range2Temp]) &&
	(m_nCx[ePP_Range2Temp] <= m_cMatrix[eMT_Range].cols) &&
	(m_nCy[ePP_Range2Temp] <= m_cMatrix[eMT_Range].rows));
}

LRESULT CTempDlgEx::OnFeatureParam(WPARAM wapram, LPARAM lparam)
{
	CPrClock pr(__FUNCTIONW__);

	auto& iSrc = m_cMatrix[eMT_Src],
		& iRange = m_cMatrix[eMT_Range],
		& iCur = m_cMatrix[eMT_Temp],
		& iView = m_cMatrix[eMT_View],
		& iCache = m_cMatrix[eMT_Cache];

	// 记录当前参数，失败则恢复参数
	BackupOrRestore(true);

	UpdateData(TRUE);

	if (!IsValidInput()) { BackupOrRestore(false); return 0; }

	FeatureFrame vFrame{ 
		m_nOx[ePP_Range2Temp], 
		m_nOy[ePP_Range2Temp], 
		m_nOx[ePP_Range2Temp] + m_nCx[ePP_Range2Temp],
		m_nOy[ePP_Range2Temp] + m_nCy[ePP_Range2Temp] };

	OnSyncAlpha();
	Range2Cur(vFrame);
	Cur2View(true);

	UpdateData(FALSE);
	Invalidate();
	Sync2Parent();

	return 0;
}

LRESULT CTempDlgEx::OnFeatureZoom(WPARAM wapram, LPARAM lparam)
{
	CPrClock pr(__FUNCTIONW__);

	// 视图放大
	// 当前矩阵 = (区域矩阵 + os) * a
	// 视图矩阵 = 当前矩阵 * scale

	// 由此逆推算
	// 窗口pos -> 特征提取控件pos 
	// 控件pos -> 视图pos
	// 视图pos -> 当前pos
	// 当前pos -> 区域pos
	// 区域pos -> 原始pos
	FeatureFrame* lpParam = (FeatureFrame*)lparam;
	FeatureFrame vFrame = (*lpParam);
	delete lpParam;

	auto& l = std::get<eFFT_l>(vFrame),
		& t = std::get<eFFT_t>(vFrame),
		& r = std::get<eFFT_r>(vFrame),
		& b = std::get<eFFT_b>(vFrame);

	auto& iSrc = m_cMatrix[eMT_Src],
		& iRange = m_cMatrix[eMT_Range],
		& iCur = m_cMatrix[eMT_Temp],
		& iView = m_cMatrix[eMT_View],
		& iCache = m_cMatrix[eMT_Cache];

	Canvas2FeatureControl(vFrame);
	FeatureControl2View(vFrame);
	l = std::max(0, l); t = std::max(0, t); r = std::min(iView.cols, r); b = std::min(iView.rows, b);
	cv::Mat cViewTmp = iView.clone();
	cv::rectangle(cViewTmp, cv::Point(l, t), cv::Point(r, b), cv::Scalar(0xff));

	View2Cur(vFrame);
	cv::Mat cCurTmp = iCur.clone();
	cv::rectangle(cCurTmp, cv::Point(l, t), cv::Point(r, b), cv::Scalar(0xff));

	Cur2Range(vFrame);
	FeatureFrame vCur2Range = vFrame;
	cv::Mat cRangeTmp = iRange.clone();
	cv::rectangle(cRangeTmp, cv::Point(l, t), cv::Point(r, b), cv::Scalar(0xff));

	OnSyncAlpha();
	Range2Cur(vCur2Range);
	Cur2View(false);

	UpdateData(FALSE);
	Invalidate();
	Sync2Parent();

	return 0;
}

void CTempDlgEx::Range2Cur(FeatureFrame& vFrame)
{
	CPrClock pr(__FUNCTIONW__);

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

	m_cMatrix[eMT_Temp] = RotateImg(
		m_cMatrix[eMT_Range](rtRange),
		cv::Point(w * 0.5, h * 0.5),
		m_dAngle[ePP_Range2Temp],
		1.0);
}

void CTempDlgEx::Cur2View(bool bScale)
{
	CPrClock pr(__FUNCTIONW__);

	double w = m_rtFeature[eRT_Feature].Width();

	double h = m_rtFeature[eRT_Feature].Height();

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

cv::Point2i CTempDlgEx::Contrarotation(
	cv::Point2i ptCur,
	cv::Point2d ptCenter,
	float dAngle,
	int nImHeight)
{
	CPrClock pr(__FUNCTIONW__);

	int x1 = ptCur.x;

	int y1 = nImHeight - ptCur.y;

	int x2 = ptCenter.x;

	int y2 = nImHeight - ptCenter.y;

	int x =
		cvRound((x1 - x2) * cos(CV_PI / 180.0 * dAngle) -
			(y1 - y2) * sin(CV_PI / 180.0 * dAngle) + x2);
	int y =
		cvRound((x1 - x2) * sin(CV_PI / 180.0 * dAngle) +
			(y1 - y2) * cos(CV_PI / 180.0 * dAngle) + y2);
	y = nImHeight - y;

	return cv::Point2i(x, y);
}

std::vector<cv::Point2i> CTempDlgEx::Contrarotation(
	std::vector<cv::Point2i> vCur,
	cv::Point2d ptCenter,
	float dAngle,
	int nImHeight)
{
	CPrClock pr(__FUNCTIONW__);

	std::vector<cv::Point2i> vCont(vCur.size());

	for (int i = 0; i < vCur.size(); ++i)
	{
		vCont[i] = Contrarotation(vCur[i], ptCenter, dAngle, nImHeight);
	}
	return vCont;
}

void CTempDlgEx::Canvas2FeatureControl(FeatureFrame& vFrame)
{
	CPrClock pr(__FUNCTIONW__);

	auto& l = std::get<eFFT_l>(vFrame),
		& t = std::get<eFFT_t>(vFrame),
		& r = std::get<eFFT_r>(vFrame),
		& b = std::get<eFFT_b>(vFrame);

	auto& iFeature = m_rtFeature[eRT_Feature];

	l -= iFeature.left; r -= iFeature.left;

	t -= iFeature.top; b -= iFeature.top;
}

void CTempDlgEx::FeatureControl2View(FeatureFrame & vFrame)
{
	CPrClock pr(__FUNCTIONW__);

	auto& l = std::get<eFFT_l>(vFrame),
		& t = std::get<eFFT_t>(vFrame),
		& r = std::get<eFFT_r>(vFrame),
		& b = std::get<eFFT_b>(vFrame);

	auto& iView = m_cMatrix[eMT_View];

	const auto& iFeature = m_rtFeature[eRT_Feature];

	int ox = (iFeature.Width() - iView.cols) * 0.5;

	int oy = (iFeature.Height() - iView.rows) * 0.5;

	l -= ox; r -= ox;

	t -= oy; b -= oy;
}

void CTempDlgEx::View2Cur(FeatureFrame& vFrame)
{
	CPrClock pr(__FUNCTIONW__);

	auto& l = std::get<eFFT_l>(vFrame),
		& t = std::get<eFFT_t>(vFrame),
		& r = std::get<eFFT_r>(vFrame),
		& b = std::get<eFFT_b>(vFrame);

	l = (double)l / m_dScale[ePP_View];

	t = (double)t / m_dScale[ePP_View];

	r = (double)r / m_dScale[ePP_View];

	b = (double)b / m_dScale[ePP_View];
}

void CTempDlgEx::Cur2Range(FeatureFrame& vCur)
{
	CPrClock pr(__FUNCTIONW__);

	Contrarotation(
		vCur, 
		m_cMatrix[eMT_Temp], 
		m_cMatrix[eMT_Cache], 
		m_dAngle[ePP_Range2Temp]);

	Contrarotation(
		vCur, m_cMatrix[eMT_Cache], 
		m_nOx[ePP_Range2Temp], m_nOy[ePP_Range2Temp],
		m_nCx[ePP_Range2Temp], m_nCy[ePP_Range2Temp]);
}

void CTempDlgEx::Contrarotation(
	FeatureFrame& vFrame,
	cv::Mat& vCon, 
	int& ox, 
	int& oy, 
	int& cx, 
	int& cy)
{
	CPrClock pr(__FUNCTIONW__);

	auto& l = std::get<eFFT_l>(vFrame),
		& t = std::get<eFFT_t>(vFrame),
		& r = std::get<eFFT_r>(vFrame),
		& b = std::get<eFFT_b>(vFrame);

	int Cox = (vCon.cols - cx) * 0.5;

	int Coy = (vCon.rows - cy) * 0.5;

	l -= Cox; r -= Cox;

	t -= Coy; b -= Coy; // 填充抵消

	l += ox; r += ox;

	t += oy; b += oy; // 当前偏执
}

void CTempDlgEx::Contrarotation(
	FeatureFrame& vFrame, 
	cv::Mat& vRot, 
	cv::Mat& vCon, 
	double& dAngle)
{
	CPrClock pr(__FUNCTIONW__);

	auto& l = std::get<eFFT_l>(vFrame),
		& t = std::get<eFFT_t>(vFrame),
		& r = std::get<eFFT_r>(vFrame),
		& b = std::get<eFFT_b>(vFrame);

	cv::Point2d ptCenter;

	ptCenter.x = (double)vRot.cols * 0.5;

	ptCenter.y = (double)vRot.rows * 0.5;

	vCon = (dAngle != 0.0 ? RotateImg(vRot, ptCenter, -dAngle, 1.0) : vRot);

	std::vector<cv::Point2i> vPeak
	{
		cv::Point2i(l, t),
		cv::Point2i(r, t),
		cv::Point2i(l, b),
		cv::Point2i(r, b)
	};

	vPeak = Contrarotation(vPeak, ptCenter, -dAngle, vCon.rows);

	int ox = (vCon.cols - vRot.cols) * 0.5;

	int oy = (vCon.rows - vRot.rows) * 0.5;

	int x[4] = { vPeak[0].x, vPeak[1].x, vPeak[2].x, vPeak[3].x };

	int y[4] = { vPeak[0].y, vPeak[1].y, vPeak[2].y, vPeak[3].y };

	l = (*std::min_element(x, x + 4));	r = (*std::max_element(x, x + 4));

	t = (*std::min_element(y, y + 4));	b = (*std::max_element(y, y + 4));

	l += ox; r += ox; t += oy; b += oy;

	cv::Mat cConTmp = vCon.clone();

	cv::rectangle(cConTmp, cv::Point(l, t), cv::Point(r, b), cv::Scalar(0xff));

	int* lp = new int();
}

LRESULT CTempDlgEx::OnFeatureFrame(WPARAM wapram, LPARAM lparam)
{
	CPrClock pr(__FUNCTIONW__);

	FeatureFrame* lpParam = (FeatureFrame*)lparam;

	m_vFeatureFrame = (*lpParam);

	delete lpParam;

	Invalidate();

	return 0;
}

BOOL CTempDlgEx::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN)
	{
		if (pMsg->wParam == VK_RETURN)
		{
			int nFocusControl = GetFocus()->GetDlgCtrlID();

			if ((nFocusControl != IDC_FEATURE_ET_X) &&
				(nFocusControl != IDC_FEATURE_ET_Y) &&
				(nFocusControl != IDC_FEATURE_ET_W) &&
				(nFocusControl != IDC_FEATURE_ET_H) &&
				(nFocusControl != IDC_FEATURE_ET_A) &&
				(nFocusControl != IDC_FEATURE_ET_S) &&
				(nFocusControl != IDC_ET_ALPHA)) return TRUE;

			PostMessage(PQM_FEATURE_RESET_PARAM, 0, 0);

			return TRUE;
		}
	}
	return CTempDlg::PreTranslateMessage(pMsg);
}

void CTempDlgEx::OnDraw(Gdiplus::Graphics& graph)
{
	CPrClock pr(__FUNCTIONW__);

	if (!m_bLbdown) return; // 绘制选取区域

	const auto& l = std::get<eFFT_l>(m_vFeatureFrame);

	const auto& t = std::get<eFFT_t>(m_vFeatureFrame);

	const auto& r = std::get<eFFT_r>(m_vFeatureFrame);

	const auto& b = std::get<eFFT_b>(m_vFeatureFrame);

	Gdiplus::PointF ptOpt[5] =
	{
		{(float)l, (float)t},
		{(float)r, (float)t},
		{(float)r, (float)b},
		{(float)l, (float)b},
		{(float)l, (float)t}
	};

	Gdiplus::Pen pen(Gdiplus::Color::Black, 1.0f);

	graph.DrawLines(&pen, ptOpt, 5);
}

void CTempDlgEx::OnDrawFrame(unsigned int* lpPixel, int nHight, int nWidth)
{
	CPrClock pr(__FUNCTIONW__);

	// 绘制对话框边框
	POINT ptBottomRight{ nWidth - 1, nHight - 1 };

	for (int x = 0; x < nWidth; ++x)
	{
		int nTop = 0 * nWidth + x;

		int nBottom = ptBottomRight.y * nWidth + x;

		*(lpPixel + nTop) = 0x80000000;

		*(lpPixel + nBottom) = 0x80000000;
	}

	for (int y = 0; y < nHight; ++y)
	{
		int nLeft = y * nWidth + 0;

		int nRight = y * nWidth + ptBottomRight.x;

		*(lpPixel + nLeft) = 0x80000000;

		*(lpPixel + nRight) = 0x80000000;
	}

	std::vector<POINT> vPt
	{
		{2, 1},
		{3, 1},
		{1, 2},
		{1, 3},
		{ptBottomRight.x - 2, 1},
		{ptBottomRight.x - 3, 1},
		{ptBottomRight.x - 1, 2},
		{ptBottomRight.x - 1, 3},
	};
	for (auto& iPt : vPt)
	{
		*(lpPixel + iPt.y * nWidth + iPt.x) = 0x80000000;
	}
}

void CTempDlgEx::OnDraw(unsigned char* lpPixel, int nHight, int nWidth)
{
	CPrClock pr(__FUNCTIONW__);

	OnDrawFrame((unsigned int*)lpPixel, nHight, nWidth);

	// 绘制视图矩阵
	if (m_cMatrix[eMT_View].empty()) return;

	const auto& iFeature = m_rtFeature[eRT_Feature];

	auto iLeft = iFeature.left, iTop = iFeature.top;

	auto iHeight = iFeature.Height(), iWidth = iFeature.Width();

	for (int y = iTop; y < iFeature.bottom; ++y)
	{
		for (int x = iLeft; x < iFeature.right; ++x)
		{
			int nImageIndex = (y * nWidth + x) * 4;

			lpPixel[nImageIndex + 3] = 0x00;
		}
	} // 置空

	const auto& iView = m_cMatrix[eMT_View];

	int ox = (iWidth - iView.cols) * 0.5;

	int oy = (iHeight - iView.rows) * 0.5;

	for (int y = 0; y < iView.rows; ++y)
	{
		for (int x = 0; x < iView.cols; ++x)
		{
			int nOptIndex = (y * iView.cols + x) * 4; // bgra

			int nImageIndex = ((y + oy + iTop) * nWidth + x + ox + iLeft) * 4;

			lpPixel[nImageIndex + 0] = iView.data[nOptIndex + 0]; // rgb

			lpPixel[nImageIndex + 1] = iView.data[nOptIndex + 1];

			lpPixel[nImageIndex + 2] = iView.data[nOptIndex + 2];

			lpPixel[nImageIndex + 3] = iView.data[nOptIndex + 3];
		}
	}
}

void CTempDlgEx::OnSyncAlpha()
{
	CPrClock pr(__FUNCTIONW__);

	// 时间消耗过大，是否弃用?
	//// 同步透明通道
	if (m_cMatrix[eMT_Src].empty()) return;

	for (unsigned long long y = 0; y < m_cMatrix[eMT_Src].rows; ++y)
	{
		for (unsigned long long x = 0; x < m_cMatrix[eMT_Src].cols; ++x)
		{
			unsigned long long nOptIndex = (y * m_cMatrix[eMT_Src].cols + x) * 4; // bgra

			m_cMatrix[eMT_Src].data[nOptIndex + 3] = m_nAlpha[ePP_View];
		}
	}
}

cv::Mat CTempDlgEx::RotateImg(cv::Mat src, cv::Point center, float angle, double scale)
{
	CPrClock pr(__FUNCTIONW__);

	if (angle == 0.0) return src.clone();

	cv::Mat dst;

	int height = src.rows;

	int width = src.cols;

	cv::Mat affine_matrix = getRotationMatrix2D(center, angle/*angle / CV_PI * 180.0*/, scale);

	float a[12] = { 0,width,0,width,0,0,height,height,1,1,1,1 };

	cv::Mat A(3, 4, CV_32FC1, a);

	float b[9] = { 
		affine_matrix.at<double>(0,0),affine_matrix.at<double>(0,1),affine_matrix.at<double>(0,2),
		affine_matrix.at<double>(1,0),affine_matrix.at<double>(1,1),affine_matrix.at<double>(1,2),
		0,0,1 };

	cv::Mat B(3, 3, CV_32FC1, b);

	cv::Mat C = B * A;
	int MinWidth = cv::min(cv::min(C.at<float>(0, 0), C.at<float>(0, 1)), cv::min(C.at<float>(0, 2), C.at<float>(0, 3)));
	int MaxWidth = cv::max(cv::max(C.at<float>(0, 0), C.at<float>(0, 1)), cv::max(C.at<float>(0, 2), C.at<float>(0, 3)));
	int MinHeight = cv::min(cv::min(C.at<float>(1, 0), C.at<float>(1, 1)), cv::min(C.at<float>(1, 2), C.at<float>(1, 3)));
	int MaxHeight = cv::max(cv::max(C.at<float>(1, 0), C.at<float>(1, 1)), cv::max(C.at<float>(1, 2), C.at<float>(1, 3)));


	int dxLeft, dxRight, dyTop, dyBottom;
	int Cutx, Cuty;

	if (MinWidth <= 0)
	{
		dxLeft = -MinWidth;
		affine_matrix.at<double>(0, 2) += dxLeft;
		Cutx = 0;
	}
	else
	{
		dxLeft = 0;
		Cutx = MinWidth;
	}

	if (MinHeight <= 0)
	{
		dyTop = -MinHeight;
		affine_matrix.at<double>(1, 2) += dyTop;
		Cuty = 0;
	}
	else
	{
		dyTop = 0;
		Cuty = MinHeight;
	}

	if (MaxWidth - width >= 0)
	{
		dxRight = MaxWidth - width;
	}
	else
	{
		dxRight = 0;
	}

	if (MaxHeight - height >= 0)
	{
		dyBottom = MaxHeight - height;
	}
	else
	{
		dyBottom = 0;
	}

	copyMakeBorder(src, dst, dyTop, dyBottom, dxLeft, dxRight, cv::BORDER_CONSTANT);

	warpAffine(src, dst, affine_matrix, dst.size(), 1, 0, cv::Scalar(0xff, 0xff, 0xff, 0x00));

	cv::Rect rect(Cutx, Cuty, MaxWidth - MinWidth, MaxHeight - MinHeight);

	dst = cv::Mat(dst, rect);

	return dst;
}