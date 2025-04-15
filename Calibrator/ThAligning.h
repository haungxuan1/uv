#pragma once
#include "ThReference.h"

using namespace cv;
using namespace std;

class CThAligning : public CThReference
{
protected:
	enum { ePT_w = 0, ePT_h, ePT_FrameInLine, ePT_Stride, ePT_x, ePT_y, ePT_sw, ePT_sh};

protected:
	CThAligning();
	virtual ~CThAligning();

protected:
	virtual bool Reg();
	virtual void Unreg();

protected:
	virtual void OnAligning(int nRemaining, double dScore, ImPtr::CImPtr cImPtr) = 0;
	virtual void OnAlign(int nRemaining, double dScore, ImPtr::CImPtr cImPtr, int nPassInStride) = 0;

protected:
	void SetAligningParam(vector<array<wstring, 2>>& vFile, array<int, 8> vThParam, array<int, 6> vKernel, string srtAligned);

private:
	void OnImAligning();
	bool InitAlignmentParam(Mat& cCol);
	void InstallAlignedParam(string& srtAligned, vector<array<int, 2>>& vTL);
	 
private:
	bool IsIdle();
	void OnImCache(lpIm2Mat& lpCache);
	void Push(lpIm2Mat& lpCache);
	bool Pop(lpIm2Mat& lpCache);
	void Clear();

private:
	int m_nPassInStride;
	Mat m_cImSign;
	array<int, 8> m_vThParam;
	array<int, 6> m_vKernel;
	string m_srtAligned;
	vector<array<int, 2>> m_vTL;

private:
	CSRWLockBase m_lLock;
	CCondition m_cImCache;
	vector<lpIm2Mat> m_vImCache;
};