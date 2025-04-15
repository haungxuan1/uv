#pragma once
#include "Homography.h"

// —È÷§
class CPrinterInit : public CHomography
{
protected:
	CPrinterInit();
	virtual ~CPrinterInit();

protected:
	virtual bool Reg();
	virtual void Unreg();
	virtual void AllocateMmemory();
	virtual void DeallocateMemory();

protected:
	void PrinterInit(vParam iParam);

private:
	void OnPrinterInit(vParam& iParam);

private:
	vThParam m_vThParam;
	vParams m_vParams;
	bool m_bTermination;
	bool m_bThread;
	CSRWLockBase m_lLock;
	CCondition m_cImInit;
	CCondition m_cTermination;
};