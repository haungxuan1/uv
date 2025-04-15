#include "pch.h"
#include "MoldDlg.h"
#include "PQSetting.h"
#include "Transform.h"
#include "PQMsg.h"

IMPLEMENT_DYNAMIC(CMoldDlg, CDialog)

BEGIN_MESSAGE_MAP(CMoldDlg, CDialog)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_MOLD_LOAD, OnClickedLoad)
	ON_MESSAGE(PQM_MOLD_PARAM, OnMoldParam)
	ON_MESSAGE(PQM_FEATURE_PARAM, OnFeatureParam)
	ON_MESSAGE(PQM_FEATURE_OK, OnFeatureOk)
	ON_MESSAGE(PQM_TEMP_MATCHED, OnTempMatched)
	ON_MESSAGE(PQM_FEATURE_SYNC_OS_PARAM, OnSyncOsParam)
	ON_BN_CLICKED(IDC_MOLD_BT_RESET, OnClickedReset)
	ON_BN_CLICKED(IDC_MOLD_BT_SAVE_RANGE, OnClickedRange)
	ON_BN_CLICKED(IDC_MOLD_BT_SAVE, OnClickedSave)
	ON_BN_CLICKED(IDC_MOLD_BT_RECOVER, OnClickedRecover)
END_MESSAGE_MAP()

CMoldDlg::CMoldDlg(CWnd* pParent) 
	: CDialog(CMoldDlg::IDD, pParent), 
	m_nOx{ 0, 0 }, m_nOy{ 0, 0 }, m_nCx(0), m_nCy(0), m_dScale(0.0), m_dSame(0.0)
{
	m_lpFeature = new Mat();
}

CMoldDlg::~CMoldDlg()
{
	delete m_lpFeature;
}

void CMoldDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_MOLD_ST_MOLD, m_stMold);

	DDX_Control(pDX, IDC_MOLD_ST_CONTRAST, m_stMatching);

	DDX_Text(pDX, IDC_MOLD_ET_X, m_nOx[0]);

	DDX_Text(pDX, IDC_MOLD_ET_Y, m_nOy[0]);
	
	DDX_Text(pDX, IDC_MOLD_ET_W, m_nCx);
	
	DDX_Text(pDX, IDC_MOLD_ET_H, m_nCy);
	
	DDX_Text(pDX, IDC_MOLD_ET_S, m_dScale);

	DDX_Text(pDX, IDC_TEMP_ET_X, m_nOx[1]);

	DDX_Text(pDX, IDC_TEMP_ET_Y, m_nOy[1]);

	DDX_Text(pDX, IDC_ET_TEMP_SAME, m_dSame);
}

void CMoldDlg::OnSyncFeature(cv::Mat* lpMatrix)
{
	Feature(*lpMatrix, *m_lpFeature);
}

BOOL CMoldDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	CRect rtClient;

	m_stMold.GetClientRect(rtClient);

	m_stMold.InitCanvas(&m_stMold, rtClient);

	m_stMatching.GetClientRect(rtClient);

	m_stMatching.InitCanvas(&m_stMatching, rtClient);

	m_cFeatureDlg.Create(CTempDlgEx::IDD, this);

	m_cFeatureDlg.OnSyncFeature(*m_lpFeature);

	m_cFeatureDlg.ShowWindow(SW_SHOW);

	return TRUE;
}

LRESULT CMoldDlg::OnSyncOsParam(WPARAM wparam, LPARAM lparam)
{
	// 计算相对位置
	CRect rtMold, rtTemp;

	::GetWindowRect(m_stMold.GetSafeHwnd(), rtMold);

	::GetWindowRect(m_cFeatureDlg.GetSafeHwnd(), rtTemp);

	int ox1 = 0, oy1 = 0;

	int ox2 = 0, oy2 = 0;

	m_stMold.GetViewOs(ox1, oy1);

	m_cFeatureDlg.GetViewOs(ox2, oy2);

	ox1 += rtMold.left; oy1+= rtMold.top;

	ox2 += rtTemp.left, oy2 += rtTemp.top; // 视图坐标转屏幕坐标

	m_nOx[1] = ox2 - ox1, m_nOy[1] = oy1 - oy2; 

	UpdateData(FALSE);

	OnSyncMatching();

	return 0;
}

void CMoldDlg::OnClickedReset()
{
	m_stMold.OnReset();
}

void CMoldDlg::OnClickedRange()
{
	m_stMold.OnRange();
}

void CMoldDlg::OnClickedSave()
{
	m_stMold.OnSave();
}

void CMoldDlg::OnClickedRecover()
{
	m_stMold.OnRecover();
}

LRESULT CMoldDlg::OnFeatureParam(WPARAM wparam, LPARAM lparam)
{
	OnSyncMatching();

	return 0;
}

