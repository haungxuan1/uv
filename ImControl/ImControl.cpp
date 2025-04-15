#include "pch.h"
#include "ImControl.h"
#include "DrawPng.h"
#include "PQMsg.h"
using namespace std;

namespace ImControl
{
	IMPLEMENT_DYNAMIC(CImControl, ImCtrl::CImCtrl)

	BEGIN_MESSAGE_MAP(CImControl, ImCtrl::CImCtrl)
		ON_WM_ERASEBKGND()

		ON_MESSAGE(PQM_IM_LAYER_FOCUS, OnLayerFocusChanged)
		ON_MESSAGE(PQM_IM_LAYER_ENABLE, OnLayerEnableChanged)
		ON_MESSAGE(PQM_IM_LAYER_SCALING, OnLayerScalingChanged)

		//ON_MESSAGE(PQM_IM_PARAM_OPACITY, OnOpacityChangedMsg)
		//ON_MESSAGE(PQM_IM_PARAM_SCALING, OnScalingChangedMsg)
		//ON_MESSAGE(PQM_IM_PARAM_TL, OnTLChangedMsg)
		//ON_MESSAGE(PQM_IM_PARAM_WH, OnWHChangedMsg)

		ON_MESSAGE(PQM_IM_VIEW_DRAGGING, OnViewDraggingMsg)
		ON_MESSAGE(PQM_IM_VIEW_DRAG, OnViewDragMsg)
		ON_MESSAGE(PQM_IM_VIEW_SCALING, OnViewScalingMsg)
	END_MESSAGE_MAP()

	CImControl::CImControl()
	{

	}

	CImControl::~CImControl()
	{

	}

	void CImControl::SetIcon(std::wstring strIconDir)
	{
		m_cView.SetIcon(strIconDir);
	}

	LRESULT CImControl::OnLayerFocusChanged(WPARAM wparam, LPARAM lparam)
	{
		int nFocus = (int)lparam;

		auto iTL = m_cLayers.GetTL(nFocus);

		auto iDragging = m_cLayers.GetDragging(nFocus);

		auto iScaling = m_cLayers.GetPrScaling(nFocus);

		iTL[0] += (iDragging[0] / iScaling), iTL[1] += (iDragging[1] / iScaling);

		m_cImParam.SetTL(iTL);

		m_cImParam.SetWH(m_cLayers.GetWH(nFocus));

		m_cImParam.SetOpacity(m_cLayers.GetOpacity(nFocus));

		m_cImParam.SetScaling(iScaling);

		::PostMessage(m_cView.GetSafeHwnd(), PQM_IM_VIEW_RENDER, 0, 0);

		return 0;
	}

	LRESULT CImControl::OnLayerEnableChanged(WPARAM wparam, LPARAM lparam)
	{
		::PostMessage(m_cView.GetSafeHwnd(), PQM_IM_VIEW_RENDER, 0, 0);

		return 0;
	}

	LRESULT CImControl::OnLayerScalingChanged(WPARAM wparam, LPARAM lparam)
	{
		m_cImParam.SetScaling(m_cLayers.GetPrScaling(m_cLayers.GetFocus()));

		::PostMessage(m_cView.GetSafeHwnd(), PQM_IM_VIEW_RENDER, 0, 0);

		return 0;
	}

	LRESULT CImControl::OnOpacityChangedMsg(WPARAM wparam, LPARAM lparam)
	{
		if (m_cLayers.Empty()) return 0;

		int nFocus = m_cLayers.GetFocus();

		m_cLayers.SetOpacity(nFocus, lparam);

		::PostMessage(m_cView.GetSafeHwnd(), PQM_IM_VIEW_RENDER, 0, 0);

		return 0;
	}

	LRESULT CImControl::OnScalingChangedMsg(WPARAM wparam, LPARAM lparam)
	{
		if (m_cLayers.Empty()) return 0;

		int nFocus = m_cLayers.GetFocus();

		int nBackground = m_cLayers.GetBackgroundLayer();

		nFocus == nBackground ? m_cLayers.SetPrScaling(lparam) : m_cLayers.SetPrScaling(nFocus, lparam);

		return 0;
	}

	LRESULT CImControl::OnTLChangedMsg(WPARAM wparam, LPARAM lparam)
	{
		if (m_cLayers.Empty()) return 0;

		int nFocus = m_cLayers.GetFocus();

		m_cLayers.SetTL(nFocus, { (int)(lparam >> 32 & 0xffffffff),  (int)(lparam & 0xffffffff) });

		::PostMessage(m_cView.GetSafeHwnd(), PQM_IM_VIEW_RENDER, 0, 0);

		return 0;
	}

	LRESULT CImControl::OnWHChangedMsg(WPARAM wparam, LPARAM lparam)
	{
		if (m_cLayers.Empty()) return 0;

		int nFocus = m_cLayers.GetFocus();

		m_cLayers.SetWH(nFocus, { (int)(lparam >> 32 & 0xffffffff),  (int)(lparam & 0xffffffff) });

		::PostMessage(m_cView.GetSafeHwnd(), PQM_IM_VIEW_RENDER, 0, 0);

		return 0;
	}

