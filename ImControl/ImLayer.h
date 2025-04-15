#pragma once
#include <array>
#include <tuple>
#include <vector>
#include <string>
#include "opencv.hpp"
#include "ImScrollbar.h"
#include "SRWLock.h"
#include "Condition.h"
#include "ImPtr.h"

namespace ImLayer
{
	class CPreview : public ImCtrl::CImCtrl
	{
		DECLARE_DYNAMIC(CPreview)

	public:
		CPreview();
		virtual ~CPreview();

	public:
		void SetPreview(ImPtr::CImPtr cPreview);

	private:
		ImPtr::CImPtr m_cPreview;

	private:
		virtual void OnDrawEx(CDC& dc, const CRect& rtCanvas, unsigned char*& lpPixel, const long& biWidth);

	private:
		afx_msg void OnLButtonDown(UINT nFlags, CPoint point);

		DECLARE_MESSAGE_MAP()
	};

	class CLayerInfo : public ImCtrl::CImCtrl
	{
		DECLARE_DYNAMIC(CLayerInfo)

	protected:
		CLayerInfo();
		virtual ~CLayerInfo();

	protected:
		virtual void OnCreateEx(LPCREATESTRUCT& lpCreateStruct);
		virtual void OnDestroyEx();

	public: // 预览图
		void BindPreviewParam(std::array<int, 2> vWH);

		// 预设值 与 已设置
		// 外部设置为预设值
		// 经线程处理后为已设置
		// 参数调整
		// 缩放、更新图片为 预设值
	public:
		void SetPrScaling(int nScaling);
		void SetImage(ImPtr::CImPtr& cImage);

	public:
		void SetUserParam(void* lpUserParam);
		void SetTL(std::array<int, 2> vTL);
		void SetWH(std::array<int, 2> vWH);
		void SetDragging(std::array<int, 2> vDragging);
		void SetScaling(int nScaling);

	private:
		void SetImBGRA(ImPtr::CImPtr& cImage);
		void SetImPreview(ImPtr::CImPtr& cImage);
		void SetImScaling(ImPtr::CImPtr& cImage);

	public:
		void SetEnable(bool bEnable);
		void SetImName(CString rName);
		void SetOpacity(int nOpacity);

	public:
		int GetPrScaling();

	public:
		void* GetUserParam();
		std::array<int, 2> GetTL();
		std::array<int, 2> GetWH();
		std::array<int, 2> GetDragging();
		int GetScaling();
		ImPtr::CImPtr GetImage();
		ImPtr::CImPtr GetImBRGA();
		ImPtr::CImPtr GetImPreview();
		ImPtr::CImPtr GetImScaling();

	public:
		bool GetEnable();
		CString GetImName();
		int GetOpacity();

	private:
		static unsigned int __stdcall OnChExitThread(void* lpUesrParam);
		static unsigned int __stdcall OnPrExitThread(void* lpUesrParam);
		static unsigned int __stdcall OnScExitThread(void* lpUesrParam);
		static unsigned int __stdcall OnBGRAThread(void* lpUesrParam);
		static unsigned int __stdcall OnPreviewThread(void* lpUesrParam);
		static unsigned int __stdcall OnScalingThread(void* lpUesrParam);


	private:
		void SetTermination(int nIndex, bool bTermination);
		bool IsTermination(int nIndex);
		void SetTimer(int nTimer, long long nTime);
		std::array<long long, 5> GetTimer();

	private:
		bool m_bEnable; // 是否显示
		CString m_rName; // 图层名
		int m_nOpacity; // 透明度 // 百分制
		void* m_lpUserParam; // 用户参数
		std::array<int, 2> m_vTL; // x,y = 原图显示于背景图层位置
		std::array<int, 2> m_vWH; // w,h = 原图有效显示宽高
		std::array<int, 2> m_vDragging;
		std::array<int, 2> m_nScaling; // 缩放 // 千分制 // 0 = 设置 | 1 = 当前
		std::array<int, 2> m_vPreview; // 预览图控件大小

	private:
		ImPtr::CImPtr m_cImage; // 图像
		ImPtr::CImPtr m_cImBRGA; // 4通道图像
		ImPtr::CImPtr m_cImPreview; // 预览图
		ImPtr::CImPtr m_cImScaling; // 缩放后

	private:
		bool m_bTermination[3];
		std::array<long long, 5> m_vTimer; // 0 = 图像修改时间 | 1 = bgra修改时间 | 2 = 缩放系数修改 | 3 = 预览图修改时间 | 4 = 缩放图修改时间 

	private:
		CSRWLockBase m_lLock;
		CCondition m_cBGRA;
		CCondition m_cPreview;
		CCondition m_cScaling;
		CCondition m_cTermination[3];

	private:
		DECLARE_MESSAGE_MAP()
	};

	// 图层
	class CLayer : public CLayerInfo
	{
		DECLARE_DYNAMIC(CLayer)

	public:
		CLayer();
		virtual ~CLayer();

	public:
		void SetImName(CString rName);
		void SetIcon(COLORREF crNormal, COLORREF crPushed, std::wstring strIconDir);
		void SetFocus(bool bFocus);

	public:
		bool GetFocus();

	private:
		void OnClickedEnable();
		void OnNameChanged();

