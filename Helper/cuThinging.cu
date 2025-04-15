#include "Helper.h"
#include <cuda_runtime.h>
#include <device_launch_parameters.h>

using namespace cv;
using namespace std;

typedef int ImType;

void ImEncode(Mat& cImage, Mat& cEncode)
{
	// 仅x方向压缩
	ImType idx1 = 0, idx2 = 0;

	for (ImType y = 0; y < cEncode.rows; ++y)
	{
		for (ImType x = 0; x < cEncode.cols; ++x)
		{
			idx2 = y * cEncode.cols + x;

			for (int i = 0; i < 8; ++i)
			{
				idx1 = y * cImage.cols + x * 8 + i;

				cEncode.data[idx2] |= ((cImage.data[idx1] & 0x1) << i);
			}
		}
	}
}

void ImDecode(Mat& cEncode, Mat& cDecode)
{
	ImType idx1 = 0, idx2 = 0;

	for (ImType y = 0; y < cEncode.rows; ++y)
	{
		for (ImType x = 0; x < cEncode.cols; ++x)
		{
			idx2 = y * cEncode.cols + x;

			for (int i = 0; i < 8; ++i)
			{
				idx1 = y * cDecode.cols + x * 8 + i;

				cDecode.data[idx1] = ((cEncode.data[idx2] >> i) & 0x1) * 0xff;
			}
		}
	}
}

struct tagImPoint
{
	ImType x, y, b, idx; // b = x 方向encode bit 补偿
	__device__ tagImPoint() :x(0), y(0), idx(0){}
	__device__ tagImPoint(ImType _x, ImType _y, ImType _b) : x(_x), y(_y), b(_b), idx(0) {}
};

__global__ void cuZSSynchro(
	uchar* lpImage,
	uchar* lpSign,
	ImType sw, ImType sh/*线程子区域*/)
{
	ImType w = sw * gridDim.x + 2;

	ImType h = sh * blockDim.x + 2;

	uchar s[] = { 0, 0 };

	tagImPoint o(sw * blockIdx.x, sh * threadIdx.x, 0), p; // 子区域起始点

	for (ImType x = 0; x < sw; ++x)
	{
		for (ImType y = 0; y < sh; ++y)
		{
			p.x = o.x + x + 1;

			p.y = o.y + y + 1; // os

			p.idx = p.y * w + p.x;

			for (ImType b = 0; b < 8; ++b)
			{
				s[0] = (lpImage[p.idx] >> b & 0x1);

				lpImage[p.idx] = lpImage[p.idx] & (~(0x1 << b));

				lpImage[p.idx] |= (s[lpSign[p.idx] >> b & 0x1] << b);
			}
		}
	}
}

