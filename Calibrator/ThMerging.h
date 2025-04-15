#pragma once
#include "ThAligning.h"

using namespace cv;
using namespace std;

class CThMerging : public CThAligning
{
protected:
	CThMerging();
	virtual ~CThMerging();

protected:
	virtual bool Reg();
	virtual void Unreg();

protected:
	virtual void OnMerging(int nRemaining, ImPtr::CImPtr cImPtr) = 0;
	virtual void OnMerge(int nRemaining, ImPtr::CImPtr cImPtr) = 0;

protected:
	void SetMergingParam(vector<array<wstring, 2>>& vFile, array<int, 8> vThParam, int nPassInStride, string strAligned, string strMerged, string strROIMerged);

private:
	void OnImMerging();

private:
	void Merging(int nImWidth, int nImHeight, array<int, 2> vTL, vector<Mat*>& vLine, Mat*& lpMerging);
	void OnInstallAlignedParam(int nImWidth, int nImHeight, int nFrameInLine, string& strAligned, vector<array<int, 2>>& vImAligned);

private:
	bool IsIdle();
	void OnImCache(lpIm2Mat& lpCache);
	void Push(lpIm2Mat& lpCache);
	bool Pop(lpIm2Mat& lpCache);
	void Pop(array<int, 2>& vTL);
	void Clear();

private:
	int m_nOx; // ¶ÔÆë²¹³¥
	int m_nPassInStride;
	Point m_ptTL;
	Mat* m_lpImMerging;
	array<int, 8> m_vThParam;
	string m_strMerged;
	string m_strROIMerged;
	vector<array<int, 2>> m_vImAligned;



private:
	CSRWLockBase m_lLock;
	CCondition m_cImCache;
	vector<lpIm2Mat> m_vImCache;
};