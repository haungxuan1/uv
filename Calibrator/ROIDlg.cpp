#include "pch.h"
#include "ROIDlg.h"
#include "PQMsg.h"
#include "Helper.h"

IMPLEMENT_DYNAMIC(CROIDlg, CMergeDlg)

BEGIN_MESSAGE_MAP(CROIDlg, CMergeDlg)
	ON_MESSAGE(PQM_CALIBRATOR_PRO_ROI, OnProROIMsg)
	ON_MESSAGE(PQM_CALIBRATOR_ROI, OnROIMsg)
END_MESSAGE_MAP()

CROIDlg::CROIDlg(CString rDir, CWnd* pParent)
	:CMergeDlg(rDir, pParent)
{

}

CROIDlg::~CROIDlg()
{

}

void CROIDlg::DoDataExchange(CDataExchange* pDX)
{
	CMergeDlg::DoDataExchange(pDX);

}

BOOL CROIDlg::OnInitDialog()
{
	CMergeDlg::OnInitDialog();

	return TRUE;
}

void CROIDlg::OnDisableCtrl()
{
	CMergeDlg::OnDisableCtrl();
}

void CROIDlg::OnEnableCtrl()
{
	CMergeDlg::OnEnableCtrl();
}

void CROIDlg::OnPrROI()
{
	UpdateData(TRUE);

	m_rDir[0] = m_rBaseDir;

	m_rDir[1] = m_rBaseDir;

	m_rDir[2] = m_rBaseDir + L"\\cache";

	m_cParam.nErode = 75;

	m_cParam.nDilate = 100;

	UpdateData(FALSE);

	UpdateData(TRUE);
}

LRESULT CROIDlg::OnProROIMsg(WPARAM wparam, LPARAM lparam)
{
	m_cImControl.ClearLayer();

	OnPrROI();

	vector<array<wstring, 2>> vFile;

	vFile.push_back({ L"ROIMerged.png",  CString2WString(m_rDir[0] + L"\\ROIMerged.png") });

	SetWorkType(eWT_ROI);

	SetROIParam(
		vFile, 
		{ m_cParam.nForeground, m_cParam.nErode, m_cParam.nDilate },  
		CString2String(m_rDir[2]));

	Reg();

	return 0;
}

void CROIDlg::OnROI(ImPtr::CImPtr cImPtr, array<Point2f, 4> vVertex)
{
	PostMessage(PQM_CALIBRATOR_ROI, (WPARAM)new array<Point2f, 4>(vVertex), (LPARAM)new ImPtr::CImPtr(cImPtr));
}

LRESULT CROIDlg::OnROIMsg(WPARAM wparam, LPARAM lparam)
{
	array<Point2f, 4>* lpVertex = (array<Point2f, 4>*)wparam;

	ImPtr::CImPtr* lpPtr = (ImPtr::CImPtr*)lparam;

	m_cImControl.ClearLayer();

	m_cImControl.CreateLayer(true, true, false, "", true, 1.0, L"–À»§«¯”Ú", *lpPtr, true, 0.0, 0, 0);

	m_cParam.vVertex1 = *lpVertex;

	++m_cParam.nType;

	Unreg();

	delete lpVertex; delete lpPtr;

	PostMessage(PQM_CALIBRATOR_PARAM_CHANGED);

	PostMessage(PQM_CALIBRATOR_TYPE_CHANGED);

	PostMessage(PQM_CALIBRATOR_PRO_PRESPECTIVE);

	return 0;
}
