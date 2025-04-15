#include "pch.h"
#include "ImScrollbar.h"
#include "DrawPng.h"
#include "PQMsg.h"
#include <iostream>
using namespace std;

namespace ImScrollbar
{	
	IMPLEMENT_DYNAMIC(CThumb, ImCtrl::CImCtrl)

	BEGIN_MESSAGE_MAP(CThumb, ImCtrl::CImCtrl)
		ON_WM_SIZE()
		ON_WM_LBUTTONUP()
		ON_WM_LBUTTONDOWN()
		ON_WM_MOUSEMOVE()
		ON_WM_MOUSEHOVER()
		ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)
	END_MESSAGE_MAP()

	CThumb::CThumb(bool bVer, int nMinThumb):
		m_bVer(bVer), m_nMinThumb(nMinThumb),
		m_bStatus{ false, false },
		m_bIsTracking(false), m_ptButtonDown(0,0),
		m_crColor{ 0xffffff, 0xf0f0f0 }
	{
		
	}

	CThumb::~CThumb()
	{

	}

	void CThumb::SetIcon(COLORREF crNormal, COLORREF crPushed)
	{
		m_crColor[0] = crNormal; m_crColor[1] = crPushed;
	}

	inline void CThumb::SetStatus(bool bPushed)
	{
		m_bStatus[0] = bPushed;

		if (m_bStatus[0] == m_bStatus[1]) return;

		m_bStatus[1] = m_bStatus[0];

		SetBackdrop(m_crColor[IsPushed()]);

		Invalidate();
	}

	inline bool CThumb::IsPushed()
	{
		return m_bStatus[1];
	}

	inline void CThumb::OnDrag(UINT nFlags, CPoint point)
	{
		if (!(nFlags & MK_LBUTTON)) return;

		int ox = point.x - m_ptButtonDown.x, oy = point.y - m_ptButtonDown.y;

		::PostMessage(
			GetParent()->GetSafeHwnd(),
			WM_MOUSEWHEEL,
			MAKELONG(nFlags, (m_bVer ? oy : ox) * (-WHEEL_DELTA)),
			MAKELONG(point.x, point.y));
	}

	void CThumb::OnCreateEx(LPCREATESTRUCT& lpCreateStruct)
	{
		SetBackdrop(m_crColor[IsPushed()]);
	}

	void CThumb::OnSize(UINT nType, int cx, int cy)
	{
		ImCtrl::CImCtrl::OnSize(nType, cx, cy);

		ResetCanvas(this, CRect(0, 0, cx, cy));
	}

	void CThumb::OnLButtonDown(UINT nFlags, CPoint point)
	{
		m_ptButtonDown = point;

		SetStatus(true);

		SetCapture();
	}

	void CThumb::OnLButtonUp(UINT nFlags, CPoint point)
	{
		ReleaseCapture();

		SetStatus(false);
	}

	void CThumb::OnMouseMove(UINT nFlags, CPoint point)
	{
		SetStatus(nFlags & MK_LBUTTON);

		OnDrag(nFlags, point);

		ImCtrl::TrackMouseEvent(m_bIsTracking, TME_LEAVE | TME_HOVER, GetSafeHwnd(), 100);
	}

	void CThumb::OnMouseHover(UINT nFlags, CPoint point)
	{
		SetStatus(nFlags & MK_LBUTTON);

		OnDrag(nFlags, point);

		m_bIsTracking = false;

		ImCtrl::TrackMouseEvent(m_bIsTracking, TME_LEAVE | TME_HOVER, GetSafeHwnd(), 100);
	}

	LRESULT CThumb::OnMouseLeave(WPARAM wParam, LPARAM lParam)
	{
		SetStatus(false);

		m_bIsTracking = false;

		return 0;
	}