__global__ void cuZSKernel1(
	uchar* lpImage,
	uchar* lpSign,
	uchar* lpRef/*刷新标志*/,
	ImType sw, ImType sh/*线程子区域*/)
{
	// 以^替换逻辑判断
	int bit[2] = { 8, 1 };

	int begin = 0, end = 0;

	ImType w = sw * gridDim.x + 2;

	ImType h = sh * blockDim.x + 2;

	int sum = 0, changed = 0;

	uchar sync[6] = { 0 };

	tagImPoint p[9] = { tagImPoint() }; // 领域点

	tagImPoint o(sw * blockIdx.x, sh * threadIdx.x, (ImType)0); // 子区域起始点

	// 邻域补偿
	tagImPoint c[9] = {
		{ 0, 0, 0 }, { 0, -1, 0 }, { 0, -1, 1 }, 
		{ 0, 0, 1 }, { 0, 1, 1 }, { 0, 1, 0 }, 
		{ 0, 1, -1 }, { 0, 0, -1 }, { 0, -1, -1 } };

	for (ImType x = 0; x < sw; ++x)
	{
		for (ImType y = 0; y < sh; ++y)
		{
			p[0].x = o.x + x + 1;

			p[0].y = o.y + y + 1;// os

			p[0].idx = p[0].y * w + p[0].x;

			for (ImType b = 1; b <= 8; ++b)
			{
				p[0].b = b;

				for (int i = 1; i < 9; ++i)
				{
					p[i].x = p[0].x + c[i].x;

					p[i].y = p[0].y + c[i].y;

					p[i].b = p[0].b + c[i].b;
				
					// b = 0 -> b = 8, x = x -1
					// b = 9 -> b = 1, x = x + 1
					begin = !!!p[i].b;

					end = !(p[i].b ^ 9);

					p[i].x -= begin;

					p[i].x += end;

					p[i].b = (begin * bit[0]) ^ p[i].b;

					bit[0] = (begin * bit[0]) ^ p[i].b;

					p[i].b = (begin * bit[0]) ^ p[i].b;

					p[i].b = (end * bit[1]) ^ p[i].b;

					bit[1] = (end * bit[1]) ^ p[i].b;

					p[i].b = (end * bit[1]) ^ p[i].b;

					p[i].idx = p[i].y * w + p[i].x;

					sum += (lpImage[p[i].idx] >> (p[i].b - 1) & 0x1);

					bit[0] = 8; bit[1] = 1;
				} // 8邻域

				changed += max(0, (lpImage[p[1].idx] >> (p[1].b - 1) & 0x1) -  (lpImage[p[8].idx] >> (p[8].b - 1) & 0x1));

				for (int i = 2; i < 9; ++i)
				{
					changed += max(0, (lpImage[p[i].idx] >> (p[i].b - 1) & 0x1) - (lpImage[p[i-1].idx] >> (p[i-1].b - 1) & 0x1));
				}

				sync[0] = !((lpImage[p[0].idx] >> (p[0].b - 1) & 0x1) ^1);// p1 = 1
				sync[1] = !(sum ^ min(max(2, sum), 6)); // 2 <= sum <=6
				sync[2] = !(changed ^ 1);// changed = 1
				sync[3] = !((lpImage[p[1].idx] >> (p[1].b - 1) & 0x1)*
					(lpImage[p[3].idx] >> (p[3].b - 1) & 0x1)*
					(lpImage[p[5].idx] >> (p[5].b - 1) & 0x1));// p2*p4*p6 = 0
				sync[4] = !((lpImage[p[7].idx] >> (p[7].b - 1) & 0x1) *
					(lpImage[p[3].idx] >> (p[3].b - 1) & 0x1) *
					(lpImage[p[5].idx] >> (p[5].b - 1) & 0x1));// p4*p6*p8 = 0

				sync[5] = (sync[0] & sync[1] & sync[2] & sync[3] & sync[4]);

				lpSign[p[0].idx] |= (sync[5] << (p[0].b - 1));

				lpRef[p[0].idx] |= (sync[5] << (p[0].b - 1));

				sum = 0, changed = 0;;
			}
		}
	}
}

