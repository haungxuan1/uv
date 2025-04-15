#pragma once
#include "Im2Mat.h"

class CImPadding : public CIm2Mat
{
	typedef tuple<CImPadding*, function<bool()>> vThParam, * lpThParam;

protected:
	enum { eWT_Foreground, eWT_Reference, eWT_Aligning, eWT_Merging, eWT_MergingEx, eWT_ROI, eWT_ROIEx, eWT_Slope, eWT_Perspective, eWT_Homography, eWT_DPI };

protected:
	CImPadding();
	virtual ~CImPadding();

protected:
	void SetWorkType(uint8 uType);
	void SetDebugDir(string strDir);

protected:
	void SetThParam(CCondition* lpWork, function<void()> fWork);

protected:
	virtual bool Reg();
	virtual void Unreg();

private:
	static unsigned int __stdcall OnExitThread(void* lpUesrParam);
	static unsigned int __stdcall OnWorkThread(void* lpUesrParam);

private:
	void Termination();
	bool IsTermination();

protected:
	string m_strDebugDir;

protected:
	uint8 m_uWorkType;
	CCondition* m_lpWork;
	function<void()> m_fWork;

private:
	bool m_bThread;
	bool m_bTermination;
	CSRWLockBase m_lLock;
	CCondition m_cTermination;
};