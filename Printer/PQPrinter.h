#pragma once
#include "Fleece.h"

/*
* 打印模块具有三种形态
* 1. 空打印   - 将相机采集到的图像进行存储
* 2. 校准打印 - 所见即所得，将相机采集到的图像以一定阈值处理后反打回台面
* 3. 常态打印 - 模板识别+prt转换
*/
/*
* 在prt状态下pm软件还不能很好的中断一份作业从而半途开启新作业，因此打印作业做出以下改动
* 1. 及时断开tcp连接
* 2. 清空堆积作业
*/
class CPQPrinter : public CFleece
{
public:
	CPQPrinter();
	virtual ~CPQPrinter();
public:
	virtual bool Reg(bool bCollect = false);
	virtual void Unreg();
	virtual void AllocateMmemory();
	virtual void DeallocateMemory();

public:
	void ImCapture(vParam iParam);
	void OnPrt();
	void StopSend();

private:
	void OnImCapture(vParam& iParam);
	void OnImWriteTh(vParam& iParam);
	void OnImFleeceTh(vParam& iParam);
	void OnImPrintTh(vParam& iParam);
	void OnImCaptureTh(vParam& iParam);

private:
	bool IsFleeceSkip(int& nFrameIdx);
	bool IsSkip(int& nFrameIdx); // 由于打印起始时小车移位造成首行包含移位距离
	bool IsEnable(int& nFrameIdx); // 与上帧图像交集最小或末尾帧称作视野帧
	void OnPrFrame(vParam& iParam);

private:
	vector<vParam>* m_lpImInLine; // 帧/每行

private:
	vThParam m_vThParam;
	vParams m_vParams;
	bool m_bTermination;
	bool m_bThread;
	CSRWLockBase m_lLock;
	CCondition m_cImCapture;
	CCondition m_cTermination;
};
