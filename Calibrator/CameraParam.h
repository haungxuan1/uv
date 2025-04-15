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
	public: // �����������ָ��׼׼����
		int nType;	// �÷ݲ������� // ��������Ƿ���� // �ϵ�У׼
		double dAssessment; // ������׼1 // ������������� // ��ֵԽ�ӽ�0���������Խ��

	public: // ����������ڲ���У׼ // ���⿪�ţ������������
		int nExtTriggerFrameCount; // ֡/����// CCameraDlg ����
		int nForeground;			// ǰ����ֵ // CForegroundDlg
		int nErode;					// ��ʴ	// CReferenceDlg
		int nDilate;				// ���� // CReferenceDlg
		int nGaussian;				// ��˹ // CReferenceDlg
		int nPixelInPass;			// ����/pass // CAlignDlg	// 543
		int nPassInStride;			// pass/���� // CAlignDlg
		array<float, 2> vWH1;		// �ɼ�ͼ���С // CAlignDlg	// 30000 * 13056
		array<float, 2> vWH2;		// ������ͼ���С // CAlignDlg
		array<int, 2> vWH2Ex;		// ������ͼ��ʵ�ʴ�С
		array<double, 2> vDPI1;		// ��ӡdpi		// CDPIDlg	635.0 * 1200.0
		array<double, 2> vDPI2;		// ���dpi 
		array<int, 2> vDist;		// CChartDlg	// 100140, 30900
		array<double, 9> M;			// �任����M
		array<int, 4> vFill;		// �任ǰ���ϵ��

	public: // �����������У׼������ʱ�洢 // �����⿪��
		int nBlock;	// ǰ����ֵ��ȡ�ֿ��� // CForegroundDlg
		int nDensity; // �ܶ� // CReferenceDlg
		array<int, 4> nReference; // ������λ�� // CReferenceDlg
		array<Point2f, 4> vVertex1;	// �任ǰ4������Ϣ
		array<Point2f, 4> vVertex2; // �任��4������Ϣ

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