#pragma once
#include "ImageLine.h"

class CHomography : public CImageLine
{
protected:
	CHomography();
	virtual ~CHomography();

protected:
	virtual bool Reg();
	virtual void Unreg();
	virtual void AllocateMmemory();
	virtual void DeallocateMemory();

protected:
	void ImHomography(vParam iParam);

private:
	void OnImHomography(vParam& iParam);

private:
	vThParam m_vThParam;
	vParams m_vParams;
	bool m_bTermination;
	bool m_bThread;
	CSRWLockBase m_lLock;
	CCondition m_cImFrame;
	CCondition m_cTermination;
};