#include "pch.h"
#include "ThMergingEx.h"
#include "Transform.h"
#include "Helper.h"
#include <numeric>

CThMergingEx::CThMergingEx():
	m_nOx(0), m_cM(3, 3, CV_64FC1)
{

}

CThMergingEx::~CThMergingEx()
{

}

bool CThMergingEx::Reg()
{
	if (m_uWorkType == eWT_MergingEx)
	{
		m_ptTL.x = 0, m_ptTL.y = 0;

		CIm2Mat::SetSignal(bind(&CThMergingEx::OnImCache, this, placeholders::_1), bind(&CThMergingEx::IsIdle, this));

		CImPadding::SetThParam(&m_cImCache, bind(&CThMergingEx::OnImMerging, this));
	}
	return CThHomography::Reg();
}

void CThMergingEx::Unreg()
{
	CThHomography::Unreg();

	Clear();
}

void CThMergingEx::OnInstallAlignedParam(int nImWidth, int nImHeight, int nFrameInLine, string& strAligned, int& nPassInStride, vector<array<int, 2>>& vImAligned)
{
	Mat cParam = imread(strAligned.c_str(), 0);

	vImAligned.resize(cParam.cols / (sizeof(int) * 2));

	memcpy(vImAligned.data(), cParam.data, cParam.cols); // 获取到对齐参数

	CRect rtImage(0, 0, nImWidth * nFrameInLine, nImHeight);

	array<int, 2> vTL{ 0,0 };

	for (auto& iAlig : vImAligned) 
	{
		vTL[0] += iAlig[0], 
			vTL[1] += iAlig[1];
		rtImage.MoveToXY(vTL[0], vTL[1]); 
		m_nOx = max((long)m_nOx, -min(0l, rtImage.left));
	}
	m_nOy = 500; m_nOx += 500;

	m_lpImMerging = new Mat(rtImage.bottom + 1000, (rtImage.Width() + abs(rtImage.left)) + 1000, CV_8UC1, Scalar(0xff));
}

void CThMergingEx::SetMergingExParam(vector<array<wstring, 2>>& vFile,
	int nFrameInLine,
	int nPassInStride,
	array<int, 4> vReference,
	array<float, 2> vImWH1,
	array<float, 2> vImWH2,
	array<int, 2> vImWH2Ex,
	array<Point2f, 4> vVertex1,
	array<Point2f, 4> vVertex2,
	array<int, 4> vFill,
	array<double, 9> vM,
	string strAligned,
	string strMerged,
	string strROIMerged)
{
	m_strMerged = strMerged;

	m_strROIMerged = strROIMerged;

	m_vReference = vReference;

	m_vWH1 = vImWH1; m_vWH2 = vImWH2; m_vWH2Ex = vImWH2Ex;

	m_vVertex = vVertex1; m_vVertex2 = vVertex2;

	m_vFill = vFill; memcpy(m_cM.data, vM.data(), vM.size() * sizeof(double));

	OnInstallAlignedParam(m_vWH2Ex[0], m_vWH2Ex[1], nFrameInLine, strAligned, nPassInStride, m_vImAligned);

	array<int, 2> vAlign = accumulate(m_vImAligned.begin(), m_vImAligned.end(), array<int, 2>{0, 0}, [](array<int, 2> val, array<int, 2> item)->array<int, 2> {return {val[0] + item[0], val[1] + item[1] }; });

	m_vImAlign = {abs((double)vAlign[0]/ (m_vImAligned.size()-1)), (double)vAlign[1]/ (m_vImAligned.size()-1) * 0.1 }; // 0 = {0, 0}

	vector<vector<array<string, 2>>> vCache;

	for (int i = 0; i < vFile.size(); i += nPassInStride * nFrameInLine)
	{
		vector<array<string, 2>> vLine;

		for (int j = 0; j < nFrameInLine; ++j)
		{
			int idx = i + j;

			vLine.push_back({ Wstring2String(vFile[idx][0]), Wstring2String(vFile[idx][1]) });
		}
		
		vCache.push_back(vLine);
	}
	SetFiles(vCache);
}

