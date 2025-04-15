#pragma once
#include "CImCtrl.h"
#include "ImCenter.h"

namespace ImControl2 { class CLayouter; class CImParamMsg; class CImControl; }

namespace ImParam2
{
	class CLayouter : public ImCtrl::CImCtrl
	{
		DECLARE_DYNAMIC(CLayouter)

	protected:
		enum { eCT_Width = 0, eCT_Left, eCT_Height, eCT_Top, eCT_HOR = 0, eCT_VER };

	protected:
		CLayouter();
		virtual ~CLayouter();

	protected:
		void SetIcon(wstring strIconDir);

		void SetTempPicPath(wstring strPicPath);

	protected:
		virtual void OnCreateEx(LPCREATESTRUCT& lpCreateStruct);

	private:
		void OnInitParamCtrl();
		void OnInitScalingCtrl();
		void OnInitOpacityCtrl();
		void OnInitAngleCtrl();

		void OnInitImParamCtrl(); // edit
		void OnInitImParamCtrlEx(); // static

		void OnInitMoveCtrl();
		void OnInitScaleCtrl();
		void OnInitRotateCtrl();
		void OnInitSelectCtrl();
		void OnInitCopyCtrl();
		void OnInitSaveCtrl();

		void OnInitOptCtrl();

		void OnInitPicCtrl();

		void OnInitSelAllCtrl();

	protected:
		ImCtrl::CImStatic m_stParam[4];
		ImCtrl::CImEdit m_etParam[4]; // w. x. h. y

		ImCtrl::CImStatic m_stOpacity;
		ImCtrl::CImEdit m_etOpacity;	// 透明度

		ImCtrl::CImStatic m_stScaling;
		ImCtrl::CImEdit m_etScaling[2];
		ImCtrl::CImCheckBox m_btAssociated; // 缩放

		ImCtrl::CImStatic m_stAngle;
		ImCtrl::CImEdit m_etAngle;  // 角度

		ImCtrl::CImCheckBox m_btMoving;
		ImCtrl::CImCheckBox m_btScaling;
		ImCtrl::CImCheckBox m_btRotating;
		ImCtrl::CImCheckBox m_btSelecting;
		ImCtrl::CImButton m_btCopy;
		ImCtrl::CImButton m_btSave;
		ImCtrl::CImButton m_btPicture;			// 模版图片
		ImCtrl::CImButton m_btSelAll;

		bool isMainView = false;

	private:

		DECLARE_MESSAGE_MAP()
	};

	class CImParam : public CLayouter
	{
		DECLARE_DYNAMIC(CImParam)

		friend class ImControl2::CLayouter;
		friend class ImControl2::CImParamMsg;
		friend class ImControl2::CImControl;

	private:
		enum { eCBT_Moving = 0, eCBT_Scaling, eCBT_Rotating, eCBT_Selecting };

	protected:
		CImParam();
		virtual ~CImParam();

	protected:
		void Bind(CWnd* lpHandle) { m_lpHandle = lpHandle; } // 代替GetParent
		void OnSyncParam(ImLayer::tagLayer& tFocus);

	private:
		virtual void OnCreateEx(LPCREATESTRUCT& lpCreateStruct);

	private:
		void OnClickedAssociated();
		void OnClickedMoving();
		void OnClickedScaling();
		void OnClickedRotating();
		void OnClickedSelecting();
		void OnClickedCopy();
		void OnClickedSave();
		void OnClickedSelAll();

	private:
		void OnLeftChanged();
		void OnTopChanged();
		void OnWidthChanged();
		void OnHeightChanged();
		void OnOpacityChanged();
		void OnHorScalChanged();
		void OnVerScalChanged();
		void OnAngleChanged();

	private:
		void OptChanged(int nOpt);

	private:
		long long m_nFocus;
		CWnd* m_lpHandle;
		vector<ImCtrl::CImCheckBox*> m_vGroup;

	private:

		DECLARE_MESSAGE_MAP()
	};
}
