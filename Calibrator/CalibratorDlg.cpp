#include "pch.h"
#include "CalibratorDlg.h"
#include <numeric>
#include <iostream>
#include <math.h>
#include <cmath>
#include "PQMsg.h"
#include "PQSetting.h"
#include "Helper.h"
#include "Transform.h"

IMPLEMENT_DYNAMIC(CCalibratorDlg, CChartDlg)

BEGIN_MESSAGE_MAP(CCalibratorDlg, CChartDlg)
	ON_BN_CLICKED(IDC_BT_CALIBRATION_SAVE, OnClickedSave)
	ON_BN_CLICKED(IDC_BT_PADDING_DIR_INPUT, OnClickedInputDir)
	ON_BN_CLICKED(IDC_BT_PADDING_DIR_OUTPUT, OnClickedOutputDir)
	ON_BN_CLICKED(IDC_BT_PADDING_DIR_CACHE, OnClickedCacheDir)
	ON_BN_CLICKED(IDC_BT_PADDING_DIR_PARAM, OnClickedParamDir)
	ON_BN_CLICKED(IDC_BT_CALIBRATION_PARAM, OnClickedParam)

	ON_MESSAGE(PQM_CALIBRATOR_LOAD_PARAM, OnLoadParamMsg)
	ON_MESSAGE(PQM_CALIBRATOR_PARAM_CHANGED, OnParamChangedMsg)
	ON_MESSAGE(PQM_CALIBRATOR_TYPE_CHANGED, OnTypeChangedMsg)
	ON_MESSAGE(PQM_CALIBRATOR_PARAM_BEGIN, OnParamBeginMsg)
	ON_MESSAGE(PQM_CALIBRATOR_PARAM_END, OnParamEndMsg)
	ON_MESSAGE(PQM_CALIBRATOR_DISABLE, OnDisableCtrlMsg)
	ON_MESSAGE(PQM_CALIBRATOR_ENABLE, OnEnableCtrlMsg)
	ON_WM_DESTROY()
END_MESSAGE_MAP()

// 校准模块
// 图片逆时针旋转90°后参与计算

CCalibratorDlg::CCalibratorDlg(CString rDir, CWnd* pParent)
	: CChartDlg(rDir, pParent)
{

}

CCalibratorDlg::~CCalibratorDlg()
{

}

void CCalibratorDlg::Mem2Str(lpFiler mlp, unsigned char*& mem, int& size)
{

}

void CCalibratorDlg::DoDataExchange(CDataExchange* pDX)
{
	CChartDlg::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_BT_CALIBRATION_PARAM, m_btCalibrator);

	DDX_Control(pDX, IDC_BT_CALIBRATION_SAVE, m_btSave);

	DDX_Control(pDX, IDC_BT_PADDING_DIR_PARAM, m_btParam);

	DDX_Control(pDX, IDC_ET_PADDING_DIR_PARAM, m_etParam);

	DDX_Text(pDX, IDC_ET_PADDING_DIR_PARAM, m_rParam);

	DDX_Control(pDX, IDC_ET_CALIBRATION_TYPE, m_etType);

	DDX_Text(pDX, IDC_ET_CALIBRATION_TYPE, m_rType);
}

BOOL CCalibratorDlg::OnInitDialog()
{
	CChartDlg::OnInitDialog();

	CRect rt;

	::GetWindowRect(GetDlgItem(IDC_ST_VIDEO)->GetSafeHwnd(), rt);

	ScreenToClient(rt);

	m_cImControl.SetLayerDir(CString2String(m_rBaseDir));

	m_cImControl.SetIcon(L"D:\\ImControl");

	m_cImControl.Create(L"", WS_CHILD | WS_VISIBLE | SS_NOTIFY, CRect(rt.left, rt.top, rt.right, rt.bottom), this, IDC_ST_VIDEO);

	return TRUE;
}

