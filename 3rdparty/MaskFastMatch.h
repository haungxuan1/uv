#pragma once
#ifndef __MASKFASTMATCH_H__
#define __MASKFASTMATCH_H__
#include "tiffio.h"
#include "opencv2/opencv.hpp"


/***********************************************************************
 * 功能: 实现模板匹配并返回。
 * 参数:
 *     cutNum：将采集图进行分割，cutNum为分割的数量
 *     scalNum:   缩小图片，缩小2^scalNum倍。0代表不缩小。
 *     dScore:  用户设置最低匹配精度
 *     tImageMat:     待匹配采集图片
 *     tTemplateMat:   模板图
 *     productTif: 产品图(TIFF->CMYK格式)
 *     vPrintImage： 返回目标的位置信息Ret以及旋转后的productTif
 *     std::vector<std::tuple<cv::Mat, cv::Rect, double>>: 返回匹配信息，图片，分数
************************************************************************/
extern "C" __declspec(dllexport) int MatchAndCreatPrintMat(int cutNum, uint8_t scalNum, cv::Mat tImageMat,
	cv::Mat tTemplateMat, cv::Mat tTempMaskMat, TIFF * productTif,
	std::vector<std::tuple<cv::Mat, cv::Rect, double>>&vPrintImage);

#endif
