#pragma once
#include "PrinterBase.h"

class CPrtTCP : public CPrinterBase
{
protected:
	CPrtTCP();
	virtual ~CPrtTCP();

protected:
	virtual bool Reg();
	virtual void Unreg();
	virtual void AllocateMmemory();
	virtual void DeallocateMemory();

//protected:
public:
	void TCP(vParam iParam);

private:
	void OnTCP(vParam& iParam);
	bool OnSend(unsigned int& nSent, unsigned int& nSize, unsigned char*& lpBuffer);

private:
	SOCKET m_lpHandle;

private:
	vThParam m_vThParam;
	vParams m_vParams;
	bool m_bTermination;
	bool m_bThread;
	CSRWLockBase m_lLock;
	CCondition m_cSend;
	CCondition m_cTermination;
};