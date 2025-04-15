#pragma once
#include <vector>
#include <tuple>
#include <array>
#include <string>
#include "opencv.hpp"

extern "C"
{
	// �Ҷ�ֱ��ͼ
	// ����Ҷ�ͼ
	// ����Ҷ�ֱ��ͼ
	void Histogram(cv::Mat& cGray, cv::Mat& cHistogram);

	// �Ҷ�ֱ��ͼ
	// ����Histogram���
	// �����ֵ��ֵ
	// ���ֱ��ͼ
	void HistogramView(cv::Mat& cImHistogtam, cv::Mat& cHistogram, int nThreshold);

	// ��ֵͼ����ֵ
	// ����Ҷ�ֱ��ͼ
	// �����ֵ��ֵ������
	void BimodalThreshold(cv::Mat& cHistogram, int& nThreshold, int& nThresholdType);
	void BimodalThresholdEx(cv::Mat& cHistogram, int& nThreshold, int& nThresholdType);

	// ��ֵͼ��
	// ����Ҷ�ͼ
	// �����ֵͼ
	void Binarization(cv::Mat& cImage, cv::Mat& cBin);

	// prt�ļ�ͷ
	// ����ͼ���\��\ͨ����
	// ����ļ�ͷ
	void PrtHeader(unsigned int uImagew,unsigned int uImageh,unsigned int uChannel, unsigned int uBit, std::tuple<unsigned int, unsigned char*>& vPrtHeader);

	void Chart(uint64 uChartInCol, uint64 uChartInRow, cv::Mat*& lpChart);

	// �Ҷ�תcmyk
	// ����Ҷ�ͼ
	// ����cmyk�任ͨ��
	// ���cmykͼ
	void Gray2Cmyk(cv::Mat& cImage, cv::Mat& cChannel, std::vector<int> ch);

	// �Ҷ�תcmykw
	// ����Ҷ�ͼ
	// ����cmykw�任ͨ��
	// ���cmykwͼ
	void Gray2Cmykwv(cv::Mat& cImage, cv::Mat& cChannel, std::vector<int> ch);

	// ͼ�����
	// ����ͼ��a,b
	// �������Բֱ����ͼ������n*pass����������Χ����ʴ�ˣ�30�������ͺˣ�80��
	// ���븡����Χ 
	// �������ƫ��
	void cuAlign(
		cv::Mat& cIm1, cv::Mat& cIm2, 
		cv::Mat& cBin1, cv::Mat& cBin2,
		cv::Mat& cImAlign1, cv::Mat& cImAlign2,
		cv::Mat& cImUnite1, cv::Mat& cImUnite2,
		std::array<cv::Rect, 2>& rtImUnite, std::array<int, 2>& vAlign,
		double& dScore, double& dTime,
		std::array<int, 6> vParam, std::array<int, 2>& vInterval);

	// ͼ��ϸ��
	// ����ͼ��
	// ���ͼ��
	int cuThinging(cv::Mat*& lpImage, cv::Mat*& lpThing);

	// ��ά��λ�� - ������
	// ����Ҷ�ͼ
	// �����˹�ˡ��ܶȺˡ���ʴ�ˡ����ͺˡ���ֵ��ֵ
	// �����ά��ͼ��λ��
	void QEncodePos(std::array<int, 5> vParam, cv::Mat& cImage, cv::Mat& cQEncode, cv::Rect& rtQencode);

	// �ļ������ļ�����
	// �����ļ���·��
	// ����ļ��б�
	void FileInDir(std::wstring strDir, std::vector<std::array<std::wstring, 2>>& vFile);
	
	// �ļ�����
	void SortingFiles(std::vector<std::array<std::wstring, 2>>& vFile);

	void ReadTif(std::string strTif, cv::Mat& cTif);
	void WriteTif(std::string strHeader, std::string strTif, cv::Mat& cTif);


	// ͨ��Ħ��ͶƱȡ��������ֲ���ֵ
	// .def ��������,ͨ����ͬʵ��������ģ�庯��
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
