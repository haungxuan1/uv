#include "pch.h"
#include "ImImage.h"
#include "PQMsg.h"
#include "Transform.h"

namespace ImImage
{
	IMPLEMENT_DYNAMIC(CImImage, ImRender::CImRender)

	BEGIN_MESSAGE_MAP(CImImage, ImRender::CImRender)
		ON_WM_LBUTTONDOWN()
		ON_WM_LBUTTONUP()
		ON_WM_MOUSEMOVE()
		ON_WM_MOUSEWHEEL()
		ON_WM_SETCURSOR()
	END_MESSAGE_MAP()

	CImImage::CImImage() :
		m_nFocus(0),
		m_nOpt(ImLayer::tagLayer::eOT_Move),
		m_rtFocus(0,0,0,0),
		m_hCursor{nullptr, nullptr , nullptr , nullptr }
	{

	}

	CImImage::~CImImage()
	{

	}

	void CImImage::SetIcon(wstring strIconDir)
	{
		m_strIconDir = strIconDir;
	}

	void CImImage::OnSyncParam(ImLayer::tagLayer& tParam)
	{
		m_nFocus = tParam.nSign;

		m_nOpt = tParam.CurOpt();

		ClearFocusRect();
	}

	BOOL CImImage::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
	{
		::SetCursor(m_hCursor[m_nOpt]);

		return TRUE;
	}

	void CImImage::ClearFocusRect()
	{
		DrawFocusRect();

		m_rtFocus = CRect(0, 0, 0, 0);
	}

	void CImImage::SetFocusRect(CPoint& pt)
	{
		if (m_nOpt != ImLayer::tagLayer::eOT_Selecte) return;

		m_rtFocus.left = min(m_ptLBDown.x, pt.x);

		m_rtFocus.right = max(m_ptLBDown.x, pt.x);

		m_rtFocus.top = min(m_ptLBDown.y, pt.y);

		m_rtFocus.bottom = max(m_ptLBDown.y, pt.y);
	}

	void CImImage::DrawFocusRect()
	{
		if (m_rtFocus.IsRectEmpty()) return;

		CClientDC dc(this);

		dc.SetTextColor((COLORREF)0xff);

		dc.SetROP2(R2_NOTXORPEN);

		dc.DrawFocusRect(m_rtFocus);
	}

	// 上正下负
	BOOL CImImage::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
	{
		if (GetAsyncKeyState(VK_LBUTTON) & 0x8000) return TRUE; // 鼠标左键按下，则不响应

		if (GetAsyncKeyState(VK_MENU) & 0x8000) // alt 键同时被按下，则认为缩放被启动
		{
			ImCtrl::uWParam s;

			s.d = (zDelta / WHEEL_DELTA);
			
			s.d = abs(s.d) * (s.d < 0 ? 0.9 : 1.1); // 向上则放大，向下则缩小

			::PostMessage(GetParent()->GetSafeHwnd(), PQM_IM_VIEW_SCALE, (WPARAM)s.w, m_nFocus);
		}
		return TRUE;
	}

	BOOL CImImage::PreTranslateMessage(MSG* pMsg)
	{
		if (pMsg->message == WM_KEYDOWN)
		{
			if (!!!(GetAsyncKeyState(VK_LBUTTON) & 0x8000)) // 鼠标左键按下，则不响应
			{
				if (VK_LEFT <= pMsg->wParam && pMsg->wParam <= VK_DOWN) // 仅处理方向键
				{
					POINT* lpPt = new POINT;

					bool bHor = (pMsg->wParam % 2);

					lpPt->x = (bHor ? (pMsg->wParam == VK_LEFT ? -1 : 1): 0);

					lpPt->y = (bHor ? 0 : (pMsg->wParam == VK_UP ? -1 : 1));

					::PostMessage(GetParent()->GetSafeHwnd(), PQM_IM_VIEW_MOVED, (WPARAM)lpPt, m_nFocus);

					return TRUE;
				}
			}
		}
		return ImRender::CImRender::PreTranslateMessage(pMsg);
	}

	void CImImage::OnLButtonDown(UINT nFlags, CPoint point)
	{
		SetFocus();

		SetCapture();

		m_ptLBDown = point;

		ClearFocusRect();

		SetFocusRect(point);
	}

	void CImImage::OnLButtonUp(UINT nFlags, CPoint point)
	{
		OnMoved(point);

		OnScaled(point);

		OnRotated(point);

		ReleaseCapture();
	}

	void CImImage::OnMouseMove(UINT nFlags, CPoint point)
	{
		if (!(nFlags & MK_LBUTTON)) return;

		ClearFocusRect();

		SetFocusRect(point);

		DrawFocusRect();

		OnMoving(point);

		OnScaling(point);

		OnRotating(point);
	}

	Point2f CImImage::GetCenter()
	{
		Point2f* lpPtCenter = new Point2f();

		::SendMessage(GetParent()->GetSafeHwnd(), PQM_IM_VIEW_CENTER, 0, (LPARAM)lpPtCenter);
		
		Point2f ptCenter = *lpPtCenter;

		delete lpPtCenter;

		return ptCenter;
	}

	Point2f CImImage::Pt2Pt(CPoint point)
	{
		Point2f* lpPt = new Point2f(point.x, point.y);

		::SendMessage(GetParent()->GetSafeHwnd(), PQM_IM_VIEW_PT2PT, 0, (LPARAM)lpPt);

		Point2f pt = *lpPt;

		delete lpPt;

		return pt;
	}

