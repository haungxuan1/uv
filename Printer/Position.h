#pragma once
#include "PrtMatrix.h"
#include <string>
#include <vector>
#include "tiffio.h"

using namespace cv;

class CPosition : public CPrtMatrix
{
public:
	Mat getTargetImate() { return m_iTargetImage; }
	enum { eMI_PrPtr = 0, eMI_PrPos, eMI_PrScore }; // match idx 

public:
	typedef vector<tuple<Mat, Rect, double, double>> vMatcher;

protected:
	CPosition();
	virtual ~CPosition();

protected:
	virtual bool Reg();
	virtual void Unreg();
	virtual void AllocateMmemory();
	virtual void DeallocateMemory();

protected:
	void Position(vParam iParam);

public: // ��ʱ�����ӿڣ�����ͼ����(��ǰdll������ͼʶ�����뵥ͼ�����ͬ)
	void OnPosition(vParam iParam);

private:
	TIFF* m_lpMold;

private:
	vThParam m_vThParam;
	vParams m_vParams;
	bool m_bTermination;
	bool m_bThread;
	CSRWLockBase m_lLock;
	CCondition m_cImView;
	CCondition m_cTermination;
	Mat m_iTargetImage;
};