	LRESULT CImControl::OnViewDraggingMsg(WPARAM wparam, LPARAM lparam)
	{
		if (m_cLayers.Empty()) return 0;

		int nFocus = m_cLayers.GetFocus();

		union { int n; uint u; }x, y;

		x.u = wparam; y.n = lparam;

		m_cLayers.SetDragging(nFocus, { x.n, y.n });

		PostMessage(PQM_IM_LAYER_FOCUS, 0, (LPARAM)nFocus);

		return 0;
	}

	LRESULT CImControl::OnViewDragMsg(WPARAM wparam, LPARAM lparam)
	{
		if (m_cLayers.Empty()) return 0;

		union { int n; uint u; }x, y;

		x.u = wparam; y.n = lparam;

		int nFocus = m_cLayers.GetFocus();

		double dScaling = 0.001 * m_cLayers.GetPrScaling(nFocus);

		auto iTL = m_cLayers.GetTL(nFocus);

		iTL[0] += (x.n / dScaling); iTL[1] += (y.n / dScaling);

		m_cLayers.SetTL(nFocus, iTL);

		m_cLayers.SetDragging(nFocus, { 0, 0 });

		PostMessage(PQM_IM_LAYER_FOCUS, 0, (LPARAM)nFocus);

		return 0;
	}

	LRESULT CImControl::OnViewScalingMsg(WPARAM wparam, LPARAM lparam)
	{
		if (m_cLayers.Empty()) return 0;

		int nScaling = (int)lparam;

		int nFocus = m_cLayers.GetFocus();

		int nBackground = m_cLayers.GetBackgroundLayer();

		auto iScaling = m_cLayers.GetPrScaling(nFocus);

		iScaling += nScaling;

		nFocus == nBackground ? m_cLayers.SetPrScaling(iScaling) : m_cLayers.SetPrScaling(nFocus, iScaling);

		PostMessage(PQM_IM_LAYER_FOCUS, 0, (LPARAM)nFocus);

		return 0;
	}

	BOOL CImControl::OnEraseBkgnd(CDC* pDC)
	{
		return TRUE;
	}

	void CImControl::OnCreateEx(LPCREATESTRUCT& lpCreateStruct)
	{
		m_cView.BindLayers(&m_cLayers);

		OnInitCtrl(lpCreateStruct->cx, lpCreateStruct->cy);

		OnInitCtrl();
	}

	void CImControl::OnInitCtrl(const int& cx, const int& cy)
	{
		static int nImParamHight = 300; // 1080 时 120
		static int nImParamWidth = 499; // 1920 时 299

		m_cView.SetSpace({ 0,0 });
		m_cView.SetResolution({ cx - nImParamWidth - 1, cy });

		m_cImParam.SetSpace({ 1, 0 });
		m_cImParam.SetResolution({ nImParamWidth, nImParamHight });

		m_cLayers.SetSpace({ 0, 1 });
		m_cLayers.SetResolution({ nImParamWidth, cy - nImParamHight - 1 });
	}

	void CImControl::OnInitCtrl()
	{
		CRect rtCtrl;

		// 显示
		rtCtrl.left = m_cView.GetSpace().x;
		rtCtrl.top = m_cView.GetSpace().y;
		rtCtrl.right = rtCtrl.left + m_cView.GetResolution().x;
		rtCtrl.bottom = rtCtrl.top + m_cView.GetResolution().y;
		m_cView.Create(L"", WS_CHILD | WS_VISIBLE | SS_NOTIFY, rtCtrl, this, 1);

		// 参数
		rtCtrl = m_cView.GetChildRect();
		rtCtrl.left = rtCtrl.right + m_cImParam.GetSpace().x;
		rtCtrl.top = rtCtrl.top + m_cImParam.GetSpace().y;
		rtCtrl.right = rtCtrl.left + m_cImParam.GetResolution().x;
		rtCtrl.bottom = rtCtrl.top + m_cImParam.GetResolution().y;
		m_cImParam.Create(L"", WS_CHILD | WS_VISIBLE | SS_NOTIFY, rtCtrl, this, 2);

		// 图层
		rtCtrl = m_cImParam.GetChildRect();
		rtCtrl.left = rtCtrl.left + m_cLayers.GetSpace().x;
		rtCtrl.top = rtCtrl.bottom + m_cLayers.GetSpace().y;
		rtCtrl.right = rtCtrl.left + m_cLayers.GetResolution().x;
		rtCtrl.bottom = rtCtrl.top + m_cLayers.GetResolution().y;
		m_cLayers.Create(L"", WS_CHILD | WS_VISIBLE | SS_NOTIFY, rtCtrl, this, 3);
	}
}
