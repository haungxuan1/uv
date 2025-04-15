#include "pch.h"
#include "CImView.h"
#include "PQMsg.h"
#include "ImControl.h"
using namespace cv;
using namespace std;

namespace ImView
{
	IMPLEMENT_DYNAMIC(CImThread, ImCtrl::CImCtrl)

	BEGIN_MESSAGE_MAP(CImThread, ImCtrl::CImCtrl)
		ON_MESSAGE(PQM_IM_VIEW_RENDER, OnRenderMsg)
	END_MESSAGE_MAP()
	CImThread::CImThread() :
		m_bTermination(false), m_vTimer{ 0, 0 }, m_lpRender(nullptr), m_vImThumb{ 0,0.0,0,0.0 }, m_lpLayers(nullptr)
	{

	}

	CImThread::~CImThread()
	{

	}

	void CImThread::OnCreateEx(LPCREATESTRUCT& lpCreateStruct)
	{
		m_lpRender = new uchar[m_uBufferSize];

		CloseHandle((HANDLE)_beginthreadex(NULL, 0, OnRenderThread, (void*)this, 0, NULL));
	}

	void CImThread::OnDestroyEx()
	{
		CloseHandle((HANDLE)_beginthreadex(NULL, 0, OnExitThread, (void*)this, 0, NULL));

		m_cTermination.wait();

		delete[] m_lpRender;
	}

	void CImThread::Termination()
	{
		CSWLock lock(&m_lLock);

		m_bTermination = true;
	}

	bool CImThread::IsTermination()
	{
		CSRLock lock(&m_lLock);

		return m_bTermination;
	}

	unsigned int __stdcall CImThread::OnExitThread(void* lpUesrParam)
	{
		CImThread* lpThis = (CImThread*)lpUesrParam;

		lpThis->Termination();

		lpThis->m_cRender.signal();

		return 0;
	}

	LRESULT CImThread::OnRenderMsg(WPARAM wparam, LPARAM lparam)
	{
		tuple<int, double, int, double>* lpImThumb = (tuple<int, double, int, double>*)lparam;

		LARGE_INTEGER tTimer = { 0 }; QueryPerformanceCounter(&tTimer);

		SetRenderParam(*lpImThumb);

		SetTimer(0, tTimer.QuadPart);

		m_cRender.signal();

		delete lpImThumb;

		return 0;
	}

	void CImThread::SetRenderParam(tuple<int, double, int, double>& vImThumb)
	{
		CSWLock lock(&m_lLock);

		m_vImThumb = vImThumb;
	}

	void CImThread::GetRenderParam(tuple<int, double, int, double>& vImThumb)
	{
		CSRLock lock(&m_lLock);

		vImThumb = m_vImThumb;
	}

	void CImThread::SetTimer(int nTimer, long long nTime)
	{
		CSWLock lock(&m_lLock);

		m_vTimer[nTimer] = nTime;
	}

	std::array<long long, 2> CImThread::GetTimer()
	{
		CSRLock lock(&m_lLock);

		return m_vTimer;
	}

	unsigned int __stdcall CImThread::OnRenderThread(void* lpUesrParam)
	{
		CImThread* lpThis = (CImThread*)lpUesrParam;

		while (!lpThis->IsTermination())
		{
			auto iTimer = lpThis->GetTimer();

			if (iTimer[0] <= iTimer[1]) { lpThis->m_cRender.wait(); continue; }

			tuple<int, double, int, double> vImThumb;

			lpThis->GetRenderParam(vImThumb);

			lpThis->Render(vImThumb);

			lpThis->SetTimer(1, iTimer[0]);
		}
		lpThis->m_cTermination.signal();

		return 0;
	}

	void CImThread::Render(std::tuple<int, double, int, double>& vImThumb)
	{
		Mat cRender(Mat::zeros(m_rtCanvas.Height(), m_rtCanvas.Width(), CV_8UC4));

		if (m_lpLayers->Empty()) { Render(cRender); Invalidate(); return; }
	
		int nFocus = m_lpLayers->GetFocus();
	
		int nBackground = m_lpLayers->GetBackgroundLayer();

		nFocus = (nFocus == nBackground ? (-1) : nFocus);

		int nLayer = m_lpLayers->GetLayerCount();

		for (int i = 0; i < nLayer; ++i) if (m_lpLayers->GetEnable(i)) Render(vImThumb, nFocus, i, cRender);

		Render(cRender); Invalidate();
	}