__global__ void cuZSKernel2(
	uchar* lpImage,
	uchar* lpSign,
	uchar* lpRef/*刷新标志*/,
	ImType sw, ImType sh/*线程子区域*/)
{
	// 以^替换逻辑判断
	int bit[2] = { 8, 1 };

	int begin = 0, end = 0;

	ImType w = sw * gridDim.x + 2;

	ImType h = sh * blockDim.x + 2;

	int sum = 0, changed = 0;

	uchar sync[6] = { 0 };

	tagImPoint p[9] = { tagImPoint() }; // 领域点

	tagImPoint o(sw * blockIdx.x, sh * threadIdx.x, 0); // 子区域起始点

	// 邻域补偿
	tagImPoint c[9] = {
		{ 0, 0, 0 }, { 0, -1, 0 }, { 0, -1, 1 },
		{ 0, 0, 1 }, { 0, 1, 1 }, { 0, 1, 0 },
		{ 0, 1, -1 }, { 0, 0, -1 }, { 0, -1, -1 } };

	for (ImType x = 0; x < sw; ++x)
	{
		for (ImType y = 0; y < sh; ++y)
		{
			p[0].x = o.x + x + 1;

			p[0].y = o.y + y + 1;// os

			p[0].idx = p[0].y * w + p[0].x;

			for (ImType b = 1; b <= 8; ++b)
			{
				p[0].b = b;

				for (int i = 1; i < 9; ++i)
				{
					p[i].x = p[0].x + c[i].x;

					p[i].y = p[0].y + c[i].y;

					p[i].b = p[0].b + c[i].b;

					// b = 0 -> b = 8, x = x -1
					// b = 9 -> b = 1, x = x + 1
					begin = !!!p[i].b;

					end = !(p[i].b ^ 9);

					p[i].x -= begin;

					p[i].x += end;

					p[i].b = (begin * bit[0]) ^ p[i].b;

					bit[0] = (begin * bit[0]) ^ p[i].b;

					p[i].b = (begin * bit[0]) ^ p[i].b;

					p[i].b = (end * bit[1]) ^ p[i].b;

					bit[1] = (end * bit[1]) ^ p[i].b;

					p[i].b = (end * bit[1]) ^ p[i].b;

					p[i].idx = p[i].y * w + p[i].x;

					sum += (lpImage[p[i].idx] >> (p[i].b - 1) & 0x1);

					bit[0] = 8; bit[1] = 1;
				} // 8邻域

				changed += max(0, (lpImage[p[1].idx] >> (p[1].b - 1) & 0x1) - (lpImage[p[8].idx] >> (p[8].b - 1) & 0x1));

				for (int i = 2; i < 9; ++i)
				{
					changed += max(0, (lpImage[p[i].idx] >> (p[i].b - 1) & 0x1) - (lpImage[p[i - 1].idx] >> (p[i - 1].b - 1) & 0x1));
				}

				sync[0] = !((lpImage[p[0].idx] >> (p[0].b - 1) & 0x1) ^ 1);// p1 = 1
				sync[1] = !(sum ^ min(max(2, sum), 6)); // 2 <= sum <=6
				sync[2] = !(changed ^ 1);// changed = 1
				sync[3] = !((lpImage[p[1].idx] >> (p[1].b - 1) & 0x1) *
					(lpImage[p[3].idx] >> (p[3].b - 1) & 0x1) *
					(lpImage[p[7].idx] >> (p[7].b - 1) & 0x1));// p2*p4*p8 = 0

				sync[4] = !((lpImage[p[7].idx] >> (p[7].b - 1) & 0x1) *
					(lpImage[p[1].idx] >> (p[1].b - 1) & 0x1) *
					(lpImage[p[5].idx] >> (p[5].b - 1) & 0x1));// p2*p6*p8 = 0

				sync[5] = (sync[0] & sync[1] & sync[2] & sync[3] & sync[4]);

				lpSign[p[0].idx] |= (sync[5] << (p[0].b - 1));

				lpRef[p[0].idx] |= (sync[5] << (p[0].b - 1));

				sum = 0, changed = 0;;
			}
		}
	}
}

// 共享内存48k
// 48 block 1024 thread
__global__ void cuZSSign(
	int* lpChanged,
	uchar* lpRef/*刷新标志*/,
	ImType sw, ImType sh/*线程子区域*/)
{
	ImType w = sw * gridDim.x + 2;

	ImType h = sh * blockDim.x + 2;

	tagImPoint o(sw * blockIdx.x, sh * threadIdx.x, 0), p; // 子区域起始点

	ImType idx = 0;

	for (ImType x = 0; x < sw; ++x)
	{
		for (ImType y = 0; y < sh; ++y)
		{
			idx = (o.y + y + 1) * w + o.x + x + 1;

			if (!!lpRef[idx])
			{
				atomicMax(lpChanged, (int)lpRef[idx]);
			}
		}
	}
}

