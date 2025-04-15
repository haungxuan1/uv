#pragma once  // 确保头文件只被编译一次，防止重复包含
#include <afxwin.h>  // 包含MFC的Windows界面编程相关头文件
#include "opencv2/opencv.hpp"  // 包含OpenCV库的头文件

// 定义鼠标模式的枚举类型
typedef enum {
	E_MM_None = 0,  // 无操作模式
	E_MM_Move,  // 移动模式
	E_MM_Size,  // 调整大小模式
	E_MM_ROTATE  // 旋转模式
}E_MouseMode;

// 定义MakeTemplateView类，继承自CScrollView
class MakeTemplateView :public CScrollView
{
public:
	MakeTemplateView() noexcept;  // 构造函数，声明为noexcept表示不会抛出异常
	~MakeTemplateView();  // 析构函数
	DECLARE_DYNCREATE(MakeTemplateView)  // 声明动态创建类的宏

protected:
	virtual void OnDraw(CDC* pDC);  // 绘制视图内容的虚函数
	// 准备设备上下文的虚函数，用于打印或显示前的准备工作，重写父类方法
	virtual void OnPrepareDC(CDC* pDC, CPrintInfo* pInfo = NULL) override;

protected:
	// 鼠标激活消息处理函数
	afx_msg int OnMouseActivate(CWnd* ptWnd, UINT uHitTest, UINT uMsg);
	// 鼠标左键按下消息处理函数
	afx_msg void OnLButtonDown(UINT uFlags, CPoint tPoint);
	// 鼠标移动消息处理函数
	afx_msg void OnMouseMove(UINT uFlags, CPoint tPoint);
	// 鼠标左键释放消息处理函数
	afx_msg void OnLButtonUp(UINT uFlags, CPoint tPoint);
	// 鼠标右键释放消息处理函数
	afx_msg void OnRButtonUp(UINT uFlags, CPoint tPoint);
	// 鼠标滚轮消息处理函数
	afx_msg BOOL OnMouseWheel(UINT uFlags, short sDelta, CPoint tPoint);
	// 水平滚动条消息处理函数
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	// 垂直滚动条消息处理函数
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	DECLARE_MESSAGE_MAP()  // 声明消息映射宏

private:
	CDC	m_tMemDC;				// 内存设备上下文，用于内存缓存
	CDC m_tProductDC;			// 产品相关的内存设备上下文
	CDC	m_tTemplateDC;			// 模板相关的内存设备上下文

	CBitmap m_ptBmpBg;  // 背景位图
	CBitmap m_ptBmpProduct;  // 产品位图
	CBitmap m_ptBmpTemplate;  // 模板位图
	CBitmap* m_ptOldBmpBg = nullptr;  // 旧的背景位图指针，初始化为空
	CBitmap* m_ptOldBmpProduct = nullptr;  // 旧的产品位图指针，初始化为空
	CBitmap* m_ptOldBmpTemplate = nullptr;  // 旧的模板位图指针，初始化为空

	CRect m_tOriginRect;					// 图像原始矩形区域
	CRect m_tScrollRect;					// 模板滚动矩形区域
	CRect m_tMemDCRect;						// 内存设备上下文矩形区域
	int m_iScrollX = 0;  // 水平滚动位置
	int m_iScrollY = 0;  // 垂直滚动位置

	CPoint m_tPointBegin;  // 鼠标操作起始点
	CPoint m_tPointEnd;  // 鼠标操作结束点

private:
	void resetTemplate();  // 重置模板相关设置
	void resetProduct();  // 重置产品相关设置
	void reset();  // 重置所有设置
	void setScrollbar();  // 设置滚动条

private:
	bool m_bMousePressed = false;  // 鼠标是否按下的标志
	Mat m_tTemplateMatBGRA;  // 模板的BGRA格式OpenCV矩阵
	Mat m_tProductMatBGRA;  // 产品的BGRA格式OpenCV矩阵

	CSize m_tTemplateSize;						// 模板大小
	CSize m_tProductSize;						// 产品大小
	float m_fScale		   = 0.25f;				// 缩放比例
	float m_fContrastRatio = 1.0f;				// 模板对比度
	float m_fProductScale  = 1.0f;				// 产品图像缩放比例
	float m_fAngle		   = 0.0f;				// 产品图像旋转角度
	int m_iAlpha		   = 128;				// 产品图像透明度
	int m_iProductX		   = 0;					// 产品图像X偏移量
	int m_iProductY		   = 0;					// 产品图像Y偏移量

public:
	// 初始化函数，传入模板矩阵、产品矩阵和缩放比例
	void init(Mat& tTemplateMat, Mat& tProductMat, float fScale);

	void setScale(float fScale);  // 设置缩放比例
	float getScale();  // 获取缩放比例

	void addProductScale(float fOffset);  // 增加产品图像缩放比例
	float getProductScale();  // 获取产品图像缩放比例

	void addAngle(float fDelta);  // 增加产品图像旋转角度
	float getAngle();  // 获取产品图像旋转角度

	void setOffsetX(float fValue);  // 设置产品图像X偏移量
	void setOffsetY(float fValue);  // 设置产品图像Y偏移量

	void setAlpha(int iAlpha);  // 设置产品图像透明度
	void setContrastRatio(float fContrastRatio);  // 设置模板对比度
	void resetAll();							// 重置所有设置
	// 自动设置产品位置，传入旋转角度、中心点X和中心点Y
	void autoProductPosition(float fAngle, int iCenterX, int iCenterY);

	cv::Rect getTemplateSelectRect();  // 获取模板选择矩形区域
	cv::Rect getProductSelectRect();  // 获取产品选择矩形区域
};