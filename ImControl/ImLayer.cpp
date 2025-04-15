#include "pch.h"
#include "ImLayer.h"
#include "DrawPng.h"
#include "PQMsg.h"
#include <iostream>
#include <imm.h>
#pragma comment(lib, "Imm32.lib")
using namespace cv;
using namespace std;

namespace ImLayer
{
	IMPLEMENT_DYNAMIC(CPreview, CImCtrl)

	BEGIN_MESSAGE_MAP(CPreview, CImCtrl)
		ON_WM_LBUTTONDOWN()
	END_MESSAGE_MAP()
	CPreview::CPreview()
	{

	}

	CPreview::~CPreview()
	{

	}

	void CPreview::OnLButtonDown(UINT nFlags, CPoint point)
	{
		::PostMessage(GetParent()->GetSafeHwnd(),
			WM_LBUTTONDOWN,
			MAKELONG(GetCtrlID(), nFlags),
			MAKELONG(point.x, point.y));
	}

	void CPreview::SetPreview(ImPtr::CImPtr cPreview)
	{
		m_cPreview = cPreview;

		GetSafeHwnd() ? Invalidate() : []() {}();
	}

	void CPreview::OnDrawEx(CDC& dc, const CRect& rtCanvas, unsigned char*& lpPixel, const long& biWidth)
	{
		if (m_cPreview.Empty()) return;

		uint64 idx[2] = { 0,0 };

		for (uint64 y = 0; y < min(m_cPreview.Get()->rows, rtCanvas.Height()); ++y)
		{
			for (uint64 x = 0; x < min(m_cPreview.Get()->cols, rtCanvas.Width()); ++x)
			{
				idx[0] = (y * m_cPreview.Get()->cols + x) * 4;

				idx[1] = (y * biWidth + x) * 4;

				for (uint64 c = 0; c < 4; ++c)
				{
					lpPixel[idx[1] + c] = m_cPreview.Get()->data[idx[0] + c];
				}
			}
		}
		lpPixel = UpdatePixel(lpPixel);
	}
	/********************************************************************************/
	IMPLEMENT_DYNAMIC(CLayerInfo, ImCtrl::CImCtrl)

	BEGIN_MESSAGE_MAP(CLayerInfo, ImCtrl::CImCtrl)

	END_MESSAGE_MAP()

	CLayerInfo::CLayerInfo():
		m_vPreview{ 0,0 },
		m_bEnable(true),
		m_lpUserParam(nullptr),
		m_nOpacity(100),
		m_nScaling{ 1000, 1000 },
		m_vTL{ 0,0 },
		m_vWH{ 0,0 },
		m_bTermination{ false, false },
		m_vTimer{ 0,0,0,0 }
	{

	}

	CLayerInfo::~CLayerInfo()
	{

	}

	void CLayerInfo::OnCreateEx(LPCREATESTRUCT& lpCreateStruct)
	{
		CloseHandle((HANDLE)_beginthreadex(NULL, 0, OnScalingThread, (void*)this, 0, NULL));

		CloseHandle((HANDLE)_beginthreadex(NULL, 0, OnPreviewThread, (void*)this, 0, NULL));

		CloseHandle((HANDLE)_beginthreadex(NULL, 0, OnBGRAThread, (void*)this, 0, NULL));
	}

	void CLayerInfo::OnDestroyEx()
	{
		CloseHandle((HANDLE)_beginthreadex(NULL, 0, OnChExitThread, (void*)this, 0, NULL));

		m_cTermination[0].wait();

		CloseHandle((HANDLE)_beginthreadex(NULL, 0, OnPrExitThread, (void*)this, 0, NULL));

		m_cTermination[1].wait();

		CloseHandle((HANDLE)_beginthreadex(NULL, 0, OnScExitThread, (void*)this, 0, NULL));

		m_cTermination[2].wait();
	}

