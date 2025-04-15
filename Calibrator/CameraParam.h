#pragma once
#include <array>

using namespace std;

class CCameraParam
{
public:
	enum {
		eCT_Foreground = 0, 
		eCT_Reference,
		eCT_Align,
		eCT_Merge,
		eCT_ROI,
		eCT_Perspective,
		eCT_Homography,
		eCT_Reference2,
		eCT_Align2,
		eCT_Merge2,
		eCT_DPI,
		eCT_Param
	};
public:
	struct tagCalibrator 
	{
	public: // 该类参数用于指导准准参数
		int nType;	// 该份参数类型 // 表面参数是否可用 // 断点校准
		double dAssessment; // 评估标准1 // 参照物对齐质量 // 该值越接近0则对齐质量越高

	public: // 该类参数用于参数校准 // 对外开放，用于相机纠正
		int nExtTriggerFrameCount; // 帧/触发// CCameraDlg 管理
		int nForeground;			// 前景阈值 // CForegroundDlg
		int nErode;					// 腐蚀	// CReferenceDlg
		int nDilate;				// 膨胀 // CReferenceDlg
		int nGaussian;				// 高斯 // CReferenceDlg
		int nPixelInPass;			// 像素/pass // CAlignDlg	// 543
		int nPassInStride;			// pass/步进 // CAlignDlg
		array<float, 2> vWH1;		// 采集图像大小 // CAlignDlg	// 30000 * 13056
		array<float, 2> vWH2;		// 纠正后图像大小 // CAlignDlg
		array<int, 2> vWH2Ex;		// 纠正后图像实际大小
		array<double, 2> vDPI1;		// 打印dpi		// CDPIDlg	635.0 * 1200.0
		array<double, 2> vDPI2;		// 相机dpi 
		array<int, 2> vDist;		// CChartDlg	// 100140, 30900
		array<double, 9> M;			// 变换矩阵M
		array<int, 4> vFill;		// 变换前填充系数

	public: // 该类参数用于校准功能临时存储 // 不对外开放
		int nBlock;	// 前景阈值提取分块数 // CForegroundDlg
		int nDensity; // 密度 // CReferenceDlg
		array<int, 4> nReference; // 参照物位置 // CReferenceDlg
		array<Point2f, 4> vVertex1;	// 变换前4顶点信息
		array<Point2f, 4> vVertex2; // 变换后4顶点信息

		tagCalibrator():
			nType(0),
			dAssessment(0.0), 
			nExtTriggerFrameCount(5),
			nForeground(0),
			nErode(0),
			nDilate(0),
			nGaussian(0),
			nPixelInPass(543),
			nPassInStride(0),
			vWH1{30000.0, 13056.0},
			vWH2{ 0.0, 0.0 },
			vWH2Ex{0,0},
			vDPI1{635.0, 1200.0},
			vDPI2{ 0.0, 0.0 },
			vDist{0, 0},
			M{0.0},
			vFill{0},
			nBlock(10),
			nDensity(30)
		{}
	};

protected:
	tagCalibrator m_cParam;
};