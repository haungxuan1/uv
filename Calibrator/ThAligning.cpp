#include "pch.h"
#include "ThAligning.h"
#include "Transform.h"
#include "Helper.h"

CThAligning::CThAligning():
	m_nPassInStride(0)
{

}

CThAligning::~CThAligning()
{

}

bool CThAligning::Reg()
{
	if (m_uWorkType == eWT_Aligning)
	{
		CIm2Mat::SetSignal(bind(&CThAligning::OnImCache, this, placeholders::_1), bind(&CThAligning::IsIdle, this));

		CImPadding::SetThParam(&m_cImCache, bind(&CThAligning::OnImAligning, this));
	}
	return CThReference::Reg();
}

void CThAligning::Unreg()
{
	CThReference::Unreg();

	Clear();
}

void CThAligning::SetAligningParam(
	vector<array<wstring, 2>>& vFile, 
	array<int, 8> vThParam, // 帧/行 | y轴步进/帧 | 参照物.y |  参照物.w | 参照物.h | 帧.w | 帧.h
	array<int, 6> vKernel,  // 前景阈值 | 参照物.外接圆直径 | y轴步进/帧 | x&y轴浮动范围 | 腐蚀核 | 膨胀核
	string srtAligned)
{
	m_srtAligned = srtAligned;

	m_vKernel = vKernel; m_vThParam = vThParam;

	memcpy(m_vThParam.data(), vThParam.data(), sizeof(vThParam));

	m_nPassInStride = (m_vThParam[ePT_h] - 2*m_vThParam[ePT_sh]) / m_vThParam[ePT_Stride]; // npass / stride

	m_nPassInStride = 10;

	m_vKernel[2] *= m_nPassInStride;

	vector<vector<array<string, 2>>> vCache;

	for (int i = 0; i < vFile.size(); i += m_nPassInStride * m_vThParam[ePT_FrameInLine])
	{
		vCache.push_back({ { Wstring2String(vFile[i][0]), Wstring2String(vFile[i][1]) } });
	}
	SetFiles(vCache);
}

bool CThAligning::IsIdle()
{
	int nSize = 0; static int g_nTh = 10;
	{
		CSRLock lock(&m_lLock);

		nSize = m_vImCache.size();
	}
	return nSize < g_nTh;
}

void CThAligning::OnImCache(lpIm2Mat& lpCache)
{
	Push(lpCache);

	m_cImCache.signal();
}

void CThAligning::Push(lpIm2Mat& lpCache)
{
	CSWLock lock(&m_lLock);

	m_vImCache.push_back(lpCache);
}

bool CThAligning::Pop(lpIm2Mat& lpCache)
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

void CThAligning::Clear()
{
	CSWLock lock(&m_lLock);

	m_vImCache.clear();

	m_cImSign = Mat();

	m_vTL.clear();
}

bool CThAligning::InitAlignmentParam(Mat& cCol)
{
	if (!m_cImSign.empty()) return false;

	m_cImSign = cCol(Rect(m_vThParam[ePT_x], 0, m_vThParam[ePT_sw], cCol.rows)).clone();

	m_vTL.push_back({ 0, 0 });

	return true;
}

void CThAligning::InstallAlignedParam(string& srtAligned, vector<array<int, 2>>& vTL)
{
	Mat cParam(1, vTL.size() * sizeof(int) * 2, CV_8UC1);

	memcpy(cParam.data, vTL.data(), cParam.cols);

	imwrite(srtAligned.c_str(), cParam);
}

void CThAligning::OnImAligning()
{
	lpIm2Mat lpCache = nullptr;

	if (!Pop(lpCache)) { m_cImCache.wait(); return; }
	
	ASSERT(lpCache->vIm2Mat.size() == 1);

	auto& strImage = lpCache->strFirst;

	auto& lpImage = lpCache->vIm2Mat[0];

	array<int, 2> vTL{ 0, 0 }; // 上负 | 左负 

	ImPtr::CImPtr cImPtr;

	if (!InitAlignmentParam(*lpImage)) // 参照物区域初始化
	{
		// 拼合位置查找
		Mat cImSign = (*lpImage)(Rect(m_vThParam[ePT_x], 0, m_vThParam[ePT_sw], lpImage->rows)).clone();

		Mat cBin1, cBin2, cImAlign1, cImAlign2, cImUnite1, cImUnite2;

		std::array<cv::Rect, 2> rtImUnite; std::array<int, 2> vAlign;

		double dScore = 0.0, dTime = 0.0;

		cuAlign(m_cImSign, cImSign, cBin1, cBin2, cImAlign1, cImAlign2, cImUnite1, cImUnite2, rtImUnite, vAlign, dScore, dTime, m_vKernel, vTL);

		cout << strImage.c_str() << ": " << dec <<
			rtImUnite[0] << ", " << rtImUnite[1] << " | " <<
			vAlign[0] << ", " << vAlign[1] << " | " <<
			dScore << " | " << dTime << endl;

		cout << "TL: " << dec<< vTL[0] << ", " << vTL[1] << endl;

		cImPtr = ImPtr::CImPtr(new Mat(cImUnite1.rows, cImUnite1.cols * 2, CV_8UC1));

		cImUnite1.copyTo((*cImPtr.Get())(Rect(0, 0, cImUnite1.cols, cImUnite1.rows)));

		cImUnite2.copyTo((*cImPtr.Get())(Rect(cImUnite1.cols, 0, cImUnite1.cols, cImUnite1.rows)));

		m_vTL.push_back(vTL);

		m_cImSign = cImSign;

		if(!!!lpCache->nRemaining) InstallAlignedParam(m_srtAligned, m_vTL);

		!!lpCache->nRemaining ? OnAligning(lpCache->nRemaining, dScore, cImPtr) : OnAlign(lpCache->nRemaining, dScore, cImPtr, m_nPassInStride);
	}
	delete lpCache;
}