	void CLayerInfo::BindPreviewParam(std::array<int, 2> vWH)
	{
		m_vPreview = vWH;
	}

	void CLayerInfo::SetPrScaling(int nScaling)
	{		
		LARGE_INTEGER tTimer = { 0 }; QueryPerformanceCounter(&tTimer);
		{
			CSWLock lock(&m_lLock);

			m_nScaling[0] = nScaling; m_vTimer[1] = tTimer.QuadPart;
		}
		m_cScaling.signal();
	}

	void CLayerInfo::SetImage(ImPtr::CImPtr& cImage)
	{
		LARGE_INTEGER tTimer = { 0 }; QueryPerformanceCounter(&tTimer);
		{
			CSWLock lock(&m_lLock);

			m_cImage = cImage; 
			
			m_vTimer[0] = tTimer.QuadPart;
		}
		m_cBGRA.signal();
	}

	void CLayerInfo::SetUserParam(void* lpUserParam)
	{
		m_lpUserParam = lpUserParam;
	}

	void CLayerInfo::SetTL(std::array<int, 2> vTL)
	{
		CSWLock lock(&m_lLock);

		m_vTL = vTL;
	}

	void CLayerInfo::SetWH(std::array<int, 2> vWH)
	{
		CSWLock lock(&m_lLock);

		m_vWH = vWH;
	}

	void CLayerInfo::SetDragging(std::array<int, 2> vDragging)
	{
		CSWLock lock(&m_lLock);

		m_vDragging = vDragging;
	}

	void CLayerInfo::SetScaling(int nScaling)
	{
		CSWLock lock(&m_lLock);

		m_nScaling[1] = nScaling;
	}

	void CLayerInfo::SetImBGRA(ImPtr::CImPtr& cImage)
	{
		{
			CSWLock lock(&m_lLock);

			m_cImBRGA = cImage;
		}
		m_cPreview.signal(); m_cScaling.signal();
	}

	void CLayerInfo::SetImPreview(ImPtr::CImPtr& cImage)
	{
		{
			CSWLock lock(&m_lLock);

			m_cImPreview = cImage;
		}
		PostMessage(PQM_IM_LAYER_PREVIEW);
	}

	void CLayerInfo::SetImScaling(ImPtr::CImPtr& cImage)
	{
		{
			CSWLock lock(&m_lLock);

			m_cImScaling = cImage;
		}
		PostMessage(PQM_IM_LAYER_SCALING);
	}

	void CLayerInfo::SetImName(CString rName)
	{
		m_rName = rName;
	}

	void CLayerInfo::SetEnable(bool bEnable)
	{
		m_bEnable = bEnable;
	}

	void CLayerInfo::SetOpacity(int nOpacity)
	{
		m_nOpacity = nOpacity;
	}
	/************************************************************************************/
	void* CLayerInfo::GetUserParam()
	{
		return m_lpUserParam;
	}

	int CLayerInfo::GetPrScaling()
	{
		CSRLock lock(&m_lLock);

		return m_nScaling[0];
	}

	std::array<int, 2> CLayerInfo::GetTL()
	{
		CSRLock lock(&m_lLock);

		return m_vTL;
	}

	std::array<int, 2> CLayerInfo::GetWH()
	{
		CSRLock lock(&m_lLock);

		return m_vWH;
	}

	std::array<int, 2> CLayerInfo::GetDragging()
	{
		CSRLock lock(&m_lLock);

		return m_vDragging;
	}

	int CLayerInfo::GetScaling()
	{
		CSRLock lock(&m_lLock);

		return m_nScaling[1];
	}

	ImPtr::CImPtr CLayerInfo::GetImage()
	{
		CSRLock lock(&m_lLock);

		return m_cImage;
	}

	ImPtr::CImPtr CLayerInfo::GetImBRGA()
	{
		CSRLock lock(&m_lLock);

		return m_cImBRGA;
	}

