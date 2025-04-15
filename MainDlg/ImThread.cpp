#include "pch.h"
#include "ImThread.h"
#include <string>

static int g_nIndex[2] = { 17, 419 };

static std::string g_strSrcDir = "F:\\im2.21\\1.0\\camera\\";

CImThread::CImThread()
{
	for (int i = g_nIndex[0]; i <= g_nIndex[1]; ++i) m_vImIndex.push_back(i);
}

CImThread::~CImThread()
{

}

void CImThread::Run()
{
	CloseHandle((HANDLE)_beginthreadex(NULL, 0, OnScheduling, (void*)this, 0, NULL));

	for (int i = 0; i < 2; ++i)
	{
		CloseHandle((HANDLE)_beginthreadex(NULL, 0, OnReadImage, (void*)this, 0, NULL));
	}
}

unsigned int __stdcall CImThread::OnScheduling(void* lpUesrParam)
{
	//CImThread* lpThis = (CImThread*)lpUesrParam;

	//int nIndex = g_nIndex[0]; cv::Mat* lpImMatrix = 0;

	//while (1)
	//{
	//	if (lpThis->GetImMatrix(nIndex, lpImMatrix))
	//	{
	//		PQPrinter().ImCapture(std::make_tuple(nIndex, nIndex, lpImMatrix));

	//		++nIndex; if (g_nIndex[1] < nIndex) break;

	//		continue;
	//	}
	//	lpThis->m_cScheduling.wait();
	//}
	//_cwprintf(L"%s: end\r\n", __FUNCTIONW__);

	return 0;
}

unsigned int __stdcall CImThread::OnReadImage(void* lpUesrParam)
{
	int nIndex = 0;

	CImThread* lpThis = (CImThread*)lpUesrParam;

	std::string strDir = g_strSrcDir;

	while (lpThis->GetImIndex(nIndex))
	{
		std::string strFile = strDir + std::to_string(nIndex) + ".png";

		cv::Mat* lpImage = new cv::Mat();

		*lpImage = cv::imread(strFile.c_str(), cv::ImreadModes::IMREAD_GRAYSCALE);

		lpThis->SetImMatrix(nIndex, lpImage);

//		Sleep(1000);
	}
	_cwprintf(L"%s: end\r\n", __FUNCTIONW__);

	return 0;
}

bool CImThread::GetImIndex(int& nIndex)
{
	bool bEmpty = true;
	{
		CSWLock lock(&m_cLock);

		if (!(bEmpty = m_vImIndex.empty()))
		{
			auto iPop = m_vImIndex.begin();

			nIndex = (*iPop);

			iPop = m_vImIndex.erase(iPop);
		}
	}
	return !bEmpty;
}

void CImThread::SetImMatrix(int& nIndex, cv::Mat*& lpImMatrix)
{
	{
		CSWLock lock(&m_cLock);

		m_vImMatrixs.push_back(std::make_tuple(nIndex, lpImMatrix));
	}
	m_cScheduling.signal();
}

bool CImThread::GetImMatrix(int& nIndex, cv::Mat*& lpImMatrix)
{
	CSWLock lock(&m_cLock);

	if (m_vImMatrixs.empty()) return false;

	auto iCur = _Find_pr(m_vImMatrixs.begin(), m_vImMatrixs.end(), nIndex,
		[](std::tuple<int, cv::Mat*>& iIm, const int& nIndex) { return (nIndex == std::get<0>(iIm)); });

	if (iCur == m_vImMatrixs.end()) return false;

	lpImMatrix = std::get<1>(*iCur);

	iCur = m_vImMatrixs.erase(iCur);

	return true;
}