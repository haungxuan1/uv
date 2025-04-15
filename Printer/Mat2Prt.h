#pragma once
#include "PrtTCP.h"

class CMat2Prt : public CPrtTCP
{
private:
	typedef void (*fMat2Prt1bit)(
		unsigned short*, int,
		int,
		int,
		int, void*, int, int, int);  // 年前打印 1bit

	typedef void (*fMat2Prt2bit)(unsigned short*, int, int, int);	// 新dll 2bit

protected:
	CMat2Prt();
	virtual ~CMat2Prt();

protected:
	virtual bool Reg();
	virtual void Unreg();
	virtual void AllocateMmemory();
	virtual void DeallocateMemory();

public:
	void Mat2Prt(vParam iParam);

private:
	void OnMat2Prt(vParam& iParam);
	void M8U2M16U(cv::Mat& v8U, unsigned short*& lp16U, unsigned int& uSize);
	void M16U2M8U(unsigned short*& lp16U, cv::Mat& v8U, cv::Mat& vImage);
	void Matrix2Buffer(cv::Mat& vMatrix, unsigned char* lpBuffer, unsigned int& uSize);
	void Swap(cv::Mat& a, cv::Mat& b);

private:
	HMODULE m_lpHandle1bit;
	HMODULE m_lpHandle2bit;
	fMat2Prt1bit m_fMat2Prt1bit;
	fMat2Prt2bit m_fMat2Prt2bit;

private:
	vThParam m_vThParam;
	vParams m_vParams;
	bool m_bTermination;
	bool m_bThread;
	CSRWLockBase m_lLock;
	CCondition m_cPrtMatrix;
	CCondition m_cTermination;
};