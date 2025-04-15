#include "pch.h"
#include "ImageLine.h"
#include "PrClock/PrClock.h"

CImageLine::CImageLine() :
	m_bTermination(false), m_bThread(false)
{
	m_vThParam = make_tuple(
		__FUNCTION__,
		this,
		&m_lLock,
		&m_cImFrame,
		&m_cTermination,
		&m_bTermination,
		&m_vParams,
		bind(&CImageLine::OnImLine, this, placeholders::_1));
}

CImageLine::~CImageLine()
{

}

bool CImageLine::Reg()
{
	if (!CCameraDPIAlignment::Reg()) return false;

	_cwprintf(L"%s\r\n", __FUNCTIONW__);

	Restart(m_bTermination, m_lLock);

	HANDLE lpHandle = (HANDLE)_beginthreadex(NULL, 0, OnWorkThread, (void*)&m_vThParam, 0, NULL);

	CloseHandle(lpHandle);

	return (m_bThread = (!!lpHandle));
}

void CImageLine::Unreg()
{
	m_bThread ? CloseHandle((HANDLE)_beginthreadex(NULL, 0, OnExitThread, (void*)&m_vThParam, 0, NULL)) : []()->BOOL {return 0; }();

	m_bThread ? m_cTermination.wait() : []()->bool { return 0; }(); m_bThread = false;

	_cwprintf(L"%s\r\n", __FUNCTIONW__);

	CCameraDPIAlignment::Unreg();

	_cwprintf(L"%s\r\n", __FUNCTIONW__);
}

void CImageLine::AllocateMmemory()
{
	CCameraDPIAlignment::AllocateMmemory();

}

void CImageLine::DeallocateMemory()
{
	for (auto& iParam : m_vParams) 
	{ 
		vector<vParam>* lpImInLine = (vector<vParam>*)get<ePI_UserParam>(iParam); 
		
		lpImInLine->clear(); 
		
		delete lpImInLine; 
	}

	m_vParams.clear();

	CCameraDPIAlignment::DeallocateMemory();
}

void CImageLine::ImLine(vParam& iParam)
{
	Pushback(iParam, m_vParams, m_lLock);

	m_cImFrame.signal();
}

void CImageLine::OnImLine(vParam& iParam)
{
	vector<vParam>* lpImInLine = (vector<vParam>*)get<ePI_UserParam>(iParam);

	OnCover(lpImInLine, get<ePI_ImPtr>(iParam));

	get<ePI_Timer>(iParam).push_back({ __FUNCTION__, clock() });

	CCameraDPIAlignment::CameraDPIAlignment(iParam);
}

Point2f CImageLine::Target(Point2f ptSrc, Mat& M)
{
	Mat cSrc(3, 1, CV_64FC1, Scalar_<double>(1.0));

	cSrc.at<double>(0, 0) = ptSrc.x; cSrc.at<double>(1, 0) = ptSrc.y;

	Mat cTarg = M * cSrc;

	return { (float)(cTarg.at<double>(0,0) / cTarg.at<double>(2,0)), (float)(cTarg.at<double>(1,0) / cTarg.at<double>(2,0)) };
}

void CImageLine::Mixed(Mat& m1, Mat& m2)
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

void CImageLine::Mixed(vector<vParam>*& lpImInLine)
{
	Point2f ptTL(0, 0), ptTR(m_vImWH[0] + 1, 0);

	Point2f ptTL2 = Target(ptTL, m_cHomography), ptTR2 = Target(ptTR, m_cHomography);

	Rect2f rtL(ptTR2.x, ptTR2.y, m_vImWH2[0] - ptTR2.x, m_vImWH2[1] - ptTR2.y); // l

	Rect2f rtR(0, ptTL2.y, ptTL2.x, m_vImWH2[1] - ptTL2.y); // r

	Rect rt; rt.x = 0; rt.width = min(rtL.width, rtR.width); rt.y = max(rtL.y, rtR.y), rt.height = min(rtL.y + rtL.height, rtR.y + rtR.height) - rt.y;

	rtL.width = rt.width; rtL.x = m_vImWH2[0] - rtL.width; rtL.height = rt.height; rtL.y = rt.y;

	rtR.width = rt.width; rtR.x = 0; rtR.height = rt.height; rtR.y = rt.y;

	if (rtL.empty() || rtR.empty()) return;

	Mat cL(rtL.height, rtL.width, CV_8UC1, Scalar(0xff)), cR, cTmp;

	for (int i = 0; i < lpImInLine->size(); ++i)
	{
		auto& iImMat = *get<ePI_ImPtr>((*lpImInLine)[i]).Get();

		cR = iImMat(rtR); cTmp = cL.clone();

		Mixed(cL, cTmp); cTmp.copyTo(cR);

		cL = iImMat(rtL).clone();
	}
}

void CImageLine::Line(vector<vParam>*& lpImInLine, Mat& cLine)
{
	Point2f ptTL(0, 0), ptTR(m_vImWH[0] + 1, 0);

	Point2f ptTL2 = Target(ptTL, m_cHomography), ptTR2 = Target(ptTR, m_cHomography);

	int ox = m_vImWH2Ex[0] - ptTR2.x + ptTL2.x, oy = ptTL2.y - ptTR2.y;

	int s = lpImInLine->size(), w = m_vImWH2Ex[0] * s - (s - 1) * ox, h = m_vImWH2Ex[1] + (s - 1) * oy, os = (0 < oy ? ((s - 1) * oy) : 0);

	cLine = Mat(h, w, CV_8UC1, Scalar(0)); Rect rt(0, 0, m_vImWH2Ex[0], m_vImWH2Ex[1]);

	for (int i = 0; i < s; ++i)
	{
		auto& iFrame = *get<ePI_ImPtr>((*lpImInLine)[i]).Get();

		rt.x = m_vImWH2Ex[0] * i - ox * i;

		rt.y = os - oy * i;

		iFrame.copyTo(cLine(rt));
	}
}

void CImageLine::OnCover(vector<vParam>*& lpImInLine, ImPtr::CImPtr& iImLine)
{
	Mat cLine;

	int nImWidth = m_vImWH[0], nImHeight = m_vImWH[1];

	int nImWidth2 = round(m_vImWH2[0]), nImHeight2 = floor(m_vImWH2[1]);

	Mixed(lpImInLine);

	Line(lpImInLine, cLine);

	iImLine = ImPtr::CImPtr(new Mat(cLine.rows, cLine.cols, CV_8UC1));

	cLine.copyTo(*iImLine.Get());

	lpImInLine->clear(); delete lpImInLine;
}

