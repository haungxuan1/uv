#include "pch.h"
#include "FeatureTh.h"
#include "PQMsg.h"

CFeatureTh::CFeatureTh(CWnd* lpInitiator):
	m_lpInitiator(lpInitiator),
	m_bTermination(false),
	m_cHomography(3, 3, CV_64FC1)
{

}

CFeatureTh::~CFeatureTh()
{

}

void CFeatureTh::InitParam(array<int, 4> vFill, array<double, 9> M, array<float, 2> vWH2, array<double, 2> vDPI2)
{
	m_vFill = vFill; m_vWH2 = vWH2; m_vDPI2 = vDPI2;

	memcpy(m_cHomography.data, M.data(), sizeof(double) * M.size());
}

void CFeatureTh::Reg()
{
	SetTermination(false);

	CloseHandle((HANDLE)_beginthreadex(NULL, 0, OnFeatureThread, (void*)this, 0, NULL));
}

void CFeatureTh::Unreg()
{
	CloseHandle((HANDLE)_beginthreadex(NULL, 0, OnFeaExitThread, (void*)this, 0, NULL));

	m_cTermination.wait();
}

void CFeatureTh::SetTermination(bool bTermination)
{
	CSWLock lock(&m_lLock);

	m_bTermination = bTermination;
}

bool CFeatureTh::IsTermination()
{
	CSRLock lock(&m_lLock);

	return m_bTermination;
}

unsigned int __stdcall CFeatureTh::OnFeaExitThread(void* lpUesrParam)
{
	CFeatureTh* lpThis = (CFeatureTh*)lpUesrParam;

	lpThis->SetTermination(true);

	lpThis->m_cTermination.wait();

	return 0;
}

unsigned int __stdcall CFeatureTh::OnFeatureThread(void* lpUesrParam)
{
	CFeatureTh* lpThis = (CFeatureTh*)lpUesrParam;

	while (!lpThis->IsTermination())
	{
		ImPtr::CImPtr* lpFrame = nullptr;

		if (!lpThis->Pop(lpFrame)) { lpThis->m_cFeature.wait(); continue; }

		lpThis->OnFeature(lpFrame);

		::PostMessage(lpThis->m_lpInitiator->GetSafeHwnd(), PQM_FEATURE_CONVERT, 0, (LPARAM)lpFrame);
	}

	lpThis->m_cTermination.signal();

	return 0;
}

void CFeatureTh::OnCameraFrame(ImPtr::CImPtr*& lpFrame)
{
	Push(lpFrame);

	m_cFeature.signal();
}

void CFeatureTh::Push(ImPtr::CImPtr*& lpFrame)
{
	CSWLock lock(&m_lLock);

	m_vFrame.push_back(lpFrame);
}

bool CFeatureTh::Pop(ImPtr::CImPtr*& lpFrame)
{
	bool bEmpty = false;
	{
		CSWLock lock(&m_lLock);

		if (!(bEmpty = m_vFrame.empty()))
		{
			auto iFront = m_vFrame.begin();

			lpFrame = *iFront;

			iFront = m_vFrame.erase(iFront);
		}
	}
	return !bEmpty;
}

void CFeatureTh::OnFeature(ImPtr::CImPtr*& lpFrame)
{
	rotate(*lpFrame->Get(), *lpFrame->Get(), ROTATE_90_COUNTERCLOCKWISE); // 13056 * 30000 -> 30000 * 13056

	copyMakeBorder(*lpFrame->Get(), *lpFrame->Get(),
		m_vFill[1], m_vFill[3], m_vFill[0], m_vFill[2], BORDER_CONSTANT, Scalar(0xff)); // 变换前边界填充

	warpPerspective(*lpFrame->Get(), *lpFrame->Get(),
		m_cHomography, Size2f(m_vWH2[0], m_vWH2[1]), 1, 0, Scalar(0xff));

	double dX2Y = m_vDPI2[1] / m_vDPI2[0];

	resize(*lpFrame->Get(), *lpFrame->Get(), Size2d(dX2Y * lpFrame->Get()->cols, lpFrame->Get()->rows)); // 相机xy dpi对齐

	Rect rtSub(0, 0, 30000, lpFrame->Get()->rows); // warpAffine 源码bug // 暂时仍然以warpAffine进行旋转、缩放操作 // 这里增加个补丁

	Mat cTmp(rtSub.height, rtSub.width, lpFrame->Get()->type());

	(*lpFrame->Get())(rtSub).copyTo(cTmp);

	*lpFrame->Get() = cTmp.clone();
}