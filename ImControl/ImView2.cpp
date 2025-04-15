#include "pch.h"
#include "ImView2.h"
#include "PQMsg.h"

namespace ImView2
{
	IMPLEMENT_DYNAMIC(CLayouter, ImCtrl::CImCtrl)

	BEGIN_MESSAGE_MAP(CLayouter, ImCtrl::CImCtrl)

	END_MESSAGE_MAP()

	CLayouter::CLayouter():
		m_cHor(false), m_cVer(true)
	{

	}

	CLayouter::~CLayouter()
	{

	}

	void CLayouter::SetIcon(wstring strIconDir)
	{
		m_cImage.SetIcon(strIconDir + L"\\ImImage");

		m_cHor.SetIcon(strIconDir + L"\\ScrollBar");

		m_cVer.SetIcon(strIconDir + L"\\ScrollBar");
	}

	void CLayouter::OnCreateEx(LPCREATESTRUCT& lpCreateStruct)
	{
		static int nScrollbarResolution = 26;

		SetBackdrop(RGB(34, 41, 48));

		auto& cx = lpCreateStruct->cx, & cy = lpCreateStruct->cy;

		SetCtrlParam(&m_cImage, { 0,0 }, { cx - nScrollbarResolution - 1, cy - nScrollbarResolution - 1 });

		SetCtrlParam(&m_cHor, { 0, m_cImage.GetResolution().y + 1 }, { cx - nScrollbarResolution - 1, nScrollbarResolution });

		SetCtrlParam(&m_cVer, { m_cImage.GetResolution().x + 1, 0 }, { nScrollbarResolution, cy - nScrollbarResolution - 1 });

		CreateCtrl(&m_cImage, nullptr, L"", WS_CHILD | WS_VISIBLE | SS_NOTIFY, this, 1);

		CreateCtrl(&m_cHor, &m_cImage, L"", WS_CHILD | WS_VISIBLE | SS_NOTIFY, this, 2);

		CreateCtrl(&m_cVer, &m_cImage, L"", WS_CHILD | WS_VISIBLE | SS_NOTIFY, this, 3);
	}
	/*************************************************************************************************************/
	IMPLEMENT_DYNAMIC(CImView, CLayouter)

	BEGIN_MESSAGE_MAP(CImView, CLayouter)
		ON_WM_HSCROLL()
		ON_WM_VSCROLL()
		ON_MESSAGE(PQM_IM_VIEW_CENTER, OnImCenterMsg)
		ON_MESSAGE(PQM_IM_VIEW_PT2PT, OnImPt2PtMsg)
		ON_MESSAGE(PQM_IM_VIEW_MOVING, OnImMovingMsg)
		ON_MESSAGE(PQM_IM_VIEW_MOVED, OnImMovedMsg)
		ON_MESSAGE(PQM_IM_VIEW_SCALING, OnImScalingMsg)
		ON_MESSAGE(PQM_IM_VIEW_SCALED, OnImScaledMsg)
		ON_MESSAGE(PQM_IM_VIEW_SCALE, OnImScaleMsg)
		ON_MESSAGE(PQM_IM_VIEW_ROTATING, OnImRotatingMsg)
		ON_MESSAGE(PQM_IM_VIEW_ROTATED, OnImRotatedMsg)
	END_MESSAGE_MAP()

	CImView::CImView() :
		m_lpHandle(nullptr),
		m_vImThumb{ 0,0.0,0,0.0 }
	{

	}

	CImView::~CImView()
	{

	}

	void CImView::OnSyncParam(ImLayer::tagLayer& tFocus, ImLayer::tagLayer& tBackground)
	{
		m_cImage.OnSyncParam(tFocus);

		m_cHor.OnScalarChanged(m_cImage.GetResolution().x, tBackground.nLeft + tBackground.nWidth);

		m_cVer.OnScalarChanged(m_cImage.GetResolution().y, tBackground.nTop + tBackground.nHeight);	// 同步滚动条
	}

