#pragma once
#include "resource.h"
#include "TempDlg.h"
#include "opencv.hpp" 
#include <map>
#include <tuple>
#include <vector>
#include <functional>

class CTempDlgEx : public CTempDlg
{
	DECLARE_DYNAMIC(CTempDlgEx)

private:
	enum { eMT_Src, eMT_Range, eMT_Temp, eMT_View, eMT_Cache };

public:
	CTempDlgEx(CWnd* pParent = NULL);
	virtual ~CTempDlgEx();

	enum { IDD = IDD_MAKE_FEATURE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);

public:
	void OnSyncFeature(cv::Mat& cMatrix);

public:
	std::tuple<cv::Mat*, cv::Mat *> *GetView();
	void GetViewOs(int &ox, int &oy);

private:
	virtual void OnDraw(Gdiplus::Graphics& graph);
	virtual void OnDraw(unsigned char* lpPixel, int nHight, int nWidth);

private:
	void OnDrawFrame(unsigned int* lpPixel, int nHight, int nWidth);

private:
	cv::Mat RotateImg(cv::Mat src, cv::Point center, float angle, double scale);
	void OnSyncAlpha();

private:
	void OnClearRangeParam();
	void OnClearTempParam();
	void OnClearViewParam();

private:
	void Canvas2FeatureControl(FeatureFrame& vFrame);
	void FeatureControl2View(FeatureFrame& vFrame);
	void View2Cur(FeatureFrame& vFrame);
	void Cur2Range(FeatureFrame& vFrame);
	void Range2Cur(FeatureFrame& vFrame);
	void Cur2View(bool bScale);

private:
	void Contrarotation(FeatureFrame& vFrame, cv::Mat &vRot, cv::Mat &vCon, double &dAngle);
	void Contrarotation(FeatureFrame& vCur, cv::Mat& vCon, int &ox, int &oy, int &cx, int &cy);
	std::vector<cv::Point2i> Contrarotation(
		std::vector<cv::Point2i> vCur,
		cv::Point2d ptCenter,
		float dAngle,
		int nImHeight);
	cv::Point2i Contrarotation(
		cv::Point2i ptCur,
		cv::Point2d ptCenter,
		float dAngle,
		int nImHeight);

private:
	void Sync2Parent();

private:
	void BackupOrRestore(bool bBackup);
	bool IsValidInput();

private:
	FeatureFrame m_vFeatureFrame;
	
	enum { 
		ePP_Backup, 
		ePP_Range2Temp, 
		ePP_View = 0x1};

private:
	int m_nOx[2];  // 视图相对于当前图
	int m_nOy[2];  
	int m_nCx[2];
	int m_nCy[2];
	double m_dAngle[2];

	double m_dScale[2];
	int m_nAlpha[2]; // 视图相对于控件偏置

private:
	cv::Mat m_cMatrix[5]; 

protected:
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);

private:
	afx_msg LRESULT OnFeatureDefault(WPARAM wapram, LPARAM lparam);
	afx_msg LRESULT OnFeatureZoom(WPARAM wapram, LPARAM lparam);
	afx_msg LRESULT OnFeatureFrame(WPARAM wapram, LPARAM lparam);
	afx_msg LRESULT OnFeatureParam(WPARAM wapram, LPARAM lparam);

private:
	afx_msg void OnClickedReset();
	afx_msg void OnClickedRange();
	afx_msg void OnClickedSave();
	afx_msg void OnClickedRecover();
	afx_msg void OnSyncOsParam();
	afx_msg void OnClickedOk();

	DECLARE_MESSAGE_MAP()
};