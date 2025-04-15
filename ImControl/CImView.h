#pragma once
#include "ImScrollbar.h"
#include "ImLayer.h"
#include "opencv.hpp"
#include "SRWLock.h"
#include "Condition.h"

namespace ImLayer { class CLayers; }

namespace ImView
{
	class CImThread : public ImCtrl::CImCtrl
	{
		DECLARE_DYNAMIC(CImThread)

	protected:
		CImThread();
		virtual ~CImThread();

	public:
		void BindLayers(ImLayer::CLayers* lpLayers) { m_lpLayers = lpLayers; }

	protected:
		void GetRender(uchar*& lpRender);

	private:
		static unsigned int __stdcall OnExitThread(void* lpUesrParam);
		static unsigned int __stdcall OnRenderThread(void* lpUesrParam);

	private:
		void Render(std::tuple<int, double, int, double>& vImThumb);
		void Render(std::tuple<int, double, int, double>& vImThumb, int nFocus, int nLayer, cv::Mat& cRender);
		void Render(cv::Mat& cRender);
	
	protected:
		virtual void OnCreateEx(LPCREATESTRUCT& lpCreateStruct);
		virtual void OnDestroyEx();

	private:
		void Termination();
		bool IsTermination();
		void SetTimer(int nTimer, long long nTime);
		std::array<long long, 2> GetTimer();
		void SetRenderParam(std::tuple<int, double, int, double>& vImThumb);
		void GetRenderParam(std::tuple<int, double, int, double>& vImThumb);

	private:
		bool m_bTermination;
		CSRWLockBase m_lLock;
		CCondition m_cRender;
		CCondition m_cTermination;

	protected:
		ImLayer::CLayers* m_lpLayers;

	private:
		uchar* m_lpRender;
		std::array<long long, 2> m_vTimer; // 0 = 接受到修改指令 | 1 = 修改完成
		std::tuple<int, double, int, double> m_vImThumb; // 视图大小 | 滑块位置 | 缩放比例
	
	private:
		afx_msg LRESULT OnRenderMsg(WPARAM wparam, LPARAM lparam);

		DECLARE_MESSAGE_MAP()
	};

	class CImage : public CImThread
	{
		DECLARE_DYNAMIC(CImage)

	public:
		CImage();
		virtual ~CImage();

	public:
//		void SetIcon(std::wstring strIconDir);

	private:
		virtual void OnCreateEx(LPCREATESTRUCT& lpCreateStruct);
		virtual void OnDrawEx(CDC& dc, const CRect& rtCanvas, unsigned char*& lpPixel, const long& biWidth);
		virtual void OnDestroyEx();

	private:
		uchar* m_lpRender;
		CPoint m_ptLBDown;

	private:
		virtual BOOL PreTranslateMessage(MSG* pMsg);

	private:
		afx_msg void OnMouseMove(UINT nFlags, CPoint point);
		afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
		afx_msg void OnLButtonDown(UINT nFlags, CPoint point);

		DECLARE_MESSAGE_MAP()
	};

	class CImView : public ImCtrl::CImCtrl
	{
		DECLARE_DYNAMIC(CImView)

	public:
		CImView();
		virtual ~CImView();

	public:
		void SetIcon(std::wstring strIconDir);
		void BindLayers(ImLayer::CLayers* lpLayers) { m_lpLayers = lpLayers; }

	private:
		virtual void OnCreateEx(LPCREATESTRUCT& lpCreateStruct);
		void OnInitCtrl(const int& cx, const int& cy);
		void OnInitCtrl();

	private:
		void OnScaling(int nScaling);
		void OnHWheel(UINT nFlags, short zDelta, CPoint pt);
		void OnWheel(UINT nFlags, short zDelta, CPoint pt);

	private:
		std::tuple<int, double, int, double> m_vImThumb;

	private:
		CImage m_cImage;
		ImLayer::CLayers* m_lpLayers;
		ImScrollbar::CImScrollbar m_cHor;
		ImScrollbar::CImScrollbar m_cVer;

	private:
		afx_msg LRESULT OnRenderMsg(WPARAM wparam, LPARAM lparam);
		afx_msg LRESULT OnDraggingMsg(WPARAM wparam, LPARAM lparam);
		afx_msg LRESULT OnDragMsg(WPARAM wparam, LPARAM lparam);

	private:
		afx_msg BOOL OnEraseBkgnd(CDC* pDC);
		afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
		afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
		afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);


		DECLARE_MESSAGE_MAP()
	};
}


