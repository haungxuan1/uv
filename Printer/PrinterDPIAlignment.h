#pragma once
#include "Mat2Prt.h"

class CPrinterDPIAlignment : public CMat2Prt
{
protected:
	CPrinterDPIAlignment();
	virtual ~CPrinterDPIAlignment();

protected:
	virtual bool Reg();
	virtual void Unreg();
	virtual void AllocateMmemory();
	virtual void DeallocateMemory();

protected:
	void DPIAlignment(vParam iParam);

private:
	void OnDPIAlignment(vParam& iParam);
	void MakeBorder(Mat& cMat, int top, int bottom, int left, int right);

private:
	int64 m_nPrintRows;

private:
	vThParam m_vThParam;
	vParams m_vParams;
	bool m_bTermination;
	bool m_bThread;
	CSRWLockBase m_lLock;
	CCondition m_cAlignment;
	CCondition m_cTermination;
};