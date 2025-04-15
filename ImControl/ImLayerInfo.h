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
		long long nSign;	// ͼ��ǩ�� - Ψһ
		void* lpUserParam;

	public:
		bool bEnable, bFocus, bBackground;

	public:
		array<double, 3> x, y; // �ڱ���ͼ�е����ĵ����� // [1] ���ڲ�������϶�ʱ��������ʱ���  // p = p[0]+p[1]
							// [2] ���ڱ���Ƿ�Ϊ����϶� // 0 ������϶�
	public:
		int nTop, nLeft; // ���Ͻ�����
		int nWidth, nHeight; // �߿�

	public:
		double dRotated;
		double dContrastRotio;		//�Աȶ�
		array<double, 3> dRotating; // [1] ���ڲ�������϶�ʱ��������ʱ���  // angle = r[0]+r[1]
									// [2] ���ڱ���Ƿ�Ϊ����϶�	// 0.0 ������϶�
	public:
		array<double, 2> dScaled; // hor | ver
		array<double, 5> dScaling;// scaling = hro[0]*[2]  ver[1]*[3] 
								  // [0] [3] ���ڲ�������϶�ʱ��������ʱ��� 
								  // [4] ���ڱ���Ƿ�Ϊ����϶� // 0.0 ������϶�

		array<int, 2> nScaledSize;// ����dScaling[2]��[3]���� 

	public:
		CString rName;
		double dOpacity;
		bool bAssociated;
		array<bool, 4> bOpt;// ��������	// �����ϵ

	public:
		bool bTIFF;			// ��������ͼtif // Ĭ��ΪCMYK��bgra����
		string strTIFF;		// ����tif�Ĳ���

	public:
		ImPtr::CImPtr cImage;
		
	public:
		array<long long, 6> nTimer; // �ü�ʱ����¼��matָ��䶯�źŵķ���ʱ��
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
		void GetSyncInfo(tagLayer& other);	  // �ⲿ��ȡ��Ϣͬ���ӿ�
		void GetRenderParam(tagLayer& other); // ��Ⱦ�߳�ͬ���ӿ�

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