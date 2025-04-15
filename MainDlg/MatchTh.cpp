#include "pch.h"
#include "MatchTh.h"
#include "PQMsg.h"

CMatchTh::CMatchTh(double dScale, CWnd* lpInitiator):
	m_dScale(dScale), m_lpInitiator(lpInitiator), m_bTermination(false)
{

}

CMatchTh::~CMatchTh()
{

}

void CMatchTh::Reg()
{
	SetTermination(false);

	CloseHandle((HANDLE)_beginthreadex(NULL, 0, OnConvertThread, (void*)this, 0, NULL));
}

void CMatchTh::Unreg()
{
	CloseHandle((HANDLE)_beginthreadex(NULL, 0, OnCovExitThread, (void*)this, 0, NULL));

	m_cTermination.wait();
}

void CMatchTh::OnConvert(Res& vMatch)
{
	Push(vMatch);

	m_cConvert.signal();
}

void CMatchTh::SetTermination(bool bTermination)
{
	CSWLock lock(&m_lLock);

	m_bTermination = bTermination;
}

bool CMatchTh::IsTermination()
{
	CSRLock lock(&m_lLock);

	return m_bTermination;
}

unsigned int __stdcall CMatchTh::OnCovExitThread(void* lpUesrParam)
{
	CMatchTh* lpThis = (CMatchTh*)lpUesrParam;

	lpThis->SetTermination(true);

	lpThis->m_cTermination.wait();

	return 0;
}

void CMatchTh::Push(Res& vMatch)
{
	CSWLock lock(&m_lLock);

	m_vIn.push_back(vMatch);
}

bool CMatchTh::Pop(Res& vMatch)
{
	bool bEmpty = false;
	{
		CSWLock lock(&m_lLock);

		if (!(bEmpty = m_vIn.empty()))
		{
			auto iFront = m_vIn.begin();

			vMatch = *iFront;

			iFront = m_vIn.erase(iFront);
		}
	}
	return !bEmpty;
}

void CMatchTh::Push(tuple<UserParam, vector<SingleTarget>>& vMatch)
{
	CSWLock lock(&m_lLock);

	m_vOut.push_back(vMatch);
}

bool CMatchTh::Pop(tuple<UserParam, vector<SingleTarget>>& vMatch)
{
	bool bEmpty = false;
	{
		CSWLock lock(&m_lLock);

		if (!(bEmpty = m_vOut.empty()))
		{
			auto iFront = m_vOut.begin();

			vMatch = *iFront;

			iFront = m_vOut.erase(iFront);
		}
	}
	return !bEmpty;
}

void CMatchTh::OnConvert(Res& vMatch, tuple<UserParam, vector<SingleTarget>>& vOut)
{
	auto& iImTarget = get<eMI_ImPtr>(vMatch);

	auto& iMatch = get<eMI_Match>(vMatch);

	auto& iUserParam = get<0>(vOut);

	auto& iTarget = get<1>(vOut);

	iUserParam = { get<eMI_FrameId>(vMatch), get<eMI_Fleece>(vMatch), get<eMI_Timer>(vMatch), get<eMI_Param>(vMatch) };

	ImPtr::CImPtr cRender(new Mat);

	resize(*iImTarget.Get(), *cRender.Get(), Size(m_dScale * iImTarget.Get()->cols, m_dScale * iImTarget.Get()->rows));

	iTarget.push_back({ 0.0, Rect(0, 0, iImTarget.Get()->cols, iImTarget.Get()->rows), iImTarget, 
		Rect(0,0,cRender.Get()->cols, cRender.Get()->rows), cRender }); // 目标主体

	for (auto& iTarg : iMatch)
	{
		auto& iPrPtr = get<CPosition::eMI_PrPtr>(iTarg);

		auto& iPrPos = get<CPosition::eMI_PrPos>(iTarg);

		auto& iPrScore = get<CPosition::eMI_PrScore>(iTarg);

		ImPtr::CImPtr iRender(new Mat); Rect rtRender;

		resize(iPrPtr, *iRender.Get(), Size(m_dScale * iPrPtr.cols, m_dScale * iPrPtr.rows));

		rtRender.x = m_dScale * iPrPos.x; rtRender.y = m_dScale * iPrPos.y;

		rtRender.width = iRender.Get()->cols; rtRender.height = iRender.Get()->rows;

		iTarget.push_back(make_tuple(iPrScore, iPrPos, ImPtr::CImPtr(new Mat(iPrPtr)), rtRender, iRender));
	}

	get<eMI_Timer>(iUserParam).push_back({ __FUNCTION__, clock() });
}

unsigned int __stdcall CMatchTh::OnConvertThread(void* lpUesrParam)
{
	CMatchTh* lpThis = (CMatchTh*)lpUesrParam;

	while (!lpThis->IsTermination())
	{
		Res vMatch;

		if (!lpThis->Pop(vMatch)) { lpThis->m_cConvert.wait();continue; }
	
		auto& iMatch = get<eMI_Match>(vMatch);

		// 按匹配得分排序
		sort(iMatch.begin(), iMatch.end(), 
			[](const tuple<Mat, Rect, double, double>& i1, const tuple<Mat, Rect, double, double>& i2) { return get<2>(i1) < get<2>(i2); });

		tuple<UserParam, vector<SingleTarget>> vOut;

		lpThis->OnConvert(vMatch, vOut);

		lpThis->Push(vOut);

		::PostMessage(lpThis->m_lpInitiator->GetSafeHwnd(), PQM_MATCH_CONVERT, 0, 0);
	}
	lpThis->m_cTermination.signal();

	return 0;
}
