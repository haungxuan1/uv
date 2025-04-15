// 确保头文件只被编译一次，防止重复包含
#pragma once
// 包含标准库中的字符串头文件
#include <string>
// 包含OpenCV库的头文件
#include "opencv.hpp"

// 定义一个名为PQSetting的命名空间
namespace PQSetting
{
    // 声明一个函数Reg，用于注册相关设置
    void Reg();

    // 声明一个函数Unreg，用于取消注册相关设置
    void Unreg();

    // 声明一个函数ExeDir，返回一个常量引用的宽字符串，表示可执行文件的目录
    const std::wstring &ExeDir();

    // 声明一个函数MoldDir，返回一个常量引用的宽字符串，表示模具文件的目录
    const std::wstring &MoldDir();

    // 声明一个函数Tif2Mat，用于将TIFF文件转换为OpenCV的Mat对象
    // 参数szTifFile是一个宽字符串，表示TIFF文件的路径
    // 参数vTif是一个引用类型的cv::Mat对象，用于存储转换后的图像数据
    // 函数返回一个布尔值，表示转换是否成功
    bool Tif2Mat(std::wstring szTifFile, cv::Mat& vTif);
}
