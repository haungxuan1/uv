#pragma once
#include "CImCtrl.h"
#include "ImCenter.h"

namespace ImLayers { class CLayouter; class CImLayers; }

namespace ImLayer2
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
		ImCtrl::CImEdit m_cName;
		ImCtrl::CImPreview m_cPreview;
		ImCtrl::CImCheckBox m_cEnable;
		ImCtrl::CImCheckBox m_cBackground;
	
	private:

		DECLARE_MESSAGE_MAP()
	};

	class CImLayer : CLayouter
	{
		DECLARE_DYNAMIC(CImLayer)

		friend class ImLayers::CLayouter;
		friend class ImLayers::CImLayers;

	protected:
		CImLayer();
		virtual ~CImLayer();
		void setEnableClicked(bool ret);
	protected:
		void Bind(CWnd* lpHandle) { m_lpHandle = lpHandle; } // ´úÌæGetParent
		void SetSign(long long nSign) { m_nSign = nSign; }
		long long GetSign() { return m_nSign; }
		void OnSyncParam(long long nBackground, long long nFocus, long long nSign, bool bEnable, CString rName, ImPtr::CImPtr cPreview);

	protected:
		virtual void OnCreateEx(LPCREATESTRUCT& lpCreateStruct);

	private:
		void OnClickedEnable();
		void OnClickedPreview();
		void OnClickedName();
		void OnNameChanged();
		void OnClickedBackground();

	private:
		long long m_nSign;
		CWnd* m_lpHandle;

	private:
		afx_msg void OnLButtonDown(UINT, CPoint);

		DECLARE_MESSAGE_MAP()
	};
}


