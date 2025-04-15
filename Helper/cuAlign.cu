#include "pch.h"
#include "Helper.h"
#include <iostream>
#include <cuda.h>
#include <cuda_runtime.h>
#include <device_launch_parameters.h>

using namespace cv;
using namespace std;

namespace Align
{
	enum {ePT_Foreground = 0, ePT_Diameter, ePT_Stride, ePT_Range, ePT_Erode, ePT_Dilate};

	void PreImage(Mat& cImIn, Mat& cImBin, Mat& cImOut, int nForeground, int nErode, int nDilate)
	{
		GaussianBlur(cImIn, cImBin, cv::Size(7, 7), 1.5, 0);

		threshold(cImBin, cImBin, nForeground, 255, THRESH_BINARY_INV);

		Mat kErode = Mat::ones(nErode, nErode, CV_8UC1);

		Mat kDilate = Mat::ones(nDilate, nDilate, CV_8UC1);

		erode(cImBin, cImOut, kErode);

		dilate(cImOut, cImOut, kDilate);
	}

	vector<Rect> ImConnected(Mat& cIm)
	{
		Mat labels = Mat::zeros(cIm.rows, cIm.cols, CV_32S), stats, centroids;

		connectedComponentsWithStats(cIm, labels, stats, centroids);

		auto lpCon = (int*)stats.data;

		vector<Rect> vCon(stats.rows - 1);

		for (int y = 1; y < stats.rows; ++y)
		{
			vCon[y - 1].x = lpCon[y * 5 + CC_STAT_LEFT];

			vCon[y - 1].y = lpCon[y * 5 + CC_STAT_TOP];

			vCon[y - 1].width = lpCon[y * 5 + CC_STAT_WIDTH];

			vCon[y - 1].height = lpCon[y * 5 + CC_STAT_HEIGHT];
		}
		return vCon;
	}

	inline void eraseLargerArea(vector<Rect>& vCon, int nDiameter) // 补充connectedComponentsWithStats带来的误差 // 该部在理想情况下不被调用
	{
		for (auto itr = vCon.begin(); itr != vCon.end(); )
		{
			if (itr->height < nDiameter && itr->width < nDiameter) { ++itr; continue; }

			itr = vCon.erase(itr);
		}
	}

	inline void eraseSmallerArea(vector<Rect>& vCon, double dTh)
	{
		if (vCon.empty()) return;

		auto iThArea = max_element(vCon.begin(), vCon.end(), [](Rect& rt1, Rect& rt2) {return rt1.area() < rt2.area(); })->area() * dTh;

		for (auto itr = vCon.begin(); itr != vCon.end();)
		{
			if (iThArea < itr->area()) { ++itr; continue; }

			itr = vCon.erase(itr);
		}
	}

	inline Rect rtUnite(Rect& rt1, Rect rt2)
	{
		return (rt1 & rt2);
	}

	inline Rect rtUnion(Rect& rt1, Rect rt2)
	{
		return (rt1 | rt2);
	}

	void ImUnion(vector<Rect>& vRect) // 该部在理想情况下不被调用
	{
		bool bUnite = false;
		do
		{
			bUnite = false;

			for (auto itr = vRect.begin(); itr != vRect.end(); ++itr)
			{
				auto itr2 = itr; ++itr2;

				for (; itr2 != vRect.end();)
				{
					if (rtUnite(*itr, *itr2).empty()) { ++itr2; continue; }

					(*itr) = rtUnion(*itr, *itr2);

					itr2 = vRect.erase(itr2);

					bUnite = true;
				}
			}
		} while (bUnite);
	}

	void eraseIsolatedPoints(int nDiameter, vector<Rect>& vConnected)
	{
		eraseLargerArea(vConnected, nDiameter);// 排除较大的差异点

		ImUnion(vConnected);

		eraseSmallerArea(vConnected, 0.5); // 排除较小的差异点
	}

