// MatchTestViewDlg.cpp: 实现文件
//

#include "pch.h"
#include "VisionLocalization.h"
#include "afxdialogex.h"
#include "MatchTestViewDlg.h"
#include "Transform.h"
#include "opencv2/highgui/highgui_c.h"
//#include "FastMatch.h"

using namespace std;

using namespace Transform;

// MatchTestViewDlg 对话框

IMPLEMENT_DYNAMIC(MatchTestViewDlg, CDialog)

MatchTestViewDlg::MatchTestViewDlg(CWnd* pParent /*= nullptr*/, Mat* mat)
	: CDialog(IDD_MATCHVIEW_DIALOG, pParent)
	, m_rImage(mat)
{

}

MatchTestViewDlg::~MatchTestViewDlg()
{
}

void MatchTestViewDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BOOL MatchTestViewDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	namedWindow("MatchTestImage");//创建OpenCV窗口

	HWND hWnd = (HWND)cvGetWindowHandle("MatchTestImage");//嵌套opencv窗口

	HWND hParent = ::GetParent(hWnd);

	::SetParent(hWnd, GetDlgItem(IDC_PIC_IMAGE)->m_hWnd);

	::ShowWindow(hParent, SW_HIDE);

	Mat readImage;

	CRect rect;

	GetDlgItem(IDC_PIC_IMAGE)->GetClientRect(&rect);

	resize(*m_rImage, readImage, Size(rect.Width(), rect.Height()));

	imshow("MatchTestImage", readImage);

/*
	Mat cTmp = imread(CString2String(m_rTemp), 0);

	Mat readImage;

	CRect rect;

	GetDlgItem(IDC_PIC_IMAGE)->GetClientRect(&rect);

	resize(cTmp, readImage, Size(rect.Width(), rect.Height()));

	imshow("MatchTestImage", readImage);
	Mat cMask = imread(CString2String(m_rMask), 0);

	Mat cSamp = imread(CString2String(m_rSam), 0);

	TIFF* cMold = TIFFOpen(CString2String(m_rTIFF).c_str(), "rb");

	if (!cTmp.empty() && !cSamp.empty())
	{
		Mat Result;

		MatchCheck(Result, cSamp, cTmp, cMask, cMold);

		if (!Result.empty()) {
			Mat readImage;

			CRect rect;

			GetDlgItem(IDC_PIC_IMAGE)->GetClientRect(&rect);

			resize(Result, readImage, Size(rect.Width(), rect.Height()));

			imshow("MatchTestImage", readImage);
		}
		else AfxMessageBox(L"生成失败");
	}
*/	
	return TRUE;
}


BEGIN_MESSAGE_MAP(MatchTestViewDlg, CDialog)
END_MESSAGE_MAP()


// MatchTestViewDlg 消息处理程序
