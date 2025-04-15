#pragma once
#include "Mold.h"
#include "resource.h"
#include "opencv.hpp" 
#include "StMoldEx.h"
#include "TempDlgEx.h"
#include "StMatchEx.h"

class CMoldDlg : public CDialog, public CMold
{
	DECLARE_DYNAMIC(CMoldDlg)

public:
	CMoldDlg(CWnd* pParent = NULL);
	virtual ~CMoldDlg();

	enum { IDD = IDD_MOLD };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);

public:
	void OnSyncFeature(cv::Mat *cMatrix);

private:
	void OnMoldParamChange();
	void OnTempParamChange();
	void OnSyncMatching();

private:
	int m_nOx[2];  // 视图相对于当前图
	int m_nOy[2];
	int m_nCx;
	int m_nCy;
	double m_dScale;
	double m_dSame;
	cv::Mat* m_lpFeature;

private:
	CStMoldEx m_stMold;
	CStMatchEx m_stMatching;
	CTempDlgEx m_cFeatureDlg;

protected:
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);

protected:
	afx_msg void OnClose();
	afx_msg void OnClickedLoad();
	afx_msg LRESULT OnMoldParam(WPARAM wparam, LPARAM lparam);
	afx_msg LRESULT OnFeatureParam(WPARAM wparam, LPARAM lparam);
	afx_msg LRESULT OnFeatureOk(WPARAM wparam, LPARAM lparam);
	afx_msg LRESULT OnTempMatched(WPARAM wparam, LPARAM lparam);
	afx_msg LRESULT OnSyncOsParam(WPARAM wparam, LPARAM lparam);

protected:
	afx_msg void OnClickedReset();
	afx_msg void OnClickedRange();
	afx_msg void OnClickedSave();
	afx_msg void OnClickedRecover();

	DECLARE_MESSAGE_MAP()
};

