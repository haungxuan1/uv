#include "pch.h"
#include "AlignDlg.h"
#include "PQMsg.h"
#include "Helper.h"

IMPLEMENT_DYNAMIC(CAlignDlg, CReferenceDlg)

BEGIN_MESSAGE_MAP(CAlignDlg, CReferenceDlg)
	ON_MESSAGE(PQM_CALIBRATOR_PRO_ALIGN, OnProAlignMsg)
	ON_MESSAGE(PQM_CALIBRATOR_ALIGNING, OnAligningMsg)
	ON_MESSAGE(PQM_CALIBRATOR_ALIGN, OnAlignMsg)
END_MESSAGE_MAP()

CAlignDlg::CAlignDlg(CString rDir, CWnd* pParent)
	:CReferenceDlg(rDir, pParent)
{

}

CAlignDlg::~CAlignDlg()
{

}

void CAlignDlg::DoDataExchange(CDataExchange* pDX)
{
	CReferenceDlg::DoDataExchange(pDX);

	DDX_Text(pDX, IDC_ET_PADDING_STRIDE, m_cParam.nPixelInPass);

	DDX_Text(pDX, IDC_ET_PADDING_PASS_IN_STRIDE, m_cParam.nPassInStride);

	DDX_Text(pDX, IDC_ET_PADDING_IM_FRAME_W1, m_cParam.vWH1[0]);

	DDX_Text(pDX, IDC_ET_PADDING_IM_FRAME_H1, m_cParam.vWH1[1]);

	DDX_Text(pDX, IDC_ET_PADDING_IM_FRAME_W2, m_cParam.vWH2[0]);

	DDX_Text(pDX, IDC_ET_PADDING_IM_FRAME_H2, m_cParam.vWH2[1]);


	DDX_Control(pDX, IDC_ET_PADDING_STRIDE, m_etStride);

	DDX_Control(pDX, IDC_ET_PADDING_PASS_IN_STRIDE, m_etPassInStride);

	DDX_Control(pDX, IDC_ET_PADDING_IM_FRAME_W1, m_etImFrame[0]);

	DDX_Control(pDX, IDC_ET_PADDING_IM_FRAME_H1, m_etImFrame[1]);

	DDX_Control(pDX, IDC_ET_PADDING_IM_FRAME_W2, m_etImFrame[2]);

	DDX_Control(pDX, IDC_ET_PADDING_IM_FRAME_H2, m_etImFrame[3]);
}

BOOL CAlignDlg::OnInitDialog()
{
	CReferenceDlg::OnInitDialog();

	return TRUE;
}

void CAlignDlg::OnDisableCtrl()
{
	CReferenceDlg::OnDisableCtrl();
}

void CAlignDlg::OnEnableCtrl()
{
	CReferenceDlg::OnEnableCtrl();
}

void CAlignDlg::OnPrAlign()
{
	UpdateData(TRUE);

	m_rDir[0] = m_rBaseDir + L"\\camera";

	m_rDir[1] = m_rBaseDir;

	m_rDir[2] = m_rBaseDir + L"\\cache";

	m_cParam.nErode = 75;

	m_cParam.nDilate = 100;

	UpdateData(FALSE);

	UpdateData(TRUE);
}

LRESULT CAlignDlg::OnProAlignMsg(WPARAM wparam, LPARAM lparam)
{
	m_cImControl.ClearLayer();

	OnPrAlign();

	m_lpHistogram->SetName(L"");

	m_lpSplitPoint->SetName(L"²ÎÕÕÎï¶ÔÆë");

	m_lpHistogram->ClearSerie();

	m_lpSplitPoint->ClearSerie();

	vector<array<wstring, 2>> vFile;

	FileInDir(m_rDir[0].GetBuffer(), vFile);

	SortingFiles(vFile);

	SetWorkType(eWT_Aligning);

	SetAligningParam(vFile, 
		{ 
		max(int(ceil(m_cParam.vWH1[0])), int(ceil(m_cParam.vWH2[0]))),
		max(int(ceil(m_cParam.vWH1[1])), int(ceil(m_cParam.vWH2[1]))),
		m_cParam.nExtTriggerFrameCount,
		m_cParam.nPixelInPass,
		m_cParam.nReference[0],
		m_cParam.nReference[1],
		m_cParam.nReference[2],
		m_cParam.nReference[3] },

		{ m_cParam.nForeground,
		(int)(sqrt(pow(m_cParam.nReference[2], 2) + pow(m_cParam.nReference[3], 2))),
		m_cParam.nPixelInPass,
		32 * 2,
		m_cParam.nErode,
		m_cParam.nDilate},
		
		Transform::CString2String(m_rDir[2] + L"\\Aligned.png"));

	Reg();

	return 0;
}

void CAlignDlg::OnAligning(int nRemaining, double dScore, ImPtr::CImPtr cImPtr)
{
	tuple<int, double, ImPtr::CImPtr>* lpParam = new  tuple<int, double, ImPtr::CImPtr>(nRemaining, dScore, cImPtr);

	PostMessage(PQM_CALIBRATOR_ALIGNING, 0, (LPARAM)lpParam);
}

void CAlignDlg::OnAlign(int nRemaining, double dScore, ImPtr::CImPtr cImPtr, int nPassInStride)
{
	tuple<int, double, ImPtr::CImPtr>* lpParam = new  tuple<int, double, ImPtr::CImPtr>(nRemaining, dScore, cImPtr);

	PostMessage(PQM_CALIBRATOR_ALIGN, (WPARAM)nPassInStride, (LPARAM)lpParam);
}

LRESULT CAlignDlg::OnAligningMsg(WPARAM wparam, LPARAM lparam)
{
	tuple<int, double, ImPtr::CImPtr>* lpParam = (tuple<int, double, ImPtr::CImPtr>*)lparam;

	m_cImControl.ClearLayer();

	m_cImControl.CreateLayer(true, true, false, "", true, 1.0, WString2CString(to_wstring(get<0>(*lpParam))), get<2>(*lpParam), true, 0.0, 0, 0);

	m_lpSplitPoint->AddPoint(get<0>(*lpParam), get<1>(*lpParam));

	delete lpParam;

	return 0;
}

LRESULT CAlignDlg::OnAlignMsg(WPARAM wparam, LPARAM lparam)
{
	m_cParam.nPassInStride = (int)wparam;

	OnAligningMsg(wparam, lparam);

	++m_cParam.nType;

	Unreg();

	UpdateData(FALSE);

	PostMessage(PQM_CALIBRATOR_PARAM_CHANGED);

	PostMessage(PQM_CALIBRATOR_TYPE_CHANGED);

	PostMessage(m_cParam.nType == CCameraParam::eCT_Merge ? PQM_CALIBRATOR_PRO_MERGE : PQM_CALIBRATOR_PRO_MERGE2);

	return 0;
}
