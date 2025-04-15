#pragma once  // 确保头文件只被编译一次，防止重复包含
#include "resource.h"  // 包含资源文件头文件
#include "ImControl2.h"  // 包含 ImControl2 头文件
#include "CalibratorDlg.h"  // 包含 CalibratorDlg 头文件
#include "FeatureTh.h"  // 包含 FeatureTh 头文件
#include "VisionLocalizationDlg.h"  // 包含 VisionLocalizationDlg 头文件

using namespace std;  // 使用标准命名空间
using namespace cv;  // 使用 OpenCV 命名空间

/**
 * @brief CFeatureDlg 类，继承自 CDialog 和 CFeatureTh，用于处理特征对话框相关功能
 */
class CFeatureDlg : public CDialog, public CFeatureTh
{
	DECLARE_DYNAMIC(CFeatureDlg)  // 声明动态创建类所需的宏

public:
	/**
	 * @brief 构造函数
	 * @param pParent 父窗口指针，默认为 NULL
	 * @param cameraPath 相机路径，默认为 NULL
	 */
	CFeatureDlg(CWnd* pParent = NULL, CString cameraPath = NULL);
	/**
	 * @brief 析构函数
	 */
	virtual ~CFeatureDlg();

	/**
	 * @brief 更新预览内容
	 */
	void UpdatePreview();

	/**
	 * @brief 更新预值
	 */
	void UpdatePreValue();

	/**
	 * @brief 对话框资源 ID
	 */
	enum {IDD = IDD_DLG_FEATURE };

protected:
	/**
	 * @brief 数据交换函数，用于在对话框控件和成员变量之间交换数据
	 * @param pDX 数据交换对象指针
	 */
	virtual void DoDataExchange(CDataExchange* pDX);

public:
	/**
	 * @brief 初始化参数
	 * @param json 包含参数的 JSON 项映射
	 * @param id 当前 ID
	 */
	void InitParam(std::map<int, jsonItem> json, int id);

	/**
	 * @brief 更新参数
	 */
	void UpdateParam();

	/**
	 * @brief 处理水平滚动条消息
	 * @param nSBCode 滚动条通知码
	 * @param nPos 滚动条位置
	 * @param pScrollBar 滚动条控件指针
	 */
	void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);

private:
	CCalibratorDlg::tagCalibrator m_tCameraParam;  // 相机校准参数
	Mat* m_lpChart;  // 指向 OpenCV 矩阵的指针，用于图表数据
	Mat readImage;  // 读取的图像数据

	ImPtr::CImPtr m_cTIFF;  // TIFF 图像指针对象

private:
	int m_nSent;  // 发送计数
	int m_nFrameIdx;  // 帧索引
	int m_nExtTriggerFrameCount;  // 外部触发帧计数
	int m_nChart[2];  // 图表数据数组
	CString m_rTIFF;  // TIFF 文件路径
	CString m_rTIFFName;  // TIFF 文件名
	CString m_rTemp;  // 临时文件路径
	CString m_rParam;  // 参数文件路径
	CString m_rSave;  // 保存文件路径
	CString m_rSaveName;  // 保存文件名
	CString m_rMask;  // 掩码文件路径
	CString m_rSamp;  // 样本文件路径
	CString m_rScore;  // 分数信息
	CString m_rSpeed;  // 速度信息
	CString m_rDistance;  // 距离信息
	CString m_rCarSpeed;  // 车辆速度信息
	CString m_rVoltage;  // 电压信息
	CString m_rPreValue;  // 预值信息
	CString m_rOffsetY;  // Y 轴偏移信息
	CString m_rOffsetX;  // X 轴偏移信息

	char acCfgFile[MAX_PATH];  // 配置文件路径

	std::map<int, jsonItem> m_mModel;  // 模型参数映射
	int curID = 0;  // 当前 ID

