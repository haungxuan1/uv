#pragma once
#include "PrinterDPIAlignment.h"

class CPrtMatrix : public CPrinterDPIAlignment
{
public:
	typedef tuple<bool, Rect, ImPtr::CImPtr> SingleTarget;
	typedef vector<SingleTarget> vTarget;
	typedef tuple<int, bool, vTimer, array<int64, 3>> UserParam;

protected:
	CPrtMatrix();
	virtual ~CPrtMatrix();

protected:
	virtual bool Reg();
	virtual void Unreg();
	virtual void AllocateMmemory();
	virtual void DeallocateMemory();

public:
	void PrtMatrix(vParam iParam);

private:
	void OnPrtMatrix(vParam& iParam);
	void OnCopyTemp(vector<tuple<bool, Rect, ImPtr::CImPtr>>& vTarg);
	void OnCopyTemp(tuple<bool, Rect, ImPtr::CImPtr>& vTarg);
	void OnMove(int64& nOy);
	void OnImMove(int64& nOy);
	void OnTeMove(int64& nOy);

	void SendMatrix(int& nFrameId, bool& bFleece, vector<tuple<string, int>>& vTimer, bool bFinish, int64& nCy);
	bool IsUnite(Rect &rtTarg);

private:
	Mat m_cSight;
	Mat m_cCache;
	vector<Rect> m_vTarget;

private:
	vThParam m_vThParam;
	vParams m_vParams;
	bool m_bTermination;
	bool m_bThread;
	CSRWLockBase m_lLock;
	CCondition m_cPosition;
	CCondition m_cTermination;
};