void CCalibratorDlg::OnDestroy()
{
	CChartDlg::OnDestroy();

	SendMessage(PQM_CALIBRATOR_PRO_FREEZE);

	m_cImControl.ClearLayer();

	Unreg();
}

void CCalibratorDlg::OnDisableCtrl()
{
	m_btSave.EnableWindow(FALSE);

	m_btParam.EnableWindow(FALSE);

	m_btCalibrator.EnableWindow(FALSE);

	CChartDlg::OnDisableCtrl();
}

void CCalibratorDlg::OnEnableCtrl()
{
	CChartDlg::OnEnableCtrl();

	m_btSave.EnableWindow(TRUE);

	m_btParam.EnableWindow(TRUE);

	m_btCalibrator.EnableWindow(TRUE);
}

LRESULT CCalibratorDlg::OnEnableCtrlMsg(WPARAM wparam, LPARAM lparam)
{
	CCalibratorDlg::OnEnableCtrl();

	return 0;
}

LRESULT CCalibratorDlg::OnDisableCtrlMsg(WPARAM wparam, LPARAM lparam)
{
	CCalibratorDlg::OnDisableCtrl();

	return 0;
}

LRESULT CCalibratorDlg::OnTypeChangedMsg(WPARAM wparam, LPARAM lparam)
{
	static vector<CString> vType{ 
		L"前景分割", 
		L"参照物定位", 
		L"图片对齐",
		L"拼合图像",
		L"感兴趣区域",
		L"倾斜校正",
		L"单应性变换",
		L"参照物定位", 
		L"图片对齐", 
		L"拼合图像", 
		L"DPI校正"};

	m_rType = vType[m_cParam.nType];

	UpdateData(FALSE);

	return 0;
}

LRESULT CCalibratorDlg::OnParamBeginMsg(WPARAM wparam, LPARAM lparam)
{
	static vector<int> vMsg
	{
		PQM_CALIBRATOR_PRO_FOREGROUND,
		PQM_CALIBRATOR_PRO_REFERENCE,
		PQM_CALIBRATOR_PRO_ALIGN,
		PQM_CALIBRATOR_PRO_MERGE,
		PQM_CALIBRATOR_PRO_ROI,
		PQM_CALIBRATOR_PRO_PRESPECTIVE,
		PQM_CALIBRATOR_PRO_HOMOGRAPHY,
		PQM_CALIBRATOR_PRO_REFERENCE,
		PQM_CALIBRATOR_PRO_ALIGN,
		PQM_CALIBRATOR_PRO_MERGE2,
		PQM_CALIBRATOR_PRO_DPI
	};
	PostMessage(PQM_CALIBRATOR_DISABLE); // 禁用控件

	PostMessage(PQM_CALIBRATOR_PRO_FREEZE); // 停止相机采集

	PostMessage(PQM_CALIBRATOR_TYPE_CHANGED);

	PostMessage(vMsg[m_cParam.nType]);

	return 0;
}

LRESULT CCalibratorDlg::OnParamEndMsg(WPARAM wparam, LPARAM lparam)
{
	if (m_cParam.nType != CCameraParam::eCT_Param) { AfxMessageBox(L"相机校准执行失败"); return 0; }

	m_cImControl.ClearLayer();

	PostMessage(PQM_CALIBRATOR_ENABLE);

	return 0;
}

void CCalibratorDlg::OnClickedParam()
{
	if (m_cParam.nType != CCameraParam::eCT_Param) { PostMessage(PQM_CALIBRATOR_PARAM_BEGIN); return; }

	AfxMessageBox(L"当前参数已完成相机校正");
}

// 保存校准参数
void CCalibratorDlg::OnClickedSave()
{
	SendMessage(PQM_CALIBRATOR_PARAM_CHANGED);

	AfxMessageBox(L"save");
}

