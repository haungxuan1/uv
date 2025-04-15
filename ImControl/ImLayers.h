#pragma once
#include <vector>
#include "ImLayer2.h"
#include "ImScrollbar.h"
#include "ImCenter.h"

using namespace std;

namespace ImControl2 { class CLayouter; class CImControl; class CImLayerMsg; }

namespace ImLayers
{
	class CLayouter : public ImCtrl::CImCtrl
	{
		DECLARE_DYNAMIC(CLayouter)

	protected:
		CLayouter();
		virtual ~CLayouter();

	public:
		void SetIcon(wstring strIconDir);

		void OnSelectAll ();

	protected:
		void OnCreateLayer(long long nSign);
		void OnClearLayer();

	protected:
		virtual void OnCreateEx(LPCREATESTRUCT& lpCreateStruct);

	private:
		int LayerId();

	protected:
		wstring m_strIconDir;

	protected:
		ImScrollbar::CImScrollbar m_cVer;
		vector<ImLayer2::CImLayer*> m_vLayer; // 初始加入则为背景

	protected:
		afx_msg LRESULT OnClearMsg(WPARAM wparam, LPARAM lparam);

	private:
		afx_msg LRESULT OnCreateMsg(WPARAM wparam, LPARAM lparam);
		afx_msg LRESULT OnSyncLayerCtrlMsg(WPARAM wparam, LPARAM lparam);
		afx_msg LRESULT OnSyncScrollbarCtrlMsg(WPARAM wparam, LPARAM lparam);

	private:
		afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);

		DECLARE_MESSAGE_MAP()
	};

	class CImLayers : public CLayouter
	{
		DECLARE_DYNAMIC(CImLayers)

		friend class ImControl2::CLayouter;
		friend class ImControl2::CImLayerMsg;
		friend class ImControl2::CImControl;

	protected:
		CImLayers();
		virtual ~CImLayers();

	protected:
		void Bind(CWnd* lpHandle) { m_lpHandle = lpHandle; } // 代替GetParent

	protected:
		void OnSyncParam(ImLayer::tagLayer& tBackground, ImLayer::tagLayer& tFocus, ImLayer::tagLayer& tSign);
	
	private:
		CWnd* m_lpHandle;

	private:
		afx_msg LRESULT OnClearMsg(WPARAM wparam, LPARAM lparam);
		afx_msg LRESULT OnClickedEnableMsg(WPARAM wparam, LPARAM lparam);
		afx_msg LRESULT OnClickedBackgroundMsg(WPARAM wparam, LPARAM lparam);
		afx_msg LRESULT OnNameChangedMsg(WPARAM wparam, LPARAM lparam);
		afx_msg LRESULT OnFocusChangedMsg(WPARAM wparam, LPARAM lparam);

		DECLARE_MESSAGE_MAP()
	};
}