	void CImThread::Render(tuple<int, double, int, double>& vImThumb, int nFocus, int nLayer, Mat& cRender)
	{
		auto iImScaling = m_lpLayers->GetImScaling(nLayer);

		if (iImScaling.Empty()) return;
	
		auto iTL = m_lpLayers->GetTL(nLayer);
		auto iWH = m_lpLayers->GetWH(nLayer);
		auto iDragging = m_lpLayers->GetDragging(nLayer);
		double dScaling = 0.001 * m_lpLayers->GetScaling(nLayer);
		iTL[0] *= dScaling;	iTL[1] *= dScaling;	
		iTL[0] += iDragging[0], iTL[1] += iDragging[1];
		iWH[0] *= dScaling;	iWH[1] *= dScaling; 
		iWH[0] = min(iWH[0], iImScaling.Get()->cols);
		iWH[1] = min(iWH[1], iImScaling.Get()->rows);	// 浮点型转整形精度丢失
		Rect rtImage(iTL[0], iTL[1], iWH[0], iWH[1]);	// 缩放后图片在背景中位置


		Rect rtWin(0, 0, cRender.cols, cRender.rows);
		rtWin.x += (get<1>(vImThumb) * get<0>(vImThumb)), rtWin.y += (get<3>(vImThumb) * get<2>(vImThumb));// 视野窗口在背景中位置
		
		auto rtUnite = rtImage & rtWin;
		if (rtUnite.empty()) return;
		
		rtImage.x = rtUnite.x - rtImage.x, rtImage.y = rtUnite.y - rtImage.y;
		rtImage.width = rtUnite.width; rtImage.height = rtUnite.height;
		Rect rtRender;
		rtRender.x = rtUnite.x - rtWin.x, rtRender.y = rtUnite.y - rtWin.y;
		rtRender.width = rtUnite.width; rtRender.height = rtUnite.height;; // 相交区域

		auto iImage = (*iImScaling.Get())(rtImage), iView = cRender(rtRender);
		double dOpacity = 0.01 * m_lpLayers->GetOpacity(nLayer);
		addWeighted(iImage, dOpacity, iView, (1.0 - dOpacity), 0.0, iView);
		nFocus != nLayer ? []() {}(): rectangle(cRender, rtRender, Scalar(0x0, 0x0, 0xff, 0xff), 10);
	}

	void CImThread::Render(Mat& cRender)
	{
		CSWLock lock(&m_lLock);

		uint64 idx[2] = {0,0};

		for (uint y = 0; y < cRender.rows; ++y)
		{
			for (uint x = 0; x < cRender.cols; ++x)
			{
				idx[0] = (y * cRender.cols + x) * 4;

				idx[1] = (y * m_tBitmapHeader.biWidth + x) * 4;

				for (uint64 c = 0; c < 4; ++c)
				{
					m_lpRender[idx[1] + c] = cRender.data[idx[0] + c];
				}
			}
		}
	}

	void CImThread::GetRender(uchar*& lpRender)
	{
		CSRLock lock(&m_lLock);

		memcpy(lpRender, m_lpRender, m_uBufferSize);
	}
	/********************************************************************************/
	IMPLEMENT_DYNAMIC(CImage, CImThread)

	BEGIN_MESSAGE_MAP(CImage, CImThread)
		ON_WM_MOUSEMOVE()
		ON_WM_LBUTTONUP()
		ON_WM_LBUTTONDOWN()
	END_MESSAGE_MAP()
	CImage::CImage():
		m_lpRender(nullptr)
	{

	}

	CImage::~CImage()
	{

	}

	BOOL CImage::PreTranslateMessage(MSG* pMsg)
	{
		if (pMsg->message == WM_KEYDOWN)
		{
			union { int n; uint u; }x, y;

			x.n = (pMsg->wParam != VK_LEFT ? 0 : -1);

			x.n = (pMsg->wParam != VK_RIGHT ? x.n : 1);

			y.n = (pMsg->wParam != VK_UP ? 0 : -1);

			y.n = (pMsg->wParam != VK_DOWN ? y.n : 1);

			if (!!x.n || !!y.n) { ::PostMessage(GetParent()->GetSafeHwnd(), PQM_IM_VIEW_DRAG, x.u, y.n); return true; }
		}
		return CImThread::PreTranslateMessage(pMsg);
	}
	static CRect g_rt(0,0,0,0);

	void CImage::OnLButtonDown(UINT nFlags, CPoint point)
	{
		SetFocus();

		SetCapture();

		m_ptLBDown = point; 

		if (!g_rt.IsRectEmpty())
		{
			CClientDC dc(this);

			dc.SetROP2(R2_NOTXORPEN);

			dc.DrawFocusRect(g_rt);
		}
		g_rt.SetRect(point, point);
	}

