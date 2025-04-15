#pragma once  // 防止头文件重复包含
#include "CameraParamDlg.h"  // 包含相机参数对话框基类

// 打印控制对话框类
class CFleeceDlg : public CCameraParamDlg
{
    DECLARE_DYNAMIC(CFleeceDlg)  // MFC运行时类型识别支持

public:
    CFleeceDlg(CWnd* pParent = nullptr);  // 构造函数
    virtual ~CFleeceDlg();  // 析构函数
    void Stop();  // 停止打印操作

protected:
    virtual void DoDataExchange(CDataExchange* pDX);  // 数据交换方法

private:
    // 数据包校验方法
    bool PkgCheck(unsigned char* lpPkg, int nPkgSize, unsigned char& uType, wstring& strValue);

private:
    // 打印参数计算方法
    void FleeceParam();  // 计算打印参数
    int PrtWidth2Pulse();  // 打印宽度转脉冲数
    int PrtHeight2Pulse(); // 打印高度转脉冲数
    int Offset2Pulse();    // 偏移量转脉冲数
    int Origin2Pulse();    // 原点位置转脉冲数
    int Oy2Pulse();        // Y轴位置转脉冲数
    int Skip2Pulse();      // 跳过距离转脉冲数
    int Compensate2Pixel(int nCompensate); // 补偿值转像素
    void Forward();        // 前进控制

protected:
    // 状态控制变量
    tuple<unsigned char*, int, int, bool> m_vReqStatus;  // 状态请求数据
    tuple<unsigned char*, int, int, bool> m_vReqPos;     // 位置请求数据
    vector<tuple<unsigned char*, int, int, bool>> m_vFleece; // 打印数据队列
    tuple<unsigned char*, int, int, bool> m_vStop;        // 停止控制数据
    bool isStop = false;   // 停止标志
    int m_nExtSize = 0;    // 扩展尺寸

protected:
    virtual BOOL OnInitDialog();  // 对话框初始化
    void setSkipExtSize(int iSize) { m_nExtSize = iSize; }  // 设置跳过尺寸

private:
    // 消息处理函数
    afx_msg LRESULT OnFleeceRestrictMsg(WPARAM wparam, LPARAM lparam);  // 限制消息
    afx_msg LRESULT OnFleeceBeginMsg(WPARAM wparam, LPARAM lparam);     // 开始消息
    afx_msg LRESULT OnFleeceEndMsg(WPARAM wparam, LPARAM lparam);       // 结束消息
    afx_msg LRESULT OnFleeceSendMsg(WPARAM wparam, LPARAM lparam);     // 发送消息
    afx_msg LRESULT OnFleeceSendStopMsg(WPARAM wparam, LPARAM lparam);  // 发送停止消息
    afx_msg LRESULT OnFleeceRecvMsg(WPARAM wparam, LPARAM lparam);      // 接收消息
    afx_msg LRESULT OnFleecePrtMsg(WPARAM wparam, LPARAM lparam);       // 打印消息

    DECLARE_MESSAGE_MAP()  // 声明消息映射表
};
