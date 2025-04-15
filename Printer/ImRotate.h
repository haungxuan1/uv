#pragma once
#include "ImScan.h"

class CImRotate : public CImScan
{
public:
	CImRotate();
	virtual ~CImRotate();

public:
	virtual bool Reg();
	virtual void Unreg();
	virtual void AllocateMmemory();
	virtual void DeallocateMemory();

protected:
	void ImRotate(vParam& iParam);

private:
	void OnImRotate(vParam& iParam);

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
	CCondition* m_lpTermination;
};