	void CImView::GetRenderSize(int& w, int& h)
	{
		w = m_cImage.GetResolution().x;

		h = m_cImage.GetResolution().y;
	}

	Rect CImView::GetFocusRect()
	{
		CRect rtFocus = m_cImage.GetFocusRect(); // 窗口位于控件位置

		rtFocus.OffsetRect(get<0>(m_vImThumb) * get<1>(m_vImThumb), get<2>(m_vImThumb) * get<3>(m_vImThumb)); // 滚动条偏移
 
		return Rect(rtFocus.left, rtFocus.top, rtFocus.Width(), rtFocus.Height());// 窗口位于背景中位置
	}

	void CImView::OnCreateEx(LPCREATESTRUCT& lpCreateStruct)
	{
		CLayouter::OnCreateEx(lpCreateStruct);

	}

	void CImView::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
	{
		get<0>(m_vImThumb) = nPos;

		get<1>(m_vImThumb) = m_cHor.GetRatio();

		m_cImage.OnSyncImThumb(m_vImThumb);
	}

	void CImView::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
	{
		get<2>(m_vImThumb) = nPos;

		get<3>(m_vImThumb) = m_cVer.GetRatio();

		m_cImage.OnSyncImThumb(m_vImThumb);
	}

	LRESULT CImView::OnImCenterMsg(WPARAM wparam, LPARAM lparam)
	{
		::SendMessage(m_lpHandle->GetSafeHwnd(), PQM_IM_VIEW_CENTER, wparam, lparam); // 处理不了则继续上抛

		return 0;
	}

	LRESULT CImView::OnImPt2PtMsg(WPARAM wparam, LPARAM lparam)
	{
		Point2f* lpPt = (Point2f*)lparam; // 点位于控件位置

		lpPt->x = get<1>(m_vImThumb) * ((int64)(get<0>(m_vImThumb) + lpPt->x));

		lpPt->y = get<3>(m_vImThumb) * ((int64)(get<2>(m_vImThumb) + lpPt->y)); // 点位于背景中位置

		return 0;
	}

	LRESULT CImView::OnImMovingMsg(WPARAM wparam, LPARAM lparam)
	{
		::PostMessage(m_lpHandle->GetSafeHwnd(), PQM_IM_VIEW_MOVING, wparam, lparam);

		return 0;
	}

	LRESULT CImView::OnImMovedMsg(WPARAM wparam, LPARAM lparam)
	{
		::PostMessage(m_lpHandle->GetSafeHwnd(), PQM_IM_VIEW_MOVED, wparam, lparam);

		return 0;
	}

	LRESULT CImView::OnImScalingMsg(WPARAM wparam, LPARAM lparam)
	{
		::PostMessage(m_lpHandle->GetSafeHwnd(), PQM_IM_VIEW_SCALING, wparam, lparam);

		return 0;
	}

	LRESULT CImView::OnImScaledMsg(WPARAM wparam, LPARAM lparam)
	{
		::PostMessage(m_lpHandle->GetSafeHwnd(), PQM_IM_VIEW_SCALED, wparam, lparam);

		return 0;
	}

	LRESULT CImView::OnImScaleMsg(WPARAM wparam, LPARAM lparam)
	{
		::PostMessage(m_lpHandle->GetSafeHwnd(), PQM_IM_VIEW_SCALE, wparam, lparam);

		return 0;
	}

	LRESULT CImView::OnImRotatingMsg(WPARAM wparam, LPARAM lparam)
	{
		::PostMessage(m_lpHandle->GetSafeHwnd(), PQM_IM_VIEW_ROTATING, wparam, lparam);

		return 0;
	}

	LRESULT CImView::OnImRotatedMsg(WPARAM wparam, LPARAM lparam)
	{
		::PostMessage(m_lpHandle->GetSafeHwnd(), PQM_IM_VIEW_ROTATED, wparam, lparam);

		return 0;
	}
}