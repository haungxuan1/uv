#include "pch.h"
#include "StMold.h"
#include "Transform.h"
#include "PQMsg.h"
#include <algorithm>

IMPLEMENT_DYNAMIC(CStMold, CWnd)

BEGIN_MESSAGE_MAP(CStMold, CWnd)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)
END_MESSAGE_MAP()

CStMold::CStMold()
{

}

CStMold::~CStMold()
{

}

void CStMold::OnLButtonDown(UINT nFlags, CPoint point)
{
	OnDragBegin(point);

	CWnd::OnLButtonDown(nFlags, point);
}

void CStMold::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (m_bLbdown) OnMouseMove(point);

	if (m_bLbdown) OnDragEnd();

	if (m_bLbdown) { ReleaseCapture(); }

	m_bLbdown = false;

	CWnd::OnLButtonUp(nFlags, point);
}

void CStMold::OnMouseMove(UINT nFlags, CPoint point)
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

	CWnd::OnMouseMove(nFlags, point);
}

LRESULT CStMold::OnMouseLeave(WPARAM wparam, LPARAM lparam)
{
	m_bTrace = false;

	return 0;
}

void CStMold::OnDragBegin(const CPoint& point)
{
	m_bLbdown = true;

	SetCapture(); // 设置鼠标捕捉 // lbuttonup 释放

	m_ptOperation[eOT_Begin] = point;

	m_ptOperation[eOT_TopLeft] = m_ptOperation[eOT_Begin];

	m_ptOperation[eOT_BottomRight] = m_ptOperation[eOT_Begin];
}

void CStMold::OnDragEnd()
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

void CStMold::OnMouseMove(CPoint point)
{
	// 起点
	m_ptOperation[eOT_TopLeft].x = std::min(m_ptOperation[eOT_Begin].x, point.x);

	m_ptOperation[eOT_TopLeft].y = std::min(m_ptOperation[eOT_Begin].y, point.y);

	m_ptOperation[eOT_BottomRight].x = std::max(m_ptOperation[eOT_Begin].x, point.x);

	m_ptOperation[eOT_BottomRight].y = std::max(m_ptOperation[eOT_Begin].y, point.y);

	// 特征控件
	m_ptOperation[eOT_TopLeft].x = std::max(m_rtClient.left, m_ptOperation[eOT_TopLeft].x);

	m_ptOperation[eOT_TopLeft].y = std::max(m_rtClient.top, m_ptOperation[eOT_TopLeft].y);

	m_ptOperation[eOT_BottomRight].x = std::min(m_rtClient.right, m_ptOperation[eOT_BottomRight].x);

	m_ptOperation[eOT_BottomRight].y = std::min(m_rtClient.bottom, m_ptOperation[eOT_BottomRight].y);
}

void CStMold::OnTrace(UINT nFlags)
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