#pragma once
#include "CImCtrl.h"

namespace ImParam
{
	// 图像参数 #xywh
	// 移动
	// 缩放（垂直 & 水平）
	// 旋转
	// 框选
	// 透明度
	// 拷贝
	// 保存
	class CLayouter : public ImCtrl::CImCtrl
	{
		DECLARE_DYNAMIC(CLayouter)

	protected:
		enum { eCT_W = 0, eCT_X, eCT_H, eCT_Y, eCT_HOR = 0, eCT_VER };

	protected:
		CLayouter();
		virtual ~CLayouter();

	protected:
		virtual void OnCreateEx(LPCREATESTRUCT& lpCreateStruct);

	private:
		void OnInitParamCtrl();
		void OnInitScalingCtrl();
		void OnInitOpacityCtrl();
		void OnInitAngleCtrl();
		void OnInitOptCtrl();

	protected:
		ImCtrl::CImStatic m_stParam[4];
		ImCtrl::CImEdit m_etParam[4]; // w. x. h. y

		ImCtrl::CImStatic m_stOpacity;
		ImCtrl::CImEdit m_etOpacity;	// 透明度

		ImCtrl::CImStatic m_stScaling;
		ImCtrl::CImEdit m_etScaling[2];
		ImCtrl::CImButton m_btAssociated; // 缩放

		ImCtrl::CImStatic m_stAngle;
		ImCtrl::CImEdit m_etAngle;  // 角度

		ImCtrl::CImButton m_btMoving;
		ImCtrl::CImButton m_btScaling;
		ImCtrl::CImButton m_btRotating;
		ImCtrl::CImButton m_btSelecting;
		ImCtrl::CImButton m_btCopy;
		ImCtrl::CImButton m_btSave;

	private:

		DECLARE_MESSAGE_MAP()
	};

	class CImParam : public CLayouter
	{
		DECLARE_DYNAMIC(CImParam)

	public:
		CImParam();
		virtual ~CImParam();

	public:
		void OnSyncParam();

	public:
		void SetOpacity(int nOpacity);
		void SetScaling(int nScaling);
		void SetTL(std::array<int,2> vTL);
		void SetWH(std::array<int, 2> vWH);

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

	private:
		void OnXChanged();
		void OnYChanged();
		void OnWChanged();
		void OnHChanged();
		void OnOpacityChanged();
		void OnHorScalingChanged();
		void OnVerrScalingChanged();
		void OnAngleChanged();

	private:

		DECLARE_MESSAGE_MAP()
	};
}