	void CImage::OnMouseMove(UINT nFlags, CPoint point)
	{
		if (!(nFlags & MK_LBUTTON)) return;

		CClientDC dc(this);

		dc.SetROP2(R2_NOTXORPEN); // 设置绘图模式为异或模式

		if(!g_rt.IsRectEmpty())
			dc.DrawFocusRect(g_rt);

		g_rt.left = min(m_ptLBDown.x, point.x);

		g_rt.right = max(m_ptLBDown.x, point.x);

		g_rt.top = min(m_ptLBDown.y, point.y);

		g_rt.bottom = max(m_ptLBDown.y, point.y);

		if (!g_rt.IsRectEmpty())
			dc.DrawFocusRect(g_rt);




		/*HCURSOR hCursor = AfxGetApp()->LoadStandardCursor(IDC_SIZEALL);

		::SetCursor(hCursor);

		union { int n; uint u; }x, y;

		x.n = point.x - m_ptLBDown.x;

		y.n = point.y - m_ptLBDown.y;

		::PostMessage(GetParent()->GetSafeHwnd(), PQM_IM_VIEW_DRAGGING, x.u, y.n);*/
	}

	void CImage::OnLButtonUp(UINT nFlags, CPoint point)
	{
		ReleaseCapture();

		union { int n; uint u; }x, y;

		x.n = point.x - m_ptLBDown.x;

		y.n = point.y - m_ptLBDown.y;

		::PostMessage(GetParent()->GetSafeHwnd(), PQM_IM_VIEW_DRAG, x.u, y.n);
	}

	void CImage::OnDestroyEx()
	{
		CImThread::OnDestroyEx();

		delete[] m_lpRender;
	}

	void CImage::OnCreateEx(LPCREATESTRUCT& lpCreateStruct)
	{
		CImThread::OnCreateEx(lpCreateStruct);

		m_lpRender = new uchar[m_uBufferSize];

		SetBackdrop(0x0ff);
	}

	void CImage::OnDrawEx(CDC& dc, const CRect& rtCanvas, unsigned char*& lpPixel, const long& biWidth)
	{
		GetRender(m_lpRender);

		m_lpRender = UpdatePixel(m_lpRender);
	}
	/********************************************************************************/
	IMPLEMENT_DYNAMIC(CImView, ImCtrl::CImCtrl)

	BEGIN_MESSAGE_MAP(CImView, ImCtrl::CImCtrl)
		ON_MESSAGE(PQM_IM_VIEW_RENDER, OnRenderMsg)
		ON_MESSAGE(PQM_IM_VIEW_DRAGGING, OnDraggingMsg)
		ON_MESSAGE(PQM_IM_VIEW_DRAG, OnDragMsg)
		ON_WM_MOUSEWHEEL()
		ON_WM_ERASEBKGND()
		ON_WM_HSCROLL()
		ON_WM_VSCROLL()
	END_MESSAGE_MAP()
	CImView::CImView():
		m_cHor(false), m_cVer(true), 
		m_vImThumb{0,0.0,0,0.0}
	{

	}

	CImView::~CImView()
	{

	}

	void CImView::SetIcon(std::wstring strIconDir)
	{
		m_cHor.SetIcon(strIconDir + L"\\Scrollbar\\Hor");

		m_cVer.SetIcon(strIconDir + L"\\Scrollbar\\Ver");
	}

	LRESULT CImView::OnDraggingMsg(WPARAM wparam, LPARAM lparam)
	{
		::PostMessage(GetParent()->GetSafeHwnd(), PQM_IM_VIEW_DRAGGING, wparam, lparam);

		return 0;
	}

	LRESULT CImView::OnDragMsg(WPARAM wparam, LPARAM lparam)
	{
		::PostMessage(GetParent()->GetSafeHwnd(), PQM_IM_VIEW_DRAG, wparam, lparam);

		return 0;
	}

	LRESULT CImView::OnRenderMsg(WPARAM wparam, LPARAM lparam)
	{
		int nBackground = m_lpLayers->GetBackgroundLayer();

		auto iImScaling = m_lpLayers->GetImScaling(nBackground);

		if (iImScaling.Empty()) return 0;

		m_cHor.OnScalarChanged(m_cImage.GetResolution().x, iImScaling.Get()->cols);

		m_cVer.OnScalarChanged(m_cImage.GetResolution().y, iImScaling.Get()->rows);// 计算缩放后视野

		return 0;
	}

