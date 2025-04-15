#include "pch.h"
#include "ThReference.h"
#include "Transform.h"
#include "Helper.h"

CThReference::CThReference()
{

}

CThReference::~CThReference()
{

}

bool CThReference::Reg()
{
	if (m_uWorkType == eWT_Reference)
	{
		CIm2Mat::SetSignal(bind(&CThReference::OnImCache, this, placeholders::_1), bind(&CThReference::IsIdle, this));

		CImPadding::SetThParam(&m_cImCache, bind(&CThReference::OnPositioning, this));
	}
	return CThForeground::Reg();
}

void CThReference::Unreg()
{
	CThForeground::Unreg();

	Clear();
}

void CThReference::SetReferenceParam(vector<array<wstring, 2>>& vFile, int nFrameInLine, array<int, 5> vKernel)
{
	m_vKernel = vKernel; 

	vector<vector<array<string, 2>>> vCache;

	int idx[2] = { 0, (floor((double)vFile.size() / nFrameInLine)  - 1 ) * nFrameInLine };

	for (auto& i : idx)
	{
		vCache.push_back({ { Wstring2String(vFile[i][0]), Wstring2String(vFile[i][1]) } });
	}
	SetFiles(vCache);
}

bool CThReference::IsIdle()
{
	int nSize = 0; static int g_nTh = 10;
	{
		CSRLock lock(&m_lLock);

		nSize = m_vImCache.size();
	}
	return nSize < g_nTh;
}

void CThReference::OnImCache(lpIm2Mat& lpCache)
{
	Push(lpCache);

	m_cImCache.signal();
}

void CThReference::Push(lpIm2Mat& lpCache)
{
	CSWLock lock(&m_lLock);

	m_vImCache.push_back(lpCache);
}

bool CThReference::Pop(lpIm2Mat& lpCache)
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

void CThReference::Clear()
{
	CSWLock lock(&m_lLock);

	m_vImCache.clear();

	m_vReference.clear();
}

void CThReference::Ptrs2Ptr(vector<tuple<Rect, ImPtr::CImPtr>>& vReference, Rect& rtRef, ImPtr::CImPtr& cImPtr)
{
	ASSERT(vReference.size() == 2);

	rtRef = get<0>(vReference[0]) | get<0>(vReference[1]);

	cImPtr = ImPtr::CImPtr(new Mat(rtRef.height, rtRef.width, CV_8UC1, Scalar(0xff)));

	for (auto& iRef : vReference)
	{
		auto& iPos = get<0>(iRef); auto& iIm = get<1>(iRef);

		Rect rt(iPos.x - rtRef.x, iPos.y - rtRef.y, iPos.width, iPos.height);

		auto iPtr = (*cImPtr.Get())(rt);

		addWeighted(*iIm.Get(), 0.3, iPtr, 0.7, 0.0, iPtr);
	}
	rtRef.height = max(get<0>(vReference[0]).height, get<0>(vReference[1]).height);
}

void CThReference::OnPositioning()
{
	lpIm2Mat lpCache = nullptr;

	if (!Pop(lpCache)) { m_cImCache.wait(); return; }

	ASSERT(lpCache->vIm2Mat.size() == 1);

	auto& lpImage = lpCache->vIm2Mat[0];

	Rect rtReference; ImPtr::CImPtr cImPtr(new Mat());

	QEncodePos(m_vKernel, *lpImage, *cImPtr.Get(), rtReference);

	m_vReference.push_back({ rtReference, cImPtr });

	if (!!lpCache->nRemaining) { delete lpCache; return; }

	Rect rtRef;

	ImPtr::CImPtr cImReg;

	Ptrs2Ptr(m_vReference, rtRef, cImReg);

	OnReference(rtRef, cImReg);

	delete lpCache;
}
