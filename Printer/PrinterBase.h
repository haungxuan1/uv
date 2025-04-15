#pragma once
#include <tuple>
#include <vector>
#include <functional>
#include "ImPtr.h"
#include "SRWLock.h"
#include "Condition.h"
using namespace cv;
using namespace std;

// ��ӡ����
class CPrinterBase
{
public:
	enum { ePT_Empty = 0, ePT_Scan, ePT_Calibrator, ePT_Print, ePT_FastPrint }; // printer type
	enum { ePI_FrameId = 0, ePI_Fleece, ePI_Timer, ePI_UserParam, ePI_ImPtr, }; // pkg Idx

public:
	typedef vector<tuple<string, int>> vTimer;
	typedef tuple<int, bool, vTimer, void*, ImPtr::CImPtr> vParam; // idx, time, userparam, imptr

protected:
	typedef vector<vParam> vParams;
	typedef tuple<string, CPrinterBase*, CSRWLockBase*, CCondition*, CCondition*, bool*, vParams*, function<void(vParam&)>> vThParam;

protected:
	CPrinterBase();
	virtual ~CPrinterBase();

protected:
	virtual bool Reg() = 0;
	virtual void Unreg() = 0;
	virtual void AllocateMmemory() = 0;
	virtual void DeallocateMemory() = 0;

public:
	void SetBaseDir(string strBaseDir) { m_strImBaseDir = strBaseDir; }
	void SetInitiator(CWnd* lpInitiator) { m_lpInitiator = lpInitiator; }
	void SetPrinterType(unsigned char uPrinterType) { m_uPrinterType = uPrinterType; }
	void SetForegroundThreshold(int nThreshold) { m_nForegroundThreshold = nThreshold; }
	void SetTransparentThreshold(int nThreshold) { m_nTransparentThreshold = nThreshold; }
	void SetSkipFrame(array<int, 2> vSkipFrame) { m_vSkipFrame = vSkipFrame; }
	void SetSkipLine(int nSkipLine) { m_nSkipLine = nSkipLine; }
	void SetImInLine(int nImInLine) { m_nImInLine = nImInLine; }
	void SetImWH(array<float, 2> vImWH) { m_vImWH = vImWH; }
	void SetImWH2(array<float, 2> vImWH) { m_vImWH2 = vImWH; }
	void SetImWH2Ex(array<int, 2> vImWH) { m_vImWH2Ex = vImWH; }
	void SetImHomography(Mat cHomography) { m_cHomography = cHomography.clone(); }
	void SetImCompensation(array<int, 4> vFill) { m_vFill = vFill; }
	void SetSignRegion(Rect rtSign) { m_rtSign = rtSign; }
	void SetSight(array<int, 2> vSight) { m_vSight[1] = vSight[0]; m_vSight[2] = vSight[1]; }
	void SetOffset(array<int, 2> vOffset){ m_vOffset = vOffset; }
	void SetPrintHeader(array<int, 2> vPrtHeader) { m_vPrtHeader = vPrtHeader; m_vPrtHeader[0] = ceil(1.0 /32.0 * m_vPrtHeader[0]) * 32; } // prt�п�Ϊ4������,matתprt�п�Ϊ1/8,��������ȡ32����
	void SetAligParam(array<int, 6> vAligParam) { m_vAligParam = vAligParam; }
	void SetTIFF(string strTIFF, int nTIFFChannel) { m_strTIFF = strTIFF; m_nTIFFChannel = nTIFFChannel;; }
	void SetTemplate(Mat cTemplate) { m_cTemplate = cTemplate.clone(); }
	void SetCmeraDpi(array<double,2> dDPI) { m_dCameraDPI= dDPI; }
	void SetPrinteDpi(array<double,2> vDPI) { m_vPrintDPI = vDPI; }
	void Enable1bitDll(bool bEnable) { m_b1bitDll = bEnable; }
	void SetMask(Mat cMask) { m_cMask = cMask.clone(); }

public:
	string GetBaseDir() { return m_strImBaseDir; }
	array<int, 2> GetSight() { return { m_vSight[1], m_vSight[2] }; }
	array<int, 2> GetOffset() { return m_vOffset; }
	array<double,2> GetCmeraDpi() { return m_dCameraDPI; }
	array<double, 2> GetPrinteDpi() { return m_vPrintDPI; }
	unsigned char GetPrinterType() { return m_uPrinterType; }
	int GetImInLine() { return m_nImInLine; }
	array<int, 2> GetPrintHeader() { return m_vPrtHeader; }
	int GetSkipLine() { return m_nSkipLine; }

protected:
	CWnd* m_lpInitiator; // ��������Ϣ
	unsigned char m_uPrinterType; // ��ӡ����
	array<int,2> m_vSkipFrame; // ���Ӳ�����⣬��������
	int m_nSkipLine; // ��/����
	int m_nImInLine; // ֡/ÿ��
	int m_nForegroundThreshold; // ǰ�����ָ���ֵ
	int m_nTransparentThreshold; // ͸��ʱ����ֵ�任��ֵ
	Mat m_cHomography; // ��Ӧ�Ա任����
	array<float, 2> m_vImWH; // �任ǰͼ���С
	array<float, 2> m_vImWH2; // �任��ͼ�����۴�С
	array<int, 2> m_vImWH2Ex; // �任��ͼ��ʵ�ʴ�С
	array<int, 4> m_vFill; // ͸�ӱ任����ֵ
	Rect m_rtSign; // ����������
	array<int, 3> m_vSight; // ˮƽ��Ұ // ����ҰΪ m_cImSight - ������
 	array<int, 6> m_vAligParam; // ������������
	array<int, 2> m_vOffset; // ƫ�� // īͷ����� // ˮƽ | ��ֱ
	array<int, 2> m_vPrtHeader;	// prt.�� = ˮƽƫ�� + ��Ұ.��  // prt.�� = ��ֱƫ�� + ��Ұ.��
	array<double, 2> m_dCameraDPI;// ���dpi - �����dpiת����xy�����
	array<double, 2> m_vPrintDPI; // ��ӡdpi
	string m_strTIFF; // ��ͼTIFF(����ΪCMYK)
	Mat m_cTemplate; // ƥ��ģ��(����Ϊͬ���ٶ�\�����������ͼ)
	Mat m_cMask;
	bool m_b1bitDll;
	int m_nTIFFChannel;

protected:
	string m_strImBaseDir; // ���̴洢�ļ���

protected:
	void Restart(bool& bTermination, CSRWLockBase& cLock);
	void Termination(bool& bTermination, CSRWLockBase& cLock);
	bool IsTermination(bool& bTermination, CSRWLockBase& cLock);
	int Pushback(vParam&, vParams&, CSRWLockBase&);
	void Pushfront(vParam&, vParams&, CSRWLockBase&);
	bool Popfront(vParam&, vParams&, CSRWLockBase&);

protected:
	static unsigned int __stdcall OnExitThread(void* lpUesrParam);
	static unsigned int __stdcall OnWorkThread(void* lpUesrParam);
};