	void CImImage::OnMoving(CPoint point)
	{
		if (m_nOpt != ImLayer::tagLayer::eOT_Move) return;

		POINT* lpPt = new POINT;

		lpPt->x = point.x - m_ptLBDown.x;

		lpPt->y = point.y - m_ptLBDown.y;

		::PostMessage(GetParent()->GetSafeHwnd(), PQM_IM_VIEW_MOVING, (WPARAM)lpPt, m_nFocus);
	}

	void CImImage::OnMoved(CPoint point)
	{
		if (m_nOpt != ImLayer::tagLayer::eOT_Move) return;

		POINT* lpPt = new POINT;

		lpPt->x = point.x - m_ptLBDown.x;

		lpPt->y = point.y - m_ptLBDown.y;

		::PostMessage(GetParent()->GetSafeHwnd(), PQM_IM_VIEW_MOVED, (WPARAM)lpPt, m_nFocus);
	}

	void CImImage::OnScaling(CPoint point)
	{
		if (m_nOpt != ImLayer::tagLayer::eOT_Scale) return;

		POINT* lpPt = new POINT;

		lpPt->x = point.x - m_ptLBDown.x;

		lpPt->y = point.y - m_ptLBDown.y;

		::PostMessage(GetParent()->GetSafeHwnd(), PQM_IM_VIEW_SCALING, (WPARAM)lpPt, m_nFocus);
	}

	void CImImage::OnScaled(CPoint point)
	{
		if (m_nOpt != ImLayer::tagLayer::eOT_Scale) return;

		POINT* lpPt = new POINT;

		lpPt->x = point.x - m_ptLBDown.x;

		lpPt->y = point.y - m_ptLBDown.y;

		::PostMessage(GetParent()->GetSafeHwnd(), PQM_IM_VIEW_SCALED, (WPARAM)lpPt, m_nFocus);
	}

	void CImImage::OnRotating(CPoint point)
	{
		if (m_nOpt != ImLayer::tagLayer::eOT_Rotate) return;

		ImCtrl::uWParam a;

		auto w = m_rtClient.Width(), h = m_rtClient.Height();

		CPoint o(0.5 * w, 0.5 * h);

		CPoint p1(m_ptLBDown.x - o.x, -(m_ptLBDown.y - o.y)); // 图像坐标系转卡迪尔坐标系

		CPoint p2(point.x - o.x, -(point.y - o.y));

		double a1 = atan2(p1.y, p1.x);

		double a2 = atan2(p2.y, p2.x); // [-pi, pi]

		a1 = (a1 < 0.0 ? 2.0 * CV_PI + a1 : a1);

		a2 = (a2 < 0.0 ? 2.0 * CV_PI + a2 : a2); // [0, 2pi]

		a.d = (a2 - a1) * 180.0 / CV_PI;

		::PostMessage(GetParent()->GetSafeHwnd(), PQM_IM_VIEW_ROTATING, a.w, m_nFocus);
	}

	void CImImage::OnRotated(CPoint point)
	{
		if (m_nOpt != ImLayer::tagLayer::eOT_Rotate) return;

		ImCtrl::uWParam a;

		auto w = m_rtClient.Width(), h = m_rtClient.Height();

		CPoint o(0.5 * w, 0.5 * h);

		CPoint p1(m_ptLBDown.x - o.x, -(m_ptLBDown.y - o.y)); // 图像坐标系转卡迪尔坐标系

		CPoint p2(point.x - o.x, -(point.y - o.y));

		double a1 = atan2(p1.y, p1.x);

		double a2 = atan2(p2.y, p2.x); // [-pi, pi]

		a1 = (a1 < 0.0 ? 2.0 * CV_PI + a1 : a1);

		a2 = (a2 < 0.0 ? 2.0 * CV_PI + a2 : a2); // [0, 2pi]

		a.d = (a2 - a1) * 180.0 / CV_PI;

		::PostMessage(GetParent()->GetSafeHwnd(), PQM_IM_VIEW_ROTATED, a.w, m_nFocus);
	}

	void CImImage::OnCreateEx(LPCREATESTRUCT& lpCreateStruct)
	{
		ImRender::CImRender::OnCreateEx(lpCreateStruct);

		SetBackdrop(RGB(34, 41, 48));

		m_cImRender = Mat::zeros(lpCreateStruct->cy, lpCreateStruct->cx, CV_8UC4);

		m_hCursor[eCT_Move] = LoadCursorFromFile(Transform::WString2CString(m_strIconDir + L"\\cursor\\move.cur"));

		m_hCursor[eCT_Scale] = LoadCursorFromFile(Transform::WString2CString(m_strIconDir + L"\\cursor\\scale.cur"));

		m_hCursor[eCT_Rotate] = LoadCursorFromFile(Transform::WString2CString(m_strIconDir + L"\\cursor\\rotate.cur"));

		m_hCursor[eCT_Select] = LoadCursorFromFile(Transform::WString2CString(m_strIconDir + L"\\cursor\\select.cur"));
	}

	void CImImage::OnDrawEx(CDC& dc, const CRect& rtCanvas, unsigned char*& lpPixel, const long& biWidth)
	{
		GetRender(m_cImRender);

		memcpy(lpPixel, m_cImRender.data, (int64)m_cImRender.rows * m_cImRender.cols * m_cImRender.channels());

		lpPixel = UpdatePixel(lpPixel);
	}
}

