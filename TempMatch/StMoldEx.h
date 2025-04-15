#pragma once
#include "StMold.h"
#include <string>

class CStMoldEx : public CStMold
{
	DECLARE_DYNAMIC(CStMoldEx)

private:
	enum { ePP_Backup, ePP_Src2Range, ePP_Range2Temp, ePP_View = 0x1 };
	enum { eMT_Src, eMT_Range, eMT_Temp, eMT_View };

public:
	CStMoldEx();
	virtual ~CStMoldEx();

public:
	void OnReset();
	void OnRange();
	void OnSave();
	void OnRecover();
	void OnSyncMold(std::wstring strMold);

public:
	std::tuple<cv::Mat*, cv::Mat*>* GetView();
	void GetViewOs(int& ox, int& oy);
	bool GetMoldParam(int &x, int &y, std::wstring &strMold, cv::Mat &cTemp);

private:
	void OnClearRangeParam();
	void OnClearTempParam();
	void OnClearViewParam();

private:
	void Client2Canvas(FeatureFrame& vFrame);
	void Canvas2View(FeatureFrame& vFrame);
	void View2Cur(FeatureFrame& vFrame);
	void Cur2Range(FeatureFrame& vFrame);
	void Range2Cur(FeatureFrame& vFrame);
	void Cur2View(bool bScale);

private:
	void BackupOrRestore(bool bBackup);
	bool IsValidInput(std::tuple<int, int, int, int, double> &vFrame);
	void Sync2Parent();

protected:
	void OnOwnerDraw(CDC& dc, unsigned char*& lpPixel, const long& biWidth);
	void OnDrawMold(CDC& dc, unsigned char*& lpPixel, const long& biWidth);
	void OnDrawFrame(CDC& dc, unsigned char*& lpPixel, const long& biWidth);

private:
	int m_nOx[3];  // 视图相对于当前图
	int m_nOy[3];
	int m_nCx[3];
	int m_nCy[3];
	double m_dScale[2];

private:
	cv::Mat m_cMatrix[4];
	std::wstring m_strMold;
	FeatureFrame m_vFeatureFrame;

private:
	afx_msg void OnPaint();
	afx_msg LRESULT OnFeatureDefault(WPARAM wapram, LPARAM lparam);
	afx_msg LRESULT OnFeatureZoom(WPARAM wapram, LPARAM lparam);
	afx_msg LRESULT OnFeatureFrame(WPARAM wapram, LPARAM lparam);
	afx_msg LRESULT OnFeatureParam(WPARAM wapram, LPARAM lparam);

	DECLARE_MESSAGE_MAP()
};

