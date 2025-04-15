#pragma once
#include "QEncodeDlg.h"

class CChartDlg : public CQEncodeDlg
{
	DECLARE_DYNAMIC(CChartDlg)

public:
	CChartDlg(CString rDir, CWnd* pParent = NULL);
	virtual ~CChartDlg();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);

protected:
	virtual void OnDisableCtrl();
	virtual void OnEnableCtrl();

private:
	void OnPrChart();
	void OnPrScan();
	void OnPrPrint();

protected:
	int m_nArea[2];
	int m_nChart[2];


private:
	CEdit m_etArea[2];
	CEdit m_etDist[2]; // rows | cols
	CEdit m_etChart[2];
	CButton m_btImGen;
	CButton m_btImPrint;
	CButton m_btImScan;
	CButton m_btImPrint2; // 对校准图进行扫描与复打

protected:
	virtual BOOL OnInitDialog();

private:
	afx_msg void OnClickedImPrint(); // 打印校准图
	afx_msg void OnClickedImScan(); // 扫描校准图
	afx_msg void OnClickedImGen(); // 生成校准图
	afx_msg void OnClickedImPrint2(); // 校准打印

private:
	afx_msg LRESULT OnTCPFinishMsg(WPARAM wparam, LPARAM lparam);
	afx_msg LRESULT OnCaptureMsg(WPARAM wparam, LPARAM lparam);
	afx_msg LRESULT OnTransparentMsg(WPARAM wparam, LPARAM lparam);
	afx_msg LRESULT OnChartMsg(WPARAM wparam, LPARAM lparam);
	afx_msg LRESULT OnScanMsg(WPARAM wparam, LPARAM lparam);
	afx_msg LRESULT OnPrintMsg(WPARAM wparam, LPARAM lparam);

	DECLARE_MESSAGE_MAP()
};
