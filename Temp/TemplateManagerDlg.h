#pragma once  // 确保头文件只被编译一次，防止重复包含
#include "afxdialogex.h"  // 包含 MFC 扩展对话框类的头文件
#include "./Control/VideoImageStatic.h"  // 包含自定义的视频图像静态控件类的头文件
#include "./Temp/TemplateManager.h"  // 包含模板管理器类的头文件。模版管理器类用于管理模板信息
#include "../Control/StyleDialog.h"  // 包含自定义样式对话框类的头文件
#include "../Control/StyleButton.h"  // 包含自定义样式按钮类的头文件

// TemplateManagerDlg 对话框用于管理模板信息
// 继承自 StyleDialog，用于自定义样式的对话框
class TemplateManagerDlg : public StyleDialog
{
    DECLARE_DYNAMIC(TemplateManagerDlg)  // 声明动态创建类的支持MFC DECLARE_DYNAMIC宏表示该类支持动态创建

public:
    TemplateManagerDlg(CWnd* pParent = nullptr);   // 标准构造函数CWnd* pParent = nullptr表示父窗口指针，默认为nullptr
    virtual ~TemplateManagerDlg();  // 虚析构函数，确保正确释放派生类对象

    // 设置选中的列表项索引
    void setSelListItem(int iSelItem) { m_iSelListItem = iSelItem; }//int iSelItem表示要设置的选中的列表项索引 m_iSelListItem表示当前选中的列表项索引
    // 设置采集图像的 Mat 对象
    void setCollectImageMat(Mat& tCollectMat) { m_tMainDlgMat = tCollectMat;}//Mat& tCollectMat表示要设置的采集图像的 Mat 对象 m_tMainDlgMat表示当前采集图像的 Mat 对象

// 对话框数据
#ifdef AFX_DESIGN_TIME//如果在设计时使用对话框，AFX_DESIGN_TIME宏为真，否则为假
    enum { IDD = IDD_TEMPLATE_DIALOG };  // 设计时使用的对话框资源 ID
#endif

protected:
    // DDX/DDV 支持，用于对话框数据交换和验证
    virtual void DoDataExchange(CDataExchange* pDX);//DDX/DDV是MFC提供的用于对话框数据交换和验证的宏，CDataExchange* pDX表示数据交换对象指针  

    DECLARE_MESSAGE_MAP()  // 声明消息映射机制是MCF提供的用于处理消息的机制，消息映射机制是一种将消息映射到处理函数的机制，通过消息映射机制，可以将消息映射到处理函数，从而实现消息的处理

protected:
    // 对话框初始化函数，在对话框显示前调用
    virtual BOOL OnInitDialog();//初始化函数
    // 预处理消息函数，在消息分发前处理特定消息
    virtual BOOL PreTranslateMessage(MSG* pMsg);//MSG* pMsg表示消息指针  预处理消息函数是MFC提供的用于预处理消息的函数，通过预处理消息函数，可以对特定消息进行预处理，从而实现消息的预处理
    // 处理帮助信息的函数
    virtual BOOL OnHelpInfo(HELPINFO* pHelpInfo);//HELPINFO* pHelpInfo表示帮助信息指针  处理帮助信息的函数是MFC提供的用于处理帮助信息的函数，通过处理帮助信息的函数，可以对帮助信息进行处理，从而实现帮助信息的处理

public:
    // 处理重置模板消息的消息处理函数
    afx_msg LRESULT OnResetTemplate(WPARAM wparam, LPARAM lparam);//afx_msg是MFC提供的用于处理消息的宏，LRESULT是MFC提供的用于表示消息处理结果的类型，WPARAM和LPARAM是MFC提供的用于表示消息参数的类型处理重置模板消息的消息处理函数是MFC提供的用于处理重置模板消息的消息处理函数，通过处理重置模板消息的消息处理函数，可以对重置模板消息进行处理，从而实现重置模板消息的处理
    // 处理删除按钮点击事件的消息处理函数
    afx_msg void OnBnClickedBtnMaskDel();//删除模版掩码参数
    // 处理创建按钮点击事件的消息处理函数
    afx_msg void OnBnClickedButtonCreate();
    // 处理删除按钮点击事件的消息处理函数
    afx_msg void OnBnClickedButtonDelete();
    // 处理保存按钮点击事件的消息处理函数
    afx_msg void OnBnClickedButtonSave();
    // 处理匹配检查按钮点击事件的消息处理函数
    afx_msg void OnBnClickedBtnMatchCheck();
    // 处理选择产品按钮点击事件的消息处理函数
    afx_msg void OnBnClickedBtnProductChoose();
    // 处理选择模板按钮点击事件的消息处理函数
    afx_msg void OnBnClickedBtnTempChoose();
    // 处理选择模版掩码按钮点击事件的消息处理函数
    afx_msg void OnBnClickedBtnMaskChoose();
    // 处理选择图像按钮点击事件的消息处理函数
    afx_msg void OnBnClickedBtnImageChoose();
    // 处理模板列表选择改变事件的消息处理函数
    afx_msg void OnLbnSelchangeTemplateList();
    // 处理对比度滑块自定义绘制事件的消息处理函数
    afx_msg void OnNMCustomdrawSliderConstrastRatio(NMHDR* pNMHDR, LRESULT* pResult);
    // 处理模板最大化按钮点击事件的消息处理函数
    afx_msg void OnBnClickedTempMaxButton();
    // 处理特征微调按钮值改变事件的消息处理函数
    afx_msg void OnDeltaposSpinFeature(NMHDR* pNMHDR, LRESULT* pResult);
    // 处理图层组合框选择改变事件的消息处理函数
    afx_msg void OnSelchangeComboLayer();
    // 处理制作遮罩按钮点击事件的消息处理函数
    afx_msg void OnBnClickedBtnMaskMake();
    // 处理制作模板按钮点击事件的消息处理函数
    afx_msg void OnBnClickedBtnTemplateMake();



