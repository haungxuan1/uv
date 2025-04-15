#pragma once
#include "afxdialogex.h"
#include "opencv2/opencv.hpp"
#include "MakeTemplateView.h"
#include "../Control/StyleDialog.h"
#include "../Control/StyleButton.h"

#define GRID_SIZE		8

//class MakeTemplateDialog : public CDialog
class MakeTemplateDialog : public StyleDialog
{
	DECLARE_DYNAMIC(MakeTemplateDialog)

public:
	MakeTemplateDialog(CWnd* ptParent, Mat &tTemplateMat, Mat &tProductMat, CString tProductPath, bool bParentIsManager = true);   // 标准构造函数
	virtual ~MakeTemplateDialog();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MAKE_TEMPLATE_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* ptMsg);

	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnBnClickedBtnMake();
	afx_msg void OnNMCustomdrawSliderAlpha(NMHDR* ptNMHDR, LRESULT* ptResult);
	afx_msg void OnNMCustomdrawSliderContrastRatio(NMHDR* ptNMHDR, LRESULT* ptResult);
	afx_msg void OnBnClickedBtnRotateInc();
	afx_msg void OnBnClickedBtnRotateDec();
	afx_msg void OnBnClickedBtnScaleInc();
	afx_msg void OnBnClickedBtnScaleDec();
	afx_msg void OnBnClickedBtnReset();
	afx_msg void OnBnClickedBtnMoveLeft();
	afx_msg void OnBnClickedBtnMoveRight();
	afx_msg void OnBnClickedBtnMoveUp();
	afx_msg void OnBnClickedBtnMoveDown();
	afx_msg void OnCbnSelchangeCmbScale();
	afx_msg void OnBnClickedBtnAutoPosition();

private:
	CSliderCtrl m_tSliderAlpha;
	CSliderCtrl m_tSliderContrastRatio;
	CComboBox m_tCmbScale;
	CComboBox m_tCmbMoveStep;
	CComboBox m_tCmbScaleStep;
	CComboBox m_tCmbRotateStep;
	StyleButton m_tAutoPositionBtn;
	StyleButton m_tResetBtn;
	StyleButton m_tMakeBtn;
	StyleButton m_tCancelBtn;
	StyleButton m_tRotateIncBtn;
	StyleButton m_tRotateDecBtn;
	StyleButton m_tScaleIncBtn;
	StyleButton m_tScaleDecBtn;
	StyleButton m_tMoveUpBtn;
	StyleButton m_tMoveDownBtn;
	StyleButton m_tMoveRightBtn;
	StyleButton m_tMoveLeftBtn;
	CEdit m_tTemplateName;
	MakeTemplateView* m_ptMakeTemplateView = nullptr;

	//std::string m_tProductPath;							// 产品路径
	std::string m_tSavePath;								// 保存路径
	Mat m_tTemplateMat;										// 模板Mat
	Mat m_tProductMat;										// 产品Mat
	bool m_bParentIsManager = true;							// 父窗口是否是模板管理窗口

	float getCmbScaleStep(bool bFlag = false);
	float getCmbRotateStep(bool bFlag = false);
	float getCmbMoveStep();
	void initStyle();
};
