#include "pch.h"
#include "ThHomography.h"
#include "Transform.h"
#include "Helper.h"

CThHomography::CThHomography():
	m_cM(3, 3, CV_64FC1)
{

}

CThHomography::~CThHomography()
{

}

bool CThHomography::Reg()
{
	if (m_uWorkType == eWT_Homography)
	{
		CIm2Mat::SetSignal(bind(&CThHomography::OnImCache, this, placeholders::_1), bind(&CThHomography::IsIdle, this));

		CImPadding::SetThParam(&m_cImCache, bind(&CThHomography::OnImHomography, this));
	}
	return CThPerspective::Reg();
}

void CThHomography::Unreg()
{
	CThPerspective::Unreg();

	Clear();
}

void CThHomography::SetHomographyParam(
	vector<array<wstring, 2>>& vFile,
	int nFrameInLine,
	int nPassInStride,
	array<int, 4> vFill,
	array<float, 2> vWH2,
	array<double, 9> M,
	string strDir)
{
	m_strOutputDir = strDir;

	m_vFill = vFill; m_vWH2 = vWH2;

	memcpy(m_cM.data, M.data(), M.size() * sizeof(double));

	vector<vector<array<string, 2>>> vCache;

	for (int i = 0; i < vFile.size(); i += nPassInStride * nFrameInLine)
	{
		for (int j = 0; j < nFrameInLine; ++j)
		{
			int idx = i + j;

			vCache.push_back({ { Wstring2String(vFile[idx][0]), Wstring2String(vFile[idx][1]) } });
		}
	}
	SetFiles(vCache);
}

bool CThHomography::IsIdle()
{
	int nSize = 0; static int g_nTh = 10;
	{
		CSRLock lock(&m_lLock);

		nSize = m_vImCache.size();
	}
	return nSize < g_nTh;
}

void CThHomography::OnImCache(lpIm2Mat& lpCache)
{
	Push(lpCache);

	m_cImCache.signal();
}

void CThHomography::Push(lpIm2Mat& lpCache)
{
	CSWLock lock(&m_lLock);

	m_vImCache.push_back(lpCache);
}

bool CThHomography::Pop(lpIm2Mat& lpCache)
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

void CThHomography::Clear()
{
	CSWLock lock(&m_lLock);

	m_vImCache.clear();
}

void CThHomography::OnImHomography()
{
	lpIm2Mat lpCache = nullptr;

	if (!Pop(lpCache)) { m_cImCache.wait(); return; }

	ASSERT(lpCache->vIm2Mat.size() == 1);

	auto& iImMat = *lpCache->vIm2Mat[0];

	copyMakeBorder(iImMat, iImMat, m_vFill[1], m_vFill[3], m_vFill[0], m_vFill[2], BORDER_CONSTANT, Scalar(0xff));

	warpPerspective(iImMat, iImMat, m_cM, Size2f(m_vWH2[0], m_vWH2[1]), 1, BORDER_CONSTANT, Scalar(0xff));

	ImPtr::CImPtr cImPtr(new Mat(iImMat.rows, iImMat.cols, CV_8UC1));

	memcpy(cImPtr.Get()->data, iImMat.data, (uint64)iImMat.rows * iImMat.cols * iImMat.channels());

	cout << lpCache->nRemaining << ": " << lpCache->strFirst.c_str() << " | " << iImMat.cols << " * " << iImMat.rows << endl;

	!!lpCache->nRemaining ? OnTranslating(cImPtr) : OnHomography({ iImMat.cols, iImMat.rows }, cImPtr);

	string strFile = m_strOutputDir + "\\" + lpCache->strFirst;

	imwrite(strFile, iImMat);

	delete lpCache;
}
