#include "pch.h"
#include "ThForeground.h"
#include "Transform.h"
#include "Helper.h"
#include <numeric>

CThForeground::CThForeground():
	m_nBlock(0)
{

}

CThForeground::~CThForeground()
{

}

bool CThForeground::Reg()
{
	if (m_uWorkType == eWT_Foreground)
	{
		CIm2Mat::SetSignal(bind(&CThForeground::OnImCache, this, placeholders::_1), bind(&CThForeground::IsIdle, this));

		CImPadding::SetThParam(&m_cImCache, bind(&CThForeground::OnThForeground, this));
	}
	return CImPadding::Reg();
}

void CThForeground::Unreg()
{
	CImPadding::Unreg();

	Clear();
}

void CThForeground::SetForegroundParam(vector<array<wstring, 2>>& vFile, int nFrameInLine, int nBlock)
{
	m_nBlock = nBlock; m_vTh.clear();

	vector<vector<array<string, 2>>> vCache;

	for (int i = 0; i < vFile.size(); i += nFrameInLine)
	{
		vCache.push_back({ { Wstring2String(vFile[i][0]), Wstring2String(vFile[i][1]) } });
	}
	SetFiles(vCache);
}

bool CThForeground::IsIdle()
{
	int nSize = 0; static int g_nTh = 10;
	{
		CSRLock lock(&m_lLock);

		nSize = m_vImCache.size();
	}
	return nSize < g_nTh;
}

void CThForeground::OnImCache(lpIm2Mat& lpCache)
{
	Push(lpCache);

	m_cImCache.signal();
}

void CThForeground::Push(lpIm2Mat& lpCache)
{
	CSWLock lock(&m_lLock);

	m_vImCache.push_back(lpCache);
}

bool CThForeground::Pop(lpIm2Mat& lpCache)
{
	bool bEmpty = true;
	{
		CSWLock lock(&m_lLock);

		if (!(bEmpty = m_vImCache.empty()))
		{
			auto iFront = m_vImCache.begin();

			lpCache = *iFront;

			iFront = m_vImCache.erase(iFront);
		}
	}
	return !bEmpty;
}

void CThForeground::Clear()
{
	CSWLock lock(&m_lLock);

	m_vImCache.clear();

	m_vTh.clear();
}

 // 行列方向开窗
void CThForeground::OnThForeground()
{
	lpIm2Mat lpCache = nullptr;

	if (!Pop(lpCache)) { m_cImCache.wait(); return; }

	ASSERT(lpCache->vIm2Mat.size() == 1);

	auto& strImage = lpCache->strFirst;

	auto& lpImage = lpCache->vIm2Mat[0];

	int nThreshold = 0, nThresholdType = 0;

	ImPtr::CImPtr cImPtr(new Mat), cHistogram(new Mat(Mat::zeros(1, 257, CV_32SC1)));

	*cImPtr.Get() = lpImage->clone();

	int* lpHis = (int*)cHistogram.Get()->data;

	int w = 1.0 / m_nBlock * lpImage->cols, h = 1.0 / m_nBlock * lpImage->rows;

	for (int y = 0; y < m_nBlock; ++y)
	{
		for (int x = 0; x < m_nBlock; ++x)
		{
			Rect rtWin(x * w, y * h, w, h);

			Mat cWin = (*lpImage)(rtWin).clone(), cWinHis;

			Histogram(cWin, cWinHis);

			int* lpWin = (int*)cWinHis.data;

			for (int i = 0; i < cWinHis.cols; ++i)
			{
				lpHis[i] = max(lpHis[i], lpWin[i]);
			}
		}
	}

	BimodalThresholdEx(*cHistogram.Get(), nThreshold, nThresholdType);
	
	lpHis[256] = nThreshold;

	m_vTh.push_back(nThreshold);

	OnHistogram(lpCache->nRemaining, cImPtr, cHistogram);
	
	if(!!!lpCache->nRemaining) OnForeground(floor(std::accumulate(m_vTh.begin(), m_vTh.end(), 0) / m_vTh.size()));

	delete lpCache;

//	Sleep(1000 * 60*3);
}