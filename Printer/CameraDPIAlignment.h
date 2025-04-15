#pragma once
#include "ImageMosaic.h"

// —È÷§
class CCameraDPIAlignment : public CImageMosaic
{
protected:
	CCameraDPIAlignment();
	virtual ~CCameraDPIAlignment();

protected:
	virtual bool Reg();
	virtual void Unreg();
	virtual void AllocateMmemory();
	virtual void DeallocateMemory();

protected:
	void CameraDPIAlignment(vParam& iParam);

private:
	void OnCameraDPIAlignment(vParam& iParam);

private:
	vThParam m_vThParam;
	vParams m_vParams;
	bool m_bTermination;
	bool m_bThread;
	CSRWLockBase m_lLock;
	CCondition m_cImFrame;
	CCondition m_cTermination;
};