#pragma once  // 防止头文件重复包含

// 包含必要的头文件
#include "resource.h"        // 资源ID定义
#include "MatchTh.h"         // 模板匹配线程
#include "CalibratorDlg.h"   // 校准对话框
#include "./Control/StyleDialog.h"  // 自定义样式对话框基类

// DALSA相机控制对话框类
class CDALSADlg : public StyleDialog
{
    DECLARE_DYNAMIC(CDALSADlg)  // MFC运行时类型识别支持

public:
    CDALSADlg(CWnd* pParent = nullptr);  // 构造函数
    virtual ~CDALSADlg();                // 析构函数

    enum { IDD = IDD_VISIONLOCALIZATION_DIALOG };  // 对话框资源ID

protected:
    virtual BOOL OnInitDialog();         // 对话框初始化
    virtual void DoDataExchange(CDataExchange* pDX);  // 数据交换

private:
    void OnConnect();          // 连接设备
    void OnBoardsSelChange();  // 板卡选择变更处理

private:
    // 设备状态变量
    int m_nCamera;            // 当前相机索引
    int m_nFrameIdx;          // 当前帧索引
    CString m_rBoard;         // 板卡名称
    CString m_rConfig;        // 配置信息
    CString m_rModel;         // 型号信息

private:
    // 界面控件
    CComboBox m_cbBoard;      // 板卡选择下拉框
    CComboBox m_cbCamera;     // 相机选择下拉框

protected:
    vector<CString> m_vCbBoards;    // 可用板卡列表
    vector<CString> m_vCbCameras;  // 可用相机列表

protected:
    virtual void ExtTriggerFrameCount(int& nExtTriggerFrameCount) = 0;  // 外部触发帧计数(纯虚函数)

private:
    afx_msg void OnDestroy();  // 窗口销毁处理

private:
    // 消息处理函数
    afx_msg LRESULT OnArrivalMsg(WPARAM wparam, LPARAM lparam);      // 设备到达消息
    afx_msg LRESULT OnRemoveMsg(WPARAM wparam, LPARAM lparam);        // 设备移除消息
    afx_msg LRESULT OnConnectMsg(WPARAM wparam, LPARAM lparam);      // 连接消息
    afx_msg LRESULT OnDisconnectMsg(WPARAM wparam, LPARAM lparam);   // 断开连接消息
    afx_msg LRESULT OnGrabMsg(WPARAM wparam, LPARAM lparam);         // 抓取图像消息
    afx_msg LRESULT OnFreezeMsg(WPARAM wparam, LPARAM lparam);       // 冻结图像消息
    afx_msg LRESULT OnCaptureMsg(WPARAM wparam, LPARAM lparam);      // 捕获图像消息
    afx_msg LRESULT OnResetFrameIdxMsg(WPARAM wparam, LPARAM lparam); // 重置帧索引消息

protected:
    afx_msg void OnClickedGrab();   // 抓取按钮点击
    afx_msg void OnClickedFreeze();  // 冻结按钮点击

    DECLARE_MESSAGE_MAP()  // 声明消息映射表
};