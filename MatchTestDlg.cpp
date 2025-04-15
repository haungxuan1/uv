// MatchTestDlg.cpp: 实现文件
//

#include "pch.h"
#include "VisionLocalization.h"
#include "MatchTestDlg.h"
#include "afxdialogex.h"
#include "FastMatch.h"
#include "Transform.h"
#include "tiffio.h"
#include "opencv2/highgui/highgui_c.h"

using namespace cv;
using namespace Transform;

// MatchTestDlg 对话框

IMPLEMENT_DYNAMIC(MatchTestDlg, CDialog)

MatchTestDlg::MatchTestDlg(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_DLG_MATCHTEST, pParent)
{

}

MatchTestDlg::~MatchTestDlg()
{
}

void MatchTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Text(pDX, IDC_ET_MATCH_TIFF, m_rTIFF);

	DDX_Text(pDX, IDC_ET_MATCH_TEMP, m_rTemp);

	DDX_Text(pDX, IDC_ET_MATCH_MASK, m_rMask);

	DDX_Text(pDX, IDC_ET_MATCH_SAMP, m_rSamp);

}

BOOL MatchTestDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

/*	cv::namedWindow("MatchTestImage", WINDOW_AUTOSIZE);
	HWND hWnd = (HWND)cvGetWindowHandle("MatchTestImage");
	HWND hParent = ::GetParent(hWnd);
	::SetParent(hWnd, GetDlgItem(IDC_STATIC_SHOW)->m_hWnd);
	::ShowWindow(hParent, SW_HIDE);
*/	
	return true;
}


BEGIN_MESSAGE_MAP(MatchTestDlg, CDialog)
	ON_BN_CLICKED(IDC_BT_FEATURE_TIFF, &MatchTestDlg::OnBnClickedBtFeatureTiff)
	ON_BN_CLICKED(IDC_BT_FEATURE_LOADTEMP, &MatchTestDlg::OnBnClickedBtFeatureLoadtemp)
	ON_BN_CLICKED(IDC_BT_FEATURE_LOADMASK, &MatchTestDlg::OnBnClickedBtFeatureLoadmask)
	ON_BN_CLICKED(IDC_BT_FEATURE_SAMP, &MatchTestDlg::OnBnClickedBtFeatureSamp)
	ON_BN_CLICKED(IDC_BT_FEATURE_TEST, &MatchTestDlg::OnBnClickedBtFeatureTest)
END_MESSAGE_MAP()


// MatchTestDlg 消息处理程序


void MatchTestDlg::OnBnClickedBtFeatureTiff()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);

	CFileDialog dlg(TRUE, NULL, L"F:\\f\\VisionLocalization\\x64\\Release\\temp\\", OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, L"Print (*.tif)|*.tif||");

	if (dlg.DoModal() != IDOK) return;

	m_rTIFF = dlg.GetPathName();

	UpdateData(FALSE);

}


void MatchTestDlg::OnBnClickedBtFeatureLoadtemp()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);

	CFileDialog dlg(TRUE, NULL, L"F:\\f\\VisionLocalization\\x64\\Release\\param\\", OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, L"camera param (*.png)|*.png||");

	if (dlg.DoModal() != IDOK) return;

	m_rTemp = dlg.GetPathName();

	UpdateData(FALSE);

}


void MatchTestDlg::OnBnClickedBtFeatureLoadmask()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);

	CFileDialog dlg(TRUE, NULL, L"F:\\f\\VisionLocalization\\x64\\Release\\param\\", OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, L"camera param (*.png)|*.png||");

	if (dlg.DoModal() != IDOK) return;

	m_rMask = dlg.GetPathName();

	UpdateData(FALSE);

}


void MatchTestDlg::OnBnClickedBtFeatureSamp()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);

	CFileDialog dlg(TRUE, NULL, L"F:\\f\\VisionLocalization\\x64\\Release\\param\\", OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, L"camera param (*.png)|*.png||");

	if (dlg.DoModal() != IDOK) return;

	m_rSamp = dlg.GetPathName();

	UpdateData(FALSE);

}


void MatchTestDlg::OnBnClickedBtFeatureTest()
{
	// TODO: 在此添加控件通知处理程序代码
//	if (!m_rTIFF.IsEmpty() && !m_rTemp.IsEmpty() && !m_rSamp.IsEmpty())
	{

//		Mat mm = imread("Pic_temp.png");
//		imshow("MatchTestImage", mm);
		Mat cTmp = imread(CString2String(m_rTemp), 0);
		Mat cMask = imread(CString2String(m_rMask), 0);
		Mat cSamp = imread(CString2String(m_rSamp), 0);
		TIFF* cMold = TIFFOpen(CString2String(m_rTIFF).c_str(), "rb");
		if (!cTmp.empty() && !cSamp.empty())
		{
			Mat Result;
			MatchCheck(Result, cSamp, cTmp, cMask, cMold);
//			CRect rect;
//			GetDlgItem(IDC_STATIC_SHOW)->GetClientRect(&rect);
//			Mat ret;
//			resize(Result, ret, cv::Size(rect.Width(), rect.Height()));
			imshow("MatchTestImage", Result);
		}
	}
}
