#pragma once  // 防止头文件重复包含
#include "afxdialogex.h"  // MFC对话框扩展类头文件

// 屏幕截图对话框类
class CScreenShotDlg : public CDialogEx  // 继承自MFC对话框基类
{
	DECLARE_DYNAMIC(CScreenShotDlg)  // MFC动态创建宏

public:
	// 构造函数
	// ptParent: 父窗口指针
	// tWinRect: 窗口矩形区域引用
	CScreenShotDlg(CWnd* ptParent, CRect &tWinRect);   
	virtual ~CScreenShotDlg();  // 虚析构函数

	CRect getCaptureRect();  // 获取截图区域矩形

	// 对话框资源ID定义
#ifdef AFX_DESIGN_TIME  // 设计时资源ID
	enum { IDD = IDD_CScreenShotDlg };
#endif

protected:
	// 数据交换方法 (DDX/DDV支持)
	virtual void DoDataExchange(CDataExchange* pDX);    

protected:
	virtual BOOL OnInitDialog();  // 对话框初始化方法
	DECLARE_MESSAGE_MAP()  // MFC消息映射宏

private:
	// 鼠标移动消息处理
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	// 左键按下消息处理  
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	// 左键释放消息处理
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	// 右键双击消息处理
	afx_msg void OnRButtonDblClk(UINT nFlags, CPoint point);
	// 右键释放消息处理
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);

private:
	bool m_bMousePress = false;  // 鼠标按下状态标志
	CPoint m_tBeginPoint;  // 鼠标起始点坐标
	CPoint m_tEndPoint;    // 鼠标结束点坐标
	CRect m_tWinRect;      // 窗口矩形区域
	CDC m_tMemDC;          // 内存设备上下文
	CBitmap m_tBitmap;     // 位图对象
	CBitmap* m_ptOldBitmap = nullptr;  // 旧位图指针(用于恢复)

	void drawPaint();  // 绘制截图区域方法
};