	ImPtr::CImPtr CLayerInfo::GetImPreview()
	{
		CSRLock lock(&m_lLock);

		return m_cImPreview;
	}

	ImPtr::CImPtr CLayerInfo::GetImScaling()
	{
		CSRLock lock(&m_lLock);

		return m_cImScaling;
	}

	bool CLayerInfo::GetEnable()
	{
		return m_bEnable;
	}

	CString CLayerInfo::GetImName()
	{
		return m_rName;
	}

	int CLayerInfo::GetOpacity()
	{
		return m_nOpacity;
	}
	/*****************************************************************************/
	void CLayerInfo::SetTermination(int nIndex, bool bTermination)
	{
		CSWLock lock(&m_lLock);

		m_bTermination[nIndex] = bTermination;
	}

	bool CLayerInfo::IsTermination(int nIndex)
	{
		CSRLock lock(&m_lLock);

		return m_bTermination[nIndex];
	}

	unsigned int __stdcall CLayerInfo::OnChExitThread(void* lpUesrParam)
	{
		CLayerInfo* lpThis = (CLayerInfo*)lpUesrParam;

		lpThis->SetTermination(0, true);

		lpThis->m_cTermination[0].wait();

		return 0;
	}

	unsigned int __stdcall CLayerInfo::OnPrExitThread(void* lpUesrParam)
	{
		CLayerInfo* lpThis = (CLayerInfo*)lpUesrParam;

		lpThis->SetTermination(1, true);

		lpThis->m_cTermination[1].wait();

		return 0;
	}

	unsigned int __stdcall CLayerInfo::OnScExitThread(void* lpUesrParam)
	{
		CLayerInfo* lpThis = (CLayerInfo*)lpUesrParam;

		lpThis->SetTermination(2, true);

		lpThis->m_cTermination[2].wait();

		return 0;
	}

	void CLayerInfo::SetTimer(int nTimer, long long nTime)
	{
		CSWLock lock(&m_lLock);

		m_vTimer[nTimer] = nTime;
	}

	std::array<long long, 5> CLayerInfo::GetTimer()
	{
		CSRLock lock(&m_lLock);

		return m_vTimer;
	}

	unsigned int __stdcall CLayerInfo::OnBGRAThread(void* lpUesrParam)
	{
		CLayerInfo* lpThis = (CLayerInfo*)lpUesrParam;

		while (!lpThis->IsTermination(0))
		{
			auto iTimer = lpThis->GetTimer();

			if (iTimer[0] <= iTimer[1]) { lpThis->m_cBGRA.wait(); continue; }

			auto iImage = lpThis->GetImage();

			if (iImage.Empty()) { lpThis->m_cBGRA.wait(); continue; }

			if (iImage.Get()->channels() == 4){ lpThis->SetImBGRA(iImage); lpThis->SetTimer(1, iTimer[0]); continue; }

			// 当前仅有灰度与bgra
			ImPtr::CImPtr cBGRA(new Mat);

			cvtColor(*iImage.Get(), *cBGRA.Get(), cv::COLOR_GRAY2BGRA);

			lpThis->SetImBGRA(cBGRA); lpThis->SetTimer(1, iTimer[0]);
		}
		lpThis->m_cTermination[0].signal();

		return 0;
	}

