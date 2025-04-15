#include "pch.h"
#include "ImLayers.h"
#include "PQMsg.h"

namespace ImLayers
{
	IMPLEMENT_DYNAMIC(CLayouter, ImCtrl::CImCtrl)

	BEGIN_MESSAGE_MAP(CLayouter, ImCtrl::CImCtrl)
		ON_WM_VSCROLL()
		ON_MESSAGE(PQM_IM_LAYER_CLEAR, OnClearMsg)
		ON_MESSAGE(PQM_IM_LAYER_CREATE, OnCreateMsg)
		ON_MESSAGE(PQM_IM_LAYER_SYNC_LAYER_CTRL, OnSyncLayerCtrlMsg)
		ON_MESSAGE(PQM_IM_LAYER_SYNC_SCROLLBAR_CTRL, OnSyncScrollbarCtrlMsg)
	END_MESSAGE_MAP()

	CLayouter::CLayouter():
		m_cVer(true)
	{

	}

	CLayouter::~CLayouter()
	{

	}

	void CLayouter::SetIcon(wstring strIconDir)
	{
		m_strIconDir = strIconDir;

		m_cVer.SetIcon(strIconDir + L"\\ScrollBar");
	}

	int CLayouter::LayerId()
	{
		for (int i = 2; true; ++i)
		{
			auto iCur = _Find_pr(m_vLayer.begin(), m_vLayer.end(), i, [](ImLayer2::CImLayer* lpLayer, int id) { return lpLayer->GetCtrlID() == id; });

			if (iCur == m_vLayer.end()) return i;
		}
		return 0;
	}

	void CLayouter::OnCreateEx(LPCREATESTRUCT& lpCreateStruct)
	{
		static int nScrollbarResolution = 26;

		SetBackdrop(RGB(34, 41, 48));

		auto& cx = lpCreateStruct->cx, & cy = lpCreateStruct->cy;

		SetCtrlParam(&m_cVer, { cx - nScrollbarResolution, 0 }, { nScrollbarResolution, cy });

		CreateCtrl(&m_cVer, nullptr, L"", WS_CHILD | WS_VISIBLE | SS_NOTIFY, this, 1);
	}

	void CLayouter::OnClearLayer()
	{
		for (auto& iLayer : m_vLayer) delete iLayer;

		m_vLayer.clear();

		SendMessage(PQM_IM_LAYER_CLEAR);
	}

	void CLayouter::OnSelectAll()
	{
		bool isChecked = true;
		for (auto& iLayer : m_vLayer) {
			if (!iLayer->m_cEnable.GetCheck()) {
				isChecked = false;
				break;
			}
		}
		for (auto& iLayer : m_vLayer) 
		{
			iLayer->setEnableClicked(!isChecked);
//			iLayer->m_cEnable.SetCheck(!isChecked);

		}
	}

	LRESULT CLayouter::OnClearMsg(WPARAM wparam, LPARAM lparam)
	{
		PostMessage(PQM_IM_LAYER_SYNC_LAYER_CTRL);

		PostMessage(PQM_IM_LAYER_SYNC_SCROLLBAR_CTRL);

		return 0;
	}

	void CLayouter::OnCreateLayer(long long nSign)
	{
		ImLayer2::CImLayer* lpLayer = new ImLayer2::CImLayer();

		lpLayer->Bind(this); 
		
		lpLayer->SetSign(nSign);

		lpLayer->SetIcon(m_strIconDir + L"\\Layer");

		m_vLayer.push_back(lpLayer);

		SendMessage(PQM_IM_LAYER_CREATE);
	}

	LRESULT CLayouter::OnCreateMsg(WPARAM wparam, LPARAM lparam)
	{
		SendMessage(PQM_IM_LAYER_SYNC_LAYER_CTRL);

		SendMessage(PQM_IM_LAYER_SYNC_SCROLLBAR_CTRL);

		return 0;
	}

	LRESULT CLayouter::OnSyncLayerCtrlMsg(WPARAM wparam, LPARAM lparam)
	{
		ImCtrl::CImCtrl* lpFront = nullptr;

		for (int i = 0; i < m_vLayer.size(); ++i)
		{
			auto& iLayer = m_vLayer[i];

			if (!iLayer->IsWindowVisible())
			{
				SetCtrlParam(iLayer, { 0, (!!i) * 50 }, { GetResolution().x - m_cVer.GetResolution().x - 1, 50 });

				CreateCtrl(iLayer, lpFront, L"", WS_CHILD | WS_VISIBLE | SS_NOTIFY, this, LayerId());
			}
			lpFront = iLayer;
		}

		return 0;
	}