/********************************************************************************/
	IMPLEMENT_DYNAMIC(CChute, ImCtrl::CImCtrl)

	BEGIN_MESSAGE_MAP(CChute, ImCtrl::CImCtrl)
		ON_WM_MOUSEWHEEL()
		ON_WM_ERASEBKGND()
		ON_WM_LBUTTONDOWN()
		ON_WM_MOUSEMOVE()
		ON_WM_MOUSEHOVER()
		ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)
	END_MESSAGE_MAP()

	CChute::CChute(bool bVer):
		m_cThumb(bVer, 30),
		m_bVer(bVer), m_bIsTracking(false), m_vRatio{ 1.0, 1.0 }
	{

	}

	CChute::~CChute()
	{

	}

	void CChute::SetIcon(std::wstring strIconDir)
	{
		m_btLineUp.SetIcon(strIconDir + (m_bVer ? L"\\Top" : L"\\Left"));

		m_btLineDown.SetIcon(strIconDir + (m_bVer ? L"\\Bottom" : L"\\Right"));
	}

	double CChute::GetRatio()
	{
		return m_vRatio[1];
	}

	void CChute::OnScalarChanged(int nField, int nView)
	{
		if (nView <= nField) { m_vRatio[1] = 1.0; return; }

		int nHor = GetResolution().x - m_btLineUp.GetResolution().x - m_btLineDown.GetResolution().x;

		int nVer = GetResolution().y - m_btLineUp.GetResolution().y - m_btLineDown.GetResolution().y;

		int nBar = (m_bVer ? nVer : nHor);// 滑道范围

		int nHis = (m_bVer ? m_cThumb.GetResolution().y: m_cThumb.GetResolution().x); //变动前滑块大小

		// 滑块 / 滑道 = 视野 / 视图
		int nCur = max((int)floor((double)nField / nView * nBar), m_cThumb.GetMinThumb()); // 变动后滑块大小

		// 滑动比例 = (视图 - 视野) / (滑道 - 滑块)
		m_vRatio[0] = m_vRatio[1];

		m_vRatio[1] = (double)(nView - nField) / (nBar - nCur);

		m_cThumb.SetResolution({ m_bVer ? m_cThumb.GetResolution().x : nCur, m_bVer? nCur : m_cThumb.GetResolution().y });

		CRect rtCtrl;

		::GetWindowRect(m_cThumb.GetSafeHwnd(), rtCtrl);

		ScreenToClient(rtCtrl);

		double nThumb = (m_bVer ? rtCtrl.top - m_btLineUp.GetResolution().y : rtCtrl.left - m_btLineUp.GetResolution().x); // 变动前滑块位置

		double nImPos = nThumb * m_vRatio[0];

		nThumb = nImPos / m_vRatio[1];

		rtCtrl = m_btLineUp.GetChildRect();

		rtCtrl.left = (rtCtrl.right + m_cThumb.GetSpace().x) + (m_bVer ? 0 : nThumb);

		rtCtrl.top = (rtCtrl.bottom + m_cThumb.GetSpace().y) + (m_bVer ? nThumb : 0);

		rtCtrl.right = rtCtrl.left + m_cThumb.GetResolution().x;

		rtCtrl.bottom = rtCtrl.top + m_cThumb.GetResolution().y;

		auto iDown = m_btLineDown.GetChildRect();

		int ox = (m_bVer ? 0: iDown.left - rtCtrl.right);

		int oy = (m_bVer ? iDown.top - rtCtrl.bottom : 0);

		rtCtrl.MoveToXY(rtCtrl.left + min(ox, 0), rtCtrl.top + min(oy, 0));

		::SetWindowPos(m_cThumb.GetSafeHwnd(), nullptr,
			rtCtrl.left, rtCtrl.top, rtCtrl.Width(), rtCtrl.Height(), SWP_NOZORDER);

		OnDrag(0);
	}

	void CChute::OnCreateEx(LPCREATESTRUCT& lpCreateStruct)
	{
		SetBackdrop(RGB(25, 29, 39));

		OnInitCtrl(lpCreateStruct->cx, lpCreateStruct->cy);

		OnInitCtrl();

		SetClickedFunc(m_btLineUp.GetCtrlID(), bind(&CChute::OnClickedLineUp, this));

		SetClickedFunc(m_btLineDown.GetCtrlID(), bind(&CChute::OnClickedLineDown, this));

		SetClickedFunc(m_btPageUp.GetCtrlID(), bind(&CChute::OnClickedPageUp, this));

		SetClickedFunc(m_btPageDown.GetCtrlID(), bind(&CChute::OnClickedPageDown, this));
	}

	void CChute::OnInitCtrl(const int& cx, const int& cy)
	{
		int nThumb = max(cx, cy) - min(cx, cy) * 2;

		m_btLineUp.SetSpace({ 0,0 });
		m_btLineUp.SetResolution({min(cx, cy), min(cx, cy)});

		m_btLineDown.SetSpace({ m_bVer ? -min(cx, cy) : nThumb, m_bVer ? nThumb : -min(cx, cy) });
		m_btLineDown.SetResolution({ min(cx, cy), min(cx, cy) });

		m_cThumb.SetSpace({ m_bVer ? -(min(cx, cy) - 2) : 0,m_bVer ? 0 : -(min(cx, cy) - 2) });
		m_cThumb.SetResolution({ m_bVer ? min(cx, cy) - 2 * 2 : 50, m_bVer ? 50 : min(cx, cy) - 2 * 2 });
	}

	void CChute::OnInitCtrl()
	{
		CRect rtCtrl;
		rtCtrl.left = m_btLineUp.GetSpace().x;
		rtCtrl.top = m_btLineUp.GetSpace().y;
		rtCtrl.right = rtCtrl.left + m_btLineUp.GetResolution().x;
		rtCtrl.bottom = rtCtrl.top + m_btLineUp.GetResolution().y;
		m_btLineUp.Create(L"", WS_CHILD | WS_VISIBLE | SS_NOTIFY, rtCtrl, this, 1);

		rtCtrl = m_btLineUp.GetChildRect();
		rtCtrl.left = rtCtrl.right + m_btLineDown.GetSpace().x;
		rtCtrl.top = rtCtrl.bottom + m_btLineDown.GetSpace().y;
		rtCtrl.right = rtCtrl.left + m_btLineDown.GetResolution().x;
		rtCtrl.bottom = rtCtrl.top + m_btLineDown.GetResolution().y;
		m_btLineDown.Create(L"", WS_CHILD | WS_VISIBLE | SS_NOTIFY, rtCtrl, this, 2);

		rtCtrl = m_btLineUp.GetChildRect();
		rtCtrl.left = rtCtrl.right + m_cThumb.GetSpace().x;
		rtCtrl.top = rtCtrl.bottom + m_cThumb.GetSpace().y;
		rtCtrl.right = rtCtrl.left + m_cThumb.GetResolution().x;
		rtCtrl.bottom = rtCtrl.top + m_cThumb.GetResolution().y;
		m_cThumb.Create(L"", WS_CHILD | WS_VISIBLE | SS_NOTIFY, rtCtrl, this, 3);

		m_btPageUp.Create(L"", WS_CHILD | WS_VISIBLE | SS_NOTIFY, CRect(), this, 4);
		m_btPageDown.Create(L"", WS_CHILD | WS_VISIBLE | SS_NOTIFY, CRect(), this, 5);
	}

	void CChute::OnHold(bool bPushed, CPoint point)
	{
		if (!bPushed) return;

		CRect rtThumb;

		::GetWindowRect(m_cThumb.GetSafeHwnd(), rtThumb); 
		
		ScreenToClient(rtThumb);

		if (PtInRect(rtThumb, point)) return;

		if ((!m_bVer && point.x < rtThumb.left) ||
			(m_bVer && point.y < rtThumb.top))
			::PostMessage(GetSafeHwnd(), PQM_IM_CTRL_CLICKED, 0, (LPARAM)m_btPageUp.GetCtrlID());

		else if (
			(!m_bVer && rtThumb.right < point.x) ||
			(m_bVer && rtThumb.bottom < point.y))
			::PostMessage(GetSafeHwnd(), PQM_IM_CTRL_CLICKED, 0, (LPARAM)m_btPageDown.GetCtrlID());
	}

	void CChute::OnLButtonDown(UINT nFlags, CPoint point)
	{
		OnHold(true, point);
	}

	void CChute::OnMouseMove(UINT nFlags, CPoint point)
	{
		ImCtrl::TrackMouseEvent(m_bIsTracking, TME_LEAVE | TME_HOVER, GetSafeHwnd(), 100);
	}

	void CChute::OnMouseHover(UINT nFlags, CPoint point)
	{
		m_bIsTracking = false;

		OnHold(nFlags & MK_LBUTTON, point);

		ImCtrl::TrackMouseEvent(m_bIsTracking, TME_LEAVE | TME_HOVER, GetSafeHwnd(), 300);
	}

	LRESULT CChute::OnMouseLeave(WPARAM wParam, LPARAM lParam)
	{
		m_bIsTracking = false;

		return 0;
	}

	void CChute::OnDrag(int os)
	{
		CRect rtLineUp, rtLineDown, rtThumb;

		::GetWindowRect(m_cThumb.GetSafeHwnd(), rtThumb);

		::GetWindowRect(m_btLineUp.GetSafeHwnd(), rtLineUp);

		::GetWindowRect(m_btLineDown.GetSafeHwnd(), rtLineDown);

		ScreenToClient(rtThumb); ScreenToClient(rtLineUp); ScreenToClient(rtLineDown);

		long h1 = (m_bVer ? rtLineUp.bottom + m_cThumb.GetSpace().y : rtLineUp.right + m_cThumb.GetSpace().x),
			h2 = (m_bVer ? rtLineDown.top - m_cThumb.GetSpace().y : rtLineDown.left - m_cThumb.GetSpace().x);

		int l = (m_bVer ? rtThumb.left : max(rtThumb.left + os, h1)),
			r = (m_bVer ? rtThumb.right : min(rtThumb.right + max(os, 0), h2));

		int t = (m_bVer ? max(rtThumb.top + os, h1) : rtThumb.top),
			b = (m_bVer ? min(rtThumb.bottom + max(os, 0), h2) : rtThumb.bottom);

		l = (m_bVer ? l : min(l, r - rtThumb.Width()));

		t = (m_bVer ? min(t, b - rtThumb.Height()) : t);

		::SetWindowPos(m_cThumb.GetSafeHwnd(), nullptr,
			l, t, 0, 0, SWP_NOZORDER | SWP_NOSIZE);

		UpdateWindow();

		int nPos = (m_bVer ? t : l) - m_btLineUp.GetResolution().x;

		::PostMessage(
			GetParent()->GetSafeHwnd(),
			m_bVer ? WM_VSCROLL : WM_HSCROLL,
			MAKELONG(SB_THUMBTRACK, (m_bVer ? t : l) - m_btLineUp.GetResolution().x), (LPARAM)GetSafeHwnd());
	}

	BOOL CChute::OnEraseBkgnd(CDC* pDC)
	{
		return TRUE;
	}

	BOOL CChute::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
	{
		OnDrag(-zDelta / WHEEL_DELTA);

		return TRUE;
	}

	void CChute::OnClickedLineUp()
	{
		OnDrag(-1);
	}

	void CChute::OnClickedLineDown()
	{
		OnDrag(1);
	}

	void CChute::OnClickedPageUp()
	{
		OnDrag(m_bVer ? (-m_cThumb.GetResolution().y) : (-m_cThumb.GetResolution().x));
	}

	void CChute::OnClickedPageDown()
	{
		OnDrag(m_bVer ? m_cThumb.GetResolution().y : m_cThumb.GetResolution().x);
	}