	vector<tuple<Rect, Rect, int>> ImUnite(vector<Rect>& rt1, vector<Rect>& rt2, int nStride)
	{
		cout << "nStride: " << nStride << endl;

		Rect rtTmp; vector<tuple<Rect, Rect, int>> vUnite;

		for (int i = 0; i < rt1.size(); ++i)
		{
			for (int j = 0; j < rt2.size(); ++j)
			{
				rtTmp = rt2[j]; rtTmp.y += nStride;

				auto iUnite = rtUnite(rt1[i], rtTmp);

				if (iUnite.empty()) continue;

				vUnite.push_back(make_tuple(rt1[i], rt2[j], iUnite.area()));
			}
		}
		return vUnite;
	}

	bool ImUnite(Mat& cIm1, Mat& cIm2, int nDiameter, int nStride, array<Rect, 2>& vImUnite)
	{
		vector<Rect> vCon1 = ImConnected(cIm1);

		vector<Rect> vCon2 = ImConnected(cIm2);

		eraseIsolatedPoints(nDiameter, vCon1);

		eraseIsolatedPoints(nDiameter, vCon2);

		vector<tuple<Rect, Rect, int>> vUnite = ImUnite(vCon1, vCon2, nStride);

		if (vUnite.empty()) { cout << __FUNCTION__ << ": " << "empty" << endl; return false; }

		auto iMaxUnite = max_element(vUnite.begin(), vUnite.end(),
			[](tuple<Rect, Rect, int>& u1, tuple<Rect, Rect, int>& u2)
			{ return get<2>(u1) < get<2>(u2); });

		auto& iCon1 = get<0>(*iMaxUnite), & iCon2 = get<1>(*iMaxUnite);

		int w = min(iCon1.width, iCon2.width), h = min(iCon1.height, iCon2.height);

		if (!!iCon2.y)	// 顶部对齐
		{
			vImUnite[0].x = iCon1.x, vImUnite[0].y = iCon1.y;

			vImUnite[0].width = w, vImUnite[0].height = h;

			vImUnite[1].x = iCon2.x, vImUnite[1].y = iCon2.y;

			vImUnite[1].width = w, vImUnite[1].height = h;
		}
		else  //底部对齐
		{
			vImUnite[0].x = iCon1.x, vImUnite[0].y = iCon1.y + iCon1.height - h;

			vImUnite[0].width = w, vImUnite[0].height = h;

			vImUnite[1].x = iCon2.x, vImUnite[1].y = iCon2.y + iCon2.height - h;

			vImUnite[1].width = w, vImUnite[1].height = h;
		}
		return true;
	}

	__global__ void cuCorrect(
		float* a, float* b,
		int cols, int rows,
		int nCompensation,
		float* aIntegral, float* bIntegral,
		double* score)
	{
		int ox = threadIdx.x + nCompensation;

		int oy = blockIdx.x + nCompensation;

		int x1 = max(0, ox), x2 = max(0, -ox);

		int y1 = max(0, oy), y2 = max(0, -oy);

		int w = cols - abs(ox), h = rows - abs(oy);

		double dAB = 0.0;

		for (int y = 0; y < h; ++y)
		{
			for (int x = 0; x < w; ++x)
			{
				int idx1 = (y1 + y) * cols + (x1 + x);

				int idx2 = (y2 + y) * cols + (x2 + x);

				dAB += (a[idx1] * b[idx2]);
			}
		}

		int aw = cols + 1, ah = rows + 1;

		double dA =
			aIntegral[(y1 + 1) * aw + (x1 + 1)] +
			aIntegral[(y1 + h) * aw + (x1 + w)] -
			aIntegral[(y1 + h) * aw + (x1 + 1)] -
			aIntegral[(y1 + 1) * aw + (x1 + w)];

		double dB =
			bIntegral[(y2 + h) * aw + (x2 + w)] +
			bIntegral[(y2 + 1) * aw + (x2 + 1)] -
			bIntegral[(y2 + h) * aw + (x2 + 1)] -
			bIntegral[(y2 + 1) * aw + (x2 + w)];

		score[blockIdx.x * blockDim.x + threadIdx.x] = dAB / sqrt(dA * dB);
	}

