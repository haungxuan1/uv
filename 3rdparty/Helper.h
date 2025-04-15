#pragma once
#include <vector>
#include <tuple>
#include <array>
#include <string>
#include "opencv.hpp"

extern "C"
{
	// 灰度直方图
	// 输入灰度图
	// 输出灰度直方图
	void Histogram(cv::Mat& cGray, cv::Mat& cHistogram);

	// 灰度直方图
	// 输入Histogram结果
	// 输入二值阈值
	// 输出直方图
	void HistogramView(cv::Mat& cImHistogtam, cv::Mat& cHistogram, int nThreshold);

	// 二值图像阈值
	// 输入灰度直方图
	// 输出二值阈值及类型
	void BimodalThreshold(cv::Mat& cHistogram, int& nThreshold, int& nThresholdType);
	void BimodalThresholdEx(cv::Mat& cHistogram, int& nThreshold, int& nThresholdType);

	// 二值图像
	// 输入灰度图
	// 输出二值图
	void Binarization(cv::Mat& cImage, cv::Mat& cBin);

	// prt文件头
	// 输入图像宽\高\通道数
	// 输出文件头
	void PrtHeader(unsigned int uImagew,unsigned int uImageh,unsigned int uChannel, unsigned int uBit, std::tuple<unsigned int, unsigned char*>& vPrtHeader);

	void Chart(uint64 uChartInCol, uint64 uChartInRow, cv::Mat*& lpChart);

	// 灰度转cmyk
	// 输入灰度图
	// 输入cmyk变换通道
	// 输出cmyk图
	void Gray2Cmyk(cv::Mat& cImage, cv::Mat& cChannel, std::vector<int> ch);

	// 灰度转cmykw
	// 输入灰度图
	// 输入cmykw变换通道
	// 输出cmykw图
	void Gray2Cmykwv(cv::Mat& cImage, cv::Mat& cChannel, std::vector<int> ch);

	// 图像对齐
	// 输入图像a,b
	// 输入外接圆直径、图像间隔（n*pass）、浮动范围、腐蚀核（30）、膨胀核（80）
	// 输入浮动范围 
	// 输出对齐偏移
	void cuAlign(
		cv::Mat& cIm1, cv::Mat& cIm2, 
		cv::Mat& cBin1, cv::Mat& cBin2,
		cv::Mat& cImAlign1, cv::Mat& cImAlign2,
		cv::Mat& cImUnite1, cv::Mat& cImUnite2,
		std::array<cv::Rect, 2>& rtImUnite, std::array<int, 2>& vAlign,
		double& dScore, double& dTime,
		std::array<int, 6> vParam, std::array<int, 2>& vInterval);

	// 图像细化
	// 输入图像
	// 输出图像
	int cuThinging(cv::Mat*& lpImage, cv::Mat*& lpThing);

	// 二维码位置 - 参照物
	// 输入灰度图
	// 输入高斯核、密度核、腐蚀核、膨胀核、二值阈值
	// 输出二维码图像、位置
	void QEncodePos(std::array<int, 5> vParam, cv::Mat& cImage, cv::Mat& cQEncode, cv::Rect& rtQencode);

	// 文件夹内文件遍历
	// 输入文件夹路径
	// 输出文件列表
	void FileInDir(std::wstring strDir, std::vector<std::array<std::wstring, 2>>& vFile);
	
	// 文件排序
	void SortingFiles(std::vector<std::array<std::wstring, 2>>& vFile);

	void ReadTif(std::string strTif, cv::Mat& cTif);
	void WriteTif(std::string strHeader, std::string strTif, cv::Mat& cTif);


	// 通过摩尔投票取众数区间局部极值
	// .def 导出函数,通过不同实例化导出模板函数
	int Majority_int(std::vector<int>& vArray);
	float Majority_float(std::vector<float>& vArray);
	double Majority_double(std::vector<double>& vArray);
	int ModeInterval_int(std::vector<int>& vArray);
	float ModeInterval_float(std::vector<float>& vArray);
	double ModeInterval_double(std::vector<double>& vArray);
	int Mode_int(std::vector<int>& vArray);
	float Mode_float(std::vector<float>& vArray);
	double Mode_double(std::vector<double>& vArray);
	void Mode_tuple(std::vector<std::tuple<int, double>>& vArray, std::tuple<int, double> &vMode);
}