	unsigned int __stdcall CLayerInfo::OnPreviewThread(void* lpUesrParam)
	{
		CLayerInfo* lpThis = (CLayerInfo*)lpUesrParam;

		while (!lpThis->IsTermination(0))
		{
			auto iTimer = lpThis->GetTimer();

			if (iTimer[1] <= iTimer[3]) { lpThis->m_cPreview.wait(); continue; }

			auto iImage = lpThis->GetImBRGA();

			if (iImage.Empty()) { lpThis->m_cScaling.wait(); continue; }

			auto& w = lpThis->m_vPreview[0], & h = lpThis->m_vPreview[1];

			ImPtr::CImPtr cPreview(new Mat);

			double dScaling = min((double)w / iImage.Get()->cols, (double)h / iImage.Get()->rows);

			resize(*iImage.Get(), *cPreview.Get(), Size(iImage.Get()->cols * dScaling, iImage.Get()->rows * dScaling));

			int cx = w - cPreview.Get()->cols, cy = h - cPreview.Get()->rows;

			if (cx | cy)
			{
				int t = cy * 0.5, b = cy - t, l = cx * 0.5, r = cx - l;

				copyMakeBorder(*cPreview.Get(), *cPreview.Get(), t, b, l, r, BORDER_CONSTANT, cv::Scalar(0,0,0,0));
			}
			lpThis->SetImPreview(cPreview); lpThis->SetTimer(3, iTimer[1]);
		}
		lpThis->m_cTermination[1].signal();

		return 0;
	}

	unsigned int __stdcall CLayerInfo::OnScalingThread(void* lpUesrParam)
	{
		CLayerInfo* lpThis = (CLayerInfo*)lpUesrParam;

		while (!lpThis->IsTermination(1))
		{
			auto iTimer = lpThis->GetTimer();

			if(max(iTimer[1], iTimer[2]) <= iTimer[4]) { lpThis->m_cScaling.wait(); continue; }

			auto iImage = lpThis->GetImBRGA();

			if (iImage.Empty()) { lpThis->m_cScaling.wait(); continue; }

			int nScaling = lpThis->GetPrScaling();

			ImPtr::CImPtr cImScaling(new Mat);

			resize(*iImage.Get(), *cImScaling.Get(), Size((double)iImage.Get()->cols * nScaling / 1000.0, (double)iImage.Get()->rows * nScaling / 1000.0));

			lpThis->SetScaling(nScaling);

			lpThis->SetImScaling(cImScaling); lpThis->SetTimer(4, max(iTimer[1], iTimer[2]));
		}
		lpThis->m_cTermination[2].signal();

		return 0;
	}
	/********************************************************************************/
	IMPLEMENT_DYNAMIC(CLayer, CLayerInfo)

	BEGIN_MESSAGE_MAP(CLayer, CLayerInfo)
		ON_WM_LBUTTONDOWN()
		ON_MESSAGE(PQM_IM_LAYER_PREVIEW, OnPreviewMsg)
		ON_MESSAGE(PQM_IM_LAYER_SCALING, OnScalingMsg)
	END_MESSAGE_MAP()

	CLayer::CLayer(): m_bFocus(false), m_crColor{ 0x0, 0xff00ff }
	{

	}

	CLayer::~CLayer()
	{

	}

	LRESULT CLayer::OnPreviewMsg(WPARAM wparam, LPARAM lparam)
	{
		m_cPreview.SetPreview(GetImPreview());

		return 0;
	}

	LRESULT CLayer::OnScalingMsg(WPARAM wparam, LPARAM lparam)
	{
		::PostMessage(GetParent()->GetSafeHwnd(), PQM_IM_LAYER_SCALING, wparam, (LPARAM)GetCtrlID());

		return 0;
	}

	void CLayer::OnClickedEnable()
	{
		SetEnable(m_cEnable.GetCheck());

		::PostMessage(GetParent()->GetSafeHwnd(), PQM_IM_LAYER_ENABLE, 0, (LPARAM)GetCtrlID());
	}

	void CLayer::OnLButtonDown(UINT nFlags, CPoint point)
	{
		::PostMessage(GetParent()->GetSafeHwnd(), PQM_IM_LAYER_FOCUS, 0, (LPARAM)GetCtrlID());
	}

	void CLayer::OnNameChanged()
	{
		wchar_t szbuf[256] = { 0 };

		m_cName.GetWindowText(szbuf, 256);

		CLayer::SetImName(szbuf);
	}

