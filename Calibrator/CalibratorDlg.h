#pragma once
#include "ChartDlg.h"
#include "FlHeader.h"
#include "Filer.h"

using namespace Filer;

class CCalibratorDlg : public CChartDlg
{
	DECLARE_DYNAMIC(CCalibratorDlg)

public:
	CCalibratorDlg(CString rDir, CWnd* pParent = NULL);
	virtual ~CCalibratorDlg();

	void Mem2Str(lpFiler mlp, unsigned char*& mem, int& size);

	enum { IDD = IDD_DLG_CALIBRATOR };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);

protected:
	virtual void OnDisableCtrl();
	virtual void OnEnableCtrl();

public:
	static bool LoadParam(string strFile, tagCalibrator& tParam);
	static int DelayCompensation(double v1, double v2, double dCamDPI);

protected:
	void OnClickedDir(int nIdx);

private:
	static int __stdcall BrowseForFolderCallback(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData);

private:
	CString m_rParam;
	CString m_rType;

private:
	CButton m_btSave;
	CButton m_btParam;
	CButton m_btCalibrator;
	CEdit m_etParam;
	CEdit m_etType;

protected:
	virtual BOOL OnInitDialog();

protected:
	afx_msg void OnClickedSave(); // ±£´æ²ÎÊý
	afx_msg void OnClickedInputDir();
	afx_msg void OnClickedOutputDir();
	afx_msg void OnClickedCacheDir();
	afx_msg void OnClickedParamDir();
	afx_msg void OnClickedParam();

private:
	afx_msg LRESULT OnLoadParamMsg(WPARAM wparam, LPARAM lparam);
	afx_msg LRESULT OnParamChangedMsg(WPARAM wparam, LPARAM lparam);
	afx_msg LRESULT OnTypeChangedMsg(WPARAM wparam, LPARAM lparam);
	afx_msg LRESULT OnParamBeginMsg(WPARAM wparam, LPARAM lparam);
	afx_msg LRESULT OnParamEndMsg(WPARAM wparam, LPARAM lparam);
	afx_msg LRESULT OnEnableCtrlMsg(WPARAM wparam, LPARAM lparam);
	afx_msg LRESULT OnDisableCtrlMsg(WPARAM wparam, LPARAM lparam);

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnDestroy();
};
