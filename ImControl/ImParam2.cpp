#include "pch.h"
#include "ImParam2.h"
#include "PQMsg.h"
#include <iostream>
using namespace std;

namespace ImParam2
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
		m_btMoving.SetIcon(strIconDir + L"\\move");

		m_btScaling.SetIcon(strIconDir + L"\\scale");

		m_btRotating.SetIcon(strIconDir + L"\\rotate");

		m_btSelecting.SetIcon(strIconDir + L"\\select");

		m_btCopy.SetIcon(strIconDir + L"\\copy");

		m_btSave.SetIcon(strIconDir + L"\\save");

		m_btSelAll.SetIcon(strIconDir + L"\\selectAll");

		m_btAssociated.SetIcon(strIconDir + L"\\associate");

		m_etOpacity.SetIcon(strIconDir + L"\\edit");

		m_etAngle.SetIcon(strIconDir + L"\\edit");

		for(auto& iImParam: m_etParam) iImParam.SetIcon(strIconDir + L"\\edit");

		for(auto& iScale: m_etScaling) iScale.SetIcon(strIconDir + L"\\edit");
	}

	void CLayouter::SetTempPicPath(wstring strPicPath)
	{
		isMainView = true;

		m_btPicture.SetPng(strPicPath);

		m_btPicture.RedrawWindow();
	}

	void CLayouter::OnCreateEx(LPCREATESTRUCT& lpCreateStruct)
	{
		SetBackdrop(RGB(34, 41, 48));
		if(isMainView) OnInitPicCtrl();
		else 
		{
			OnInitParamCtrl();

			OnInitOpacityCtrl();

			OnInitAngleCtrl();

			OnInitScalingCtrl();

			OnInitOptCtrl();

		}
	}

	void CLayouter::OnInitImParamCtrl()
	{
		static int nSpace[2] = { 100, 20 };

		SetCtrlParam(&m_etParam[0], { nSpace[0], nSpace[1] }, { 100, 20 });

		CreateCtrl(&m_etParam[0], nullptr, L"", WS_CHILD | WS_VISIBLE | SS_NOTIFY, this, 11);

		SetCtrlParam(&m_etParam[1], { m_etParam[0].GetResolution().x + nSpace[0], 0 }, { 100, 20 });

		CreateCtrl(&m_etParam[1], &m_etParam[0], L"", WS_CHILD | WS_VISIBLE | SS_NOTIFY, this, 12);

		SetCtrlParam(&m_etParam[2], { 0, m_etParam[0].GetResolution().y + nSpace[1] }, { 100, 20 });

		CreateCtrl(&m_etParam[2], &m_etParam[0], L"", WS_CHILD | WS_VISIBLE | SS_NOTIFY, this, 13);

		SetCtrlParam(&m_etParam[3], { m_etParam[2].GetResolution().x + nSpace[0], 0 }, { 100, 20 });

		CreateCtrl(&m_etParam[3], &m_etParam[2], L"", WS_CHILD | WS_VISIBLE | SS_NOTIFY, this, 14);

		for (auto& iImParam : m_etParam) iImParam.SetTextColor(0xffffff);
	}

	void CLayouter::OnInitImParamCtrlEx()
	{
		static int nSpace[2] = { -85, 0 };

		for (auto& iImParam : m_stParam) { iImParam.SetTxFormat(DT_RIGHT | DT_VCENTER); SetCtrlParam(&iImParam, { nSpace[0], nSpace[1] }, { 80, 20 }); }

		CreateCtrl(&m_stParam[0], &m_etParam[0], L"w", WS_CHILD | WS_VISIBLE, this, 1);

		CreateCtrl(&m_stParam[1], &m_etParam[1], L"x", WS_CHILD | WS_VISIBLE, this, 2);

		CreateCtrl(&m_stParam[2], &m_etParam[2], L"h", WS_CHILD | WS_VISIBLE, this, 3);

		CreateCtrl(&m_stParam[3], &m_etParam[3], L"y", WS_CHILD | WS_VISIBLE, this, 4);
	}

	void CLayouter::OnInitParamCtrl()
	{
		OnInitImParamCtrl();

		OnInitImParamCtrlEx();
	}

	void CLayouter::OnInitOpacityCtrl()
	{
		static int nSpace[2] = { -85, 20 };

		SetCtrlParam(&m_etOpacity, { 0, m_etParam[2].GetResolution().y + nSpace[1] }, { 100,20 });

		CreateCtrl(&m_etOpacity, &m_etParam[2], L"", WS_CHILD | WS_VISIBLE | SS_NOTIFY, this, 15);

		m_stOpacity.SetTxFormat(DT_RIGHT | DT_VCENTER);

		SetCtrlParam(&m_stOpacity, { nSpace[0], 0 }, { 80, 20 });

		CreateCtrl(&m_stOpacity, &m_etOpacity, L"透明度", WS_CHILD | WS_VISIBLE, this, 5);

		m_etOpacity.SetTextColor(0xffffff);
	}

	void CLayouter::OnInitAngleCtrl()
	{
		static int nSpace[2] = { -85, 20 };

		SetCtrlParam(&m_etAngle, { 0, m_etOpacity.GetResolution().y + nSpace[1] }, { 100,20 });

		CreateCtrl(&m_etAngle, &m_etOpacity, L"", WS_CHILD | WS_VISIBLE | SS_NOTIFY, this, 16);

		m_stAngle.SetTxFormat(DT_RIGHT | DT_VCENTER);

		SetCtrlParam(&m_stAngle, { nSpace[0], 0 }, { 80, 20 });

		CreateCtrl(&m_stAngle, &m_etAngle, L"角度", WS_CHILD | WS_VISIBLE, this, 6);

		m_etAngle.SetTextColor(0xffffff);
	}

	void CLayouter::OnInitScalingCtrl()
	{
		static int nSpace[5] = { 100, -85, 10, 20, -5 };

		SetCtrlParam(&m_etScaling[eCT_HOR], { 0, m_etAngle.GetResolution().y + nSpace[3] }, { 100,20 });

		CreateCtrl(&m_etScaling[eCT_HOR], &m_etAngle, L"", WS_CHILD | WS_VISIBLE | SS_NOTIFY, this, 17);

		SetCtrlParam(&m_etScaling[eCT_VER], { m_etScaling[eCT_HOR].GetResolution().x + nSpace[0], 0 }, { 100, 20 });

		CreateCtrl(&m_etScaling[eCT_VER], &m_etScaling[eCT_HOR], L"", WS_CHILD | WS_VISIBLE | SS_NOTIFY, this, 18);

		m_stScaling.SetTxFormat(DT_RIGHT | DT_VCENTER);

		SetCtrlParam(&m_stScaling, { nSpace[1], 0 }, { 80, 20 });

		CreateCtrl(&m_stScaling, &m_etScaling[eCT_HOR], L"缩放1", WS_CHILD | WS_VISIBLE, this, 7);

		SetCtrlParam(&m_btAssociated, { m_etScaling[eCT_HOR].GetResolution().x + nSpace[2], nSpace[4] }, { 80, 30 });

		CreateCtrl(&m_btAssociated, &m_etScaling[eCT_HOR], L"", WS_CHILD | WS_VISIBLE | SS_NOTIFY, this, 19);

		for (auto& iScale : m_etScaling) iScale.SetTextColor(0xffffff);
	}

	void CLayouter::OnInitMoveCtrl()
	{
		static int nSpace[2] = { 40, 20 };

		SetCtrlParam(&m_btMoving, { nSpace[0], m_stScaling.GetResolution().y + nSpace[1] }, { 80, 36 });

		CreateCtrl(&m_btMoving, &m_stScaling, L"", WS_CHILD | WS_VISIBLE | SS_NOTIFY, this, 20);
	}

	void CLayouter::OnInitScaleCtrl()
	{
		static int nSpace[2] = { 20, 20 };

		SetCtrlParam(&m_btScaling, { m_btMoving.GetResolution().x + nSpace[0], 0 }, { 80, 36 });

		CreateCtrl(&m_btScaling, &m_btMoving, L"", WS_CHILD | WS_VISIBLE | SS_NOTIFY, this, 21);
	}

	void CLayouter::OnInitRotateCtrl()
	{
		static int nSpace[2] = { 20, 20 };

		SetCtrlParam(&m_btRotating, { m_btScaling.GetResolution().x + nSpace[0], 0 }, { 80, 36 });

		CreateCtrl(&m_btRotating, &m_btScaling, L"", WS_CHILD | WS_VISIBLE | SS_NOTIFY, this, 22);
	}

	void CLayouter::OnInitSelectCtrl()
	{
		static int nSpace[2] = { 20, 20 };

		SetCtrlParam(&m_btSelecting, { m_btRotating.GetResolution().x + nSpace[0], 0 }, { 80, 36 });

		CreateCtrl(&m_btSelecting, &m_btRotating, L"", WS_CHILD | WS_VISIBLE | SS_NOTIFY, this, 23);
	}

	void CLayouter::OnInitCopyCtrl()
	{
		static int nSpace[2] = { 20, 20 };

		SetCtrlParam(&m_btCopy, { 0, m_btRotating.GetResolution().y + nSpace[1] }, { 80, 36 });

		CreateCtrl(&m_btCopy, &m_btRotating, L"", WS_CHILD | WS_VISIBLE | SS_NOTIFY, this, 24);
	}

	void CLayouter::OnInitSaveCtrl()
	{
		static int nSpace[2] = { 40, 20 };

		SetCtrlParam(&m_btSave, { m_btSelAll.GetResolution().x + nSpace[0], 0 }, { 80, 36 });

		CreateCtrl(&m_btSave, &m_btSelAll, L"", WS_CHILD | WS_VISIBLE | SS_NOTIFY, this, 25);

/*		static int nSpace[2] = {20, 20};

		SetCtrlParam(&m_btSave, { m_btCopy.GetResolution().x + nSpace[0], 0 }, { 80, 36 });

		CreateCtrl(&m_btSave, &m_btCopy, L"", WS_CHILD | WS_VISIBLE | SS_NOTIFY, this, 25);
*/
	}

	void CLayouter::OnInitOptCtrl()
	{
/*
		OnInitMoveCtrl();

		OnInitScaleCtrl();

		OnInitRotateCtrl();

		OnInitSelectCtrl();

		OnInitCopyCtrl();

		OnInitSaveCtrl();
*/
		OnInitSelAllCtrl();

		OnInitSaveCtrl();
	}
	void CLayouter::OnInitPicCtrl()
	{
		SetCtrlParam(&m_btPicture, { 20, 20 }, { GetChildRect().Width() - 40, GetChildRect().Height() - 40 });

		CreateCtrl(&m_btPicture, &m_stScaling, L"", WS_VISIBLE, this, 26);

		m_btPicture.SetBackdrop(RGB(34, 41, 48));
	}
	void CLayouter::OnInitSelAllCtrl()
	{
		static int nSpace[2] = { 40, 40 };

		SetCtrlParam(&m_btSelAll, { nSpace[0], m_stScaling.GetResolution().y + nSpace[1] }, { 80, 36 });

		CreateCtrl(&m_btSelAll, &m_stScaling, L"", WS_CHILD | WS_VISIBLE | SS_NOTIFY, this, 27);

/*		static int nSpace[2] = {20, 20};

		SetCtrlParam(&m_btSelAll, { 0, m_btMoving.GetResolution().y + nSpace[1] }, { 80, 36 });

		CreateCtrl(&m_btSelAll, &m_btMoving, L"", WS_CHILD | WS_VISIBLE | SS_NOTIFY, this, 27);
*/
	}
	/********************************************************************************************/
	IMPLEMENT_DYNAMIC(CImParam, CLayouter)

	BEGIN_MESSAGE_MAP(CImParam, CLayouter)

	END_MESSAGE_MAP()

	CImParam::CImParam():
		m_lpHandle(nullptr), m_nFocus(0)
	{

	}

	CImParam::~CImParam()
	{

	}

	void CImParam::OnSyncParam(ImLayer::tagLayer& tFocus)
	{		
		if (!isMainView)
		{
			m_nFocus = tFocus.nSign;
			m_etParam[eCT_Left].SetWindowTextEx(to_wstring(tFocus.nLeft).c_str());
			m_etParam[eCT_Top].SetWindowTextEx(to_wstring(tFocus.nTop).c_str());
			m_etParam[eCT_Width].SetWindowTextEx(to_wstring(tFocus.nWidth).c_str());
			m_etParam[eCT_Height].SetWindowTextEx(to_wstring(tFocus.nHeight).c_str());
			m_etOpacity.SetWindowTextEx(to_wstring(tFocus.dOpacity).c_str());
			m_etScaling[eCT_HOR].SetWindowTextEx(to_wstring(tFocus.dScaled[0]).c_str());
			m_etScaling[eCT_VER].SetWindowTextEx(to_wstring(tFocus.dScaled[1]).c_str());
			m_etAngle.SetWindowTextEx(to_wstring(tFocus.dRotated).c_str());
			m_btAssociated.SetCheck(tFocus.bAssociated);
			OptChanged(tFocus.CurOpt());
		}
	}

	void CImParam::OnCreateEx(LPCREATESTRUCT& lpCreateStruct)
	{
		CLayouter::OnCreateEx(lpCreateStruct);

		SetClickedFunc(m_btAssociated.GetCtrlID(), bind(&CImParam::OnClickedAssociated, this));
		SetClickedFunc(m_btMoving.GetCtrlID(), bind(&CImParam::OnClickedMoving, this));
		SetClickedFunc(m_btScaling.GetCtrlID(), bind(&CImParam::OnClickedScaling, this));
		SetClickedFunc(m_btRotating.GetCtrlID(), bind(&CImParam::OnClickedRotating, this));
		SetClickedFunc(m_btSelecting.GetCtrlID(), bind(&CImParam::OnClickedSelecting, this));
		SetClickedFunc(m_btCopy.GetCtrlID(), bind(&CImParam::OnClickedCopy, this));
		SetClickedFunc(m_btSave.GetCtrlID(), bind(&CImParam::OnClickedSave, this));
		SetClickedFunc(m_btSelAll.GetCtrlID(), bind(&CImParam::OnClickedSelAll, this));

		SetChangedFunc(m_etParam[eCT_Left].GetCtrlID(), bind(&CImParam::OnLeftChanged, this));
		SetChangedFunc(m_etParam[eCT_Top].GetCtrlID(), bind(&CImParam::OnTopChanged, this));
		SetChangedFunc(m_etParam[eCT_Width].GetCtrlID(), bind(&CImParam::OnWidthChanged, this));
		SetChangedFunc(m_etParam[eCT_Height].GetCtrlID(), bind(&CImParam::OnHeightChanged, this));
		SetChangedFunc(m_etOpacity.GetCtrlID(), bind(&CImParam::OnOpacityChanged, this));
		SetChangedFunc(m_etScaling[eCT_HOR].GetCtrlID(), bind(&CImParam::OnHorScalChanged, this));
		SetChangedFunc(m_etScaling[eCT_VER].GetCtrlID(), bind(&CImParam::OnVerScalChanged, this));
		SetChangedFunc(m_etAngle.GetCtrlID(), bind(&CImParam::OnAngleChanged, this));

		m_vGroup.push_back(&m_btMoving);		
		m_vGroup.push_back(&m_btScaling);
		m_vGroup.push_back(&m_btRotating);
		m_vGroup.push_back(&m_btSelecting);

		OptChanged(eCBT_Moving);
	}

	void CImParam::OptChanged(int nOpt)
	{
		for (int i = 0; i < m_vGroup.size(); ++i) m_vGroup[i]->SetCheck(nOpt == i);
	}

	void CImParam::OnClickedAssociated()
	{
		ImCtrl::uWParam a;

		a.w = m_btAssociated.GetCheck();

		::PostMessage(m_lpHandle->GetSafeHwnd(), PQM_IM_PARAM_CLICKED_ASSOCIATED, a.w, (LPARAM)m_nFocus);
	}

	void CImParam::OnClickedMoving()
	{
		OptChanged(eCBT_Moving);

		::PostMessage(m_lpHandle->GetSafeHwnd(), PQM_IM_PARAM_CLICKED_MOVING, (WPARAM)0, (LPARAM)m_nFocus);
	}

	void CImParam::OnClickedScaling()
	{
		OptChanged(eCBT_Scaling);

		::PostMessage(m_lpHandle->GetSafeHwnd(), PQM_IM_PARAM_CLICKED_SCALING, (WPARAM)0, (LPARAM)m_nFocus);
	}

	void CImParam::OnClickedRotating()
	{
		OptChanged(eCBT_Rotating);

		::PostMessage(m_lpHandle->GetSafeHwnd(), PQM_IM_PARAM_CLICKED_ROTATING, (WPARAM)0, (LPARAM)m_nFocus);
	}

	void CImParam::OnClickedSelecting()
	{
		OptChanged(eCBT_Selecting);

		::PostMessage(m_lpHandle->GetSafeHwnd(), PQM_IM_PARAM_CLICKED_SELECTING, (WPARAM)0, (LPARAM)m_nFocus);
	}

	void CImParam::OnClickedCopy()
	{
		::PostMessage(m_lpHandle->GetSafeHwnd(), PQM_IM_PARAM_CLICKED_COPY, (WPARAM)0, (LPARAM)m_nFocus);
	}

	void CImParam::OnClickedSave()
	{
		::PostMessage(m_lpHandle->GetSafeHwnd(), PQM_IM_PARAM_CLICKED_SAVE, (WPARAM)0, (LPARAM)m_nFocus);
	}

	void CImParam::OnClickedSelAll()
	{
		::PostMessage(m_lpHandle->GetSafeHwnd(), PQM_IM_PARAM_CLICKED_SELECTALL, (WPARAM)0, (LPARAM)m_nFocus);
	}

	void CImParam::OnLeftChanged()
	{
		ImCtrl::uWParam x;

		wchar_t szbuf[256] = { 0 };

		m_etParam[eCT_Left].GetWindowText(szbuf, 256); x.n = _wtoi(szbuf);

		::PostMessage(m_lpHandle->GetSafeHwnd(), PQM_IM_PARAM_LEFT_CHANGED, x.w, (LPARAM)m_nFocus);
	}

	void CImParam::OnTopChanged()
	{
		ImCtrl::uWParam y;

		wchar_t szbuf[256] = { 0 };

		m_etParam[eCT_Top].GetWindowText(szbuf, 256); y.n = _wtoi(szbuf);

		::PostMessage(m_lpHandle->GetSafeHwnd(), PQM_IM_PARAM_TOP_CHANGED, y.w, (LPARAM)m_nFocus);
	}

	void CImParam::OnWidthChanged()
	{
		ImCtrl::uWParam w;

		wchar_t szbuf[256] = { 0 };

		m_etParam[eCT_Width].GetWindowText(szbuf, 256); w.n = _wtoi(szbuf);

		::PostMessage(m_lpHandle->GetSafeHwnd(), PQM_IM_PARAM_WIDTH_CHANGED, w.w, (LPARAM)m_nFocus);
	}

	void CImParam::OnHeightChanged()
	{
		ImCtrl::uWParam h;

		wchar_t szbuf[256] = { 0 };

		m_etParam[eCT_Height].GetWindowText(szbuf, 256); h.n = _wtoi(szbuf);

		::PostMessage(m_lpHandle->GetSafeHwnd(), PQM_IM_PARAM_HEIGHT_CHANGED, h.w, (LPARAM)m_nFocus);
	}

	void CImParam::OnOpacityChanged()
	{
		ImCtrl::uWParam o;

		wchar_t szbuf[256] = { 0 };

		m_etOpacity.GetWindowText(szbuf, 256); o.d = _wtof(szbuf);

		::PostMessage(m_lpHandle->GetSafeHwnd(), PQM_IM_PARAM_OPACITY_CHANGED, o.w, (LPARAM)m_nFocus);
	}

	void CImParam::OnHorScalChanged()
	{
		ImCtrl::uWParam hor;

		wchar_t szbuf[256] = { 0 };

		m_etScaling[eCT_HOR].GetWindowText(szbuf, 256); hor.d = _wtof(szbuf);

		::PostMessage(m_lpHandle->GetSafeHwnd(), PQM_IM_PARAM_HOR_SCAL_CHANGED, hor.w, (LPARAM)m_nFocus);
	}

	void CImParam::OnVerScalChanged()
	{
		ImCtrl::uWParam ver;

		wchar_t szbuf[256] = { 0 };

		m_etScaling[eCT_VER].GetWindowText(szbuf, 256); ver.d = _wtof(szbuf);

		::PostMessage(m_lpHandle->GetSafeHwnd(), PQM_IM_PARAM_VER_SCAL_CHANGED, ver.w, (LPARAM)m_nFocus);
	}

	void CImParam::OnAngleChanged()
	{
		ImCtrl::uWParam a;

		wchar_t szbuf[256] = { 0 };

		m_etAngle.GetWindowText(szbuf, 256); a.d = _wtof(szbuf);

		::PostMessage(m_lpHandle->GetSafeHwnd(), PQM_IM_PARAM_ANGLE_CHANGED, a.w, (LPARAM)m_nFocus);
	}
}