	void CLayer::SetIcon(COLORREF crNormal, COLORREF crPushed, std::wstring strIconDir)
	{
		m_crColor[0] = crNormal; m_crColor[1] = crPushed;

		m_cEnable.SetIcon(strIconDir + L"\\Visible");

		m_cName.SetIcon(strIconDir + L"\\Edit.png");
	}

	void CLayer::SetFocus(bool bFocus)
	{
		m_bFocus = bFocus;

		SetBackdrop(m_crColor[bFocus]);

		GetSafeHwnd() ? Invalidate() : []() {}();
	}

	void CLayer::SetImName(CString rName)
	{
		CLayerInfo::SetImName(rName);

		m_cName.SetWindowTextEx(CLayerInfo::GetImName());
	}

	bool CLayer::GetFocus()
	{
		return m_bFocus;
	}

	void CLayer::OnCreateEx(LPCREATESTRUCT& lpCreateStruct)
	{
		SetBackdrop(m_crColor[m_bFocus]);

		OnInitCtrl(lpCreateStruct->cx, lpCreateStruct->cy);

		OnInitCtrl();

		SetChangedFunc(m_cName.GetCtrlID(), bind(&CLayer::OnNameChanged, this));

		SetClickedFunc(m_cEnable.GetCtrlID(), bind(&CLayer::OnClickedEnable, this));

		CLayerInfo::BindPreviewParam({ m_cPreview.GetResolution().x, m_cPreview.GetResolution().y });

		CLayerInfo::OnCreateEx(lpCreateStruct);
	}

	void CLayer::OnInitCtrl(const int& cx, const int& cy)
	{
		m_cEnable.SetSpace({ 2, 2 });
		m_cEnable.SetResolution({ cy - 2 * 2, cy - 2 * 2 });

		m_cPreview.SetSpace({ 2, 0 });
		m_cPreview.SetResolution({ cy - 2 * 2, cy - 2 * 2 });

		m_cName.SetSpace({ 15, 15-2 });
		m_cName.SetResolution({ cx - 2*cy - 2 * 15, cy - 2 * 15 });
		m_cName.SetTextOrigin({10,2});
	}

	void CLayer::OnInitCtrl()
	{
		CRect rtCtrl;
		rtCtrl.left = m_cEnable.GetSpace().x;
		rtCtrl.top = m_cEnable.GetSpace().y;
		rtCtrl.right = rtCtrl.left + m_cEnable.GetResolution().x;
		rtCtrl.bottom = rtCtrl.top + m_cEnable.GetResolution().y;
		m_cEnable.Create(L"", WS_CHILD | WS_VISIBLE | SS_NOTIFY, rtCtrl, this, 1);

		rtCtrl = m_cEnable.GetChildRect();
		rtCtrl.left = rtCtrl.right + m_cPreview.GetSpace().x;
		rtCtrl.top = rtCtrl.top + m_cPreview.GetSpace().y;
		rtCtrl.right = rtCtrl.left + m_cPreview.GetResolution().x;
		rtCtrl.bottom = rtCtrl.top + m_cPreview.GetResolution().y;
		m_cPreview.Create(L"", WS_CHILD | WS_VISIBLE | SS_NOTIFY, rtCtrl, this, 2);

		rtCtrl = m_cPreview.GetChildRect();
		rtCtrl.left = rtCtrl.right + m_cName.GetSpace().x;
		rtCtrl.top = rtCtrl.top + m_cName.GetSpace().y;
		rtCtrl.right = rtCtrl.left + m_cName.GetResolution().x;
		rtCtrl.bottom = rtCtrl.top + m_cName.GetResolution().y;
		m_cName.Create(L"", WS_CHILD | WS_VISIBLE | WS_TABSTOP | SS_NOTIFY, rtCtrl, this, 3);
	}
	/********************************************************************************/
	IMPLEMENT_DYNAMIC(CLayers, ImCtrl::CImCtrl)