	private:
		virtual void OnCreateEx(LPCREATESTRUCT& lpCreateStruct);
		void OnInitCtrl(const int& cx, const int& cy);
		void OnInitCtrl();
		
	private:
		bool m_bFocus;
		CPreview m_cPreview;
		ImCtrl::CImEdit m_cName;
		ImCtrl::CImCheckBox m_cEnable;
		std::array<COLORREF, 2> m_crColor;

	private:
		afx_msg LRESULT OnPreviewMsg(WPARAM wparam, LPARAM lparam);
		afx_msg LRESULT OnScalingMsg(WPARAM wparam, LPARAM lparam);

	private:
		afx_msg void OnLButtonDown(UINT nFlags, CPoint point);

		DECLARE_MESSAGE_MAP()
	};

	// 图层模块 // 图层0为背景 // 所有画布绘制与背景之上
	class CLayers : public ImCtrl::CImCtrl
	{
		DECLARE_DYNAMIC(CLayers)

	public:
		CLayers();
		virtual ~CLayers();

	public:
		void SetIcon(std::wstring strIconDir);

	public:
		bool Empty() { return m_vLayer.empty(); }

	public:
		void SetPrScaling(int nScaling) { for(auto& iLayer: m_vLayer)iLayer->SetPrScaling(nScaling); }
		void SetPrScaling(int nLayer, int nScaling) { m_vLayer[nLayer]->SetPrScaling(nScaling); }
		void SetImage(int nLayer, ImPtr::CImPtr& cImage) { m_vLayer[nLayer]->SetImage(cImage); }
		void SetTL(int nLayer, std::array<int, 2> vTL) { m_vLayer[nLayer]->SetTL(vTL); }
		void SetWH(int nLayer, std::array<int, 2> vWH) { m_vLayer[nLayer]->SetWH(vWH); }
		void SetDragging(int nLayer, std::array<int, 2> vDragging) { m_vLayer[nLayer]->SetDragging(vDragging); }
		void SetEnable(int nLayer, bool bEnable) { m_vLayer[nLayer]->SetEnable(bEnable); }
		void SetImName(int nLayer, CString rName) { m_vLayer[nLayer]->SetImName(rName); }
		void SetOpacity(int nLayer, int nOpacity) { m_vLayer[nLayer]->SetOpacity(nOpacity); }
		void SetUserParam(int nLayer, void* lpUserParam) { m_vLayer[nLayer]->SetUserParam(lpUserParam);; }

	public:
		int GetPrScaling(int nLayer) { return m_vLayer[nLayer]->GetPrScaling(); }
		std::array<int, 2> GetTL(int nLayer) { return m_vLayer[nLayer]->GetTL(); }
		std::array<int, 2> GetWH(int nLayer) { return m_vLayer[nLayer]->GetWH(); }
		std::array<int, 2> GetDragging(int nLayer) { return m_vLayer[nLayer]->GetDragging(); }
		int GetScaling(int nLayer) { return m_vLayer[nLayer]->GetScaling(); }
		ImPtr::CImPtr GetImage(int nLayer) { return m_vLayer[nLayer]->GetImage(); }
		ImPtr::CImPtr GetImScaling(int nLayer) { return m_vLayer[nLayer]->GetImScaling(); }
		bool GetEnable(int nLayer) { return m_vLayer[nLayer]->GetEnable(); }
		CString GetImName(int nLayer) { return m_vLayer[nLayer]->GetImName(); }
		int GetOpacity(int nLayer) { return m_vLayer[nLayer]->GetOpacity(); }
		void* GetUserParam(int nLayer) { return m_vLayer[nLayer]->GetUserParam(); }

	public:
		void OnCreateLayer();
		void OnCreateLayer(int nLayer);
		void OnDestoryLayer();
		void OnDestoryLayer(int nIndex);
		void OnDestoryLayer(std::vector<int> vIndex);
		int GetLayerCount();
		int GetFocus();
		void SetFocus(int nIndex);
		void SetBackgroundLayer(int nBackground) { m_nBackground = nBackground; }
		int GetBackgroundLayer() { return m_nBackground; }

	private:
		virtual void OnCreateEx(LPCREATESTRUCT& lpCreateStruct);
		virtual void OnDestroyEx();
		void OnInitCtrl(const int& cx, const int& cy);
		void OnInitCtrl();

	private:
		int LayerId();
		void OnLayerCountChanged();
		int Id2Index(int nCtrId);
		void ClearFocus();
		void RefreshLayer();

	private:
		int m_nFocusLayer;
		int m_nBackground;

	private:
		CSRWLockBase m_lLock;
		std::vector<CLayer*> m_vLayer;
		ImScrollbar::CImScrollbar m_cVer;

	private:
		afx_msg LRESULT OnLayerFocusChanged(WPARAM wparam, LPARAM lparam);
		afx_msg LRESULT OnLayerEnableChanged(WPARAM wparam, LPARAM lparam);
		afx_msg LRESULT OnLayerScalingChanged(WPARAM wparam, LPARAM lparam);

	private:
		afx_msg BOOL OnEraseBkgnd(CDC* pDC);
		afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);

		DECLARE_MESSAGE_MAP()
	};
}


