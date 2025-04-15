#include "pch.h"
#include "CImParam.h"
#include "PQMsg.h"
#include <iostream>
using namespace std;

namespace ImParam
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

	void CLayouter::OnCreateEx(LPCREATESTRUCT& lpCreateStruct)
	{
		SetBackdrop(0);

		OnInitParamCtrl();

		OnInitOpacityCtrl();

		OnInitScalingCtrl();

		OnInitAngleCtrl();

		OnInitOptCtrl();
	}

	void CLayouter::OnInitParamCtrl()
	{
		static int nSpace[2] = { 5, 20 }; // 1920*1080 时 5,5

		array<int, 8> vCtrlId{ 1, 11, 2, 12, 3, 13, 4, 14 };

		array<CString, 8> vTx{ L"w", L"", L"x", L"", L"h", L"", L"y", L"" };

		array<int, 2> vResolution[8] = { {20,20},{100,20},{20,20}, {100, 20}, {20,20},{100,20},{20,20}, {100, 20} };

		array<int, 2> vSpace[8] = {
			{nSpace[1], nSpace[1]}, {vResolution[0][0] + nSpace[0], 0}, {vResolution[1][0] + nSpace[0] * 8, 0}, {vResolution[2][0] + nSpace[0], 0},
			{0, vResolution[0][1] + nSpace[1] }, {0, vResolution[1][1] + nSpace[1] }, {0, vResolution[2][1] + nSpace[1] }, {0, vResolution[3][1] + nSpace[1] } };

		CImCtrl* lpCtrl[8] = { &m_stParam[0], &m_etParam[0], &m_stParam[1], &m_etParam[1], &m_stParam[2], &m_etParam[2], &m_stParam[3], &m_etParam[3] };

		for (int y = 0; y < 2; ++y)
		{
			for (int x = 0; x < 4; ++x)
			{
				int idx = y * 4 + x;

				int idx2 = (!!y ? idx - 4 : idx - 1);

				idx2 = (!!idx * idx2);

				unsigned long uStyle = WS_CHILD | WS_VISIBLE; uStyle |= (x % 2 * SS_NOTIFY);

				SetCtrlParam(lpCtrl[idx], vSpace[idx], vResolution[idx]);

				CreateCtrl(lpCtrl[idx], (idx | idx2) ? lpCtrl[idx2] : nullptr, vTx[idx], uStyle, this, vCtrlId[idx]);
			}
		}
	}

	void CLayouter::OnInitOpacityCtrl()
	{
		static int nSpace[2] = { 5, 20 }; // 1920*1080 时 5,5

		SetCtrlParam(&m_stOpacity, { 0, m_stParam[2].GetResolution().y + nSpace[1] }, { 70,20 });

		SetCtrlParam(&m_etOpacity, { m_stOpacity.GetResolution().x + nSpace[0], 0 }, { 100,20 });

		CreateCtrl(&m_stOpacity, &m_stParam[2], L"透明度", WS_CHILD | WS_VISIBLE, this, 5);

		CreateCtrl(&m_etOpacity, &m_stOpacity, L"", WS_CHILD | WS_VISIBLE | SS_NOTIFY, this, 15);
	}

	void CLayouter::OnInitScalingCtrl()
	{
		static int nSpace[2] = { 5, 20 }; // 1920*1080 时 5,5

		SetCtrlParam(&m_stScaling, { 0, m_stOpacity.GetResolution().y + nSpace[1] }, { 50,20 });

		SetCtrlParam(&m_etScaling[eCT_HOR], { m_stScaling.GetResolution().x + nSpace[0], 0 }, { 100,20 });

		SetCtrlParam(&m_btAssociated, { m_etScaling[eCT_HOR].GetResolution().x + nSpace[0], 0 }, { 30,20 });

		SetCtrlParam(&m_etScaling[eCT_VER], { m_btAssociated.GetResolution().x + nSpace[0], 0 }, { 100,20 });

		m_btAssociated.SetTxFormat(DT_CENTER | DT_TOP); m_btAssociated.SetTextColor(0);

		CreateCtrl(&m_stScaling, &m_stOpacity, L"缩放", WS_CHILD | WS_VISIBLE, this, 6);

		CreateCtrl(&m_etScaling[eCT_HOR], &m_stScaling, L"", WS_CHILD | WS_VISIBLE | SS_NOTIFY, this, 16);

		CreateCtrl(&m_btAssociated, &m_etScaling[0], L"&&", WS_CHILD | WS_VISIBLE | SS_NOTIFY, this, 17);

		CreateCtrl(&m_etScaling[eCT_VER], &m_btAssociated, L"", WS_CHILD | WS_VISIBLE | SS_NOTIFY, this, 18);
	}

	void CLayouter::OnInitAngleCtrl()
	{
		static int nSpace[2] = { 5, 20 }; // 1920*1080 时 5,5

		SetCtrlParam(&m_stAngle, { 0, m_stScaling.GetResolution().y + nSpace[1] }, { 50,20 });

		SetCtrlParam(&m_etAngle, { m_stAngle.GetResolution().x + nSpace[0],0 }, { 100,20 });

		CreateCtrl(&m_stAngle, &m_stScaling, L"角度", WS_CHILD | WS_VISIBLE, this, 9);

		CreateCtrl(&m_etAngle, &m_stAngle, L"", WS_CHILD | WS_VISIBLE | SS_NOTIFY, this, 19);
	}

	void CLayouter::OnInitOptCtrl()
	{
		static int nSpace[2] = { 20, 20 }; // 1920*1080 时 5,5

		array<int, 6> vCtrlId{ 20,21,22,23,24,25 };

		array<CString, 6> vTx{ L"移动", L"缩放", L"旋转", L"框选", L"拷贝", L"保存" };

		array<int, 2> vResolution{ 100,20 };

		array<int, 2> vSpace[6] = {
			{0, m_stAngle.GetResolution().y + nSpace[1] }, {vResolution[0] + nSpace[0], 0}, {vResolution[0] + nSpace[0], 0},
			{0, vResolution[1] + nSpace[1]},{0, vResolution[1] + nSpace[1]},{0, vResolution[1] + nSpace[1]} };

		CImCtrl* lpCtrl[6] = { &m_btMoving, &m_btScaling, &m_btRotating, &m_btSelecting, &m_btCopy, &m_btSave };

		for (int y = 0; y < 2; ++y)
		{
			for (int x = 0; x < 3; ++x)
			{
				int idx = y * 3 + x;

				int idx2 = (!!y ? idx - 3 : idx - 1);

				idx2 = (!!idx * idx2);

				SetCtrlParam(lpCtrl[idx], vSpace[idx], vResolution);

				lpCtrl[idx]->SetTxFormat(DT_CENTER | DT_TOP); lpCtrl[idx]->SetTextColor(0);

				CreateCtrl(lpCtrl[idx], (idx | idx2) ? lpCtrl[idx2] : &m_stAngle, vTx[idx], WS_CHILD | WS_VISIBLE | SS_NOTIFY, this, vCtrlId[idx]);
			}
		}
	}
	/********************************************************************************************/
	IMPLEMENT_DYNAMIC(CImParam, CLayouter)

	BEGIN_MESSAGE_MAP(CImParam, CLayouter)

	END_MESSAGE_MAP()

	CImParam::CImParam()
	{

	}

	CImParam::~CImParam()
	{

	}

	void CImParam::SetOpacity(int nOpacity)
	{
		m_etOpacity.SetWindowTextEx(to_wstring(nOpacity).c_str());
	}

	void CImParam::SetScaling(int nScaling)
	{
	//	m_etScaling.SetWindowTextEx(to_wstring(nScaling).c_str());
	}

	void CImParam::SetTL(std::array<int, 2> vTL)
	{
		if (GetFocus() != m_etParam[1].GetCtrlItem()) m_etParam[1].SetWindowTextEx(to_wstring(vTL[0]).c_str());

		if (GetFocus() != m_etParam[3].GetCtrlItem()) m_etParam[3].SetWindowTextEx(to_wstring(vTL[1]).c_str());
	}

	void CImParam::SetWH(std::array<int, 2> vWH)
	{
		if (GetFocus() != m_etParam[0].GetCtrlItem()) m_etParam[0].SetWindowTextEx(to_wstring(vWH[0]).c_str());

		if (GetFocus() != m_etParam[2].GetCtrlItem()) m_etParam[2].SetWindowTextEx(to_wstring(vWH[1]).c_str());
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

		SetChangedFunc(m_etParam[eCT_X].GetCtrlID(), bind(&CImParam::OnXChanged, this));
		SetChangedFunc(m_etParam[eCT_Y].GetCtrlID(), bind(&CImParam::OnYChanged, this));
		SetChangedFunc(m_etParam[eCT_W].GetCtrlID(), bind(&CImParam::OnWChanged, this));
		SetChangedFunc(m_etParam[eCT_H].GetCtrlID(), bind(&CImParam::OnHChanged, this));
		SetChangedFunc(m_etOpacity.GetCtrlID(), bind(&CImParam::OnOpacityChanged, this));
		SetChangedFunc(m_etScaling[eCT_HOR].GetCtrlID(), bind(&CImParam::OnHorScalingChanged, this));
		SetChangedFunc(m_etScaling[eCT_VER].GetCtrlID(), bind(&CImParam::OnVerrScalingChanged, this));
		SetChangedFunc(m_etAngle.GetCtrlID(), bind(&CImParam::OnAngleChanged, this));
	}

	void CImParam::OnClickedAssociated()
	{
		::PostMessage(GetParent()->GetSafeHwnd(), PQM_IM_PARAM_CLICKED_ASSOCIATED, (WPARAM)0, (LPARAM)0);
	}

	void CImParam::OnClickedMoving()
	{
		::PostMessage(GetParent()->GetSafeHwnd(), PQM_IM_PARAM_CLICKED_MOVING, (WPARAM)0, (LPARAM)0);
	}

	void CImParam::OnClickedScaling()
	{
		::PostMessage(GetParent()->GetSafeHwnd(), PQM_IM_PARAM_CLICKED_SCALING, (WPARAM)0, (LPARAM)0);
	}

	void CImParam::OnClickedRotating()
	{
		::PostMessage(GetParent()->GetSafeHwnd(), PQM_IM_PARAM_CLICKED_ROTATING, (WPARAM)0, (LPARAM)0);
	}

	void CImParam::OnClickedSelecting()
	{
		::PostMessage(GetParent()->GetSafeHwnd(), PQM_IM_PARAM_CLICKED_SELECTING, (WPARAM)0, (LPARAM)0);
	}

	void CImParam::OnClickedCopy()
	{
		::PostMessage(GetParent()->GetSafeHwnd(), PQM_IM_PARAM_CLICKED_COPY, (WPARAM)0, (LPARAM)0);
	}

	void CImParam::OnClickedSave()
	{
		::PostMessage(GetParent()->GetSafeHwnd(), PQM_IM_PARAM_CLICKED_SAVE, (WPARAM)0, (LPARAM)0);
	}

	void CImParam::OnXChanged()
	{
		wchar_t szbuf[256] = { 0 };

		m_etParam[eCT_X].GetWindowText(szbuf, 256); int x = _wtoi(szbuf);

		::PostMessage(GetParent()->GetSafeHwnd(), PQM_IM_PARAM_LEFT_CHANGED, (WPARAM)0, (LPARAM)x);
	}

	void CImParam::OnYChanged()
	{
		wchar_t szbuf[256] = { 0 };

		m_etParam[eCT_Y].GetWindowText(szbuf, 256); int y = _wtoi(szbuf);

		::PostMessage(GetParent()->GetSafeHwnd(), PQM_IM_PARAM_TOP_CHANGED, (WPARAM)0, (LPARAM)y);
	}

	void CImParam::OnWChanged()
	{
		wchar_t szbuf[256] = { 0 };

		m_etParam[eCT_W].GetWindowText(szbuf, 256); int w = _wtoi(szbuf);

		::PostMessage(GetParent()->GetSafeHwnd(), PQM_IM_PARAM_WIDTH_CHANGED, (WPARAM)0, (LPARAM)w);
	}

	void CImParam::OnHChanged()
	{
		wchar_t szbuf[256] = { 0 };

		m_etParam[eCT_H].GetWindowText(szbuf, 256); int h = _wtoi(szbuf);

		::PostMessage(GetParent()->GetSafeHwnd(), PQM_IM_PARAM_HEIGHT_CHANGED, (WPARAM)0, (LPARAM)h);
	}

	void CImParam::OnOpacityChanged()
	{
		wchar_t szbuf[256] = { 0 };

		m_etOpacity.GetWindowText(szbuf, 256); int o = _wtoi(szbuf);

		::PostMessage(GetParent()->GetSafeHwnd(), PQM_IM_PARAM_OPACITY_CHANGED, 0, (LPARAM)o);
	}

	void CImParam::OnHorScalingChanged()
	{
		wchar_t szbuf[256] = { 0 };

		m_etScaling[eCT_HOR].GetWindowText(szbuf, 256); int hor = _wtoi(szbuf);

		::PostMessage(GetParent()->GetSafeHwnd(), PQM_IM_PARAM_HOR_SCAL_CHANGED, 0, (LPARAM)hor);
	}

	void CImParam::OnVerrScalingChanged()
	{
		wchar_t szbuf[256] = { 0 };

		m_etScaling[eCT_VER].GetWindowText(szbuf, 256); int ver = _wtoi(szbuf);

		::PostMessage(GetParent()->GetSafeHwnd(), PQM_IM_PARAM_VER_SCAL_CHANGED, 0, (LPARAM)ver);
	}

	void CImParam::OnAngleChanged()
	{
		wchar_t szbuf[256] = { 0 };

		m_etAngle.GetWindowText(szbuf, 256); int a = _wtoi(szbuf);

		::PostMessage(GetParent()->GetSafeHwnd(), PQM_IM_PARAM_ANGLE_CHANGED, 0, (LPARAM)a);
	}
}