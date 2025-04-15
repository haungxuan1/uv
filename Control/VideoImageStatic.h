// 确保头文件只被编译一次，防止重复包含
#pragma once
// 包含MFC的Windows界面相关头文件
#include <afxwin.h>
// 包含OpenCV库的头文件
#include "opencv.hpp"

/**
 * @brief 用于显示视频图像的静态控件类
 * 
 * 该类继承自CStatic，提供了设置图像、背景颜色、对比度等功能，
 * 同时支持鼠标交互操作，如选择区域等。
 */
class VideoImageStatic : public CStatic
{
    // 声明动态创建类所需的宏
    DECLARE_DYNAMIC(VideoImageStatic)

public:
    /**
     * @brief 构造函数
     */
    VideoImageStatic();
    /**
     * @brief 析构函数
     */
    ~VideoImageStatic();

    /**
     * @brief 设置要显示的图像
     * 
     * @param tImgMat 要显示的OpenCV Mat对象
     * @return bool 设置是否成功
     */
    bool setImage(Mat& tImgMat);
    /**
     * @brief 设置控件的背景颜色
     * 
     * @param uClr 背景颜色的COLORREF值
     */
    void setBgColor(COLORREF uClr);
    /**
     * @brief 设置图像的对比度
     * 
     * @param fContrastRatio 对比度比例值
     */
    void setContrastRatio(float fContrastRatio);
    /**
     * @brief 设置是否启用绘制功能
     * 
     * @param bDrawEnable 是否启用绘制功能的布尔值
     */
    void setDrawEnable(bool bDrawEnable);
    /**
     * @brief 获取用户选择的矩形区域
     * 
     * @return cv::Rect 用户选择的矩形区域
     */
    cv::Rect getSelectRect();
	//添加处理放大缩小操作，并以当前鼠标位置为中心进行进行局部显示
	void zoomout(double scaleFactor, CPoint center);
    cv::Mat getCurrentImage(); // 添加 getCurrentImage 函数声明
    int getWidth(); // 添加 getWidth 函数声明
    int getHeight(); // 添加 getHeight 函数声明
    void displayImage(cv::Mat image); // 添加 displayImage 函数声明
private:
//HBITMAP是Windows API中用于表示位图的一种数据类型，它是一个指向位图数据的指针。
    HBITMAP matToBitmap(cv::Mat image); // 添加 matToBitmap 函数声明

    /**
     * @brief 处理控件创建消息
     * 
     * @param ptCreateStruct 创建结构体指针
     * @return int 处理结果
     */
    afx_msg int OnCreate(LPCREATESTRUCT ptCreateStruct);
    /**
     * @brief 处理控件重绘消息
     */
    afx_msg void OnPaint();
    /**
     * @brief 处理控件销毁消息
     */
    afx_msg void OnDestroy();
    /**
     * @brief 处理鼠标移动消息
     * 
     * @param uFlags 鼠标标志位
     * @param tPoint 鼠标当前位置
     */
    afx_msg void OnMouseMove(UINT uFlags, CPoint tPoint);
    /**
     * @brief 处理鼠标左键按下消息
     * 
     * @param uFlags 鼠标标志位
     * @param tPoint 鼠标当前位置
     */
    afx_msg void OnLButtonDown(UINT uFlags, CPoint tPoint);
    /**
     * @brief 处理鼠标左键释放消息
     * 
     * @param uFlags 鼠标标志位
     * @param tPoint 鼠标当前位置
     */
    afx_msg void OnLButtonUp(UINT uFlags, CPoint tPoint);
    /**
     * @brief 处理鼠标右键释放消息
     * 
     * @param uFlags 鼠标标志位
     * @param tPoint 鼠标当前位置
     */
    afx_msg void OnRButtonUp(UINT uFlags, CPoint tPoint);

    // 声明消息映射宏
    DECLARE_MESSAGE_MAP()

private:
    // 控件的矩形区域
    CRect m_tRect;
    // 内存设备上下文
    CDC	  m_tMemDC;
    // 画刷对象，用于绘制背景
    CBrush m_tBrush;
    // 位图对象，作为绘制的画布
    CBitmap m_tBmpCanvas;

    // 图像缩放比例
    float m_fScale;
    // 图像对比度比例
    float m_fContrastRatio;
    // Mat对象的X偏移量
    int m_iMatX;
    // Mat对象的Y偏移量
    int m_iMatY;
    // 要显示的OpenCV Mat对象
    Mat m_tDispMat; 

    // 是否启用绘制功能
    bool m_bDrawEnable;
    // 鼠标是否按下
    bool m_bMousePress;
    // 鼠标按下时的起始位置
    CPoint m_tPointBegin;
    // 鼠标释放时的结束位置
    CPoint m_tPointEnd;

    /**
     * @brief 重新绘制控件内容
     */
    void reDraw();
};

