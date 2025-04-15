#include "pch.h"
#include "ThMerging.h"
#include "Transform.h"
#include "Helper.h"

CThMerging::CThMerging():
	m_nPassInStride(0), m_nOx(0)
{

}

CThMerging::~CThMerging()
{

}

bool CThMerging::Reg()
{
	if (m_uWorkType == eWT_Merging)
	{
		m_ptTL.x = 0, m_ptTL.y = 0;

		CIm2Mat::SetSignal(bind(&CThMerging::OnImCache, this, placeholders::_1), bind(&CThMerging::IsIdle, this));

		CImPadding::SetThParam(&m_cImCache, bind(&CThMerging::OnImMerging, this));
	}
	return CThAligning::Reg();
}

void CThMerging::Unreg()
{
	CThAligning::Unreg();

	Clear();
}

void CThMerging::OnInstallAlignedParam(int nImWidth, int nImHeight, int nFrameInLine, string& strAligned, vector<array<int, 2>>& vImAligned)
{
	Mat cParam = imread(strAligned.c_str(), 0);

	vImAligned.resize(cParam.cols / (sizeof(int) * 2));

	memcpy(vImAligned.data(), cParam.data, cParam.cols); // 获取到对齐参数

	CRect rtImage(0, 0, nImWidth * nFrameInLine, nImHeight);

	array<int, 2> vTL{ 0,0 };

	for (auto& iAlig : vImAligned)
	{
		vTL[0] += iAlig[0], vTL[1] += iAlig[1];
		rtImage.MoveToXY(vTL[0], vTL[1]);
		m_nOx = max((long)m_nOx, -min(0l, rtImage.left));
	}
	
	m_lpImMerging = new Mat(rtImage.bottom, rtImage.Width() + abs(rtImage.left) + 1000, CV_8UC1, Scalar(0xff));
}

void CThMerging::SetMergingParam(
	vector<array<wstring, 2>>& vFile, 
	array<int, 8> vThParam, // 帧/行 | y轴步进/帧 | 参照物.y |  参照物.w | 参照物.h | 帧.w | 帧.h
	int nPassInStride, // 由图像对齐计算
	string strAligned, 
	string strMerged,
	string strROIMerged)
{
	m_strMerged = strMerged; m_strROIMerged = strROIMerged; m_vThParam = vThParam; m_nPassInStride = nPassInStride;

	OnInstallAlignedParam(m_vThParam[ePT_w], m_vThParam[ePT_h], m_vThParam[ePT_FrameInLine], strAligned, m_vImAligned);

	vector<vector<array<string, 2>>> vCache;

	for (int i = 0; i < vFile.size(); i += m_nPassInStride * m_vThParam[ePT_FrameInLine])
	{
		vector<array<string, 2>> vLine;

		for (int j = 0; j < m_vThParam[ePT_FrameInLine]; ++j)
		{
			int idx = i + j;

			vLine.push_back({ Wstring2String(vFile[idx][0]), Wstring2String(vFile[idx][1]) });
		}
		
		vCache.push_back(vLine);
	}
	SetFiles(vCache);
}

bool CThMerging::IsIdle()
{
	int nSize = 0; static int g_nTh = 1;
	{
		CSRLock lock(&m_lLock);

		nSize = m_vImCache.size();
	}
	return nSize < g_nTh;
}

void CThMerging::OnImCache(lpIm2Mat& lpCache)
{
	Push(lpCache);

	m_cImCache.signal();
}

void CThMerging::Push(lpIm2Mat& lpCache)
{
	CSWLock lock(&m_lLock);

	m_vImCache.push_back(lpCache);
}

bool CThMerging::Pop(lpIm2Mat& lpCache)
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

void CThMerging::Clear()
{
	CSWLock lock(&m_lLock);

	m_vImCache.clear();

	m_vImAligned.clear();

	!!m_lpImMerging ? [this]() { delete m_lpImMerging; m_lpImMerging = nullptr; }() : []() {}();
}

void CThMerging::Pop(array<int, 2>& vTL)
{
	ASSERT(!m_vImAligned.empty());

	auto iFront = m_vImAligned.begin();

	vTL = (*iFront);

	iFront = m_vImAligned.erase(iFront);
}

void CThMerging::Merging(int nImWidth, int nImHeight, array<int, 2> vTL, vector<Mat*>& vLine, Mat*& lpMerging)
{
	Rect rtIm(0, 0, nImWidth, nImHeight); 
	
	rtIm.x += vTL[0], rtIm.y += vTL[1];

	for (int i = 0; i < vLine.size(); ++i)
	{
		auto& iFrame = *vLine[i];

		Rect rt(rtIm.x + i * nImWidth, rtIm.y, rtIm.width, rtIm.height);

		iFrame.copyTo((*lpMerging)(rt));
	}
}

void CThMerging::OnImMerging()
{
	lpIm2Mat lpCache = nullptr;

	if (!Pop(lpCache)) { m_cImCache.wait(); return; }
	
	array<int, 2> vTL; Pop(vTL); m_ptTL.x += vTL[0]; m_ptTL.y += vTL[1];// 上负 | 左负 

	cout << lpCache->strFirst.c_str() << ": " << vTL[0] << ", " << vTL[1] << endl;

	Merging(m_vThParam[ePT_w], m_vThParam[ePT_h], { m_ptTL.x + m_nOx, m_ptTL.y }, lpCache->vIm2Mat, m_lpImMerging);

	ImPtr::CImPtr cImPtr = ImPtr::CImPtr(new Mat());

	resize(*m_lpImMerging, *cImPtr.Get(), Size(m_lpImMerging->cols * 0.1, m_lpImMerging->rows * 0.1));

	Rect rtROI(m_vThParam[ePT_x] + m_vThParam[ePT_sw] * 2, 0, m_lpImMerging->cols - m_vThParam[ePT_x] - m_vThParam[ePT_sw] * 2, m_lpImMerging->rows);

	!!lpCache->nRemaining ? OnMerging(lpCache->nRemaining, cImPtr) : OnMerge(lpCache->nRemaining, cImPtr);

	!!lpCache->nRemaining ? [](){}() :
		[](Mat* lpMer, Rect rtROI, string strMerged, string strROIMerged) 
	{
		imwrite(strMerged.c_str(), *lpMer);

		Mat cImage = imread(strMerged.c_str(), 0);

		imwrite(strROIMerged.c_str(), cImage(rtROI));
	}(m_lpImMerging, rtROI, m_strMerged, m_strROIMerged);
	
	delete lpCache;
}
