#pragma once
#include "ImImage.h"
#include "ImScrollbar.h"

namespace ImControl2 { class CLayouter; class CImViewMsg; class CImControl; }

namespace ImView2
{
	class CLayouter : public ImCtrl::CImCtrl
	{
		DECLARE_DYNAMIC(CLayouter)

	protected:
		CLayouter();
		virtual ~CLayouter();

	public:
		void SetIcon(wstring strIconDir);

	protected:
		virtual void OnCreateEx(LPCREATESTRUCT& lpCreateStruct);

	protected:
		ImImage::CImImage m_cImage;
		ImScrollbar::CImScrollbar m_cHor;
		ImScrollbar::CImScrollbar m_cVer;

	private:

		DECLARE_MESSAGE_MAP()
	};
	/****************************************************************************************/
	class CImView : public CLayouter
	{
		DECLARE_DYNAMIC(CImView)

		friend class ImControl2::CLayouter;
		friend class ImControl2::CImViewMsg;
		friend class ImControl2::CImControl;

	protected:
		CImView();
		virtual ~CImView();

	protected:
		void Bind(CWnd* lpHandle) { m_lpHandle = lpHandle; } // ´úÌæGetParent
		void SetCenter(ImCenter::CImCenter* lpImCenter) { m_cImage.SetCenter(lpImCenter); }

	protected:
		void OnSyncParam(ImLayer::tagLayer& tFocus, ImLayer::tagLayer& tBackground);
		void GetRenderSize(int& w, int& h);
		Rect GetFocusRect();

	private:
		virtual void OnCreateEx(LPCREATESTRUCT& lpCreateStruct);

	private:
		CWnd* m_lpHandle;
		std::tuple<int, double, int, double> m_vImThumb;

	private:
		afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
		afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);

	private:
		afx_msg LRESULT OnImCenterMsg(WPARAM wparam, LPARAM lparam);
		afx_msg LRESULT OnImPt2PtMsg(WPARAM wparam, LPARAM lparam);
		afx_msg LRESULT OnImMovingMsg(WPARAM wparam, LPARAM lparam);
		afx_msg LRESULT OnImMovedMsg(WPARAM wparam, LPARAM lparam);
		afx_msg LRESULT OnImScalingMsg(WPARAM wparam, LPARAM lparam);
		afx_msg LRESULT OnImScaledMsg(WPARAM wparam, LPARAM lparam);
		afx_msg LRESULT OnImScaleMsg(WPARAM wparam, LPARAM lparam);
		afx_msg LRESULT OnImRotatingMsg(WPARAM wparam, LPARAM lparam);
		afx_msg LRESULT OnImRotatedMsg(WPARAM wparam, LPARAM lparam);

		DECLARE_MESSAGE_MAP()
	};	
}