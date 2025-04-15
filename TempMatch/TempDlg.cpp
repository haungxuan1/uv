#include "pch.h"
#include "TempDlg.h"
#include "PQMsg.h"

IMPLEMENT_DYNAMIC(CTempDlg, CGDIPlusDlg)

BEGIN_MESSAGE_MAP(CTempDlg, CGDIPlusDlg)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)
END_MESSAGE_MAP()

CTempDlg::CTempDlg(CWnd* pParent) 
	:CGDIPlusDlg(CTempDlg::IDD, pParent)
{
	m_bLbdown = false;

	m_bTrace = false;
}

CTempDlg::~CTempDlg()
{

}

void CTempDlg::DoDataExchange(CDataExchange* pDX)
{
	CGDIPlusDlg::DoDataExchange(pDX);

}

BOOL CTempDlg::OnInitDialog()
{
	CGDIPlusDlg::OnInitDialog();

	m_rtFeature[eRT_Frame] = CRect(30, 30, 515, 515);

	m_rtFeature[eRT_Feature] = CRect(40, 40, 505, 505); // 设置特征模块位置 size = 模具控件

	OnInitGDIPlusImage(m_rtFeature[eRT_Frame], m_rtFeature[eRT_Feature]);

	return TRUE;
}

void CTempDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	PtInRect(m_rtFeature[eRT_Feature], point) ? OnDragBegin(point) : OnDragMove(point);

	CDialog::OnLButtonDown(nFlags, point);
}

void CTempDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	if(m_bLbdown) OnMouseMove(point);

	if (m_bLbdown) OnDragEnd();

	if (m_bLbdown) { ReleaseCapture();}

	m_bLbdown = false;

	CDialog::OnLButtonUp(nFlags, point);
}

void CTempDlg::OnDragEnd()
{
	if (m_ptOperation[eOT_TopLeft] == m_ptOperation[eOT_BottomRight]) return;

	int nMsg = (m_ptOperation[eOT_Begin] != m_ptOperation[eOT_TopLeft] ?
		PQM_FEATURE_DEFAULT : PQM_FEATURE_ZOOM);

	const auto& l = m_ptOperation[eOT_TopLeft].x;

	const auto& t = m_ptOperation[eOT_TopLeft].y;

	const auto& r = m_ptOperation[eOT_BottomRight].x;

	const auto& b = m_ptOperation[eOT_BottomRight].y;

	PostMessage(nMsg, 0, (LPARAM)new FeatureFrame(l, t, r, b));
}

void CTempDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	if (m_bLbdown)
	{
		OnTrace(nFlags);

		OnMouseMove(point);

		const auto& l = m_ptOperation[eOT_TopLeft].x;

		const auto& t = m_ptOperation[eOT_TopLeft].y;

		const auto& r = m_ptOperation[eOT_BottomRight].x;

		const auto& b = m_ptOperation[eOT_BottomRight].y;

		PostMessage(PQM_FEATURE_FRAME_CHANGED, 0, (LPARAM)new FeatureFrame(l, t, r, b));
	}

	CDialog::OnMouseMove(nFlags, point);
}

LRESULT CTempDlg::OnMouseLeave(WPARAM wparam, LPARAM lparam)
{
	m_bTrace = false;

	return 0;
}

void CTempDlg::OnDragMove(const CPoint& point)
{
	::SendMessage(GetSafeHwnd(), WM_SYSCOMMAND, 0xF012, 0);   // 0xF012 = SC_DRAGMOVE
}

void CTempDlg::OnDragBegin(const CPoint& point)
{
	m_bLbdown = true;

	SetCapture(); // 设置鼠标捕捉 // lbuttonup 释放

	m_ptOperation[eOT_Begin] = point;

	m_ptOperation[eOT_TopLeft] = m_ptOperation[eOT_Begin];

	m_ptOperation[eOT_BottomRight] = m_ptOperation[eOT_Begin];
}

void CTempDlg::OnTrace(UINT nFlags)
{
	if (m_bTrace) return;

	if (nFlags == (-1)) return;

	m_bTrace = true;

	TRACKMOUSEEVENT tme;

	tme.cbSize = sizeof(tme);

	tme.hwndTrack = m_hWnd;

	tme.dwFlags = TME_LEAVE;

	tme.dwHoverTime = 1;

	m_bTrace = (_TrackMouseEvent(&tme) == TRUE);
}

void CTempDlg::OnMouseMove(CPoint point)
{
	// 起点
	m_ptOperation[eOT_TopLeft].x = std::min(m_ptOperation[eOT_Begin].x, point.x);

	m_ptOperation[eOT_TopLeft].y = std::min(m_ptOperation[eOT_Begin].y, point.y);

	m_ptOperation[eOT_BottomRight].x = std::max(m_ptOperation[eOT_Begin].x, point.x);

	m_ptOperation[eOT_BottomRight].y = std::max(m_ptOperation[eOT_Begin].y, point.y);

	// 特征控件
	m_ptOperation[eOT_TopLeft].x = std::max(m_rtFeature[eRT_Feature].left, m_ptOperation[eOT_TopLeft].x);

	m_ptOperation[eOT_TopLeft].y = std::max(m_rtFeature[eRT_Feature].top, m_ptOperation[eOT_TopLeft].y);

	m_ptOperation[eOT_BottomRight].x = std::min(m_rtFeature[eRT_Feature].right, m_ptOperation[eOT_BottomRight].x);

	m_ptOperation[eOT_BottomRight].y = std::min(m_rtFeature[eRT_Feature].bottom, m_ptOperation[eOT_BottomRight].y);
}

void CTempDlg::OnInitGDIPlusImage(const CRect& rtFrame, const CRect& rtFeature)
{
	Gdiplus::BitmapData data; POINT ptPixel;

	Gdiplus::Rect rc(0, 0, m_nWidth, m_nHeigh);

	m_lpImage->LockBits(&rc, Gdiplus::ImageLockModeWrite, m_lpImage->GetPixelFormat(), &data);

	unsigned char* lpPixel = (unsigned char*)data.Scan0;

	for (ptPixel.y = 0; ptPixel.y < m_nHeigh; ++ptPixel.y)
	{
		for (ptPixel.x = 0; ptPixel.x < m_nWidth; ++ptPixel.x)
		{
			int nIndex = (ptPixel.y * m_nWidth + ptPixel.x) * 4;

			for (int c = 0; c < 3; ++c) lpPixel[nIndex + c] = 0xf0;

			bool bFrame = PtInRect(m_rtFeature[eRT_Frame], ptPixel);

			bool bFeature = PtInRect(m_rtFeature[eRT_Feature], ptPixel);

			lpPixel[nIndex + 3] = (bFeature ? 0x80 : (bFrame ? 0x00 : 0xff));
		}
	}
	m_lpImage->UnlockBits(&data);
}

void CTempDlg::OnDraw(Gdiplus::Bitmap*& lpImage)
{
	Gdiplus::BitmapData data;

	Gdiplus::Rect rc(0, 0, m_nWidth, m_nHeigh);

	m_lpImage->LockBits(&rc, Gdiplus::ImageLockModeWrite, m_lpImage->GetPixelFormat(), &data);

	OnDraw((unsigned char*)data.Scan0, data.Height, data.Width);

	m_lpImage->UnlockBits(&data);
}