	//新添加鼠标滚动事件处理函数
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);//为什么返回值为BOOL类型？因为OnMouseWheel函数是一个消息处理函数，它的返回值类型为BOOL，用于指示消息是否被处理。如果消息被处理，返回值为TRUE；如果消息未被处理，返回值为FALSE。

private:
    // 视频图像静态控件对象，用于显示遮罩
    VideoImageStatic m_tVideoMaskStatic;
    // 视频图像静态控件对象，用于显示图像
    VideoImageStatic m_tVideoImageStatic;
    // 视频图像静态控件对象，用于显示模板
    VideoImageStatic m_tVideoTemplateStatic;
    // 模板管理器指针，初始化为空
    TemplateManager* m_ptTemplateManager = nullptr;
    // 用于检查的 Mat 对象
    Mat m_tCheckMat;
    // 图像的 Mat 对象
    Mat m_tImageMat;
    // 主对话框的 Mat 对象
    Mat m_tMainDlgMat;
    // 选中的列表项索引，初始化为 0
    int m_iSelListItem = 0;

    // 填充模板列表
    void fillTemplateList();
    // 填充模板参数
    void fillTemplateParam();
    // 填充模板信息
    void fillTemplateInfo();
    // 填充模板特征
    void fillTemplateFeature(Size tTemplatesize, int iPyrLayer);

private:
    // 模板列表框控件对象
    CListBox m_tListBoxTemplate;
    // 名称编辑框控件对象
    CEdit m_tEdtName;
    // 产品编辑框控件对象
    CEdit m_tEdtProduct;
    // 模板文件编辑框控件对象
    CEdit m_tEdtTempFile;
    // 遮罩文件编辑框控件对象
    CEdit m_tEdtMaskFile;
    // 图像文件编辑框控件对象
    CEdit m_tEdtImageFile;
    // 分数编辑框控件对象
    CEdit m_tEdtScore;
    // 最大重叠编辑框控件对象
    CEdit m_tEdtMaxOverLap;
    // X 偏移量编辑框控件对象
    CEdit m_tEdtOffsetX;
    // Y 偏移量编辑框控件对象
    CEdit m_tEdtOffsetY;
    // 打印速度编辑框控件对象
    CEdit m_tEdtPrintSpeed;
    // 打印高度编辑框控件对象
    CEdit m_tEdtPrintHei;
    // 灯光电压编辑框控件对象
    CEdit m_tEdtLightVol;
    // 特征编辑框控件对象
    CEdit m_tEdtFeature;
    // 对比度静态文本控件对象
    CStatic m_tStaticContrastRatio;
    // 图层组合框控件对象
    CComboBox m_tCmbLayer;
    // 搜索类型组合框控件对象
    CComboBox m_tCmbSearchType;
    // 对比度滑块控件对象
    CSliderCtrl m_tSliderContrastRatio;
    // 匹配结果列表控件对象
    CListCtrl m_tListCtrlMatchResult;
    // 特征微调按钮控件对象
    CSpinButtonCtrl m_tSpinBtnFeature;
    // 最大化按钮控件对象
    CButton m_tMaxBtn;
    // 形状匹配复选框控件对象
    CButton m_tCheckShapeMatch;
    // 图像显示复选框控件对象
    CButton m_tCheckBoxImageShow;
    // 自定义样式新建按钮控件对象
    StyleButton m_tNewBtn;
    // 自定义样式删除按钮控件对象
    StyleButton m_tDelBtn;
    // 自定义样式保存按钮控件对象
    StyleButton m_tSaveBtn;
    // 自定义样式检查按钮控件对象
    StyleButton m_tCheckBtn;
    // 自定义样式选择产品按钮控件对象
    StyleButton m_tProductChooseBtn;
    // 自定义样式选择模板按钮控件对象
    StyleButton m_tTempChooseBtn;
    // 自定义样式制作模板按钮控件对象
    StyleButton m_tTempMakeBtn;
    // 自定义样式选择遮罩按钮控件对象
    StyleButton m_tMaskChooseBtn;
    // 自定义样式删除遮罩按钮控件对象
    StyleButton m_tMaskDelBtn;
    // 自定义样式制作遮罩按钮控件对象
    StyleButton m_tMaskMakeBtn;
    // 自定义样式图像按钮控件对象
    StyleButton m_tImageBtn;

    // 初始化控件样式
    void initStyle();
};
