// ParamSetDialog.cpp: 实现文件
//

#include "pch.h"
#include "VisionLocalization.h"
#include "afxdialogex.h"
#include "ParamSetDialog.h"
#include "def.h"
#include "../utils/FileUtil.h"
#include "../utils/IniUtil.h"
#include "CameraParamDlg.h"
#include "PQMsg.h"


// ParamSetDialog 对话框

IMPLEMENT_DYNAMIC(ParamSetDialog, StyleDialog)

ParamSetDialog::ParamSetDialog(CWnd* pParent /*=nullptr*/)
	: StyleDialog(IDD_PARAM_DIALOG, pParent)
{

}

ParamSetDialog::~ParamSetDialog()
{
}

void ParamSetDialog::DoDataExchange(CDataExchange* pDX)
{
	StyleDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CHECK_DEBUG, m_tCheckDebug);
	DDX_Control(pDX, IDC_EDIT_SETTING_CAMERA_PARAM, m_tSettingCameraParam);
	DDX_Control(pDX, IDC_EDIT_SETTING_CALIBRATOR_PARAM, m_tSettingCalibratorParam);
	DDX_Control(pDX, IDC_EDIT_SETTING_CCF_FILE, m_tSettingCcfParam);
	DDX_Control(pDX, IDC_EDIT_SETTING_FOLDER, m_tSettingPathParam);
	DDX_Control(pDX, IDC_BUTTON_SETTING_CAMERA_PARAM, m_tCameraParamBtn);
	DDX_Control(pDX, IDC_BUTTON_SETTING_CALIBRATOR_PARAM, m_tCalibratorParamBtn);
	DDX_Control(pDX, IDC_BUTTON_SETTING_FOLDER, m_tPathParamBtn);
	DDX_Control(pDX, IDC_BUTTON_SETTING_CCF, m_tCcfParamBtn);
	DDX_Control(pDX, IDC_BUTTON_OK, m_tOkBtn);
	DDX_Control(pDX, IDC_BUTTON_CANCEL, m_tCancelBtn);
	DDX_Control(pDX, IDC_BUTTON_APPLY, m_tApplyBtn);
}


BEGIN_MESSAGE_MAP(ParamSetDialog, StyleDialog)
	ON_BN_CLICKED(IDC_CHECK_DEBUG, &ParamSetDialog::OnBnClickedDebugCheck)
	ON_BN_CLICKED(IDC_BUTTON_SETTING_CAMERA_PARAM, &ParamSetDialog::OnBnClickedButtonSettingCameraParam)
	ON_BN_CLICKED(IDC_BUTTON_SETTING_CALIBRATOR_PARAM, &ParamSetDialog::OnBnClickedButtonSettingCalibratorParam)
	
	ON_BN_CLICKED(IDC_BUTTON_OK, &ParamSetDialog::OnBnClickedButtonOk)
	ON_BN_CLICKED(IDC_BUTTON_CANCEL, &ParamSetDialog::OnBnClickedButtonCancel)
	ON_BN_CLICKED(IDC_BUTTON_APPLY, &ParamSetDialog::OnBnClickedButtonApply)
	ON_BN_CLICKED(IDC_BUTTON_SETTING_FOLDER, &ParamSetDialog::OnBnClickedButtonSettingFolder)
END_MESSAGE_MAP()


// ParamSetDialog 消息处理程序
BOOL ParamSetDialog::OnInitDialog()
{
	StyleDialog::OnInitDialog();

	// 设置按钮图标
	/*HICON hBtnIcon = (HICON)LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_LOADING_BTN_ICON), IMAGE_ICON, 28, 28, LR_DEFAULTCOLOR | LR_CREATEDIBSECTION);
	m_tCameraParamBtn.SetIcon(hBtnIcon);

	hBtnIcon = (HICON)LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_CALIBRATION_BTN_ICON), IMAGE_ICON, 28, 28, LR_DEFAULTCOLOR | LR_CREATEDIBSECTION);
	m_tCalibratorParamBtn.SetIcon(hBtnIcon);*/

	// 设置参数值
	CString tCfgFile = FileUtil::getAppPath(UV_MATCH_CFG);
	m_tSettingCameraParam.SetWindowText(IniUtil::readStringValue(tCfgFile, L"camera", L"param", L"G:\\12.04\\param.png"));
	m_tSettingCalibratorParam.SetWindowText(IniUtil::readStringValue(tCfgFile, L"camera", L"calibrator_param", L""));

	int iDebug = IniUtil::readIntValue(tCfgFile, L"template", L"debug_enable", 0);
	if (iDebug) {
		m_tCheckDebug.SetCheck(BST_CHECKED);
	}
	else {
		m_tCheckDebug.SetCheck(BST_UNCHECKED);
	}

	SetDlgItemText(IDC_EDIT_SETTING_FOLDER, IniUtil::readStringValue(tCfgFile, L"template", L"open_folder", L""));
	SetDlgItemText(IDC_EDIT_SETTING_CCF_FILE, TEXT("D:\\Teledyne DALSA\\Sapera\\CamFiles\\User\\N_No_Name_Default_Default.ccf"));

	return TRUE;
}