bool CThMergingEx::IsIdle()
{
	int nSize = 0; static int g_nTh = 1;
	{
		CSRLock lock(&m_lLock);

		nSize = m_vImCache.size();
	}
	return nSize < g_nTh;
}

void CThMergingEx::OnImCache(lpIm2Mat& lpCache)
{
	Push(lpCache);

	m_cImCache.signal();
}

void CThMergingEx::Push(lpIm2Mat& lpCache)
{
	CSWLock lock(&m_lLock);

	m_vImCache.push_back(lpCache);
}

bool CThMergingEx::Pop(lpIm2Mat& lpCache)
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

void CThMergingEx::Clear()
{
	CSWLock lock(&m_lLock);

	m_vImCache.clear();

	m_vImAligned.clear();

	!!m_lpImMerging ? [this]() { delete m_lpImMerging; m_lpImMerging = nullptr; }() : []() {}();
}

void CThMergingEx::Pop(array<int, 2>& vTL)
{
	ASSERT(!m_vImAligned.empty());

	auto iFront = m_vImAligned.begin();

	vTL = (*iFront);

	iFront = m_vImAligned.erase(iFront);
}

Point2f CThMergingEx::Target(Point2f ptSrc, Mat& M)
{
	Mat cSrc(3, 1, CV_64FC1, Scalar_<double>(1.0));

	cSrc.at<double>(0, 0) = ptSrc.x; cSrc.at<double>(1, 0) = ptSrc.y;

	Mat cTarg = M * cSrc; // 3*3 // 3*1

	// t[0,0] = m[0,0]*s[0,0] + m[0,1]*s[1,0] + m[0,2]*s[2,0]
	// t[1,0] = m[1,0]*s[0,0] + m[1,1]*s[1,0] + m[1,2]*s[2,0]
	// t[2,0] = m[2,0]*s[0,0] + m[2,1]*s[1,0] + m[2,2]*s[2,0]

	// x = m[0,0]*s[0,0] + m[0,1]*s[1,0] + m[0,2]*s[2,0] / m[2,0]*s[0,0] + m[2,1]*s[1,0] + m[2,2]*s[2,0]
	// y = m[1,0]*s[0,0] + m[1,1]*s[1,0] + m[1,2]*s[2,0] / m[2,0]*s[0,0] + m[2,1]*s[1,0] + m[2,2]*s[2,0]

	return { (float)(cTarg.at<double>(0,0) / cTarg.at<double>(2,0)), (float)(cTarg.at<double>(1,0) / cTarg.at<double>(2,0)) };
}

void CThMergingEx::Mixed(Mat& m1, Mat& m2)
{
	ASSERT(m1.rows == m2.rows);

	ASSERT(m1.cols == m2.cols);

	for (int64 y = 0; y < m1.rows; ++y)
	{
		for (int64 x = 0; x < m1.cols; ++x)
		{
			int64 idx = y * m1.cols + x;

			m2.data[idx] = (uchar)min((int)m1.data[idx] + m2.data[idx], 255);;
		}
	}
}

