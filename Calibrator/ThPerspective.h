#pragma once
#include "ThSlope.h"
#include <map>

// 该类移除线程操作
class CThPerspective : public CThSlope
{
protected:
	CThPerspective();
	virtual ~CThPerspective();

protected:
	virtual bool Reg();
	virtual void Unreg();

protected:
	virtual void OnPerspective(
		array<double, 9> vM,
		array<int, 4> vFill,
		array<float, 2> vImWH2,
		array<Point2f, 4> vVertex2) = 0;

protected:
	void SetPerspectiveParam(array<float, 2> vImWH1, array<Point2f, 4> vVertex);

private:
	void Translate(Point2f& ptTL, Point2f& ptTR, Point2f& ptBR, Point2f& ptBL);
	void Reverse(Point2f& ptTL, Point2f& ptTR, Point2f& ptBR, Point2f& ptBL);
	void k(Point2f& ptTL, Point2f& ptTR, Point2f& ptBR, Point2f& ptBL, float& dTBL, float& dTBR, float& dLRT, float& dLRB);
	void b(
		float& w, float& h,
		float& dTBLk, float& dTBRk, float& dLRTk, float& dLRBk,
		float& dTBLb, float& dTBRb, float& dLRTb, float& dLRBb);
	void v(
		float& dTBLk, float& dTBRk, float& dLRTk, float& dLRBk,
		float& dTBLb, float& dTBRb, float& dLRTb, float& dLRBb,
		Point2f& ptTL, Point2f& ptTR, Point2f& ptBR, Point2f& ptBL);
	void r(Point2f& ptTL, Point2f& ptTR, Point2f& ptBR, Point2f& ptBL, Rect2f& rtCircumscribed);
	void f(float& w, float& h, Rect2f& rtCircumscribed, array<int, 4>& vFill);
	void wh(Point2f& ptTL, Point2f& ptTR, Point2f& ptBR, Point2f& ptBL, float& w, float& h);
	void v(float& w, float& h, Point2f& ptTL, Point2f& ptTR, Point2f& ptBR, Point2f& ptBL);
	void v(
		array<int, 4>& vFill,
		float& w, float& h,
		float& w2, float& h2,
		Point2f& ptTL, Point2f& ptTR, Point2f& ptBR, Point2f& ptBL,
		Point2f& ptTL2, Point2f& ptTR2, Point2f& ptBR2, Point2f& ptBL2);

private:
	void PerspectiveTransform();

private:
	array<int, 4> m_vFill;// 4边填充系数
	array<double, 9> m_vM; // 变换矩阵M
	array<float, 2> m_vImWH1; // 变换前
	array<float, 2> m_vImWH2; // 变换后
	array<Point2f, 4> m_vVertex1; // 倾斜4顶点
	array<Point2f, 4> m_vVertex2; // 纠正后4顶点
};
