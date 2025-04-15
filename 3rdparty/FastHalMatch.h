#if _MSC_VER >= 1600						//VS2015>VS>VS2010, MSVC VER= 10.0 -14.0
#pragma execution_character_set("utf-8")
#endif

#pragma once
#ifndef __FASTHALMATCH_H__
#define __FASTHALMATCH_H__
#include "tiffio.h"
#include "opencv2/opencv.hpp"

/***********************************************************************
 * 功能: 创建模板模型+保存偏移量
 * 参数:
 *      pcTemplateImg:	模板图片文件名
 *      pcModeFile:		生成模板识别model文件
 *      pcTiffFileName: 贴图
 *      fCenterOffsetX: 中心点X偏移坐标
 *      fCenterOffsetY: 中心点X偏移坐标
 * 返回值:
 *      成功: true
 *      失败: false
 ***********************************************************************/
extern "C" __declspec(dllexport) bool PQ_CreateModel(const char* pcTemplateImg, const char* pcNewName, const char* pcTemplateMaskImg, const char* pcTiffFileName, float fCenterOffsetX, float fCenterOffsetY);


/***********************************************************************
 * 功能: 导入(读取)模板到内存
 * 参数:
 *      pcModelName:    生成模板识别model文件
 *      pcTiffName:     贴图tiff文件名
 * 返回值:
 *      成功: true
 *      失败: false
 ***********************************************************************/
extern "C" __declspec(dllexport) bool PQ_MatchBegin(const char* pcModelName);


/***********************************************************************
 * 功能: 释放模板数据+Tiff数据(减少内存占用)
 * 参数:
 *      无
 * 返回值:
 *      成功: true
 *      失败: false
 ***********************************************************************/
extern "C" __declspec(dllexport) bool PQ_MatchEnd();


/***********************************************************************
 * 功能: 模板匹配
 * 参数:
 *      tDstData: 匹配结果数据
 *      tImageMat: 采集图片
 * 返回值:
 *      成功: true
 *      失败: false
 ***********************************************************************/
extern "C" __declspec(dllexport) bool PQ_MatchAndFill(vector<tuple<Mat, Rect, double, double>>& tDstData, const Mat tImageMat);


/***********************************************************************
 * 功能: 模板匹配
 * 参数:
 *      tDstData: 匹配结果数据
 *      tImageMat: 采集图片
 *      pcModelFileName: 模板文件名
 *      pcTiffFileName: 贴图tiff文件名
 * 返回值:
 *      成功: true
 *      失败: false
 ***********************************************************************/
extern "C" __declspec(dllexport) bool PQ_Match(vector<tuple<Mat, Rect, double, double>>& tDstData, const Mat tImageMat, const char* pcModelFileName, const char* pcTiffFileName);


/***********************************************************************
 * 功能: 匹配检测
 * @param tResultMat:		结果图
 * @param tVecPrintImage:	结果
 * @param tImageMat:		采集图
 ***********************************************************************/
extern "C" __declspec(dllexport) bool PQ_MatchCheck(cv::Mat& tResultMat, vector<tuple<Mat, Rect, double, double>>& tVecPrintImage, cv::Mat tImageMat);


/***********************************************************************
 * 功能: 参数设置
 * 参数:
 *      pcKey:   参数名, 与cfg文件中域与关键字组合，以下划线分割，示例：match_min_score
 *      pcValue: 参数值
 * 返回值:
 *      成功: true
 *      失败: false
 ***********************************************************************/
extern "C" __declspec(dllexport) bool PQ_SetParam(const char* pcKey, const char* pcValue, bool bSave = false);


/***********************************************************************
 * 功能: 获取参数
 * 参数:
 *      pcKey:   参数名，以下划线分割，示例：match_min_score
 *      pcValue: 参数值
 * 返回值:
 *      成功: true
 *      失败: false
 ***********************************************************************/
extern "C" __declspec(dllexport) bool PQ_GetParam(const char* pcKey, char* pcValue);


/***********************************************************************
 * 功能: 初始化参数
 * 参数:
 *      pcName:   文件名
 * 返回值:
 *      成功: true
 *      失败: false
 ***********************************************************************/
extern "C" __declspec(dllexport) bool PQ_InitParam(const char* pcName);
#endif