#pragma once
#include "Position.h"

class CImTransparent : public CPosition
{
protected:
	CImTransparent();
	virtual ~CImTransparent();

protected:
	virtual bool Reg();
	virtual void Unreg();
	virtual void AllocateMmemory();
	virtual void DeallocateMemory();

protected:
	void ImTransparent(vParam iParam);

private:
	void OnImTransparent(vParam& iParam);

private:
	Mat m_cTransparent;

private:
	vThParam m_vThParam;
	vParams m_vParams;
	bool m_bTermination;
	bool m_bThread;
	CSRWLockBase m_lLock;
	CCondition m_cImFrame;
	CCondition m_cTermination;
};