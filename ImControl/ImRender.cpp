#include "pch.h"
#include "ImRender.h"

namespace ImRender
{
	IMPLEMENT_DYNAMIC(CImRender, ImCtrl::CImCtrl)

	BEGIN_MESSAGE_MAP(CImRender, ImCtrl::CImCtrl)

	END_MESSAGE_MAP()

	CImRender::CImRender():
		m_lpImCenter(nullptr), m_nTimer{0,0}, m_bTermination(false)
	{

	}

	CImRender::~CImRender()
	{

	}

	long long CImRender::Timer()
	{
		LARGE_INTEGER tTimer = { 0 };

		QueryPerformanceCounter(&tTimer);

		return tTimer.QuadPart;
	}

	bool CImRender::IsTimeOut()
	{
		CSRLock lock(&m_lLock);

		return (m_nTimer[1] < m_nTimer[0]);
	}

	void CImRender::ResetTimer(int nTimer, long long nTime)
	{
		CSWLock lock(&m_lLock);

		m_nTimer[nTimer] = nTime;
	}

	void CImRender::OnSyncImThumb(tuple<int, double, int, double> vImThumb)
	{
		long long nTime = Timer();

		SetImThumb(vImThumb);

		ResetTimer(0, nTime);

		m_cRender.signal();
	}

	void CImRender::SetImThumb(tuple<int, double, int, double>& vImThumb)
	{
		CSWLock lock(&m_lLock);

		m_vImThumb = vImThumb;
	}

	void CImRender::GetImThumb(tuple<int, double, int, double>& vImThumb)
	{
		CSRLock lock(&m_lLock);

		vImThumb = m_vImThumb;
	}

	void CImRender::OnCreateEx(LPCREATESTRUCT& lpCreateStruct)
	{
		SetBackdrop(0);

		m_cImRender = Mat::zeros(lpCreateStruct->cy, lpCreateStruct->cx, CV_8UC4);

		m_cImCache = Mat::zeros(lpCreateStruct->cy, lpCreateStruct->cx, CV_8UC4);

		CloseHandle((HANDLE)_beginthreadex(NULL, 0, OnThRender, (void*)this, 0, NULL));
	}

	void CImRender::OnDestroyEx()
	{
		CloseHandle((HANDLE)_beginthreadex(NULL, 0, OnThExit, (void*)this, 0, NULL));

		m_cTermination.wait();
	}

	unsigned int __stdcall CImRender::OnThRender(void* lpUesrParam)
	{
		CImRender* lpThis = (CImRender*)lpUesrParam;

		while (!lpThis->IsTermination())
		{
			if (!lpThis->IsTimeOut()) { lpThis->m_cRender.wait(); continue; }

			long long nTime = lpThis->Timer();

			lpThis->OnRender();

			lpThis->ResetTimer(1, nTime);
		}
		lpThis->m_cTermination.signal();

		return 0;
	}

	void CImRender::OnRender()
	{
		vector<long long> vSign;

		tuple<int, double, int, double> vImThumb;

		GetImThumb(vImThumb);

		m_lpImCenter->GetSign(vSign);

		memset(m_cImRender.data, 0, (uint64)m_cImRender.rows* m_cImRender.cols * m_cImRender.channels());

		for (auto& iSign : vSign) OnRender(iSign, vImThumb, m_cImRender);

		OnRender(m_cImRender, m_cImCache); Invalidate();
	}

	void CImRender::OnRender(long long& nSign, tuple<int, double, int, double>& vImThumb, Mat& cImRender)
	{
		ImLayer::tagLayer tParam;

		if (!m_lpImCenter->GetRenderParam(nSign, tParam)) return;

		if (!tParam.bEnable) return;

		OnRender(tParam, vImThumb, m_cImRender);
	}

	void CImRender::OnRender(ImLayer::tagLayer& tParam, tuple<int, double, int, double>& vImThumb, Mat& cImRender)
	{
		if (tParam.cBRGA.Empty()) return;

		auto iRender = tParam.cBRGA;

		auto iRenderTo = ImPtr::CImPtr(new Mat);

		double dCRotio = (double)(tParam.dContrastRotio / 100);

		cv::convertScaleAbs(*iRender.Get(), *iRenderTo.Get(), dCRotio);

		auto x = tParam.nLeft, y = tParam.nTop, w = tParam.nWidth, h = tParam.nHeight;

		w = min(w, iRenderTo.Get()->cols), h = min(h, iRenderTo.Get()->rows); // 渲染线程未同步完成

		Rect rtImage(x, y, w, h);// 缩放后图片位于背景中位置 		

		Rect rtWin(get<1>(vImThumb) * get<0>(vImThumb), get<3>(vImThumb) * get<2>(vImThumb), cImRender.cols, cImRender.rows); // // 视野窗口在背景中位置
	
		auto rtUnite = rtImage & rtWin;

		if (rtUnite.empty()) return;	// 相交则绘制

		rtImage.x = rtUnite.x - rtImage.x, rtImage.y = rtUnite.y - rtImage.y;

		rtImage.width = rtUnite.width; rtImage.height = rtUnite.height;

		rtWin.x = rtUnite.x - rtWin.x, rtWin.y = rtUnite.y - rtWin.y;

		rtWin.width = rtUnite.width; rtWin.height = rtUnite.height;; // 相交区域

		auto iImage = (*iRenderTo.Get())(rtImage), iWin = cImRender(rtWin);

		addWeighted(iImage, tParam.dOpacity, iWin, (1.0 - tParam.dOpacity), 0.0, iWin);
	}

	void CImRender::GetRender(Mat& cImRender)
	{
		CSRLock lock(&m_lLock);

		memcpy(cImRender.data, m_cImCache.data, m_cImCache.rows * m_cImCache.cols * m_cImCache.channels());
	}

	void CImRender::OnRender(Mat& cImRender, Mat& cImCache)
	{
		CSWLock lock(&m_lLock);

		memcpy(cImCache.data, cImRender.data, cImRender.rows * cImRender.cols * cImRender.channels());
	}

	void CImRender::Termination()
	{
		CSWLock lock(&m_lLock);

		m_bTermination = true;
	}

	bool CImRender::IsTermination()
	{
		CSRLock lock(&m_lLock);

		return m_bTermination;
	}

	unsigned int __stdcall CImRender::OnThExit(void* lpUesrParam)
	{
		CImRender* lpThis = (CImRender*)lpUesrParam;

		lpThis->Termination();

		lpThis->m_cRender.signal();

		return 0;
	}
}