	BEGIN_MESSAGE_MAP(CLayers, ImCtrl::CImCtrl)
		ON_WM_VSCROLL()
		ON_WM_ERASEBKGND()
		ON_MESSAGE(PQM_IM_LAYER_FOCUS, OnLayerFocusChanged)
		ON_MESSAGE(PQM_IM_LAYER_ENABLE, OnLayerEnableChanged)
		ON_MESSAGE(PQM_IM_LAYER_SCALING, OnLayerScalingChanged)
	END_MESSAGE_MAP()

	CLayers::CLayers():
		m_cVer(true), m_nFocusLayer(0), m_nBackground(0)
	{

	}

	CLayers::~CLayers()
	{

	}

	LRESULT CLayers::OnLayerFocusChanged(WPARAM wparam, LPARAM lparam)
	{
		SetFocus(Id2Index(lparam));

		return 0;
	}

	LRESULT CLayers::OnLayerEnableChanged(WPARAM wparam, LPARAM lparam)
	{
		::PostMessage(GetParent()->GetSafeHwnd(), PQM_IM_LAYER_ENABLE, wparam, (LPARAM)Id2Index(lparam));

		return 0;
	}

	LRESULT CLayers::OnLayerScalingChanged(WPARAM wparam, LPARAM lparam)
	{
		::PostMessage(GetParent()->GetSafeHwnd(), PQM_IM_LAYER_SCALING, wparam, (LPARAM)Id2Index(lparam));

		return 0;
	}

	void CLayers::ClearFocus()
	{
		for(auto& iLayer : m_vLayer) iLayer->SetFocus(false);
	}

	int CLayers::Id2Index(int nCtrId)
	{
		for (int i = 0; i < m_vLayer.size(); ++i)
		{
			if (m_vLayer[i]->GetCtrlID() == nCtrId) return i;
		}
		return 0;
	}

	int CLayers::GetFocus()
	{
		return m_nFocusLayer;
	}

	void CLayers::SetFocus(int nIndex)
	{
		m_nFocusLayer = nIndex;

		ClearFocus(); 
		
		m_vLayer[m_nFocusLayer]->SetFocus(true); // 重置选中

		::PostMessage(GetParent()->GetSafeHwnd(), PQM_IM_LAYER_FOCUS, 0, (LPARAM)m_nFocusLayer);
	}

	int CLayers::GetLayerCount() 
	{ 
		return m_vLayer.size(); 
	}

	void CLayers::OnCreateLayer()
	{
		CLayer* lpLayer = new CLayer();

		lpLayer->Create(L"", WS_CHILD | WS_VISIBLE | SS_NOTIFY, CRect(0, 0, GetResolution().x - m_cVer.GetResolution().x - 1, 50), this, LayerId());

		m_vLayer.push_back(lpLayer);

		RefreshLayer();
	}

	void CLayers::OnCreateLayer(int nLayer)
	{
		for (int i = 0; i < nLayer; ++i)
		{
			CLayer* lpLayer = new CLayer();

			lpLayer->Create(L"", WS_CHILD | WS_VISIBLE | SS_NOTIFY, CRect(0, 0, GetResolution().x - m_cVer.GetResolution().x - 1, 50), this, LayerId());

			m_vLayer.push_back(lpLayer);
		}
		RefreshLayer();
	}

	void CLayers::RefreshLayer()
	{
		if (m_vLayer.empty()) {m_cVer.ShowWindow(SW_HIDE); return;}

		for (int i = 0; i < m_vLayer.size(); ++i)
		{
			auto& iLayer = m_vLayer[i];

			iLayer->SetSpace({ 0, i * 50 });

			iLayer->SetResolution({ GetResolution().x - m_cVer.GetResolution().x - 1, 50 });

			CRect rtCtrl;

			rtCtrl.left = iLayer->GetSpace().x;

			rtCtrl.top = iLayer->GetSpace().y;

			rtCtrl.right = rtCtrl.left + iLayer->GetResolution().x;

			rtCtrl.bottom = rtCtrl.top + iLayer->GetResolution().y;

			m_vLayer[i]->SetChildRect(rtCtrl);
		}
		OnLayerCountChanged();
	}

