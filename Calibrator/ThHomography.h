#pragma once
#include "ThPerspective.h"

using namespace cv;
using namespace std;

class CThHomography : public CThPerspective
{
protected:
	CThHomography();
	virtual ~CThHomography();

protected:
	virtual bool Reg();
	virtual void Unreg();

protected:
	virtual void OnTranslating(ImPtr::CImPtr cImPtr) = 0;
	virtual void OnHomography(array<int, 2> vWH2Ex, ImPtr::CImPtr cImPtr) = 0;

protected:
	void SetHomographyParam(
		vector<array<wstring, 2>>& vFile, 
		int nFrameInLine, 
		int nPassInStride,
		array<int, 4> vFill,
		array<float, 2> vWH2,
		array<double, 9> M,
		string strDir);

private:
	void OnImHomography();

private:
	bool IsIdle();
	void OnImCache(lpIm2Mat& lpCache);
	void Push(lpIm2Mat& lpCache);
	bool Pop(lpIm2Mat& lpCache);
	void Clear();

private:
	Mat m_cM;
	array<int, 4> m_vFill;
	array<float, 2> m_vWH2;
	string m_strOutputDir;

private:
	CSRWLockBase m_lLock;
	CCondition m_cImCache;
	vector<lpIm2Mat> m_vImCache;
};