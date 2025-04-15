#include "pch.h"
#include "CameraParamDlg.h"
#include "PQMsg.h"

IMPLEMENT_DYNAMIC(CCameraParamDlg, CDALSADlg)

BEGIN_MESSAGE_MAP(CCameraParamDlg, CDALSADlg)
	ON_BN_CLICKED(IDC_BT_MAIN_CAMERA_PARAM, OnClickedCameraParam)
	ON_BN_CLICKED(IDC_BT_MAIN_CALIBRATOR, OnClickedCalibratorParam)
	ON_MESSAGE(PQM_MAIN_CAMERA_PARAM, OnCameraParamMsg)
	ON_MESSAGE(PQM_MAIN_MAKE_CAMERA_PARAM, OnMakeCameraParamMsg)
END_MESSAGE_MAP()
CCameraParamDlg::CCameraParamDlg(CWnd* pParent):
	CDALSADlg(pParent)
{

}

CCameraParamDlg::~CCameraParamDlg()
{

}

// 函数功能：在对话框和成员变量之间交换数据
// 参数：
// - pDX: 指向 CDataExchange 对象的指针，用于数据交换
// 返回值：无
void CCameraParamDlg::DoDataExchange(CDataExchange* pDX)
{
    // 调用基类的 DoDataExchange 函数，确保基类的数据交换逻辑得以执行
    CDALSADlg::DoDataExchange(pDX);

    // 将 IDC_ET_MAIN_CAMERA_PARAM 对应的文本框内容与成员变量 m_rCameraParam 进行数据交换
    DDX_Text(pDX, IDC_ET_MAIN_CAMERA_PARAM, m_rCameraParam);
    // 将 IDC_ET_MAIN_CALIBRATOR 对应的文本框内容与成员变量 m_rCalibratorParam 进行数据交换
    DDX_Text(pDX, IDC_ET_MAIN_CALIBRATOR, m_rCalibratorParam);

    // 将 IDC_ET_MAIN_CAMERA_PARAM 对应的控件与成员变量 m_etCameraParam 进行关联
    DDX_Control(pDX, IDC_ET_MAIN_CAMERA_PARAM, m_etCameraParam);
    // 将 IDC_ET_MAIN_CALIBRATOR 对应的控件与成员变量 m_etCalibratorParam 进行关联
    DDX_Control(pDX, IDC_ET_MAIN_CALIBRATOR, m_etCalibratorParam);
    // 将 IDC_BT_MAIN_CAMERA_PARAM 对应的控件与成员变量 m_btCameraParam 进行关联
    DDX_Control(pDX, IDC_BT_MAIN_CAMERA_PARAM, m_btCameraParam);
    // 将 IDC_BT_MAIN_CALIBRATOR 对应的控件与成员变量 m_btCalibratorParam 进行关联
    DDX_Control(pDX, IDC_BT_MAIN_CALIBRATOR, m_btCalibratorParam);
}

BOOL CCameraParamDlg::OnInitDialog()
{
	CDALSADlg::OnInitDialog();

	// ���ð�ťͼ��
	HICON hBtnIcon = (HICON)LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_LOADING_BTN_ICON), IMAGE_ICON, 28, 28, LR_DEFAULTCOLOR | LR_CREATEDIBSECTION);
	m_btCameraParam.SetIcon(hBtnIcon);

	hBtnIcon = (HICON)LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_CALIBRATION_BTN_ICON), IMAGE_ICON, 28, 28, LR_DEFAULTCOLOR | LR_CREATEDIBSECTION);
	m_btCalibratorParam.SetIcon(hBtnIcon);

	m_etCameraParam.EnableWindow(FALSE);
	m_etCalibratorParam.EnableWindow(FALSE);

	return TRUE;
}

// ������� - У׼
void CCameraParamDlg::OnClickedCalibratorParam()
{
	/*BROWSEINFO bi = { 0 };

	bi.lpszTitle = _T("У׼����");

	bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;

	bi.hwndOwner = this->GetSafeHwnd();

	LPITEMIDLIST pidl = SHBrowseForFolder(&bi);
	if (!!!pidl) return;

	TCHAR szPath[MAX_PATH] = {0};

	if (!SHGetPathFromIDList(pidl, szPath)) return;

	m_rCalibratorParam = szPath;

	UpdateData(FALSE);*/

	PostMessage(PQM_MAIN_MAKE_CAMERA_PARAM);

	/*IMalloc* imalloc = nullptr;

	if (!SUCCEEDED(SHGetMalloc(&imalloc))) return;

	imalloc->Free(pidl);

	imalloc->Release();*/
}

LRESULT CCameraParamDlg::OnMakeCameraParamMsg(WPARAM wparam, LPARAM lparam)
{
	UpdateData(TRUE);

	CCalibratorDlg dlg(m_rCalibratorParam);

	dlg.DoModal();

	return 0;
}

// ������� - ����
void CCameraParamDlg::OnClickedCameraParam()
{
	/*CFileDialog dlg(TRUE, NULL, L"F:\\f\\VisionLocalization\\x64\\Release\\param\\", OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, L"camera param (*.png)|*.png||");

	if (dlg.DoModal() != IDOK) return;

	if (dlg.GetFileName() != "param.png") { AfxMessageBox(L"��ֹ������������"); return; }

	m_rCameraParam = dlg.GetPathName();

	UpdateData(FALSE);*/

	PostMessage(PQM_MAIN_CAMERA_PARAM, 0, 0);
}

// ������� - ����
LRESULT CCameraParamDlg::OnCameraParamMsg(WPARAM wparam, LPARAM lparam)
{
	UpdateData(TRUE);

	if (!CCalibratorDlg::LoadParam(CString2String(m_rCameraParam), m_tCameraParam)) {
		AfxMessageBox(L"��ֹ������������");

		m_rCameraParam = "";

		UpdateData(FALSE);
	}

	return 0;
}