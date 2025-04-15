#include "pch.h"
#include "PrespectiveDlg.h"
#include "PQMsg.h"
#include "Helper.h"

IMPLEMENT_DYNAMIC(CPrespectiveDlg, CROIDlg)

BEGIN_MESSAGE_MAP(CPrespectiveDlg, CROIDlg)
	ON_MESSAGE(PQM_CALIBRATOR_PRO_PRESPECTIVE, OnProPerspectiveMsg)
	ON_MESSAGE(PQM_CALIBRATOR_PERSPECTIVE, OnPerspectiveMsg)
END_MESSAGE_MAP()

CPrespectiveDlg::CPrespectiveDlg(CString rDir, CWnd* pParent)
	:CROIDlg(rDir, pParent)
{

}

CPrespectiveDlg::~CPrespectiveDlg()
{

}

void CPrespectiveDlg::DoDataExchange(CDataExchange* pDX)
{
	CROIDlg::DoDataExchange(pDX);
}

BOOL CPrespectiveDlg::OnInitDialog()
{
	CROIDlg::OnInitDialog();

	return TRUE;
}

void CPrespectiveDlg::OnDisableCtrl()
{
	CROIDlg::OnDisableCtrl();
}

void CPrespectiveDlg::OnEnableCtrl()
{
	CROIDlg::OnEnableCtrl();
}

void CPrespectiveDlg::OnPrPerspective()
{
	UpdateData(TRUE);

	m_rDir[0] = m_rBaseDir;

	m_rDir[1] = m_rBaseDir;

	m_rDir[2] = m_rBaseDir + L"\\cache";

	UpdateData(FALSE);

	UpdateData(TRUE);
}

LRESULT CPrespectiveDlg::OnProPerspectiveMsg(WPARAM wparam, LPARAM lparam)
{
	m_cImControl.ClearLayer();

	OnPrPerspective();

	SetPerspectiveParam(m_cParam.vWH1, m_cParam.vVertex1);

	return 0;
}

void CPrespectiveDlg::OnPerspective(
	array<double, 9> vM,
	array<int, 4> vFill,
	array<float, 2> vImWH2,
	array<Point2f, 4> vVertex2)
{
	tuple<array<double, 9>, array<int, 4>, array<float, 2>, array<Point2f, 4>>* lpParam = 
		new tuple<array<double, 9>, array<int, 4>, array<float, 2>, array<Point2f, 4>>(vM, vFill, vImWH2, vVertex2);

	PostMessage(PQM_CALIBRATOR_PERSPECTIVE, 0, (LPARAM)lpParam);
}

LRESULT CPrespectiveDlg::OnPerspectiveMsg(WPARAM wparam, LPARAM lparam)
{
	tuple<array<double, 9>, array<int, 4>, array<float, 2>, array<Point2f, 4>>* lpParam =
		(tuple<array<double, 9>, array<int, 4>, array<float, 2>, array<Point2f, 4>>*)lparam;

	m_cParam.M = get<0>(*lpParam);

	m_cParam.vFill = get<1>(*lpParam);

	m_cParam.vWH2 = get<2>(*lpParam);

	m_cParam.vVertex2 = get<3>(*lpParam);

	++m_cParam.nType;

	UpdateData(FALSE);

	delete lpParam;

	PostMessage(PQM_CALIBRATOR_PARAM_CHANGED);

	PostMessage(PQM_CALIBRATOR_TYPE_CHANGED);

	PostMessage(PQM_CALIBRATOR_PRO_HOMOGRAPHY);

	return 0;
}
