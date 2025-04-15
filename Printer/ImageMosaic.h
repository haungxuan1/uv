#pragma once
#include "ImTransparent.h"

/*
*	镜头y方向距离墨头的距离为相机视野
* 
*	拼图行为如下：
*		与上帧图像参照物进行对齐
*		识别请求发出 if 拼帧超出视野
*		当前图像由图像底部加入
*		整体图像上移
*		参照物替换
*/
/*
* Transparent与Position为并列关系，但此处以顺序继承替换菱形继承
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

private: // 缓存变量
	bool m_bFinish;
	Point m_ptTL;
	int m_nTempDiameter;
	array<int64, 4> m_nImMosaic; // 0 = 打印高 | 1 = 拼接.高 | 2 = 当前拼接.高 | 3 = 上移.高
	int m_nRerverse;
	Mat m_cImSign; // 参照物标识区域
	Mat m_cImSight; // 发送视野 n*frame
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
