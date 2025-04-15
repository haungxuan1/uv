#pragma once  // 防止头文件重复包含
#include <afxwin.h>  // MFC基础头文件

// 自定义样式对话框基类
class StyleDialog : public CDialog
{
    DECLARE_DYNAMIC(StyleDialog)  // MFC运行时类型识别支持

public:
    StyleDialog(UINT uTemplateId, CWnd* ptParent = nullptr);  // 构造函数
    ~StyleDialog();  // 析构函数

    // 设置主对话框标志
    void setMainDialogFlag(bool bIsMainDlg);

protected:
    // 重写基类方法
    virtual void DoDataExchange(CDataExchange* ptDX);  // 数据交换
    virtual BOOL OnInitDialog();  // 对话框初始化
    virtual BOOL PreTranslateMessage(MSG* ptMsg);  // 消息预处理
    virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);  // 默认窗口过程

private:
    // 消息处理函数
    afx_msg void OnNcLButtonDown(UINT nHitTest, CPoint point);  // 非客户区左键按下
    afx_msg void OnNcMouseMove(UINT uHitTest, CPoint tPoint);    // 非客户区鼠标移动
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);  // 背景擦除
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);  // 窗口创建
    afx_msg HBRUSH OnCtlColor(CDC* ptDC, CWnd* ptWnd, UINT uCtlColor);  // 控件颜色设置


    //新添加的消息处理函数
    afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);

    
    DECLARE_MESSAGE_MAP()  // 声明消息映射表

private:
    // 成员变量
    bool m_bIsMainDlg = false;  // 是否为主对话框(控制标题栏样式)
    CRect m_tRectIcon;         // 标题栏图标区域
    CRect m_tRectBtnMax;       // 最大化按钮区域
    CRect m_tRectBtnMin;       // 最小化按钮区域
    CRect m_tRectBtnExit;      // 关闭按钮区域

    // 绘制自定义标题栏
    void drawTitleBar(CDC* pDC);
};

