#pragma once
#ifndef __MASKFASTMATCH_H__
#define __MASKFASTMATCH_H__
#include "tiffio.h"
#include "opencv2/opencv.hpp"


/***********************************************************************
 * ����: ʵ��ģ��ƥ�䲢���ء�
 * ����:
 *     cutNum�����ɼ�ͼ���зָcutNumΪ�ָ������
 *     scalNum:   ��СͼƬ����С2^scalNum����0������С��
 *     dScore:  �û��������ƥ�侫��
 *     tImageMat:     ��ƥ��ɼ�ͼƬ
 *     tTemplateMat:   ģ��ͼ
 *     productTif: ��Ʒͼ(TIFF->CMYK��ʽ)
 *     vPrintImage�� ����Ŀ���λ����ϢRet�Լ���ת���productTif
 *     std::vector<std::tuple<cv::Mat, cv::Rect, double>>: ����ƥ����Ϣ��ͼƬ������
************************************************************************/
extern "C" __declspec(dllexport) int MatchAndCreatPrintMat(int cutNum, uint8_t scalNum, cv::Mat tImageMat,
	cv::Mat tTemplateMat, cv::Mat tTempMaskMat, TIFF * productTif,
	std::vector<std::tuple<cv::Mat, cv::Rect, double>>&vPrintImage);

#endif
