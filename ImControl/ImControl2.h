#pragma once
#include "ImView2.h"
#include "ImParam2.h"
#include "ImLayers.h"
#include "ImCenter.h"

namespace ImControl2
{
	class CLayouter : public ImCtrl::CImCtrl
	{
		DECLARE_DYNAMIC(CLayouter)

	protected:
		CLayouter();
		virtual ~CLayouter();

	public:
		void SetIcon(wstring strIconDir);

		void SetPic(wstring strPicPath);

		//void SaveBackground();

	protected:
		virtual void OnCreateEx(LPCREATESTRUCT& lpCreateStruct);

	protected:
		ImView2::CImView m_cImView;
		ImParam2::CImParam m_cImParam;
		ImLayers::CImLayers m_cImLayers;

	private:
		DECLARE_MESSAGE_MAP()
	};
	/*********************************************************************************************/
	class CImLayerMsg : public CLayouter
	{
		DECLARE_DYNAMIC(CImLayerMsg)

	public:
		CImLayerMsg();
		virtual ~CImLayerMsg();

	public:
		void CreateLayer(ImLayer::tagLayer tParam);
		void CreateLayer(
			bool bBackground, 
			bool bFocus, 
			bool bCMYK, string strTIFF,
			bool bEnable, double dOpacity, CString rName, ImPtr::CImPtr cImage,
			bool bAuto, double dScal, int x, int y, void* lpUserParam = nullptr, double contrast = 100);

		void ClearLayer();

		void SelectAll();

	public:
		void SetLayerDir(string strDir) { m_strLayerDir = strDir; }

	private:
		void Auto(ImPtr::CImPtr& cImage, double& dScal, int& x, int& y);
		double AutoScal(ImPtr::CImPtr& cImage, Size cRenderSize);
		tuple<int, int> AutoCenter(ImPtr::CImPtr& cImage, Size cRenderSize, double dScal);

	protected:
		virtual void OnCreateEx(LPCREATESTRUCT& lpCreateStruct);

	protected:
		void OnSync(ImLayer::tagLayer& tBackground, ImLayer::tagLayer& tFocus, ImLayer::tagLayer& tSign);

	private:
		void CopyLayer(long long& nSign, Rect& rtWin);
		void CopyLayer(ImLayer::tagLayer& tLayer, Rect& rtWin);
		Rect Unite(ImLayer::tagLayer& tLayer, Rect& rtWin);
		Rect Layer(ImLayer::tagLayer& tLayer, Rect& rtUnite);
		Point2f Offset(ImLayer::tagLayer& tLayer, Rect& rtLayer);
		void CopyLayer(ImLayer::tagLayer& tLayer, Rect& rtCopy, Point2f& ptOs);
		Point2d PtS2R(Point2d pt, double dSx, double dSy);
		Rect RtS2R(Rect rt, double dSx, double dSy);

	private:
		void SaveLayer(long long& nSign, ImLayer::tagLayer& tReference);
		void SaveLayer(ImLayer::tagLayer& tLayer, ImLayer::tagLayer& tReference);
		//void SaveBackgroundLayer();

	private:
		string m_strLayerDir;

	protected:
		ImCenter::CImCenter m_cImCenter;

	private:
		afx_msg LRESULT OnCreateLayerMsg(WPARAM wparam, LPARAM lparam);
		afx_msg LRESULT OnDestroyLayerMsg(WPARAM wparam, LPARAM lparam);
		afx_msg LRESULT OnClearLayerMsg(WPARAM wparam, LPARAM lparam);

	private:
		afx_msg LRESULT OnClickedEnableMsg(WPARAM wparam, LPARAM lparam);
		afx_msg LRESULT OnClickedBackGroundMsg(WPARAM wparam, LPARAM lparam);
		afx_msg LRESULT OnNameChangedMsg(WPARAM wparam, LPARAM lparam);
		afx_msg LRESULT OnFocusChangedMsg(WPARAM wparam, LPARAM lparam);

	private:
		afx_msg LRESULT OnCopyLayerMsg(WPARAM wparam, LPARAM lparam);
		afx_msg LRESULT OnSaveLayerMsg(WPARAM wparam, LPARAM lparam);
		//afx_msg LRESULT OnSaveBackgroundMsg(WPARAM wparam, LPARAM lparam);

		DECLARE_MESSAGE_MAP()
	};
	/*********************************************************************************************/
	class CImParamMsg : public CImLayerMsg
	{
		DECLARE_DYNAMIC(CImParamMsg)

	public:
		CImParamMsg();
		virtual ~CImParamMsg();

	protected:
		virtual void OnCreateEx(LPCREATESTRUCT& lpCreateStruct);

	protected:
		void OnSync(ImLayer::tagLayer& tBackground, ImLayer::tagLayer& tFocus, ImLayer::tagLayer& tSign);

