#pragma once  // 防止头文件重复包含

// 依赖的头文件
#include "FleeceDlg.h"       // 打印控制基类
#include "ImControl.h"       // 图像控制模块
#include "ImControl2.h"      // 图像控制模块扩展
#include "./Temp/TemplateManager.h"  // 模板管理模块
#include "./Control/StyleButton.h"    // 自定义样式按钮

// 主对话框类定义
class CVisionLocalizationDlg : public CFleeceDlg, public CMatchTh 
{
    DECLARE_DYNAMIC(CVisionLocalizationDlg)  // MFC动态创建支持
public:
    CVisionLocalizationDlg(CWnd* pParent = nullptr);  // 构造函数
    
    BOOL InitInstance();  // 初始化实例
    void SetPrinterEnable(bool ret);  // 设置打印功能状态
    void SetGatherEnable(bool ret);    // 设置采集功能状态

protected:
    virtual void DoDataExchange(CDataExchange* pDX);  // 数据交换方法
    BOOL SetTipText(UINT id, NMHDR* pTTTStruct, LRESULT* pResult);  // 设置工具提示文本

private:
    // 打印参数
    double m_dPrintInfos[3];  // 打印宽度/高度/视野(mm)
    
    // 文件路径相关
    CString m_tTemplateName;   // 当前使用的模板名称
    CString m_tTemplateFile;  // 模板文件路径
    CString m_tProductFile;    // 产品图像文件路径
    CString m_tProductName;    // 产品图像文件名
    
    // 状态显示
    CString m_tPrintHeiStatic;      // 打印高度显示
    CString m_tPrintSpeedStatic;   // 打印速度显示
    CString m_tLightVoltageStatic; // 光源电压显示
    
    // 图像采集相关
    Mat m_tCollectImgMat;         // 当前采集的图像
    vector<Mat> m_vCollectImgMat; // 采集的图像集合
    bool m_bCollectFlag = false;   // 采集状态标志 true是采集 false是打印
    int m_iCollectNum = 0;        // 采集计数
    
    TemplateManager* m_ptTemplateManager = nullptr;  // 模板管理器指针

protected:
    // UI控件
    HICON m_hIcon;  // 窗口图标
    ImControl2::CImControl m_cImControl;  // 图像显示控件
    CComboBox m_tCmbTemplateList;         // 模板选择下拉框
    CProgressCtrl m_tProgressPrint;        // 打印进度条
    CToolTipCtrl m_tToolTip;               // 工具提示
    
    // 功能按钮
    StyleButton m_tBtnPrint;       // 打印按钮
    StyleButton m_tBtnGather;      // 采集按钮
    StyleButton m_tModelBtn;       // 模板管理按钮
    StyleButton m_tSysParamBtn;    // 系统参数按钮
    StyleButton m_tSaveBgBtn;      // 保存背景按钮
    StyleButton m_tNewTempBtn;     // 新建模板按钮
    
    Mat *m_ptImageMat;  // 当前处理的图像矩阵指针

protected:
    // 重写方法
    virtual BOOL OnInitDialog();           // 对话框初始化
    virtual BOOL PreTranslateMessage(MSG* pMsg);  // 消息预处理
    virtual BOOL OnHelpInfo(HELPINFO* pHelpInfo); // 帮助信息处理

private:
    // 消息处理函数
    afx_msg void OnTimer(UINT_PTR nIDEvent);  // 定时器处理
    
    // 自定义消息处理
    afx_msg LRESULT OnTempMsg(WPARAM wparam, LPARAM lparam);  // 模板消息
    afx_msg LRESULT OnFrameMsg(WPARAM wparam, LPARAM lparam); // 图像帧消息
    afx_msg LRESULT OnMatrixMsg(WPARAM wparam, LPARAM lparam); // 矩阵计算消息
    afx_msg LRESULT OnConvertMsg(WPARAM wparam, LPARAM lparam); // 转换消息
    afx_msg LRESULT OnSentMsg(WPARAM wparam, LPARAM lparam);    // 发送消息
    afx_msg LRESULT OnScheduleMsg(WPARAM wparam, LPARAM lparam); // 调度消息
    afx_msg LRESULT OnImTcpMsg(WPARAM wparam, LPARAM lparam);   // TCP图像消息
    afx_msg LRESULT OnSaveBackground(WPARAM wparam, LPARAM lparam); // 保存背景消息
    afx_msg LRESULT OnUpadteCfgParamMsg(WPARAM wparam, LPARAM lparam); // 配置更新消息

    // 按钮点击事件
    afx_msg void OnClickedSend();      // 发送按钮
    afx_msg void OnClickedTIFF();      // TIFF文件选择
    afx_msg void OnClickedPrinter();   // 打印按钮
    afx_msg void OnClickedMakeFeature(); // 创建特征
    afx_msg void OnClickedGather();    // 采集按钮
    afx_msg void OnBnClickedBtMainCaptureImage(); // 捕获图像
    afx_msg void OnClickedSaveBackground(); // 保存背景
    afx_msg void OnBnClickedBtMainCutImage(); // 裁剪图像
    afx_msg void OnBnClickedBtMainNewtemp();  // 新建模板
    afx_msg void OnClickedSysParamBtn();     // 系统参数
    afx_msg void OnCbnSelchangeCbMainModel(); // 模板选择变更

    DECLARE_MESSAGE_MAP()  // 声明消息映射表

private:
    // 辅助方法
    void checkAndCreateDirectory();  // 检查并创建工作目录
    void fillData();                 // 填充初始数据
    void updateMatchParam();         // 更新匹配参数
    void saveConfigParam();          // 保存配置参数
    void updateConfigParam();        // 更新配置参数
    Mat captureImage();              // 捕获当前图像
};
