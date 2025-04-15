#pragma once
#include "CameraDPIAlignment.h"

// ∆¥Õº¥Ê‘⁄Œ Ã‚
class CImageLine : public CCameraDPIAlignment
{
protected:
	CImageLine();
	virtual ~CImageLine();

protected:
	virtual bool Reg();
	virtual void Unreg();
	virtual void AllocateMmemory();
	virtual void DeallocateMemory();

protected:
	void ImLine(vParam& iParam);

private:
	void OnImLine(vParam& iParam);

private:
	Point2f Target(Point2f ptSrc, Mat& M);
	void Mixed(Mat& m1, Mat& m2);
	void Mixed(vector<vParam>*& lpImInLine);
	void Line(vector<vParam>*& lpImInLine, Mat& cLine);
	void OnCover(vector<vParam>*& lpImInLine, ImPtr::CImPtr& iImLine);

private:
	vThParam m_vThParam;
	vParams m_vParams;
	bool m_bTermination;
	bool m_bThread;
	CSRWLockBase m_lLock;
	CCondition m_cImFrame;
	CCondition m_cTermination;
};