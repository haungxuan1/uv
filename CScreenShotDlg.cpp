// 屏幕截图对话框实现文件

// 包含必要的头文件
#include "pch.h"  // 预编译头文件
#include "VisionLocalization.h"  // 主程序头文件
#include "afxdialogex.h"  // MFC对话框扩展类
#include "CScreenShotDlg.h"  // 当前对话框头文件

// 实现动态创建宏
IMPLEMENT_DYNAMIC(CScreenShotDlg, CDialogEx)

// 构造函数
// ptParent: 父窗口指针
// tWinRect: 窗口矩形区域引用
CScreenShotDlg::CScreenShotDlg(CWnd* ptParent, CRect& tWinRect)
    : CDialogEx(IDD_CScreenShotDlg, ptParent)
{
    m_tWinRect = tWinRect;  // 初始化窗口矩形
}

// 析构函数
CScreenShotDlg::~CScreenShotDlg()
{
    // 清理内存DC资源
    if (m_ptOldBitmap != nullptr) {
        m_tMemDC.SelectObject(m_ptOldBitmap);
        m_tMemDC.DeleteDC();
    }
}

// 数据交换方法
void CScreenShotDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);  // 调用基类方法
}

// 消息映射表
BEGIN_MESSAGE_MAP(CScreenShotDlg, CDialogEx)
    ON_WM_PAINT()  // 绘制消息
    ON_WM_MOUSEMOVE()  // 鼠标移动消息
    ON_WM_LBUTTONDOWN()  // 左键按下消息
    ON_WM_LBUTTONUP()  // 左键释放消息
    ON_WM_RBUTTONDBLCLK()  // 右键双击消息
    ON_WM_RBUTTONUP()  // 右键释放消息
END_MESSAGE_MAP()

// 对话框初始化
BOOL CScreenShotDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();  // 调用基类初始化

    // 设置窗口位置和大小
    MoveWindow(&m_tWinRect);

    // 修改窗口样式：无标题栏和边框
    LONG style = GetWindowLong(GetSafeHwnd(), GWL_STYLE);
    style &= ~(WS_CAPTION | WS_THICKFRAME);
    SetWindowLong(GetSafeHwnd(), GWL_STYLE, style);
    SetWindowPos(&wndTopMost, 0, 0, m_tWinRect.Width(), m_tWinRect.Height(), 
                SWP_NOACTIVATE | SWP_NOREDRAW | SWP_NOMOVE);

    // 设置窗口透明属性
    style |= WS_EX_LAYERED;
    SetWindowLong(m_hWnd, GWL_EXSTYLE, style);
    SetLayeredWindowAttributes(RGB(0, 0, 0), 100, LWA_COLORKEY | LWA_ALPHA);

    // 创建兼容内存DC
    CDC* ptDC = GetDC();
    if (ptDC != nullptr) {
        m_tMemDC.CreateCompatibleDC(ptDC);
        m_tBitmap.CreateCompatibleBitmap(ptDC, m_tWinRect.Width(), m_tWinRect.Height());
        m_ptOldBitmap = m_tMemDC.SelectObject(&m_tBitmap);
        ReleaseDC(ptDC);
    }
    return TRUE;
}

// 绘制截图区域
void CScreenShotDlg::drawPaint()
{
    CDC* ptDC = GetDC();
    if (ptDC != nullptr) {
        // 准备绘制工具
        CRect tRect;
        CBrush tBrush(RGB(255, 255, 255));  // 白色背景刷
        GetClientRect(&tRect);
        m_tMemDC.FillRect(&tRect, &tBrush);

        // 设置透明画刷和红色边框
        tBrush.CreateStockObject(NULL_BRUSH);
        CBrush* ptOldBrush = m_tMemDC.SelectObject(&tBrush);
        CPen tPen(PS_SOLID, 2, RGB(200, 0, 0));
        CPen* ptOldPen = m_tMemDC.SelectObject(&tPen);

        // 绘制截图矩形
        CRect tCaptureRect = getCaptureRect();
        m_tMemDC.Rectangle(&tCaptureRect);

        // 恢复原有工具并输出到屏幕
        m_tMemDC.SelectObject(ptOldPen);
        m_tMemDC.SelectObject(ptOldBrush);
        ptDC->BitBlt(0, 0, tRect.Width(), tRect.Height(), &m_tMemDC, 0, 0, SRCCOPY);
        ReleaseDC(ptDC);
    }
}

// 鼠标移动处理
void CScreenShotDlg::OnMouseMove(UINT nFlags, CPoint point)
{
    if (m_bMousePress) {
        m_tEndPoint = point;  // 更新终点坐标
        drawPaint();  // 重绘截图区域
    }
}

// 左键按下处理
void CScreenShotDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
    m_bMousePress = true;  // 设置按下标志
    m_tBeginPoint = point;  // 记录起点
    m_tEndPoint = point;  // 初始化终点
}

// 左键释放处理
void CScreenShotDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
    if (m_bMousePress) {
        m_bMousePress = false;  // 清除按下标志
        m_tEndPoint = point;  // 更新终点坐标
    }
}

// 获取截图区域
CRect CScreenShotDlg::getCaptureRect()
{
    return CRect(m_tBeginPoint, m_tEndPoint);  // 返回起点和终点定义的矩形
}

// 右键双击处理
void CScreenShotDlg::OnRButtonDblClk(UINT nFlags, CPoint point)
{
    EndDialog(IDCANCEL);  // 取消截图
}

// 右键释放处理
void CScreenShotDlg::OnRButtonUp(UINT nFlags, CPoint point)
{
    EndDialog(IDOK);  // 确认截图
}