	array<int, 2> cuAlign(
		Mat& a, Mat& b,
		Mat& aIntegral, Mat& bIntegral,
		int nRange, double& dScore) // 210ms
	{
		Mat cScore(nRange * 2, nRange * 2, CV_64FC1);

		float* cuA, * cuB, * cuAcIntegral, * cuBIntegral; double* cuScore;

		cudaMalloc<double>(&cuScore, sizeof(double) * cScore.rows * cScore.cols);

		cudaMalloc<float>(&cuA, sizeof(float) * a.rows * a.cols);

		cudaMalloc<float>(&cuB, sizeof(float) * a.rows * a.cols);

		cudaMalloc<float>(&cuAcIntegral, sizeof(float) * aIntegral.rows * aIntegral.cols);

		cudaMalloc<float>(&cuBIntegral, sizeof(float) * aIntegral.rows * aIntegral.cols);

		cudaMemcpy(cuA, a.data, sizeof(float) * a.rows * a.cols, cudaMemcpyHostToDevice);

		cudaMemcpy(cuB, b.data, sizeof(float) * a.rows * a.cols, cudaMemcpyHostToDevice);

		cudaMemcpy(cuAcIntegral, aIntegral.data, sizeof(float) * aIntegral.rows * aIntegral.cols, cudaMemcpyHostToDevice);

		cudaMemcpy(cuBIntegral, bIntegral.data, sizeof(float) * aIntegral.rows * aIntegral.cols, cudaMemcpyHostToDevice);

		cuCorrect << <nRange * 2, nRange * 2 >> > (
			cuA, cuB,
			a.cols, a.rows,
			-nRange,
			cuAcIntegral, cuBIntegral,
			cuScore);

		cudaMemcpy(cScore.data, cuScore, sizeof(double) * cScore.rows * cScore.cols, cudaMemcpyDeviceToHost);

		double* lpMax = max_element<double*>((double*)cScore.data, (double*)(cScore.data + cScore.rows * cScore.cols * sizeof(double)));

		int idxMax = lpMax - (double*)cScore.data; dScore = *lpMax;

		cudaFree(cuA);

		cudaFree(cuB);

		cudaFree(cuAcIntegral);

		cudaFree(cuBIntegral);

		cudaFree(cuScore);

		return array<int, 2>{ idxMax% cScore.cols - nRange, idxMax / cScore.cols - nRange };
	}

	array<int, 2> ImAlign(Mat& a, Mat& b, int nRange, double& dScore)
	{
		a.convertTo(a, CV_32F);	b.convertTo(b, CV_32F);

		Mat a2 = a.mul(a), b2 = b.mul(b);

		Mat aIntegral, bIntegral;

		integral(a2, aIntegral, CV_32FC1);

		integral(b2, bIntegral, CV_32FC1);

		return cuAlign(a, b, aIntegral, bIntegral, nRange, dScore);
	}
}
/******************************************************************************************/
// 反色、输入外接圆直径、图像间隔（n*pass）、浮动范围、腐蚀核（30）、膨胀核（80）
void cuAlign(
	cv::Mat& cIm1, cv::Mat& cIm2,
	cv::Mat& cImBin1, cv::Mat& cImBin2,
	cv::Mat& cImAlign1, cv::Mat& cImAlign2,
	cv::Mat& cImUnite1, cv::Mat& cImUnite2,
	std::array<cv::Rect, 2>& rtImUnite, std::array<int, 2>& vAlign,
	double& dScore, double& dTime, 
	std::array<int, 6> vParam, std::array<int, 2>& vInterval)
{
	Align::PreImage(cIm1, cImBin1, cImAlign1, vParam[Align::ePT_Foreground], vParam[Align::ePT_Erode], vParam[Align::ePT_Dilate]);

	Align::PreImage(cIm2, cImBin2, cImAlign2, vParam[Align::ePT_Foreground], vParam[Align::ePT_Erode], vParam[Align::ePT_Dilate]);

	if (!Align::ImUnite(cImAlign1, cImAlign2, vParam[Align::ePT_Diameter], vParam[Align::ePT_Stride], rtImUnite)) { vInterval = { 0, vParam[2] }; return; }

	cImUnite1 = cImBin1(rtImUnite[0]).clone();

	cImUnite2 = cImBin2(rtImUnite[1]).clone();

	vAlign = Align::ImAlign(cImUnite1, cImUnite2, vParam[Align::ePT_Range], dScore);

	vInterval[0] = rtImUnite[0].x - rtImUnite[1].x + vAlign[0];

	vInterval[1] = rtImUnite[0].y - rtImUnite[1].y + vAlign[1];
}
