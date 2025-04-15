#pragma once
#include "Fleece.h"

/*
* ��ӡģ�����������̬
* 1. �մ�ӡ   - ������ɼ�����ͼ����д洢
* 2. У׼��ӡ - ���������ã�������ɼ�����ͼ����һ����ֵ����󷴴��̨��
* 3. ��̬��ӡ - ģ��ʶ��+prtת��
*/
/*
* ��prt״̬��pm��������ܺܺõ��ж�һ����ҵ�Ӷ���;��������ҵ����˴�ӡ��ҵ�������¸Ķ�
* 1. ��ʱ�Ͽ�tcp����
* 2. ��նѻ���ҵ
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
	bool IsSkip(int& nFrameIdx); // ���ڴ�ӡ��ʼʱС����λ������а�����λ����
	bool IsEnable(int& nFrameIdx); // ����֡ͼ�񽻼���С��ĩβ֡������Ұ֡
	void OnPrFrame(vParam& iParam);

private:
	vector<vParam>* m_lpImInLine; // ֡/ÿ��

private:
	vThParam m_vThParam;
	vParams m_vParams;
	bool m_bTermination;
	bool m_bThread;
	CSRWLockBase m_lLock;
	CCondition m_cImCapture;
	CCondition m_cTermination;
};