	void CImView::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
	{
		get<0>(m_vImThumb) = nPos;

		get<1>(m_vImThumb) = m_cHor.GetRatio();

		::PostMessage(m_cImage.GetSafeHwnd(), 
			PQM_IM_VIEW_RENDER, 
			0,
			(LPARAM)new tuple<int, double, int, double>(m_vImThumb));
	}

	void CImView::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
	{
		get<2>(m_vImThumb) = nPos;

		get<3>(m_vImThumb) = m_cVer.GetRatio();

		::PostMessage(m_cImage.GetSafeHwnd(),
			PQM_IM_VIEW_RENDER,
			0,
			(LPARAM)new tuple<int, double, int, double>(m_vImThumb));
	}

	void CImView::OnScaling(int nScaling)
	{
		::PostMessage(GetParent()->GetSafeHwnd(), PQM_IM_VIEW_SCALING, 0, (LPARAM)nScaling);
	}

	void CImView::OnHWheel(UINT nFlags, short zDelta, CPoint pt)
	{
		if (!m_cHor.IsWindowVisible()) return;

		::PostMessage(
			m_cHor.GetSafeHwnd(),
			WM_MOUSEWHEEL,
			MAKELONG(nFlags, zDelta),
			MAKELONG(pt.x, pt.y));
	}

	void CImView::OnWheel(UINT nFlags, short zDelta, CPoint pt)
	{
		if (!m_cVer.IsWindowVisible()) return;

		::PostMessage(
			m_cVer.GetSafeHwnd(),
			WM_MOUSEWHEEL,
			MAKELONG(nFlags, zDelta),
			MAKELONG(pt.x, pt.y));
	}

	BOOL CImView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
	{
		if (GetAsyncKeyState(VK_MENU) & 0x8000) { OnScaling(zDelta / WHEEL_DELTA); return TRUE; }

		if (GetAsyncKeyState(VK_CONTROL) & 0x8000) { OnHWheel(nFlags, zDelta, pt); return TRUE; }

		OnWheel(nFlags, zDelta, pt);

		return TRUE;
	}

	BOOL CImView::OnEraseBkgnd(CDC* pDC)
	{
		return TRUE;
	}

	void CImView::OnCreateEx(LPCREATESTRUCT& lpCreateStruct)
	{
		m_cImage.BindLayers(m_lpLayers);

		SetBackdrop(0xf0f0f0);

		OnInitCtrl(lpCreateStruct->cx, lpCreateStruct->cy);

		OnInitCtrl();
	}

	void CImView::OnInitCtrl(const int& cx, const int& cy)
	{
		m_cImage.SetSpace({ 0,0 });
		m_cImage.SetResolution({cx-26-1,cy-26-1});

		m_cHor.SetSpace({ 0, 1 });
		m_cHor.SetResolution({ cx - 26, 26 });

		m_cVer.SetSpace({ 1, 0 });
		m_cVer.SetResolution({ 26, cy-26 });
	}

	void CImView::OnInitCtrl()
	{
		CRect rtCtrl;

		rtCtrl.left = m_cImage.GetSpace().x;
		rtCtrl.top = m_cImage.GetSpace().y;
		rtCtrl.right = rtCtrl.left + m_cImage.GetResolution().x;
		rtCtrl.bottom = rtCtrl.top + m_cImage.GetResolution().y;
		m_cImage.Create(L"", WS_CHILD | WS_VISIBLE | SS_NOTIFY, rtCtrl, this, 1);

		rtCtrl = m_cImage.GetChildRect();
		rtCtrl.left = rtCtrl.left + m_cHor.GetSpace().x;
		rtCtrl.top = rtCtrl.bottom + m_cHor.GetSpace().y;
		rtCtrl.right = rtCtrl.left + m_cHor.GetResolution().x;
		rtCtrl.bottom = rtCtrl.top + m_cHor.GetResolution().y;
		m_cHor.Create(L"", WS_CHILD | SS_NOTIFY, rtCtrl, this, 2);

		rtCtrl = m_cImage.GetChildRect();
		rtCtrl.left = rtCtrl.right + m_cVer.GetSpace().x;
		rtCtrl.top = rtCtrl.top + m_cVer.GetSpace().y;
		rtCtrl.right = rtCtrl.left + m_cVer.GetResolution().x;
		rtCtrl.bottom = rtCtrl.top + m_cVer.GetResolution().y;
		m_cVer.Create(L"", WS_CHILD | SS_NOTIFY, rtCtrl, this, 3);
	}
}