// 防止头文件被重复包含
#pragma once
// 包含 DALSADlg 头文件
#include "DALSADlg.h"
// 包含 CalibratorDlg 头文件
#include "CalibratorDlg.h"

/**
 * @brief CCameraParamDlg 类，继承自 CDALSADlg
 * 该类用于处理相机参数对话框相关的操作
 */
class CCameraParamDlg : public CDALSADlg
{
    // 声明动态创建机制
    DECLARE_DYNAMIC(CCameraParamDlg)

public:
    /**
     * @brief 构造函数
     * @param pParent 父窗口指针，默认为 nullptr
     */
    CCameraParamDlg(CWnd* pParent = nullptr);
    /**
     * @brief 析构函数
     */
    virtual ~CCameraParamDlg();

protected:
    /**
     * @brief 数据交换函数，用于对话框和控件之间的数据交换
     * @param pDX 数据交换对象指针
     */
    virtual void DoDataExchange(CDataExchange* pDX);

private:
    /**
     * @brief 获取外部触发帧计数
     * @param nExtTriggerFrameCount 用于存储外部触发帧计数的引用
     */
    virtual void ExtTriggerFrameCount(int& nExtTriggerFrameCount) { nExtTriggerFrameCount = m_tCameraParam.nExtTriggerFrameCount; }

protected:
    // 校准器参数结构体对象
    CCalibratorDlg::tagCalibrator m_tCalibrator;

protected:
    // 相机参数的字符串表示
    CString m_rCameraParam;
    // 校准器参数的字符串表示
    CString m_rCalibratorParam;

protected:
    // 相机参数结构体对象
    CCalibratorDlg::tagCalibrator m_tCameraParam;

private:
    // 相机参数编辑框控件
    CEdit m_etCameraParam;
    // 校准器参数编辑框控件
    CEdit m_etCalibratorParam;
    // 相机参数按钮控件
    CButton m_btCameraParam;
    // 校准器参数按钮控件
    CButton m_btCalibratorParam;

protected:
    /**
     * @brief 对话框初始化函数，在对话框创建后调用
     * @return 初始化成功返回 TRUE，失败返回 FALSE
     */
    virtual BOOL OnInitDialog();

private:
    /**
     * @brief 处理相机参数按钮点击事件
     */
    afx_msg void OnClickedCameraParam();
    /**
     * @brief 处理校准器参数按钮点击事件
     */
    afx_msg void OnClickedCalibratorParam();

private:
    /**
     * @brief 处理相机参数消息
     * @param wparam 消息的附加参数
     * @param lparam 消息的附加参数
     * @return 消息处理结果
     */
    afx_msg LRESULT OnCameraParamMsg(WPARAM wparam, LPARAM lparam);
    /**
     * @brief 处理生成相机参数消息
     * @param wparam 消息的附加参数
     * @param lparam 消息的附加参数
     * @return 消息处理结果
     */
    afx_msg LRESULT OnMakeCameraParamMsg(WPARAM wparam, LPARAM lparam);

    // 声明消息映射机制
    DECLARE_MESSAGE_MAP()
};