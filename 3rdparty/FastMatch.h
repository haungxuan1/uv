#if _MSC_VER >= 1600						//VS2015>VS>VS2010, MSVC VER= 10.0 -14.0
#pragma execution_character_set("utf-8")
#endif

#pragma once
#ifndef __FASTMATCH_H__
#define __FASTMATCH_H__
#include "tiffio.h"
#include "opencv2/opencv.hpp"

typedef struct {
	int iCenterX;
	int iCenterY;
	float fAngle;
	float fScore;
}T_MatchInfo, * PT_MatchInfo;

//匹配接口
extern "C" __declspec(dllexport) bool pq_match(vector<tuple<Mat, Rect, double, double>>&tVecPrintImage, Mat &tImageMat, Mat &tTemplateMat, Mat &tTempMaskMat, TIFF * ptProductTif);

//单匹配不贴图
//匹配接口
extern "C" __declspec(dllexport) bool pq_match_V2(vector<T_MatchInfo> &tVecMachInfo, Mat &tImageMat, Mat &tTemplateMat, Mat &tTempMaskMat);

//匹配参数设置
extern "C" __declspec(dllexport) bool pq_matchSetParam(const char* pcKey, const char* pcValue, bool bSave);

//匹配并生成结果图
extern "C" __declspec(dllexport) bool pq_matchCheck(Mat &tResultMat, vector<tuple<Mat, Rect, double, double>>&tVecPrintImage,
						Mat &tImageMat, Mat &tTemplateMat, Mat &tTempMaskMat, TIFF *ptProductTif);

//生成匹配模型
extern "C" __declspec(dllexport) bool pq_createTemplate(Mat &tTemplateMat, Mat &tTempMaskMat, const char *pcModel);

#endif