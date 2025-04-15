#pragma once
#include "ThForeground.h"
#include "ImPtr.h"

using namespace cv;
using namespace std;

class CThReference : public CThForeground
{
protected:
	CThReference();
	virtual ~CThReference();

protected:
	virtual bool Reg();
	virtual void Unreg();

protected:
	virtual void OnReference(Rect& rtRef, ImPtr::CImPtr& cImPtr) = 0;

protected:
	void SetReferenceParam(vector<array<wstring, 2>>& vFile, int nFrameInLine, array<int,5> vKernel);

private:
	void OnPositioning();
	
private:
	void Ptrs2Ptr(vector<tuple<Rect, ImPtr::CImPtr>>& vReference, Rect& rtRef, ImPtr::CImPtr& cImPtr);

private:
	bool IsIdle();
	void OnImCache(lpIm2Mat& lpCache);
	void Push(lpIm2Mat& lpCache);
	bool Pop(lpIm2Mat& lpCache);
	void Clear();

private:
	array<int, 5> m_vKernel;
	vector<tuple<Rect, ImPtr::CImPtr>> m_vReference;

private:
	CSRWLockBase m_lLock;
	CCondition m_cImCache;
	vector<lpIm2Mat> m_vImCache;
};