void CThMergingEx::Mixed(vector<Mat*>& vLine)
{
	Point2f ptTL(0, 0), ptTR(m_vWH1[0] + 1, 0);

	Point2f ptTL2 = Target(ptTL, m_cM), ptTR2 = Target(ptTR, m_cM);

	Rect2f rtL(ptTR2.x, ptTR2.y, m_vWH2[0] - ptTR2.x, m_vWH2[1] - ptTR2.y); // l

	Rect2f rtR(0, ptTL2.y, ptTL2.x, m_vWH2[1] - ptTL2.y); // r

	Rect rt; rt.x = 0; rt.width = min(rtL.width, rtR.width); rt.y = max(rtL.y, rtR.y), rt.height = min(rtL.y + rtL.height, rtR.y + rtR.height) - rt.y;

	rtL.width = rt.width; rtL.x = m_vWH2[0] - rtL.width; rtL.height = rt.height; rtL.y = rt.y;

	rtR.width = rt.width; rtR.x = 0; rtR.height = rt.height; rtR.y = rt.y;

	if (rtL.empty() || rtR.empty()) return;

	Mat cL(rtL.height, rtL.width, CV_8UC1, Scalar(0)), cR, cTmp;

	for (int i = 0; i < vLine.size(); ++i)
	{
		auto& iImMat = *vLine[i];

		cR = iImMat(rtR); cTmp = cL.clone();

		Mixed(cL, cTmp); cTmp.copyTo(cR);

		cL = iImMat(rtL).clone();
	}
}

void CThMergingEx::Line(vector<Mat*>& vLine, Mat& cLine)
{
	Point2f ptTL(0, 0), ptTR(m_vWH1[0] + 1, 0);

	Point2f ptTL2 = Target(ptTL, m_cM), ptTR2 = Target(ptTR, m_cM);

	int ox = m_vWH2Ex[0] - ptTR2.x + ptTL2.x, oy = ptTL2.y - ptTR2.y;

	int s = vLine.size(), w = m_vWH2Ex[0] * s - (s-1)*ox, h = m_vWH2Ex[1] + (s-1)*oy, os=(0<oy? ((s - 1) * oy) : 0);

	cLine = Mat(h, w, CV_8UC1, Scalar(0)); Rect rt(0, 0, m_vWH2Ex[0], m_vWH2Ex[1]);

	for (int i = 0; i < s; ++i)
	{
		auto& iFrame = *vLine[i];

		rt.x = m_vWH2Ex[0] * i - ox * i;

		rt.y = os - oy * i;

		iFrame.copyTo(cLine(rt));
	}
}

void CThMergingEx::Merging(array<int, 2> vTL, vector<Mat*>& vLine, Mat*& lpMerging)
{
	Mixed(vLine);

	Mat cLine; 
	
	Line(vLine, cLine);

	Rect rt(vTL[0], vTL[1], cLine.cols, cLine.rows);

	auto& iImage = *lpMerging;

	cLine.copyTo(iImage(rt));
}

void CThMergingEx::OnImMerging()
{
	lpIm2Mat lpCache = nullptr;

	if (!Pop(lpCache)) { m_cImCache.wait(); return; }
	
	array<int, 2> vTL; Pop(vTL); m_ptTL.x += vTL[0]; m_ptTL.y += vTL[1];// 上负 | 左负 

	cout << lpCache->strFirst.c_str() << ": " << vTL[0] << ", " << vTL[1] << endl;

	Merging({ m_ptTL.x + m_nOx, m_ptTL.y + m_nOy }, lpCache->vIm2Mat, m_lpImMerging);

	ImPtr::CImPtr cImPtr = ImPtr::CImPtr(new Mat());

	resize(*m_lpImMerging, *cImPtr.Get(), Size(m_lpImMerging->cols * 0.1, m_lpImMerging->rows * 0.1));

	Rect rtROI(m_vReference[0] + m_vReference[2] * 2, 0, m_lpImMerging->cols - m_vReference[0] - m_vReference[2] * 2, m_lpImMerging->rows);

	!!lpCache->nRemaining ? OnMergingEx(cImPtr) : OnMergeEx(m_vImAlign, cImPtr);

	!!lpCache->nRemaining ? []() {}() :
		[](Mat* lpMer, Rect rtROI, string strMerged, string strROIMerged) {

		imwrite(strMerged.c_str(), *lpMer);

		Mat cImage = imread(strMerged.c_str(), 0);

		imwrite(strROIMerged.c_str(), cImage(rtROI)); }(m_lpImMerging, rtROI, m_strMerged, m_strROIMerged);
	
	delete lpCache;
}
