
// VisionLocalization.h: PROJECT_NAME 应用程序的主头文件
//

#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含 'pch.h' 以生成 PCH"
#endif

#include "resource.h"		// 主符号
#include <gdiplus.h>

// CVisionLocalizationApp:
// 有关此类的实现，请参阅 VisionLocalization.cpp
//

// 定义一个名为 CVisionLocalizationApp 的类，继承自 CWinApp
class CVisionLocalizationApp : public CWinApp
{
public:
    // 类的构造函数，用于初始化对象
    CVisionLocalizationApp();

private:
//windows提供的api，GDI+是一个用于在Windows平台上进行图形绘制和图像处理的库，它提供了丰富的功能和接口，用于创建、操作和处理图像。
    // 用于存储 GDI+ 库的版本信息，该信息用于标识 GDI+ 库的版本
    // 用于存储 GDI+ 库的令牌，该令牌用于标识当前应用程序的 GDI+ 实例
    ULONG_PTR m_lpGdiplusToken;
    // 包含 GDI+ 启动所需的参数
    Gdiplus::GdiplusStartupInput m_tGdiplusStartupInput;

// 重写父类的虚函数
public:
    // 重写 InitInstance 函数，用于初始化应用程序实例
    virtual BOOL InitInstance();

// 类的实现部分
    // 声明消息映射，用于处理 Windows 消息
    DECLARE_MESSAGE_MAP()
    // 重写 ExitInstance 函数，用于在应用程序退出时执行清理操作
    virtual int ExitInstance();
};

// 声明一个全局的 CVisionLocalizationApp 对象，名为 theApp
extern CVisionLocalizationApp theApp;
