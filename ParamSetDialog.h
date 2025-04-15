#pragma once
#include "afxdialogex.h"
#include "../Control/StyleDialog.h"
#include "../Control/StyleButton.h"


// ParamSetDialog 对话框

//class ParamSetDialog : public CDialog
class ParamSetDialog : public StyleDialog
{
	DECLARE_DYNAMIC(ParamSetDialog)

public:
	ParamSetDialog(CWnd* pParent = nullptr);	// 标准构造函数
	virtual ~ParamSetDialog();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PARAM_DIALOG };
#endif

protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

protected:
	HWND m_lpMainHandle = nullptr;

public:
	CButton m_tCheckDebug;				
	StyleButton m_tCameraParamBtn;
	StyleButton m_tCalibratorParamBtn;
	StyleButton m_tPathParamBtn;
	StyleButton m_tCcfParamBtn;
	StyleButton m_tOkBtn;
	StyleButton m_tCancelBtn;
	StyleButton m_tApplyBtn;

	CEdit m_tSettingCameraParam;		
	CEdit m_tSettingCalibratorParam;	
	CEdit m_tSettingCcfParam;
	CEdit m_tSettingPathParam;

public:
	afx_msg void OnBnClickedButtonSettingCameraParam();
	afx_msg void OnBnClickedButtonSettingCalibratorParam();
	afx_msg void OnBnClickedDebugCheck();
	afx_msg void OnBnClickedButtonSettingFolder();

	afx_msg void OnBnClickedButtonOk();
	afx_msg void OnBnClickedButtonCancel();
	afx_msg void OnBnClickedButtonApply();

public:
	void setLpHandle(const HWND& lpHandle) { m_lpMainHandle = lpHandle; };
	
};