	void CLayers::OnDestoryLayer(int nIndex)
	{
		ASSERT(nIndex < m_vLayer.size());

		auto iDestory = m_vLayer.begin() + nIndex;

		delete *iDestory;

		iDestory = m_vLayer.erase(iDestory);

		RefreshLayer();
	}

	void CLayers::OnDestoryLayer(std::vector<int> vIndex)
	{
		sort(vIndex.begin(), vIndex.end(), [](const int& i1, const int& i2) {return i2 < 11; });

		for (auto& iIndex : vIndex)
		{
			auto iDestory = m_vLayer.begin() + iIndex;

			delete *iDestory;

			iDestory = m_vLayer.erase(iDestory);
		}
		RefreshLayer();
	}

	void CLayers::OnDestoryLayer()
	{
		for (auto& iLayer : m_vLayer) delete iLayer;
	
		m_vLayer.clear();

		RefreshLayer();
	}

	BOOL CLayers::OnEraseBkgnd(CDC* pDC)
	{
		return TRUE;
	}

	void CLayers::SetIcon(std::wstring strIconDir)
	{
		m_cVer.SetIcon(strIconDir + L"\\Scrollbar\\Ver");
	}

	int CLayers::LayerId()
	{
		for (int i = 2; true; ++i)
		{
			auto iCur = _Find_pr(m_vLayer.begin(), m_vLayer.end(), i, [](CLayer* lpLayer, int id) { return lpLayer->GetCtrlID() == id;});

			if (iCur == m_vLayer.end()) return i;
		}
		return 0;
	}

	void CLayers::OnLayerCountChanged()		// 更新滚动比例
	{
		if (m_vLayer.empty()) { m_cVer.ShowWindow(SW_HIDE); return; }

		auto iBegin = *m_vLayer.begin();

		m_cVer.OnScalarChanged(GetResolution().y, m_vLayer.size() * iBegin->GetResolution().y);
	}

	void CLayers::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
	{
		int nTop = nPos * m_cVer.GetRatio();

		HDWP hDWP = BeginDeferWindowPos(m_vLayer.size());

		if (hDWP)
		{
			for (auto& iLayer : m_vLayer)
			{
				auto iChild = iLayer->GetChildRect();
				
				hDWP = DeferWindowPos(hDWP, iLayer->GetSafeHwnd(), NULL,
					iChild.left, iChild.top - nTop, iChild.Width(), iChild.Height(), SWP_NOZORDER);
			}
			EndDeferWindowPos(hDWP);
		}
		UpdateWindow();

		ImCtrl::CImCtrl::OnVScroll(nSBCode, nPos, pScrollBar);
	}

	void CLayers::OnDestroyEx()
	{
		for (auto& iLayer : m_vLayer) delete iLayer;

		m_vLayer.clear();
	}

	void CLayers::OnCreateEx(LPCREATESTRUCT& lpCreateStruct)
	{
		SetBackdrop(0x0);

		OnInitCtrl(lpCreateStruct->cx, lpCreateStruct->cy);

		OnInitCtrl();
	}

	void CLayers::OnInitCtrl(const int& cx, const int& cy)
	{
		m_cVer.SetSpace({cx - 26, 0});

		m_cVer.SetResolution({ 26, cy });
	}

	void CLayers::OnInitCtrl()
	{
		CRect rtCtrl;
		rtCtrl.left = m_cVer.GetSpace().x;
		rtCtrl.top = m_cVer.GetSpace().y;
		rtCtrl.right = rtCtrl.left + m_cVer.GetResolution().x;
		rtCtrl.bottom = rtCtrl.top + m_cVer.GetResolution().y;
		m_cVer.Create(L"", WS_CHILD | SS_NOTIFY, rtCtrl, this, 1);
	}
}
