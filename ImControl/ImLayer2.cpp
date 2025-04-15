#include "pch.h"
#include "ImLayer2.h"
#include "DrawPng.h"
#include "PQMsg.h"
#include <iostream>
#include <imm.h>
#pragma comment(lib, "Imm32.lib")
using namespace cv;
using namespace std;

namespace ImLayer2
{
	IMPLEMENT_DYNAMIC(CLayouter, ImCtrl::CImCtrl)

	BEGIN_MESSAGE_MAP(CLayouter, ImCtrl::CImCtrl)

	END_MESSAGE_MAP()

	CLayouter::CLayouter()
	{

	}

	CLayouter::~CLayouter()
	{

	}

	void CLayouter::SetIcon(wstring strIconDir)
	{
		m_cName.SetIcon(strIconDir + L"\\Name");

		m_cEnable.SetIcon(strIconDir + L"\\Enable");

		m_cBackground.SetIcon(strIconDir + L"\\Background");
	}

	void CLayouter::OnCreateEx(LPCREATESTRUCT& lpCreateStruct)
	{
		SetBackdrop(RGB(34, 41, 48));

		auto& cx = lpCreateStruct->cx, & cy = lpCreateStruct->cy;

		auto iBackgroundSize = m_cBackground.GetIconSize();

		array<int, 2> iBackgroundRound = { iBackgroundSize[0] + 4, iBackgroundSize[1] + 4 };

		SetCtrlParam(&m_cEnable, { 2,2 }, { cy - 2 * 2, cy - 2 * 2 });

		SetCtrlParam(&m_cPreview, { m_cEnable.GetResolution().x + 2, 0 }, { cy - 2 * 2, cy - 2 * 2 });

		SetCtrlParam(&m_cName, { m_cPreview.GetResolution().x + 2, (int)((m_cPreview.GetResolution().y - 20) * 0.5) },
			{ cx - m_cEnable.GetResolution().x - m_cPreview.GetResolution().x - iBackgroundRound[0] - 4 * 2, 20 });

		SetCtrlParam(&m_cBackground, { m_cName.GetResolution().x + 2, (int)((20 - iBackgroundRound[1]) * 0.5)}, iBackgroundRound);

		CreateCtrl(&m_cEnable, nullptr, L"", WS_CHILD | WS_VISIBLE | SS_NOTIFY, this, 1);

		CreateCtrl(&m_cPreview, &m_cEnable, L"", WS_CHILD | WS_VISIBLE | SS_NOTIFY, this, 2);

		CreateCtrl(&m_cName, &m_cPreview, L"", WS_CHILD | WS_VISIBLE | SS_NOTIFY, this, 3);

		CreateCtrl(&m_cBackground, &m_cName, L"", WS_CHILD | WS_VISIBLE | SS_NOTIFY, this, 4);

		m_cName.SetTxFormat(DT_LEFT | DT_TOP); m_cName.SetTextColor(0xffffff);
	}
	/*************************************************************************************************************/
	IMPLEMENT_DYNAMIC(CImLayer, CLayouter)

	BEGIN_MESSAGE_MAP(CImLayer, CLayouter)
		ON_WM_LBUTTONDOWN()
	END_MESSAGE_MAP()

	CImLayer::CImLayer():
		m_nSign(0), m_lpHandle(nullptr)
	{

	}

	CImLayer::~CImLayer()
	{

	}

	void CImLayer::setEnableClicked(bool ret)
	{
		m_cEnable.SetCheck(ret);

		OnClickedEnable();
	}

	void CImLayer::OnSyncParam(long long nBackground, long long nFocus, long long nSign, bool bEnable, CString rName, ImPtr::CImPtr cPreview)
	{
		bool bBackground = (m_nSign == nBackground);

		bool bFocus = (m_nSign == nFocus);

		m_nSign == nSign ? m_cEnable.SetCheck(bEnable) : []() {}();

		m_nSign == nSign ? m_cPreview.SetImPtr(cPreview) : []() {}();

		m_nSign == nSign ? m_cName.SetWindowTextEx(rName) : []() {}();

		m_cBackground.SetCheck(bBackground);

		SetBackdrop(bFocus ? RGB(55,60,75) : RGB(34, 41, 48));

		GetSafeHwnd() ? Invalidate() : []() {}();
	}
	
	void CImLayer::OnCreateEx(LPCREATESTRUCT& lpCreateStruct)
	{
		CLayouter::OnCreateEx(lpCreateStruct);

		SetClickedFunc(m_cEnable.GetCtrlID(), bind(&CImLayer::OnClickedEnable, this));

		SetClickedFunc(m_cPreview.GetCtrlID(), bind(&CImLayer::OnClickedPreview, this));

		SetClickedFunc(m_cName.GetCtrlID(), bind(&CImLayer::OnClickedName, this));

		SetChangedFunc(m_cName.GetCtrlID(), bind(&CImLayer::OnNameChanged, this));

		SetClickedFunc(m_cBackground.GetCtrlID(), bind(&CImLayer::OnClickedBackground, this));
	}

	void CImLayer::OnLButtonDown(UINT, CPoint)
	{
		::PostMessage(m_lpHandle->GetSafeHwnd(), PQM_IM_LAYER_FOCUS_CHANGED, (WPARAM)0, (LPARAM)m_nSign);
	}

	void CImLayer::OnClickedEnable()
	{
		bool bCheked = m_cEnable.GetCheck();

		::PostMessage(m_lpHandle->GetSafeHwnd(), PQM_IM_LAYER_CLICKED_ENABLE, (WPARAM)bCheked, (LPARAM)m_nSign);

		::PostMessage(m_lpHandle->GetSafeHwnd(), PQM_IM_LAYER_FOCUS_CHANGED, (WPARAM)0, (LPARAM)m_nSign);
	}

	void CImLayer::OnClickedPreview()
	{
		::PostMessage(m_lpHandle->GetSafeHwnd(), PQM_IM_LAYER_FOCUS_CHANGED, (WPARAM)0, (LPARAM)m_nSign);
	}

	void CImLayer::OnClickedName()
	{
		::PostMessage(m_lpHandle->GetSafeHwnd(), PQM_IM_LAYER_FOCUS_CHANGED, (WPARAM)0, (LPARAM)m_nSign);
	}

	void CImLayer::OnClickedBackground()
	{
		::PostMessage(m_lpHandle->GetSafeHwnd(), PQM_IM_LAYER_CLICKED_BACKGROUND, (WPARAM)0, (LPARAM)m_nSign);

		::PostMessage(m_lpHandle->GetSafeHwnd(), PQM_IM_LAYER_FOCUS_CHANGED, (WPARAM)0, (LPARAM)m_nSign);
	}

	void CImLayer::OnNameChanged()
	{
		SetFocus();

		wchar_t* lpName = new wchar_t[256];

		memset(lpName,0,sizeof(wchar_t) * 256);

		m_cName.GetWindowText(lpName, 256);

		::PostMessage(m_lpHandle->GetSafeHwnd(), PQM_IM_LAYER_NAME_CHANGED, (WPARAM)lpName, (LPARAM)m_nSign);
	}
}