LRESULT CMoldDlg::OnTempMatched(WPARAM wparam, LPARAM lparam)
{
	double* lpSame = (double*)wparam;

	std::tuple<int, int>* lpParam = (std::tuple<int, int>*)lparam;

	m_nOx[1] = std::get<0>(*lpParam);

	m_nOy[1] = std::get<1>(*lpParam);

	m_dSame = (*lpSame);

	UpdateData(FALSE);

	delete lpSame; delete lpParam;

	return 0;
}

LRESULT CMoldDlg::OnFeatureOk(WPARAM wparam, LPARAM lparam)
{
	std::wstring strMold;

	int x = 0, y = 0;

	cv::Mat *lpTemp = new cv::Mat(); 

	m_stMatching.GetTempParam(*lpTemp, x, y);

	if (!m_stMold.GetMoldParam(x, y, strMold, *lpTemp)) return 0;

	CWnd* lpParent = GetParent();

	::PostMessage(lpParent->GetSafeHwnd(), 
		PQM_FEATURE_OK,
		0,
		(LPARAM)new std::tuple<int, int, std::wstring, cv::Mat*>(x, y, strMold, lpTemp));

	PostMessage(WM_CLOSE);

	return 0;
}

void CMoldDlg::OnClose()
{
	CWnd *lpParent = GetParent();

	lpParent->ShowWindow(SW_SHOWNORMAL);

	CDialog::OnClose();
}

void CMoldDlg::OnClickedLoad()
{
	CFileDialog dlg(
		TRUE,
		NULL,
		PQSetting::MoldDir().c_str(),
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		L"tif (*.tif)|*.tif||");

	if (dlg.DoModal() != IDOK) return;

	m_stMold.OnSyncMold(dlg.GetPathName().GetBuffer());
}

void CMoldDlg::OnSyncMatching()
{
	// 传递给匹配控件
	std::tuple<int, int>* lpParam = new std::tuple<int, int>(m_nOx[1], m_nOy[1]);

	// 这里由于模板匹配dll仅能识别原图，无法支持放大，因此采取原图备份
	std::tuple<
		std::tuple<cv::Mat*, cv::Mat*>*,
		std::tuple<cv::Mat*, cv::Mat*>*>* lpMatrix = new std::tuple<
		std::tuple<cv::Mat*, cv::Mat*>*,
		std::tuple<cv::Mat*, cv::Mat*>*>(m_stMold.GetView(), m_cFeatureDlg.GetView());

	::PostMessage(m_stMatching.GetSafeHwnd(),
		PQM_FEATURE_RESET_PARAM,
		(WPARAM)lpMatrix,
		(LPARAM)lpParam);
}

LRESULT CMoldDlg::OnMoldParam(WPARAM wparam, LPARAM lparam)
{
	std::tuple<int, int, int, int, double>* lpParam = 
		(std::tuple<int, int, int, int, double>*)lparam;

	m_nOx[0] = std::get<0>(*lpParam);

	m_nOy[0] = std::get<1>(*lpParam);

	m_nCx = std::get<2>(*lpParam);

	m_nCy = std::get<3>(*lpParam);

	m_dScale = std::get<4>(*lpParam);

	OnSyncMatching();

	UpdateData(FALSE);

	delete lpParam;

	return 0;
}

BOOL CMoldDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN)
	{
		if (pMsg->wParam == VK_RETURN)
		{
			int nFocusControl = GetFocus()->GetDlgCtrlID();

			if ((nFocusControl != IDC_MOLD_ET_X) &&
				(nFocusControl != IDC_MOLD_ET_Y) &&
				(nFocusControl != IDC_MOLD_ET_W) &&
				(nFocusControl != IDC_MOLD_ET_H) &&
				(nFocusControl != IDC_MOLD_ET_S) &&
				(nFocusControl != IDC_TEMP_ET_X) &&
				(nFocusControl != IDC_TEMP_ET_Y)) return TRUE;

			UpdateData(TRUE);

			OnMoldParamChange();

			OnTempParamChange();

			return TRUE;
		}
	}
	return CDialog::PreTranslateMessage(pMsg);
}

void CMoldDlg::OnMoldParamChange()
{
	int nFocusControl = GetFocus()->GetDlgCtrlID();

	if ((nFocusControl != IDC_MOLD_ET_X) &&
		(nFocusControl != IDC_MOLD_ET_Y) &&
		(nFocusControl != IDC_MOLD_ET_W) &&
		(nFocusControl != IDC_MOLD_ET_H) &&
		(nFocusControl != IDC_MOLD_ET_S)) return;

	std::tuple<int, int, int, int, double>* lpParam = new 
		std::tuple<int, int, int, int, double>(
			m_nOx[0], m_nOy[0], m_nCx, m_nCy, m_dScale);

	::PostMessage(m_stMold.GetSafeHwnd(), PQM_FEATURE_RESET_PARAM, 0, (LPARAM)lpParam);
}

void CMoldDlg::OnTempParamChange()
{
	int nFocusControl = GetFocus()->GetDlgCtrlID();

	if ((nFocusControl != IDC_TEMP_ET_X) &&
		(nFocusControl != IDC_TEMP_ET_Y)) return;

	OnSyncMatching();
}