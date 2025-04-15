#pragma once
#include "ImTransparent.h"

/*
*	��ͷy�������īͷ�ľ���Ϊ�����Ұ
* 
*	ƴͼ��Ϊ���£�
*		����֡ͼ���������ж���
*		ʶ�����󷢳� if ƴ֡������Ұ
*		��ǰͼ����ͼ��ײ�����
*		����ͼ������
*		�������滻
*/
/*
* Transparent��PositionΪ���й�ϵ�����˴���˳��̳��滻���μ̳�
*
*/
class CImageMosaic : public CImTransparent 
{
protected:
	CImageMosaic();
	virtual ~CImageMosaic();

protected:
	virtual bool Reg();
	virtual void Unreg();
	virtual void AllocateMmemory();
	virtual void DeallocateMemory();

private:
	virtual void FleeceParam(int& nOy) = 0;

protected:
	void ImMosaic(vParam& iParam);

private:
	void OnImMosaic(vParam& iParam);

private:
	bool InitSign(ImPtr::CImPtr& cImLine);
	void ImSight(ImPtr::CImPtr& cTag, int& nLeft, array<int,2>& vImSize);

	void SendSight(vParam& iParam);
	void SightMove(int& nFrame);
	void ImAlignment(int& nFrame, ImPtr::CImPtr& cImLine);
	void ImMerging(int& nFrame, ImPtr::CImPtr& cImLine);
	bool OutSight(ImPtr::CImPtr& cImLine);

private:
	void Tag(ImPtr::CImPtr& cTag, ImPtr::CImPtr& cWin, Rect& rtWin);
	void MatchTemplate(ImPtr::CImPtr& cTag, int& nTemp, double& dScore, Point& ptTL, ImPtr::CImPtr& cTemp);
	void MatchTemplate(ImPtr::CImPtr& cTag, string strTemp, double& dScore, Point& ptTL, Rect& rtTemp);

private: // �������
	bool m_bFinish;
	Point m_ptTL;
	int m_nTempDiameter;
	array<int64, 4> m_nImMosaic; // 0 = ��ӡ�� | 1 = ƴ��.�� | 2 = ��ǰƴ��.�� | 3 = ����.��
	int m_nRerverse;
	Mat m_cImSign; // �������ʶ����
	Mat m_cImSight; // ������Ұ n*frame
	Mat m_cImCache;

private:
	vThParam m_vThParam;
	vParams m_vParams;
	bool m_bTermination;
	bool m_bThread;
	CSRWLockBase m_lLock;
	CCondition m_cImLine;
	CCondition m_cTermination;
};
