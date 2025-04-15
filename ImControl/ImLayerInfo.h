#pragma once
#include "ImPtr.h"
#include <array>

using namespace cv;
using namespace std;

namespace ImLayer
{
	struct tagLayer
	{
		enum { eTT_Image = 0, eTT_BRGA, eTT_Preview, eTT_Rotate, eTT_Scale, eTT_Contrast }; // TT = timer type
		enum { eOT_Move = 0, eOT_Scale, eOT_Rotate, eOT_Selecte}; // OT = operator type

	public:
		long long nSign;	// 图层签名 - 唯一
		void* lpUserParam;

	public:
		bool bEnable, bFocus, bBackground;

	public:
		array<double, 3> x, y; // 在背景图中的中心点坐标 // [1] 用于补偿鼠标拖动时带来的延时误差  // p = p[0]+p[1]
							// [2] 用于标记是否为鼠标拖动 // 0 非鼠标拖动
	public:
		int nTop, nLeft; // 左上角坐标
		int nWidth, nHeight; // 高宽

	public:
		double dRotated;
		double dContrastRotio;		//对比度
		array<double, 3> dRotating; // [1] 用于补偿鼠标拖动时带来的延时误差  // angle = r[0]+r[1]
									// [2] 用于标记是否为鼠标拖动	// 0.0 非鼠标拖动
	public:
		array<double, 2> dScaled; // hor | ver
		array<double, 5> dScaling;// scaling = hro[0]*[2]  ver[1]*[3] 
								  // [0] [3] 用于补偿鼠标拖动时带来的延时误差 
								  // [4] 用于标记是否为鼠标拖动 // 0.0 非鼠标拖动

		array<int, 2> nScaledSize;// 用于dScaling[2]、[3]计算 

	public:
		CString rName;
		double dOpacity;
		bool bAssociated;
		array<bool, 4> bOpt;// 功能类型	// 互斥关系

	public:
		bool bTIFF;			// 来自于贴图tif // 默认为CMYK与bgra互斥
		string strTIFF;		// 对于tif的补偿

	public:
		ImPtr::CImPtr cImage;
		
	public:
		array<long long, 6> nTimer; // 该计时器记录着mat指针变动信号的发起时间
		ImPtr::CImPtr cBRGA, cPreview, cRotated, cScaled, cContrast;

	public:
		tagLayer();

	public:
		void Create();

	public:
		void GetBGRAParam(tagLayer& other);
		void GetPreviewParam(tagLayer& other);
		void GetRotateParam(tagLayer& other);
		void GetScaleParam(tagLayer& other);
		void GetContrastParam(tagLayer& other);

	public:
		void SyncBGRA(tagLayer& other);
		void SyncPreview(tagLayer& other);
		void SyncRotate(tagLayer& other);
		void SyncScale(tagLayer& other);
		void SyncContrast(tagLayer& other);

	public:
		void GetSyncInfo(tagLayer& other);	  // 外部获取信息同步接口
		void GetRenderParam(tagLayer& other); // 渲染线程同步接口

	public:
		void LeftChanged(int& _nLeft);
		void TopChanged(int& _nTop);
		void WidthChanged(int& _nWidth);
		void HeightChanged(int& _nHeight);
		void AngleChanged(double dAngle);
		void ScalChanged(double dHor, double dVer, bool bHor);

	public:
		void MoveEnable();
		void ScaleEnable();
		void RotateEnable();
		void SelecteEnable();

	public:
		void Moving(int& _ox, int& _oy);
		void Move(int& _ox, int& _oy);
		void Scaling(int& _ox, int& _oy);
		void Scale(int& _ox, int& _oy);
		void Scale(double& dScale);
		void ScaleEx(double& dScale);
		void Rotating(double dAngle);
		void Rotate(double dAngle);
		void Contrast(double dAngle);
	public:
		Point2d Center();
		double Angle();
		array<double, 2> ScalingFactor();

	public:
		int CurOpt();

	private:
		void Opt(int nOpt);
		long long Sign();
	};
}