	private:
		afx_msg LRESULT OnLeftChangedMsg(WPARAM wparam, LPARAM lparam);
		afx_msg LRESULT OnTopChangedMsg(WPARAM wparam, LPARAM lparam);
		afx_msg LRESULT OnWidthChangedMsg(WPARAM wparam, LPARAM lparam);
		afx_msg LRESULT OnHeightChangedMsg(WPARAM wparam, LPARAM lparam);
		afx_msg LRESULT OnOpacityChangedMsg(WPARAM wparam, LPARAM lparam);

		afx_msg LRESULT OnHorScalChangedMsg(WPARAM wparam, LPARAM lparam);
		afx_msg LRESULT OnVerScalChangedMsg(WPARAM wparam, LPARAM lparam);
		afx_msg LRESULT OnAngleChangedMsg(WPARAM wparam, LPARAM lparam);

	private:
		afx_msg LRESULT OnClickedAssociatedMsg(WPARAM wparam, LPARAM lparam);
		afx_msg LRESULT OnClickedMovingMsg(WPARAM wparam, LPARAM lparam);
		afx_msg LRESULT OnClickedScalingMsg(WPARAM wparam, LPARAM lparam);
		afx_msg LRESULT OnClickedRotatingMsg(WPARAM wparam, LPARAM lparam);
		afx_msg LRESULT OnClickedSelectingMsg(WPARAM wparam, LPARAM lparam);
		afx_msg LRESULT OnClickedCopyMsg(WPARAM wparam, LPARAM lparam);
		afx_msg LRESULT OnClickedSaveMsg(WPARAM wparam, LPARAM lparam);
		afx_msg LRESULT OnClickedSelectAllMsg(WPARAM wparam, LPARAM lparam);

		DECLARE_MESSAGE_MAP()
	};
	/*********************************************************************************************/
	class CImViewMsg : public CImParamMsg
	{
		DECLARE_DYNAMIC(CImViewMsg)

	public:
		CImViewMsg();
		virtual ~CImViewMsg();

	public:
		array<int, 2> GetImControlRenderSize();

	protected:
		virtual void OnCreateEx(LPCREATESTRUCT& lpCreateStruct);

	protected:
		void OnSync(ImLayer::tagLayer& tBackground, ImLayer::tagLayer& tFocus, ImLayer::tagLayer& tSign);
		
	private:
		afx_msg LRESULT OnImCenterMsg(WPARAM wparam, LPARAM lparam);
		afx_msg LRESULT OnImMovingMsg(WPARAM wparam, LPARAM lparam);
		afx_msg LRESULT OnImMovedMsg(WPARAM wparam, LPARAM lparam);
		afx_msg LRESULT OnImScalingMsg(WPARAM wparam, LPARAM lparam);
		afx_msg LRESULT OnImScaledMsg(WPARAM wparam, LPARAM lparam);
		afx_msg LRESULT OnImScaleMsg(WPARAM wparam, LPARAM lparam);
		afx_msg LRESULT OnImRotatingMsg(WPARAM wparam, LPARAM lparam);
		afx_msg LRESULT OnImRotatedMsg(WPARAM wparam, LPARAM lparam);
		afx_msg LRESULT OnImContrastMsg(WPARAM wparam, LPARAM lparam);

		DECLARE_MESSAGE_MAP()
	};
	/*********************************************************************************************/
	class CImControl : public CImViewMsg
	{
		DECLARE_DYNAMIC(CImControl)

	public:
		CImControl();
		virtual ~CImControl();

	public:
		bool Empty() { return m_cImCenter.Empty(); }
		void* GetUserParam() { return m_lpUserParam; }
		void SetUserParam(void* lpUserParam) { m_lpUserParam = lpUserParam; }
		void GetSign(vector<long long>& vSign) { m_cImCenter.GetSign(vSign); }
		bool Get(long long nSign, ImLayer::tagLayer& tParam) { return m_cImCenter.Get(nSign, tParam); }

		CRect GetImViewImageRect() { return m_cImView.m_cImage.GetChildRect(); }

	protected:
		virtual void OnCreateEx(LPCREATESTRUCT& lpCreateStruct);

	private:
		void* m_lpUserParam;
		/*bool m_bMousePressed = false;
		CPoint m_tPointBegin;
		CPoint m_tPointEnd;*/

	protected:
		afx_msg LRESULT OnSyncMsg(WPARAM wparam, LPARAM lparam);
		/*afx_msg void OnPaint();
		afx_msg void OnMouseMove(UINT uFlags, CPoint tPoint);
		afx_msg void OnLButtonDown(UINT uFlags, CPoint tPoint);
		afx_msg void OnLButtonUp(UINT uFlags, CPoint tPoint);
		afx_msg void OnRButtonUp(UINT uFlags, CPoint tPoint);*/

	private:
		afx_msg LRESULT OnSyncSelectedMsg(WPARAM wparam, LPARAM lparam);
		afx_msg LRESULT OnSyncRenderSizeMsg(WPARAM wparam, LPARAM lparam);

		DECLARE_MESSAGE_MAP()
	};
}