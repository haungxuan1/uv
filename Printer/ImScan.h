#pragma once
#include "PrinterInit.h"

// 图像扫描 - 将相机采集图像进行本地存储
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

private:// 采取复用
	uint m_uThSize; // 存图线程数

private:
	vParams m_vParams;
	vThParam* m_lpThParam;
	bool* m_lpTerminate;
	bool* m_lpThread;
	CSRWLockBase m_lLock;
	CCondition m_cImScan;
	CCondition *m_lpTermination;
};