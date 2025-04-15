#pragma once
#include "ImRotate.h"

// 程序间通讯协议_通用版.pdf
// 
// 专属于边扫边打跳白模式 - 该类作为一个临时补丁
// 1. x归原点
// 2. y归原点
// 3. y移动至 图像.h + 相机与墨头间距
// 4. x移动 距离 = 图像.w
// 5. x反向移动 
// 6. y移动
// 7. 重复4-6直至形成 图像.h = 相机与墨头间距
class CFleece : public CImRotate
{
public:
	// direction index
	enum 
	{
		eDI_Left = 1, 
		eDI_Right,
		eDI_Front,
		eDI_Back
	};
	// speed index
	enum
	{
		eSI_1200 = 0,
		eSI_121,
		eSI_191,
		eSI_262,
		eSI_332,
		eSI_700,
		eSI_1000
	};

protected:
	CFleece();
	virtual ~CFleece();

protected:
	virtual bool Reg();
	virtual void Unreg();
	virtual void AllocateMmemory();
	virtual void DeallocateMemory();

public:
	void OnFleece(vParam& iParam);
	void SetFleeceParam(vector<int> vOy);
	virtual void FleeceParam(int& nOy);
	void Begin();
	void End();

protected:
	bool IsFleece();
	void SetFleece(bool bFleece);

private:
	static unsigned int __stdcall OnRecvThread(void* lpUesrParam);
	static unsigned int __stdcall OnSendThread(void* lpUesrParam);
	static unsigned int __stdcall OnPrtThread(void* lpUesrParam);
	static unsigned int __stdcall OnTermThread(void* lpUesrParam);

private:
	void SetThStatus(uchar uTh);
	bool GetThStatus(uchar uTh);
	void ClearThStatus(uchar uTh);
	void SetWkStatus(uchar uTh);
	bool GetWkStatus(uchar uTh);
	void ClearWkStatus(uchar uTh);
	void SetTmStatus(uchar uTh);
	bool ClearTmStatus(uchar uTh);

private:
	void Push(vParam& iParam);
	bool Pop(vParam& iParam);

private:
	void Push(tuple<unsigned char*, int, int, bool> vCmd); // pkg | pkg size | pkg id | free
	void Insert(tuple<unsigned char*, int, int, bool> vCmd);
	bool Pop(tuple<unsigned char*, int, int, bool>& vCmd);

public:
	void PushCmd(tuple<unsigned char*, int, int, bool> vCmd);

	unsigned char Sum(unsigned char* lpParam, int nSize);
	void CMD(unsigned char uPriCMD, unsigned char uSecCMD, wstring strParam, unsigned char*& lpCmd, int& nCmdSize);
	void StateCmd(unsigned char*& lpCmd, int& nCmdSize);
	void InInPass(unsigned char*& lpCmd, int& nCmdSize);
	void Stop(unsigned char*& lpCmd, int& nCmdSize);
	void X0(unsigned char*& lpCmd, int& nCmdSize);
	void Y0(unsigned char*& lpCmd, int& nCmdSize);
	void Move(int nDirection, int nSpeed, unsigned long long nDistance, unsigned char*& lpCmd, int& nCmdSize);
	void Pos(unsigned char*& lpCmd, int& nCmdSize);

private:
	bool m_bFleece; // 相机与墨头间距跳白 
	vector<int> m_vOy;
	vParams m_vParams;

	SOCKET m_lpHandle;
	uchar m_uThread;
	uchar m_uWorking;
	uchar m_uTermination;

	CSRWLockBase m_lLock;
	CCondition m_cSend;
	CCondition m_cPrt;
	CCondition m_cTermination;

	vector<tuple<unsigned char*, int, int, bool>> m_vCmdPkg; // szcmd | cmd size | pkg id | free
};
