#pragma once  // 确保头文件只被编译一次，防止重复包含
#include "ImRender.h"  // 包含自定义的 ImRender 头文件
#include <functional>  // 包含标准库中的 functional 头文件，提供函数对象等功能
#include <map>  // 包含标准库中的 map 头文件，提供关联容器功能

// 声明 ImView2 命名空间，其中包含 CLayouter 和 CImView 类
namespace ImView2 { class CLayouter; class CImView; }

// 定义 ImImage 命名空间
namespace ImImage
{
	// 定义 CImImage 类，继承自 ImRender 命名空间下的 CImRender 类
	class CImImage : public ImRender::CImRender
	{
		DECLARE_DYNAMIC(CImImage)  // 声明动态类支持，通常用于 MFC 框架

		// 声明 ImView2 命名空间下的 CLayouter 和 CImView 类为友元类，允许它们访问 CImImage 的私有成员
		friend class ImView2::CLayouter;
		friend class ImView2::CImView;

	private:
		// 定义枚举类型，表示光标类型
		enum { eCT_Move = 0, eCT_Scale, eCT_Rotate, eCT_Select }; // CT = cursor type // 与 ImLayer::tagLayereOT_Move 对应

	protected:
		CImImage();  // 类的默认构造函数
		virtual ~CImImage();  // 类的虚析构函数，确保正确释放派生类对象

	public:
		// 设置图标目录的函数，接受一个宽字符串参数
		void SetIcon(wstring strIconDir);

	protected:
		// 同步参数的函数，接受一个 ImLayer::tagLayer 类型的引用参数
		void OnSyncParam(ImLayer::tagLayer& tParam);

	protected:
		// 获取焦点矩形的函数，返回一个 CRect 对象
		CRect GetFocusRect() { return m_rtFocus; }

	private:
		// 重写的创建扩展函数，接受一个 LPCREATESTRUCT 类型的常量引用参数
		virtual void OnCreateEx(LPCREATESTRUCT& lpCreateStruct);
		// 重写的绘制扩展函数，接受一个 CDC 引用、一个 CRect 引用、一个无符号字符指针引用和一个长整型引用参数
		virtual void OnDrawEx(CDC& dc, const CRect& rtCanvas, unsigned char*& lpPixel, const long& biWidth);

	private:
		// 清除焦点矩形的函数
		void ClearFocusRect();
		// 设置焦点矩形的函数，接受一个 CPoint 引用参数
		void SetFocusRect(CPoint& pt);
		// 绘制焦点矩形的函数
		void DrawFocusRect();

	private:
		// 处理移动过程中的鼠标事件的函数，接受一个 CPoint 参数
		void OnMoving(CPoint point);
		// 处理移动完成后的鼠标事件的函数，接受一个 CPoint 参数
		void OnMoved(CPoint point);
		// 处理缩放过程中的鼠标事件的函数，接受一个 CPoint 参数
		void OnScaling(CPoint point);
		// 处理缩放完成后的鼠标事件的函数，接受一个 CPoint 参数
		void OnScaled(CPoint point);
		// 处理旋转过程中的鼠标事件的函数，接受一个 CPoint 参数
		void OnRotating(CPoint point);
		// 处理旋转完成后的鼠标事件的函数，接受一个 CPoint 参数
		void OnRotated(CPoint point);
		// 获取中心点的函数，返回一个 Point2f 类型的对象
		Point2f GetCenter();
		// 将 CPoint 类型的点转换为 Point2f 类型的点的函数，接受一个 CPoint 参数
		Point2f Pt2Pt(CPoint point);

	private:
		wstring m_strIconDir;  // 存储图标目录的宽字符串成员变量

	private:
		CPoint m_ptLBDown;  // 存储鼠标左键按下时的点的成员变量
		CRect m_rtFocus;  // 存储焦点矩形的成员变量
		long long m_nFocus;  // 存储焦点相关的长整型成员变量

	private:	// 图像显示相关
		Mat m_cImRender;  // 存储图像渲染数据的 Mat 类型成员变量
		int m_nOpt; // 0 = cur | 1 = pre  // 操作选项的整型成员变量
		HCURSOR m_hCursor[4];  // 存储 4 个光标句柄的数组

	private:
		// 预处理消息的虚函数，接受一个 MSG 指针参数，返回一个布尔值
		virtual BOOL PreTranslateMessage(MSG* pMsg);

	private:
		// 处理鼠标左键按下事件的消息处理函数，接受鼠标标志和鼠标位置参数
		afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
		// 处理鼠标左键释放事件的消息处理函数，接受鼠标标志和鼠标位置参数
		afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
		// 处理鼠标移动事件的消息处理函数，接受鼠标标志和鼠标位置参数
		afx_msg void OnMouseMove(UINT nFlags, CPoint point);
		// 处理鼠标滚轮事件的消息处理函数，接受鼠标标志、滚轮滚动值和鼠标位置参数，返回一个布尔值
		afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
		// 处理设置光标的消息处理函数，接受窗口指针、命中测试值和消息类型参数，返回一个布尔值
		afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);

		DECLARE_MESSAGE_MAP()  // 声明消息映射，用于 MFC 框架处理消息
	};
}