// im = 73000*80000
// 700mb
// max int = 2gb(1,147,483,647) 
// max uint = 4gb(4,294,967,295)
// max long long = 8388607t(9,223,372,036,854,775,807)
// -> 类型 = int
// 32g
void cuZhangSuen(Mat& cEncode, Mat& cImage, ImType nBlock, ImType nThread, ImType nSW, ImType nSH)
{
	ImType uImSize = (ImType)cEncode.rows * cEncode.cols;

	int nChanged[1] = { 0 }, *cuChanged = nullptr;

	uchar* cuImage = nullptr, * cuSign = nullptr, * cuRefresh = nullptr;

	cudaMalloc(&cuSign, uImSize);

	cudaMalloc(&cuImage, uImSize);

	cudaMalloc(&cuRefresh, uImSize);

	cudaMalloc(&cuChanged, sizeof(nChanged));

	cudaMemcpy(cuImage, cEncode.data, uImSize, cudaMemcpyHostToDevice);

	do
	{
		cudaMemset(cuChanged, 0, sizeof(nChanged));

		cudaMemset(cuSign, 0, uImSize);

		cudaMemset(cuRefresh, 0, uImSize);

		cuZSKernel1 << <nBlock, nThread >> > (
			cuImage,
			cuSign,
			cuRefresh,
			nSW, nSH);
	
		cuZSSynchro << < nBlock, nThread >> > (
			cuImage, 
			cuSign,
			nSW, nSH);

		cuZSKernel2 << <nBlock, nThread >> > (
			cuImage,
			cuSign,
			cuRefresh,
			nSW, nSH);

		cuZSSynchro << < nBlock, nThread >> > (
			cuImage,
			cuSign,
			nSW, nSH);

		cuZSSign << <nBlock, nThread >> > (
			cuChanged,
			cuRefresh,
			nSW, nSH);

		cudaMemcpy(nChanged, cuChanged, sizeof(nChanged), cudaMemcpyDeviceToHost);

		cout << nChanged[0] << endl;

	} while (!!nChanged[0]);

	cudaMemcpy(cEncode.data, cuImage, uImSize, cudaMemcpyDeviceToHost);

	cudaFree(cuChanged); cudaFree(cuRefresh); cudaFree(cuImage); cudaFree(cuSign);
}

// 由于图像过大，该部分采取了x方向压缩
int cuThinging(cv::Mat*& lpImage, cv::Mat*& lpThing)
{
	static ImType encode = 8;

	static ImType g_nBlock = 26 * 16; // 26 * 16

	static ImType g_nThread = 32 * 32; // 1024

	clock_t t1 = clock();

	cout << __FUNCTION__ << ": " << t1 << endl;

	ImType iImw = lpImage->cols, iImh = lpImage->rows;

	ImType w = ceil((double)iImw / encode / g_nBlock) * g_nBlock * encode;	// +1 邻域对齐

	ImType h = ceil((double)iImh / g_nThread) * g_nThread;

	// +1 邻域对齐
	// h - cImage.rows & w - cImage.cols 线程对齐	
	copyMakeBorder(*lpImage, *lpImage,
		1, h - iImh + 1, encode, w - iImw + encode, BORDER_CONSTANT, Scalar(0));

	Mat* lpEncode = new Mat(Mat::zeros(lpImage->rows, lpImage->cols * 0.125, CV_8UC1));

	ImEncode(*lpImage, *lpEncode);

	ImType nSW = lpEncode->cols / g_nBlock;

	ImType nSH = lpEncode->rows / g_nThread;

	cuZhangSuen(*lpEncode, *lpImage, g_nBlock, g_nThread, nSW, nSH);

	ImDecode(*lpEncode, *lpImage); delete lpEncode;

	(*lpThing) = (*lpImage)(Rect(encode, 1, iImw, iImh)).clone();

	clock_t t2 = clock();

	cout << __FUNCTION__ << ": " << (t2-t1) << endl;

	return 0;
}
