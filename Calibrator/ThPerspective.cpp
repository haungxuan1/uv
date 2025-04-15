#include "pch.h"
#include "ThPerspective.h"
#include "Transform.h"
#include "Helper.h"
#include "opencv2/ximgproc.hpp"
#pragma comment(lib, "opencv_ximgproc470.lib")

CThPerspective::CThPerspective()
{

}

CThPerspective::~CThPerspective()
{

}

bool CThPerspective::Reg()
{
	return CThSlope::Reg();
}

void CThPerspective::Unreg()
{
	CThSlope::Unreg();
}

void CThPerspective::SetPerspectiveParam(array<float, 2> vImWH1, array<Point2f, 4> vVertex)
{
	m_vImWH1 = vImWH1; 
	
	m_vVertex1 = vVertex;

	PerspectiveTransform();
}

void CThPerspective::Translate(Point2f& ptTL, Point2f& ptTR, Point2f& ptBR, Point2f& ptBL)
{
	float x = min(min(min(ptTL.x, ptTR.x), ptBR.x), ptBL.x);
	float y = min(min(min(ptTL.y, ptTR.y), ptBR.y), ptBL.y);

	ptTL.x -= x; ptTL.y -= y;
	ptTR.x -= x; ptTR.y -= y;
	ptBR.x -= x; ptBR.y -= y;
	ptBL.x -= x; ptBL.y -= y;// 平移
}

void CThPerspective::Reverse(Point2f& ptTL, Point2f& ptTR, Point2f& ptBR, Point2f& ptBL)
{
	float h = max(max(max(ptTL.y, ptTR.y), ptBR.y), ptBL.y);

	ptTL.y = h - ptTL.y;
	ptTR.y = h - ptTR.y;
	ptBR.y = h - ptBR.y;
	ptBL.y = h - ptBL.y; // 图像坐标系 翻转
}

void CThPerspective::k(Point2f& ptTL, Point2f& ptTR, Point2f& ptBR, Point2f& ptBL, float& dTBL, float& dTBR, float& dLRT, float& dLRB)
{
	dTBL = (ptTL.y - ptBL.y) / (ptTL.x - ptBL.x);
	dTBR = (ptTR.y - ptBR.y) / (ptTR.x - ptBR.x);
	dLRT = (ptTL.y - ptTR.y) / (ptTL.x - ptTR.x);
	dLRB = (ptBL.y - ptBR.y) / (ptBL.x - ptBR.x);
}

void CThPerspective::b(
	float& w, float& h,
	float& dTBLk, float& dTBRk, float& dLRTk, float& dLRBk,
	float& dTBLb, float& dTBRb, float& dLRTb, float& dLRBb)
{
	dTBLb = (dTBLk < 0 ? 0 : h);
	dTBRb = (dTBRk < 0 ? (h - dTBRk * w) : (-dTBRk * w));
	dLRTb = (dLRTk < 0 ? (h - dLRTk * w) : h);
	dLRBb = (dLRBk < 0 ? 0 : (-dLRBk * w));
}

void CThPerspective::v(
	float& dTBLk, float& dTBRk, float& dLRTk, float& dLRBk,
	float& dTBLb, float& dTBRb, float& dLRTb, float& dLRBb,
	Point2f& ptTL, Point2f& ptTR, Point2f& ptBR, Point2f& ptBL)
{
	auto p = [](float k1, float b1, float k2, float b2)->Point2f
	{
		float x = (b2 - b1) / (k1 - k2);
		float y = k1 * x + b1;
		return { x, y };
	};
	ptTL = p(dTBLk, dTBLb, dLRTk, dLRTb);

	ptTR = p(dLRTk, dLRTb, dTBRk, dTBRb);
	
	ptBR = p(dTBRk, dTBRb, dLRBk, dLRBb);

	ptBL = p(dLRBk, dLRBb, dTBLk, dTBLb);
}

void CThPerspective::r(Point2f& ptTL, Point2f& ptTR, Point2f& ptBR, Point2f& ptBL, Rect2f& rtCircumscribed)
{
	float t = min(min(min(ptTL.y, ptTR.y), ptBR.y), ptBL.y);
	float b = max(max(max(ptTL.y, ptTR.y), ptBR.y), ptBL.y);
	float l = min(min(min(ptTL.x, ptTR.x), ptBR.x), ptBL.x);
	float r = max(max(max(ptTL.x, ptTR.x), ptBR.x), ptBL.x);
	rtCircumscribed.x = l, rtCircumscribed.y = t;
	rtCircumscribed.width = r - l, rtCircumscribed.height = b - t;
}

