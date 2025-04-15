#pragma once
#include "PrinterInit.h"

// ͼ��ɨ�� - ������ɼ�ͼ����б��ش洢
class CImScan : public CPrinterInit
{
protected:
	CImScan();
	virtual ~CImScan();

protected:
	virtual bool Reg();
	virtual void Unreg();
	virtual void AllocateMmemory();
	virtual void DeallocateMemory();

protected:
	void ImScan(vParam& iParam);

private:
	void OnImScan(vParam& iParam);

private:
	void InitParam();
	void DestoryParam();

private:// ��ȡ����
	uint m_uThSize; // ��ͼ�߳���

private:
	vParams m_vParams;
	vThParam* m_lpThParam;
	bool* m_lpTerminate;
	bool* m_lpThread;
	CSRWLockBase m_lLock;
	CCondition m_cImScan;
	CCondition *m_lpTermination;
};