/**
 * @brief	设置相机参数--校准
 */
void ParamSetDialog::OnBnClickedButtonSettingCalibratorParam()
{
	BROWSEINFO bi = { 0 };

	bi.lpszTitle = _T("校准参数");

	bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;

	bi.hwndOwner = this->GetSafeHwnd();

	LPITEMIDLIST pidl = SHBrowseForFolder(&bi);
	if (!!!pidl) return;

	TCHAR szPath[MAX_PATH] = { 0 };

	if (!SHGetPathFromIDList(pidl, szPath)) return;

	m_tSettingCalibratorParam.SetWindowText(szPath);
	
	IMalloc* imalloc = nullptr;

	if (!SUCCEEDED(SHGetMalloc(&imalloc))) return;

	imalloc->Free(pidl);

	imalloc->Release();
}


/**
 * @brief	设置相机参数--加载
 */
void ParamSetDialog::OnBnClickedButtonSettingCameraParam()
{
	CFileDialog dlg(TRUE, NULL, L"F:\\f\\VisionLocalization\\x64\\Release\\param\\", OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, L"camera param (*.png)|*.png||");

	if (dlg.DoModal() != IDOK) return;

	if (dlg.GetFileName() != "param.png") { 
		MessageBox(TEXT("禁止填入非相机参数！"), TEXT("加载"), MB_OK | MB_ICONERROR); 

		return; 
	}

	m_tSettingCameraParam.SetWindowText(dlg.GetPathName());
}


void ParamSetDialog::OnBnClickedDebugCheck()
{
}


void ParamSetDialog::OnBnClickedButtonSettingFolder()
{
	CFolderPickerDialog folderDlg(NULL, OFN_FILEMUSTEXIST, this);

	if (folderDlg.DoModal() == IDOK){
		CString tFolderPath = folderDlg.GetPathName(); 

		SetDlgItemText(IDC_EDIT_SETTING_FOLDER, tFolderPath + "\\");
	}
}



void ParamSetDialog::OnBnClickedButtonOk()
{
	CString tValue;
	CString tCfgFile = FileUtil::getAppPath(UV_MATCH_CFG);

	m_tSettingCameraParam.GetWindowText(tValue);
	IniUtil::writeStringValue(tCfgFile, L"camera", L"param", tValue);

	m_tSettingCalibratorParam.GetWindowText(tValue);
	IniUtil::writeStringValue(tCfgFile, L"camera", L"calibrator_param", tValue);

	GetDlgItemText(IDC_EDIT_SETTING_FOLDER, tValue);
	IniUtil::writeStringValue(tCfgFile, L"template", L"open_folder", tValue);

	int iOpenDebug = (BST_CHECKED == m_tCheckDebug.GetCheck()) ? 1 : 0;
	IniUtil::writeIntValue(tCfgFile, L"template", L"debug_enable", iOpenDebug);

	::SendMessage(m_lpMainHandle, PQM_MAIN_UPDATE_CFG_PARAM, 0, 0);
	CDialog::OnOK();
}


void ParamSetDialog::OnBnClickedButtonCancel()
{
	CDialog::OnCancel();
}


void ParamSetDialog::OnBnClickedButtonApply()
{
	CString tValue;
	CString tCfgFile = FileUtil::getAppPath(UV_MATCH_CFG);

	m_tSettingCameraParam.GetWindowText(tValue);
	IniUtil::writeStringValue(tCfgFile, L"camera", L"param", tValue);

	m_tSettingCalibratorParam.GetWindowText(tValue);
	IniUtil::writeStringValue(tCfgFile, L"camera", L"calibrator_param", tValue);

	GetDlgItemText(IDC_EDIT_SETTING_FOLDER, tValue);
	IniUtil::writeStringValue(tCfgFile, L"template", L"open_folder", tValue);

	int iOpenDebug = (BST_CHECKED == m_tCheckDebug.GetCheck()) ? 1 : 0;
	IniUtil::writeIntValue(tCfgFile, L"template", L"debug_enable", iOpenDebug);

	::SendMessage(m_lpMainHandle, PQM_MAIN_UPDATE_CFG_PARAM, 0, 0);

	MessageBox(L"参数设置成功!", L"设置参数", MB_OK);
}