LRESULT CCalibratorDlg::OnLoadParamMsg(WPARAM wparam, LPARAM lparam)
{
	LoadParam(CString2String(m_rParam), m_cParam);

	UpdateData(FALSE);

	return 0;
}



bool CCalibratorDlg::LoadParam(string strFile, tagCalibrator& tParam)
{
	Mat cParam = imread(strFile, 0);
	if (!cParam.empty())
	{
		memcpy(&tParam, cParam.data, (int64)cParam.cols * cParam.rows);

		return true;
	}
	else
		return false;
}

// t = (d2-d1)/v2-v1
// d2 - d1 = 2mm? - 0.25mm
// v2-v1= 900mm/s
// t = 2.22ms
// 
// v1 - 校准时打印速度
// v2 - 当前打印速度
int CCalibratorDlg::DelayCompensation(double v1, double v2, double dCamDPI)
{
	static double s = 2.5964 / 900.0; // 光电触发延时 = 2.5mm

	double d = (v2 - v1) * s;

	double c = d * dCamDPI / 25.4;

	return c;
}

LRESULT CCalibratorDlg::OnParamChangedMsg(WPARAM wparam, LPARAM lparam)
{
	UpdateData(TRUE);

//	uchar* data;

//	memcpy(data, &m_cParam, sizeof(tagCalibrator));



	/////////////////////////////////////////////////////////////

	Mat cParam(1, sizeof(tagCalibrator), CV_8UC1);

	memcpy(cParam.data, &m_cParam, sizeof(tagCalibrator));

	imwrite(CString2String(m_rDir[2] + L"\\param.png"), cParam);

	return 0;
}

void CCalibratorDlg::OnClickedInputDir()
{
	OnClickedDir(0);
}

void CCalibratorDlg::OnClickedOutputDir()
{
	OnClickedDir(1);
}

void CCalibratorDlg::OnClickedCacheDir()
{
	OnClickedDir(2);
}

void CCalibratorDlg::OnClickedParamDir()
{
	CString rDir = m_rBaseDir + L"\\";

	CFileDialog dlg(TRUE, NULL, m_rBaseDir, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, L"Param (*.png)|*.png||");

	if (dlg.DoModal() != IDOK) return;

	m_rParam = dlg.GetPathName();

	UpdateData(FALSE);

	PostMessage(PQM_CALIBRATOR_LOAD_PARAM);
}

void CCalibratorDlg::OnClickedDir(int nIdx)
{
	BROWSEINFO bi = { 0 };

	wchar_t szDir[MAX_PATH] = { 0 };

	memcpy(szDir, m_rDir[nIdx].GetBuffer(), m_rDir[nIdx].GetLength() * sizeof(wchar_t));

	bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_DONTGOBELOWDOMAIN | BIF_NONEWFOLDERBUTTON;

	bi.lpfn = BrowseForFolderCallback;

	bi.lParam = (LPARAM)szDir;

	bi.hwndOwner = GetSafeHwnd();

	LPITEMIDLIST pidl = SHBrowseForFolder(&bi);

	if (!!!pidl) return;

	TCHAR szPath[MAX_PATH] = { 0 };

	SHGetPathFromIDList(pidl, szPath);

	CoTaskMemFree(pidl);

	m_rDir[nIdx] = szPath;

	UpdateData(FALSE);
}

int __stdcall CCalibratorDlg::BrowseForFolderCallback(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData)
{
	wchar_t szPath[MAX_PATH] = { 0 };

	switch (uMsg)
	{
	case BFFM_INITIALIZED:
		::SendMessage(hwnd, BFFM_SETSELECTION, TRUE, lpData);
		break;

	case BFFM_SELCHANGED:
		SHGetPathFromIDList((LPCITEMIDLIST)lParam, (LPWSTR)(LPCTSTR)szPath);
		::SendMessage(hwnd, BFFM_SETSTATUSTEXT, 0, (LPARAM)&szPath[0]);
		break;

	default:
		break;
	}

	return 0;
}

