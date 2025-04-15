#include "pch.h"
#include "StMatchEx.h"
#include "PQMsg.h"
#include "MatrixOverlap.h"

IMPLEMENT_DYNAMIC(CStMatchEx, CStMatch)

BEGIN_MESSAGE_MAP(CStMatchEx, CStMatch)

END_MESSAGE_MAP()

CStMatchEx::CStMatchEx()
	:m_bTermination(false)
{
	
}

CStMatchEx::~CStMatchEx()
{
	CloseHandle((HANDLE)_beginthreadex(NULL, 0, TermiThread, (void*)this, 0, NULL));

	m_cTermination.wait();
}

void CStMatchEx::InitCanvas(CWnd* lpWnd, CRect rtCanvas)
{
	CStMatch::InitCanvas(lpWnd, rtCanvas);

	CloseHandle((HANDLE)_beginthreadex(NULL, 0, MatchThread, (void *)this, 0, NULL));
}

unsigned int __stdcall CStMatchEx::MatchThread(void* lpParam)
{
	CStMatchEx * lpThis = (CStMatchEx*)lpParam;

	lpThis->OnMathcing();

	lpThis->m_cTermination.signal();

	return 0;
}

void CStMatchEx::OnMathcing()
{
	double dSame = 0.0;

	int ox = 0, oy = 0, ox2 = 0, oy2 = 0; 
	
	cv::Mat* lpMoldTemp = nullptr, *lpMoldView = nullptr, *lpTempTemp = nullptr, *lpTempView = nullptr;

	while (!IsTermination())
	{
		if (!PopFront(ox, oy, ox2, oy2, lpMoldTemp, lpMoldView, lpTempTemp, lpTempView))
		{
			if (!!lpMoldView && !!lpTempView && !lpMoldView->empty() && !lpTempView->empty())
			{
				cv::Mat cMold; cv::Mat cTemp;

				cvtColor(*lpMoldView, cMold, cv::COLOR_BGRA2GRAY); // bgra -> gray

				cvtColor(*lpTempView, cTemp, cv::COLOR_BGRA2GRAY);

				//MatrixOverlap::SetTar(cMold, 5000);

				//MatrixOverlap::Overlap(cTemp, ptTmp, dSame);
			}
			OnSyncTask(ox, oy, ox2, oy2, lpMoldTemp, lpMoldView, lpTempTemp, lpTempView, dSame);

			continue;
		}
		m_cMatch.wait();
	}
}

void CStMatchEx::OnSyncTask(
	int& ox, int& oy, int& ox2, int& oy2,
	cv::Mat* lpMoldTemp, cv::Mat* lpMoldView, 
	cv::Mat* lpTempTemp, cv::Mat* lpTempView, double dSame)
{
	OnSyncMatched(ox, oy, ox2, oy2, lpMoldTemp, lpMoldView, lpTempTemp, lpTempView, dSame);
}

void CStMatchEx::OnTask(
	int ox, int oy, int ox2, int oy2,
	cv::Mat* lpMoldTemp, cv::Mat* lpMoldView, 
	cv::Mat* lpTempTemp, cv::Mat* lpTempView)
{
	PushBack(ox, oy, ox2, oy2, lpMoldTemp, lpMoldView, lpTempTemp, lpTempView);

	m_cMatch.signal();
}

void CStMatchEx::PushBack(
	int ox, int oy, int ox2, int oy2,
	cv::Mat* lpMoldTemp, cv::Mat* lpMoldView, 
	cv::Mat* lpTempTemp, cv::Mat* lpTempView)
{
	CSWLock lock(&m_lLock);

	m_vTask.push_back(std::make_tuple(ox, oy, ox2, oy2, lpMoldTemp, lpMoldView, lpTempTemp, lpTempView));
}

bool CStMatchEx::PopFront(
	int& ox, int& oy, int& ox2, int& oy2,
	cv::Mat*& lpMoldTemp, cv::Mat*& lpMoldView, 
	cv::Mat*& lpTempTemp, cv::Mat*& lpTempView)
{
	bool bEmpty = true;
	{
		CSWLock lock(&m_lLock);

		if (!(bEmpty = m_vTask.empty()))
		{
			auto iFront = m_vTask.begin();

			ox = std::get<0>(*iFront);

			oy = std::get<1>(*iFront);

			ox2 = std::get<2>(*iFront);

			oy2 = std::get<3>(*iFront);

			lpMoldTemp = std::get<4>(*iFront);

			lpMoldView = std::get<5>(*iFront);

			lpTempTemp = std::get<6>(*iFront);

			lpTempView = std::get<7>(*iFront);

			iFront = m_vTask.erase(iFront);
		}
	}
	return bEmpty;
}

unsigned int __stdcall CStMatchEx::TermiThread(void* lpParam)
{
	CStMatchEx* lpThis = (CStMatchEx*)lpParam;
	
	lpThis->OnTermination();

	lpThis->m_cMatch.signal();

	return 0;
}

void CStMatchEx::OnTermination()
{
	CSWLock lock(&m_lLock);

	m_bTermination = true;
}

bool CStMatchEx::IsTermination()
{
	bool bTermination = false;
	{
		CSRLock lock(&m_lLock);

		bTermination = m_bTermination;
	}
	return bTermination;
}