void CThPerspective::f(float& w, float& h, Rect2f& rtCircumscribed, array<int, 4>& vFill)
{
	vFill[0] = 0, vFill[1] = 0;

	vFill[2] = ceil(rtCircumscribed.width - w);

	vFill[3] = ceil(rtCircumscribed.height - h);
}

void CThPerspective::wh(Point2f& ptTL, Point2f& ptTR, Point2f& ptBR, Point2f& ptBL, float& w, float& h)
{
	float w1 = sqrt(pow(ptTL.x - ptTR.x, 2) + pow(ptTL.y - ptTR.y, 2));
	float w2 = sqrt(pow(ptBL.x - ptBR.x, 2) + pow(ptBL.y - ptBR.y, 2));
	float h1 = sqrt(pow(ptTL.x - ptBL.x, 2) + pow(ptTL.y - ptBL.y, 2));
	float h2 = sqrt(pow(ptTR.x - ptBR.x, 2) + pow(ptTR.y - ptBR.y, 2));
	w = (w1 + w2) * 0.5; h = (h1 + h2) * 0.5;
}

void CThPerspective::v(float& w, float& h, Point2f& ptTL, Point2f& ptTR, Point2f& ptBR, Point2f& ptBL)
{
	ptTL.x = 0, ptTL.y = h;
	ptTR.x = w, ptTR.y = h;
	ptBR.x = w, ptBR.y = 0;
	ptBL.x = 0, ptBL.y = 0;
}

void CThPerspective::v(
	array<int, 4>& vFill,
	float& w, float& h,
	float& w2, float& h2,
	Point2f& ptTL, Point2f& ptTR, Point2f& ptBR, Point2f& ptBL,
	Point2f& ptTL2, Point2f& ptTR2, Point2f& ptBR2, Point2f& ptBL2)
{
	float dTBLk, dTBRk, dLRTk, dLRBk;

	float dTBLb, dTBRb, dLRTb, dLRBb;

	Translate(ptTL, ptTR, ptBR, ptBL);

	Reverse(ptTL, ptTR, ptBR, ptBL); // 图像坐标系转卡迪尔坐标系

	k(ptTL, ptTR, ptBR, ptBL, dTBLk, dTBRk, dLRTk, dLRBk);

	b(w, h, dTBLk, dTBRk, dLRTk, dLRBk, dTBLb, dTBRb, dLRTb, dLRBb);

	v(dTBLk, dTBRk, dLRTk, dLRBk, dTBLb, dTBRb, dLRTb, dLRBb, ptTL, ptTR, ptBR, ptBL);

	Reverse(ptTL, ptTR, ptBR, ptBL); // 卡迪尔坐标系转图像坐标系

	Rect2f rtCircumscribed;

	r(ptTL, ptTR, ptBR, ptBL, rtCircumscribed);

	f(w, h, rtCircumscribed, vFill);

	wh(ptTL, ptTR, ptBR, ptBL, w2, h2);

	v(w2, h2, ptTL2, ptTR2, ptBR2, ptBL2);

	Reverse(ptTL2, ptTR2, ptBR2, ptBL2); // 卡迪尔坐标系转图像坐标系
}

void CThPerspective::PerspectiveTransform()
{
	auto& w = m_vImWH1[0], &h = m_vImWH1[1], & w2 = m_vImWH2[0], & h2 = m_vImWH2[1];
	
	v(m_vFill, w, h, w2, h2, m_vVertex1[0], m_vVertex1[1], m_vVertex1[2], m_vVertex1[3], m_vVertex2[0], m_vVertex2[1], m_vVertex2[2], m_vVertex2[3]);

	cout << "fill: " << m_vFill[0] << ", " << m_vFill[1] << ", " << m_vFill[2] << ", " << m_vFill[3] << endl;

	cout << "in: " << w << " * " << h << " | " << "out: " << w2 << " * " << h2 << endl;

	cout << "v1: " << m_vVertex1[0] << " | " << m_vVertex1[1] << " | " << m_vVertex1[2] << " | " << m_vVertex1[3] << endl;

	cout << "v2: " << m_vVertex2[0] << " | " << m_vVertex2[1] << " | " << m_vVertex2[2] << " | " << m_vVertex2[3] << endl;

	Mat M = getPerspectiveTransform(m_vVertex1, m_vVertex2);

	memcpy(m_vM.data(), M.data, sizeof(double) * m_vM.size());

	OnPerspective(m_vM, m_vFill, m_vImWH2, m_vVertex2);
}