	LRESULT CLayouter::OnSyncScrollbarCtrlMsg(WPARAM wparam, LPARAM lparam)
	{
		if (m_vLayer.empty()) { m_cVer.OnScalarChanged(GetResolution().y, 0);return 0; }

		auto& iLayer = (*m_vLayer.begin());

		m_cVer.OnScalarChanged(GetResolution().y, m_vLayer.size() * iLayer->GetResolution().y);

		return 0;
	}

	void CLayouter::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
	{
		int nTop = nPos * m_cVer.GetRatio();

		HDWP hDWP = BeginDeferWindowPos(m_vLayer.size());

		if (hDWP)
		{
			for (auto& iLayer : m_vLayer)
			{
				auto iChild = iLayer->GetChildRect();

				hDWP = DeferWindowPos(hDWP, iLayer->GetSafeHwnd(), NULL,
					iChild.left, iChild.top - nTop, iChild.Width(), iChild.Height(), SWP_NOZORDER);
			}
			EndDeferWindowPos(hDWP);
		}
		UpdateWindow();
	}
	/******************************************************************************************/
	IMPLEMENT_DYNAMIC(CImLayers, CLayouter)

	BEGIN_MESSAGE_MAP(CImLayers, CLayouter)
		ON_WM_LBUTTONDOWN()
		ON_MESSAGE(PQM_IM_LAYER_CLEAR, OnClearMsg)
		ON_MESSAGE(PQM_IM_LAYER_CLICKED_ENABLE, OnClickedEnableMsg)
		ON_MESSAGE(PQM_IM_LAYER_CLICKED_BACKGROUND, OnClickedBackgroundMsg)
		ON_MESSAGE(PQM_IM_LAYER_NAME_CHANGED, OnNameChangedMsg)
		ON_MESSAGE(PQM_IM_LAYER_FOCUS_CHANGED, OnFocusChangedMsg)
	END_MESSAGE_MAP()

	CImLayers::CImLayers():
		m_lpHandle(nullptr)
	{

	}

	CImLayers::~CImLayers()
	{

	}

	void CImLayers::OnSyncParam(ImLayer::tagLayer& tBackground, ImLayer::tagLayer& tFocus, ImLayer::tagLayer& tSign)
	{
		// 更新背景
		// 更新焦点
		// 更新标记
		for (auto& iLayer : m_vLayer) 
			iLayer->OnSyncParam(tBackground.nSign, tFocus.nSign, tSign.nSign, tSign.bEnable, tSign.rName, tSign.cPreview);
	}

	LRESULT CImLayers::OnClearMsg(WPARAM wparam, LPARAM lparam)
	{
		CLayouter::OnClearMsg(wparam, lparam);

		::SendMessage(m_lpHandle->GetSafeHwnd(), PQM_IM_LAYER_CLEAR, 0, 0);

		return 0;
	}

	LRESULT CImLayers::OnClickedEnableMsg(WPARAM wparam, LPARAM lparam)
	{
		::PostMessage(m_lpHandle->GetSafeHwnd(), PQM_IM_LAYER_CLICKED_ENABLE, wparam, lparam);

		return 0;
	}

	LRESULT CImLayers::OnClickedBackgroundMsg(WPARAM wparam, LPARAM lparam)
	{
		::PostMessage(m_lpHandle->GetSafeHwnd(), PQM_IM_LAYER_CLICKED_BACKGROUND, wparam, lparam);

		return 0;
	}
	
	LRESULT CImLayers::OnNameChangedMsg(WPARAM wparam, LPARAM lparam)
	{
		::PostMessage(m_lpHandle->GetSafeHwnd(), PQM_IM_LAYER_NAME_CHANGED, wparam, lparam);

		return 0;
	}

	LRESULT CImLayers::OnFocusChangedMsg(WPARAM wparam, LPARAM lparam)
	{
		::PostMessage(m_lpHandle->GetSafeHwnd(), PQM_IM_LAYER_FOCUS_CHANGED, wparam, lparam);

		return 0;
	}
}