private:
	CEdit m_etChart[2];  // 图表编辑框控件数组
	CEdit m_etFrame;  // 帧编辑框控件
	CButton m_btGrab;  // 抓取按钮控件
	CButton m_btFreeze;  // 冻结按钮控件
	CButton m_btScan;  // 扫描按钮控件
	CButton m_btSend;  // 发送按钮控件
	CEdit m_etPrinter[3];  // 打印机编辑框控件数组
	CComboBox m_cbSpeed;  // 速度组合框控件
	CSliderCtrl m_csPre;  // 预值滑动条控件

private:
	ImControl2::CImControl m_cImControl;  // ImControl2 控件对象

protected:
	/**
	 * @brief 对话框初始化函数，在对话框创建后调用
	 * @return 初始化成功返回 TRUE，失败返回 FALSE
	 */
	virtual BOOL OnInitDialog();

private:
	/**
	 * @brief 处理抓取按钮点击事件
	 */
	afx_msg void OnClickedGrab();
	/**
	 * @brief 处理冻结按钮点击事件
	 */
	afx_msg void OnClickedFreeze();
	/**
	 * @brief 处理扫描按钮点击事件
	 */
	afx_msg void OnClickedScan();
	/**
	 * @brief 处理发送按钮点击事件
	 */
	afx_msg void OnClickedSend();

private:
	/**
	 * @brief 处理捕获消息
	 * @param wparam 消息的 WPARAM 参数
	 * @param lparam 消息的 LPARAM 参数
	 * @return 处理结果
	 */
	afx_msg LRESULT OnCaptureMsg(WPARAM wparam, LPARAM lparam);
	/**
	 * @brief 处理转换消息
	 * @param wparam 消息的 WPARAM 参数
	 * @param lparam 消息的 LPARAM 参数
	 * @return 处理结果
	 */
	afx_msg LRESULT OnConvertMsg(WPARAM wparam, LPARAM lparam);

protected:
	/**
	 * @brief 处理抓取消息
	 * @param wparam 消息的 WPARAM 参数
	 * @param lparam 消息的 LPARAM 参数
	 * @return 处理结果
	 */
	afx_msg LRESULT OnGrabMsg(WPARAM wparam, LPARAM lparam);
	/**
	 * @brief 处理冻结消息
	 * @param wparam 消息的 WPARAM 参数
	 * @param lparam 消息的 LPARAM 参数
	 * @return 处理结果
	 */
	afx_msg LRESULT OnFreezeMsg(WPARAM wparam, LPARAM lparam);
	/**
	 * @brief 在消息被翻译和调度之前进行预处理
	 * @param pMsg 消息指针
	 * @return 处理成功返回 TRUE，否则返回 FALSE
	 */
	afx_msg BOOL PreTranslateMessage(MSG* pMsg);

	DECLARE_MESSAGE_MAP()  // 声明消息映射宏

public:
	/**
	 * @brief 处理 TIFF 文件按钮点击事件
	 */
	afx_msg void OnBnClickedBtFeatureTiff();
	/**
	 * @brief 处理临时文件按钮点击事件
	 */
	afx_msg void OnBnClickedBtFeatureTemp();
//	afx_msg void OnBnClickedBtFeatureParam();  // 注释掉的参数文件按钮点击事件处理函数
	/**
	 * @brief 处理加载临时文件按钮点击事件
	 */
	afx_msg void OnBnClickedBtFeatureLoadtemp();
	/**
	 * @brief 处理保存临时文件按钮点击事件
	 */
	afx_msg void OnBnClickedBtFeatureSavetemp();
	/**
	 * @brief 处理预值滑动条自定义绘制事件
	 * @param pNMHDR 通知消息头指针
	 * @param pResult 处理结果指针
	 */
	afx_msg void OnNMCustomdrawSliderPre(NMHDR* pNMHDR, LRESULT* pResult);
	/**
	 * @brief 处理加载掩码文件按钮点击事件
	 */
	afx_msg void OnBnClickedBtFeatureLoadmask();
	/**
	 * @brief 处理加载样本文件按钮点击事件
	 */
	afx_msg void OnBnClickedBtFeatureLoadsamp();
	/**
	 * @brief 处理匹配测试按钮点击事件
	 */
	afx_msg void OnBnClickedBtFeatureMatchtest();
};