/********************************************************************************/
	IMPLEMENT_DYNAMIC(CImScrollbar, ImCtrl::CImCtrl)

	BEGIN_MESSAGE_MAP(CImScrollbar, ImCtrl::CImCtrl)
		ON_WM_MOUSEWHEEL()
		ON_WM_ERASEBKGND()
		ON_WM_VSCROLL()
		ON_WM_HSCROLL()
	END_MESSAGE_MAP()

	CImScrollbar::CImScrollbar(bool bVer) :
		m_bVer(bVer), m_cChute(bVer)
	{

	}

	CImScrollbar::~CImScrollbar()
	{

	}

	void CImScrollbar::SetIcon(wstring strIconDir)
	{
		m_cChute.SetIcon(strIconDir);
	}

	void CImScrollbar::OnScalarChanged(int nField, int nView)
	{
		ShowWindow(nView <= nField ? SW_HIDE : SW_SHOW);

		m_cChute.OnScalarChanged(nField, nView);

		nView <= nField ? ::PostMessage(
			GetParent()->GetSafeHwnd(),
			m_bVer ? WM_VSCROLL : WM_HSCROLL,
			MAKELONG(SB_THUMBTRACK, 0), (LPARAM)m_cChute.GetSafeHwnd()) : []()->BOOL {return 0; }();
	}

	double CImScrollbar::GetRatio()
	{
		return m_cChute.GetRatio();
	}

	BOOL CImScrollbar::OnEraseBkgnd(CDC* pDC)
	{
		return TRUE;
	}

	void CImScrollbar::OnCreateEx(LPCREATESTRUCT& lpCreateStruct)
	{
		SetBackdrop(RGB(25, 29, 39));

		OnInitCtrl(lpCreateStruct->cx, lpCreateStruct->cy);

		OnInitCtrl();
	}

	void CImScrollbar::OnInitCtrl(const int& cx, const int& cy)
	{
		m_cChute.SetSpace({ 0, 0 });

		m_cChute.SetResolution({ cx, cy });
	}

	void CImScrollbar::OnInitCtrl()
	{
		CRect rtCtrl;
		rtCtrl.left = m_cChute.GetSpace().x;
		rtCtrl.top = m_cChute.GetSpace().y;
		rtCtrl.right = rtCtrl.left + m_cChute.GetResolution().x;
		rtCtrl.bottom = rtCtrl.top + m_cChute.GetResolution().y;
		m_cChute.Create(L"", WS_CHILD | WS_VISIBLE | SS_NOTIFY, rtCtrl, this, 1);
	}

	BOOL CImScrollbar::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
	{
		::PostMessage(
			m_cChute.GetSafeHwnd(),
			WM_MOUSEWHEEL,
			MAKELONG(nFlags, zDelta),
			MAKELONG(pt.x, pt.y));

		return TRUE;
	}

	void CImScrollbar::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
	{
		::PostMessage(
			GetParent()->GetSafeHwnd(),
			WM_VSCROLL,
			MAKELONG(SB_THUMBTRACK, nPos), (LPARAM)pScrollBar);
	}

	void CImScrollbar::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
	{
		::PostMessage(
			GetParent()->GetSafeHwnd(),
			WM_HSCROLL,
			MAKELONG(SB_THUMBTRACK, nPos), (LPARAM)pScrollBar);
	}
}
