// TemplateManagerDlg.cpp: 实现文件
//

#include "pch.h"
#include "VisionLocalization.h"
#include "afxdialogex.h"
#include "../Local Setting/Transform.h"
#include "TemplateManagerDlg.h"
#include "MakeTemplateMaskDialog.h"
#include "MakeTemplateDialog.h"
#include "../def.h"
#include "../StepDialog.h"
#include "../utils/WinUtil.h"
#include "../utils/FileUtil.h"
#include "../utils/TiffUtil.h"
#include "../utils/IniUtil.h"

#ifndef USE_HALCON
#include "FastMatch.h"
#else
#include "FastHalMatch.h"
#endif

using namespace Transform;

IMPLEMENT_DYNAMIC(TemplateManagerDlg, StyleDialog)// 动态创建类所需的宏MFC

TemplateManagerDlg::TemplateManagerDlg(CWnd* pParent /*=nullptr*/)
	: StyleDialog(IDD_TEMPLATE_DIALOG, pParent)
{

}

TemplateManagerDlg::~TemplateManagerDlg()
{
}

// 该函数用于在对话框和对话框中的控件之间交换数据，是MFC框架中数据交换的核心函数
void TemplateManagerDlg::DoDataExchange(CDataExchange* pDX)
{
    // 调用基类的DoDataExchange函数，确保基类的数据交换逻辑正常执行
    StyleDialog::DoDataExchange(pDX);

    // 将IDC_TEMPLATE_LIST对应的控件与成员变量m_tListBoxTemplate关联
    DDX_Control(pDX, IDC_TEMPLATE_LIST, m_tListBoxTemplate);
    // 将IDC_EDT_NAME对应的控件与成员变量m_tEdtName关联
    DDX_Control(pDX, IDC_EDT_NAME, m_tEdtName);
    // 将IDC_EDT_PRODUCT对应的控件与成员变量m_tEdtProduct关联
    DDX_Control(pDX, IDC_EDT_PRODUCT, m_tEdtProduct);
    // 将IDC_EDIT_TEMPLATE对应的控件与成员变量m_tEdtTempFile关联
    DDX_Control(pDX, IDC_EDIT_TEMPLATE, m_tEdtTempFile);
    // 将IDC_EDIT_MASK对应的控件与成员变量m_tEdtMaskFile关联
    DDX_Control(pDX, IDC_EDIT_MASK, m_tEdtMaskFile);
    // 将IDC_COMBO_LAYER对应的控件与成员变量m_tCmbLayer关联
    DDX_Control(pDX, IDC_COMBO_LAYER, m_tCmbLayer);
    // 将IDC_COMBO_SEARCH_TYPE对应的控件与成员变量m_tCmbSearchType关联
    DDX_Control(pDX, IDC_COMBO_SEARCH_TYPE, m_tCmbSearchType);
    // 将IDC_EDT_SCORE对应的控件与成员变量m_tEdtScore关联
    DDX_Control(pDX, IDC_EDT_SCORE, m_tEdtScore);
    // 将IDC_EDT_OFFSET_X对应的控件与成员变量m_tEdtOffsetX关联
    DDX_Control(pDX, IDC_EDT_OFFSET_X, m_tEdtOffsetX);
    // 将IDC_EDT_OFFSET_Y对应的控件与成员变量m_tEdtOffsetY关联
    DDX_Control(pDX, IDC_EDT_OFFSET_Y, m_tEdtOffsetY);
    // 将IDC_EDT_PRINT_SPEED对应的控件与成员变量m_tEdtPrintSpeed关联
    DDX_Control(pDX, IDC_EDT_PRINT_SPEED, m_tEdtPrintSpeed);
    // 将IDC_EDT_PRINT_HEI对应的控件与成员变量m_tEdtPrintHei关联
    DDX_Control(pDX, IDC_EDT_PRINT_HEI, m_tEdtPrintHei);
    // 将IDC_EDT_LIGHT对应的控件与成员变量m_tEdtLightVol关联
    DDX_Control(pDX, IDC_EDT_LIGHT, m_tEdtLightVol);
    // 将IDC_CHECK_SHAPE_MATCH对应的控件与成员变量m_tCheckShapeMatch关联
    DDX_Control(pDX, IDC_CHECK_SHAPE_MATCH, m_tCheckShapeMatch);
    // 将IDC_SLIDER_CONSTRAST_RATIO对应的控件与成员变量m_tSliderContrastRatio关联
    DDX_Control(pDX, IDC_SLIDER_CONSTRAST_RATIO, m_tSliderContrastRatio);
    // 将IDC_EDT_MAX_OVER_LAP对应的控件与成员变量m_tEdtMaxOverLap关联
    DDX_Control(pDX, IDC_EDT_MAX_OVER_LAP, m_tEdtMaxOverLap);
    // 将IDC_EDT_IMAGE对应的控件与成员变量m_tEdtImageFile关联
    DDX_Control(pDX, IDC_EDT_IMAGE, m_tEdtImageFile);
    // 将IDC_STATIC_CR对应的控件与成员变量m_tStaticContrastRatio关联
    DDX_Control(pDX, IDC_STATIC_CR, m_tStaticContrastRatio);
    // 将IDC_LIST_RESULT对应的控件与成员变量m_tListCtrlMatchResult关联
    DDX_Control(pDX, IDC_LIST_RESULT, m_tListCtrlMatchResult);
    // 将IDC_TEMP_MAX_BUTTON对应的控件与成员变量m_tMaxBtn关联
    DDX_Control(pDX, IDC_TEMP_MAX_BUTTON, m_tMaxBtn);
    // 将IDC_BUTTON_CREATE对应的控件与成员变量m_tNewBtn关联
    DDX_Control(pDX, IDC_BUTTON_CREATE,   m_tNewBtn);
    // 将IDC_BUTTON_DELETE对应的控件与成员变量m_tDelBtn关联
    DDX_Control(pDX, IDC_BUTTON_DELETE,   m_tDelBtn);
    // 将IDC_BUTTON_SAVE对应的控件与成员变量m_tSaveBtn关联
    DDX_Control(pDX, IDC_BUTTON_SAVE,     m_tSaveBtn);
    // 将IDC_BTN_MATCH_CHECK对应的控件与成员变量m_tCheckBtn关联
    DDX_Control(pDX, IDC_BTN_MATCH_CHECK, m_tCheckBtn);
    // 将IDC_BTN_PRODUCT_CHOOSE对应的控件与成员变量m_tProductChooseBtn关联
    DDX_Control(pDX, IDC_BTN_PRODUCT_CHOOSE, m_tProductChooseBtn);
    // 将IDC_BTN_TEMP_CHOOSE对应的控件与成员变量m_tTempChooseBtn关联
    DDX_Control(pDX, IDC_BTN_TEMP_CHOOSE,    m_tTempChooseBtn);
    // 将IDC_BTN_TEMPLATE_MAKE对应的控件与成员变量m_tTempMakeBtn关联
    DDX_Control(pDX, IDC_BTN_TEMPLATE_MAKE,  m_tTempMakeBtn);
    // 将IDC_BTN_MASK_CHOOSE对应的控件与成员变量m_tMaskChooseBtn关联
    DDX_Control(pDX, IDC_BTN_MASK_CHOOSE,    m_tMaskChooseBtn);
    // 将IDC_BTN_MASK_DEL对应的控件与成员变量m_tMaskDelBtn关联
    DDX_Control(pDX, IDC_BTN_MASK_DEL,       m_tMaskDelBtn);
    // 将IDC_BTN_MASK_MAKE对应的控件与成员变量m_tMaskMakeBtn关联
    DDX_Control(pDX, IDC_BTN_MASK_MAKE,      m_tMaskMakeBtn);
    // 将IDC_BTN_IMAGE_CHOOSE对应的控件与成员变量m_tImageBtn关联
    DDX_Control(pDX, IDC_BTN_IMAGE_CHOOSE,   m_tImageBtn);
    // 将IDC_EDT_FEATURE对应的控件与成员变量m_tEdtFeature关联
    DDX_Control(pDX, IDC_EDT_FEATURE, m_tEdtFeature);
    // 将IDC_SPIN_FEATURE对应的控件与成员变量m_tSpinBtnFeature关联
    DDX_Control(pDX, IDC_SPIN_FEATURE, m_tSpinBtnFeature);
    // 将IDC_CHECK_IMAGE_SHOW对应的控件与成员变量m_tCheckBoxImageShow关联
    DDX_Control(pDX, IDC_CHECK_IMAGE_SHOW, m_tCheckBoxImageShow);
}
// 开始消息映射，将消息与对应的处理函数关联起来，TemplateManagerDlg 是当前类，StyleDialog 是基类
BEGIN_MESSAGE_MAP(TemplateManagerDlg, StyleDialog)
	// 当 IDC_BTN_MASK_DEL 按钮被点击时，调用 TemplateManagerDlg 类的 OnBnClickedBtnMaskDel 函数
	ON_BN_CLICKED(IDC_BTN_MASK_DEL, &TemplateManagerDlg::OnBnClickedBtnMaskDel)
	// 当 IDC_BUTTON_CREATE 按钮被点击时，调用 TemplateManagerDlg 类的 OnBnClickedButtonCreate 函数
	ON_BN_CLICKED(IDC_BUTTON_CREATE, &TemplateManagerDlg::OnBnClickedButtonCreate)
	// 当 IDC_TEMPLATE_LIST 列表框的选中项发生变化时，调用 TemplateManagerDlg 类的 OnLbnSelchangeTemplateList 函数
	ON_LBN_SELCHANGE(IDC_TEMPLATE_LIST, &TemplateManagerDlg::OnLbnSelchangeTemplateList)
	// 当 IDC_BUTTON_SAVE 按钮被点击时，调用 TemplateManagerDlg 类的 OnBnClickedButtonSave 函数
	ON_BN_CLICKED(IDC_BUTTON_SAVE, &TemplateManagerDlg::OnBnClickedButtonSave)
	// 当 IDC_BTN_PRODUCT_CHOOSE 按钮被点击时，调用 TemplateManagerDlg 类的 OnBnClickedBtnProductChoose 函数
	ON_BN_CLICKED(IDC_BTN_PRODUCT_CHOOSE, &TemplateManagerDlg::OnBnClickedBtnProductChoose)
	// 当 IDC_BTN_TEMP_CHOOSE 按钮被点击时，调用 TemplateManagerDlg 类的 OnBnClickedBtnTempChoose 函数
	ON_BN_CLICKED(IDC_BTN_TEMP_CHOOSE, &TemplateManagerDlg::OnBnClickedBtnTempChoose)
	// 当 IDC_BTN_MASK_CHOOSE 按钮被点击时，调用 TemplateManagerDlg 类的 OnBnClickedBtnMaskChoose 函数
	ON_BN_CLICKED(IDC_BTN_MASK_CHOOSE, &TemplateManagerDlg::OnBnClickedBtnMaskChoose)
	// 当 IDC_BTN_IMAGE_CHOOSE 按钮被点击时，调用 TemplateManagerDlg 类的 OnBnClickedBtnImageChoose 函数
	ON_BN_CLICKED(IDC_BTN_IMAGE_CHOOSE, &TemplateManagerDlg::OnBnClickedBtnImageChoose)
	// 当 IDC_BUTTON_DELETE 按钮被点击时，调用 TemplateManagerDlg 类的 OnBnClickedButtonDelete 函数
	ON_BN_CLICKED(IDC_BUTTON_DELETE, &TemplateManagerDlg::OnBnClickedButtonDelete)
	// 当 IDC_BTN_MATCH_CHECK 按钮被点击时，调用 TemplateManagerDlg 类的 OnBnClickedBtnMatchCheck 函数
	ON_BN_CLICKED(IDC_BTN_MATCH_CHECK, &TemplateManagerDlg::OnBnClickedBtnMatchCheck)
	// 当 IDC_SLIDER_CONSTRAST_RATIO 滑动条发生自定义绘制事件时，调用 TemplateManagerDlg 类的 OnNMCustomdrawSliderConstrastRatio 函数
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_CONSTRAST_RATIO, &TemplateManagerDlg::OnNMCustomdrawSliderConstrastRatio)
	// 当 IDC_TEMP_MAX_BUTTON 按钮被点击时，调用 TemplateManagerDlg 类的 OnBnClickedTempMaxButton 函数
	ON_BN_CLICKED(IDC_TEMP_MAX_BUTTON, &TemplateManagerDlg::OnBnClickedTempMaxButton)
	// 当 IDC_SPIN_FEATURE 微调框的值发生变化时，调用 TemplateManagerDlg 类的 OnDeltaposSpinFeature 函数
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_FEATURE, &TemplateManagerDlg::OnDeltaposSpinFeature)
	// 当 IDC_COMBO_LAYER 组合框的选中项发生变化时，调用 TemplateManagerDlg 类的 OnSelchangeComboLayer 函数
	ON_CBN_SELCHANGE(IDC_COMBO_LAYER, &TemplateManagerDlg::OnSelchangeComboLayer)
	// 当 IDC_BTN_MASK_MAKE 按钮被点击时，调用 TemplateManagerDlg 类的 OnBnClickedBtnMaskMake 函数
	ON_BN_CLICKED(IDC_BTN_MASK_MAKE, &TemplateManagerDlg::OnBnClickedBtnMaskMake)
	// 当 IDC_BTN_TEMPLATE_MAKE 按钮被点击时，调用 TemplateManagerDlg 类的 OnBnClickedBtnTemplateMake 函数
	ON_BN_CLICKED(IDC_BTN_TEMPLATE_MAKE, &TemplateManagerDlg::OnBnClickedBtnTemplateMake)

	// 当接收到 WM_RESET_TEMPLATE 自定义消息时，调用 TemplateManagerDlg 类的 OnResetTemplate 函数
	ON_MESSAGE(WM_RESET_TEMPLATE, OnResetTemplate)

	// 添加消息映射，当发生鼠标滚轮事件时，调用 TemplateManagerDlg 类的 OnMouseWheel 函数
	ON_WM_MOUSEWHEEL()

	// 结束消息映射
END_MESSAGE_MAP()

// 重写 OnInitDialog 函数，该函数在对话框初始化时被调用
BOOL TemplateManagerDlg::OnInitDialog()
{
    // 调用基类的 OnInitDialog 函数，确保基类的初始化逻辑正常执行
    StyleDialog::OnInitDialog();

    // 定义一个矩形区域对象，用于存储控件的位置和大小信息
    CRect tRect;

    // 获取 IDC_STATIC_IMAGE 控件的屏幕坐标矩形区域
    ::GetWindowRect(GetDlgItem(IDC_STATIC_IMAGE)->GetSafeHwnd(), tRect);
    // 将屏幕坐标转换为对话框客户区坐标
    ScreenToClient(tRect);

    // 创建 m_tVideoImageStatic 控件，设置其样式、位置和父窗口
    m_tVideoImageStatic.Create(L"", WS_CHILD | WS_VISIBLE | SS_NOTIFY,
        CRect(tRect.left, tRect.top, tRect.right, tRect.bottom), this, IDC_STATIC_IMAGE);
    // 设置 m_tVideoImageStatic 控件的背景颜色为黑色
    m_tVideoImageStatic.setBgColor(RGB(0, 0, 0));
    // 启用 m_tVideoImageStatic 控件的绘制功能
    m_tVideoImageStatic.setDrawEnable(true);

    // 创建一个空的 Mat 对象
    Mat tEmptyMat;
    // 将空的 Mat 对象设置给 m_tVideoImageStatic 控件显示
    m_tVideoImageStatic.setImage(tEmptyMat);

    // 获取 IDC_STATIC_TEMPLATE 控件的屏幕坐标矩形区域
    ::GetWindowRect(GetDlgItem(IDC_STATIC_TEMPLATE)->GetSafeHwnd(), tRect);
    // 将屏幕坐标转换为对话框客户区坐标
    ScreenToClient(tRect);

    // 创建 m_tVideoTemplateStatic 控件，设置其样式、位置和父窗口
    m_tVideoTemplateStatic.Create(L"", WS_CHILD | WS_VISIBLE | SS_NOTIFY, 
        CRect(tRect.left, tRect.top, tRect.right, tRect.bottom), this, IDC_STATIC_TEMPLATE);
    // 注释掉的代码，原本用于设置 m_tVideoTemplateStatic 控件的背景颜色为蓝色
    //m_tVideoTemplateStatic.setBgColor(RGB(0, 0, 255));

    // 获取 IDC_STATIC_MASK 控件的屏幕坐标矩形区域
    ::GetWindowRect(GetDlgItem(IDC_STATIC_MASK)->GetSafeHwnd(), tRect);
    // 将屏幕坐标转换为对话框客户区坐标
    ScreenToClient(tRect);

    // 创建 m_tVideoMaskStatic 控件，设置其样式、位置和父窗口
    m_tVideoMaskStatic.Create(L"", WS_CHILD | WS_VISIBLE | SS_NOTIFY,
        CRect(tRect.left, tRect.top, tRect.right, tRect.bottom), this, IDC_STATIC_MASK);

    // 加载图标资源，用于设置按钮图标
    HICON hBtnIcon = (HICON)LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_MAX_ICON), IMAGE_ICON, 38, 26, LR_DEFAULTCOLOR | LR_CREATEDIBSECTION);
    // 将加载的图标设置给 m_tMaxBtn 按钮
    m_tMaxBtn.SetIcon(hBtnIcon);

    // 调用初始化样式的函数，设置按钮的样式
    initStyle();

    // 获取 TemplateManager 的单例实例，并赋值给成员变量 m_ptTemplateManager
    m_ptTemplateManager = TemplateManager::instance();

    // 向 m_tCmbSearchType 组合框中插入搜索速度选项
    // 搜索个数大小：迅速 较快 适中 稍慢 较慢 缓慢
    m_tCmbSearchType.InsertString(0, L"快速");
    m_tCmbSearchType.InsertString(1, L"中速");
    m_tCmbSearchType.InsertString(2, L"慢速");
    m_tCmbSearchType.InsertString(3, L"精细");

    // 向 m_tCmbLayer 组合框中插入金字塔层数选项
    m_tCmbLayer.InsertString(0, L"2");
    m_tCmbLayer.InsertString(1, L"3");
    m_tCmbLayer.InsertString(2, L"4");

    // 设置 m_tCheckShapeMatch 复选框为未选中状态
    m_tCheckShapeMatch.SetCheck(FALSE);

    // 设置 m_tSpinBtnFeature 微调框的取值范围为 0 到 2000
    m_tSpinBtnFeature.SetRange32(0, 2000);
    // 设置 m_tSpinBtnFeature 微调框的基数为 10
    m_tSpinBtnFeature.SetBase(10);

    // 设置 m_tSliderContrastRatio 滑动条的取值范围为 0 到 500
    m_tSliderContrastRatio.SetRange(0, 500);
    // 设置 m_tSliderContrastRatio 滑动条的刻度频率为 1
    m_tSliderContrastRatio.SetTicFreq(1);
    // 在位置 100 处设置一个刻度
    m_tSliderContrastRatio.SetTic(100);
    // 将 m_tSliderContrastRatio 滑动条的初始位置设置为 100
    m_tSliderContrastRatio.SetPos(100);

    // 设置 m_tListCtrlMatchResult 列表控件的扩展样式，包括报表视图和网格线
    m_tListCtrlMatchResult.SetExtendedStyle(m_tListCtrlMatchResult.GetExtendedStyle() | LVS_REPORT | LVS_EX_GRIDLINES);
    
    // 向 m_tListCtrlMatchResult 列表控件中插入列标题
    m_tListCtrlMatchResult.InsertColumn(0, _T("序号"));
    m_tListCtrlMatchResult.InsertColumn(1, _T("分数"));
    m_tListCtrlMatchResult.InsertColumn(2, _T("角度"));

    // 获取 m_tListCtrlMatchResult 列表控件的屏幕坐标矩形区域
    m_tListCtrlMatchResult.GetWindowRect(tRect);

    // 计算每列的宽度
    int iWid = tRect.Width() / 3;

    // 设置 m_tListCtrlMatchResult 列表控件各列的宽度
    m_tListCtrlMatchResult.SetColumnWidth(0, iWid-30);
    m_tListCtrlMatchResult.SetColumnWidth(1, iWid);
    m_tListCtrlMatchResult.SetColumnWidth(2, iWid);

    // 获取 m_tListCtrlMatchResult 列表控件的表头控件
    CHeaderCtrl* ptHeaderCtrl = m_tListCtrlMatchResult.GetHeaderCtrl();

    // 如果表头控件存在
    if (ptHeaderCtrl != nullptr) {
        // 获取表头控件的列数
        int iColumnCount = m_tListCtrlMatchResult.GetHeaderCtrl()->GetItemCount();

        // 遍历每一列
        for (int i = 0; i < iColumnCount; i++) {
            // 定义一个表头项对象
            HDITEM tHdItem;

            // 设置要获取的表头项属性为格式
            tHdItem.mask = HDI_FORMAT;

            // 获取表头项的属性
            ptHeaderCtrl->GetItem(i, &tHdItem);

            // 设置表头项的文本对齐方式为居中
            tHdItem.fmt |= HDF_CENTER;

            // 更新表头项的属性
            ptHeaderCtrl->SetItem(i, &tHdItem);
        }
    }

    // 填充模板列表、模板参数和模板信息
    fillTemplateList();
    fillTemplateParam();
    fillTemplateInfo();

#ifdef USE_HALCON
    // 获取 m_tEdtName 编辑框中的文本
    string tNewName = WinUtil::getEditText(m_tEdtName);

    // 如果文本长度大于 0
    if (tNewName.length() > 0) {
        // 初始化 Halcon 相关参数
        PQ_InitParam(tNewName.c_str());
    }
#endif

    // 注释掉的代码，原本用于设置按钮的按下样式
    //m_tNewTempBtn.SetPressedStyle(CButtonST::BTNST_PRESSED_LEFTRIGHT);
    /* 注释掉的代码，原本用于子类化按钮控件，设置图标、背景颜色和边框样式
    m_tNewTempBtn.SubclassDlgItem(IDC_BUTTON_CREATE, this);
    m_tNewTempBtn.SetIcon(IDI_ICON);
    m_tNewTempBtn.SetColor(CButtonST::BTNST_COLOR_BK_OUT, RGB(62, 62, 66));
    m_tNewTempBtn.DrawBorder(FALSE, FALSE);*/

    // 返回 TRUE，表示已经处理了对话框的初始化消息
    return TRUE;
}

//初始化样式
void TemplateManagerDlg::initStyle()
{
	StyleButton* ptBtns[] = {
		&m_tNewBtn,           &m_tDelBtn,        &m_tSaveBtn,     &m_tCheckBtn,
		&m_tProductChooseBtn, &m_tTempChooseBtn, &m_tTempMakeBtn, &m_tMaskChooseBtn, 
		&m_tMaskDelBtn,       &m_tMaskMakeBtn,   &m_tImageBtn
	};

	for (int i = 0; i < sizeof(ptBtns) / sizeof(ptBtns[0]); i++) {
		ptBtns[i]->setRadius(8);

		ptBtns[i]->setBeginColor(Gdiplus::Color(255, 0, 30, 128));
		ptBtns[i]->setEndColor(Gdiplus::Color(255, 64, 200, 255));
		ptBtns[i]->setBorderColor(Gdiplus::Color(255, 50, 50, 50));
		ptBtns[i]->setTextColor(Gdiplus::Color(255, 250, 250, 250));
	}
}

/**
 * 快捷键设置
 */
BOOL TemplateManagerDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN) {// 处理键盘按下消息
		// 处理 ESC 键和 F1 键的消息
		if (pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE || pMsg->wParam == VK_F1) {// 处理 ESC 键和 F1 键的消息		
			return TRUE;
		}
	}

	return CDialog::PreTranslateMessage(pMsg);// 调用基类的 PreTranslateMessage 函数，继续处理消息
}

BOOL TemplateManagerDlg::OnHelpInfo(HELPINFO* pHelpInfo)
{
	return 0;
}

/***********************************************************************
 * 功能: 填充左边模板列
 * 参数:
 *     无
 * 返回值：
 *	   无
************************************************************************/
void TemplateManagerDlg::fillTemplateList()//
{
	m_tListBoxTemplate.ResetContent();// 清空列表框内容

	if (m_ptTemplateManager != nullptr) {// 如果模板管理器指针不为空
		std::vector<TemplateManager::T_Template> *ptVecTemplateList = m_ptTemplateManager->getTemplateList();// 获取模板列表指针

		if (ptVecTemplateList->size() > 0) {// 如果模板列表不为空
			for (int i = 0; i < ptVecTemplateList->size(); i++) {// 遍历模板列表
				m_tListBoxTemplate.InsertString(i, String2Wstring(ptVecTemplateList->at(i).tName).c_str());// 将模板名称插入到列表框中
			}

			m_tListBoxTemplate.SetCurSel(m_iSelListItem);// 设置列表框的当前选中项为 m_iSelListItem
		}
		else {
			m_tListBoxTemplate.SetCurSel(-1);// 如果模板列表为空，则设置列表框的当前选中项为 -1
		}
	}
}

/***********************************************************************
 * 功能: 根据左边模板列选中项显示相应参数
 * 参数:
 *     无
 * 返回值：
 *	   无
************************************************************************/
void TemplateManagerDlg::fillTemplateParam()
{
	m_tEdtName.SetWindowText(L"");// 清空名称编辑框内容
	m_tEdtProduct.SetWindowText(L"");// 清空产品编辑框内容
	m_tEdtTempFile.SetWindowText(L"");// 清空模板文件编辑框内容	
	m_tEdtMaskFile.SetWindowText(L"");// 清空掩码文件编辑框内容
	m_tEdtImageFile.SetWindowText(L"");// 清空图像文件编辑框内容

	m_tEdtScore.SetWindowText(L"60");// 设置分数编辑框内容为 "60"
	m_tEdtMaxOverLap.SetWindowText(L"50");// 设置最大重叠编辑框内容为 "50"
	m_tEdtOffsetX.SetWindowText(L"0");// 设置偏移X编辑框内容为 "0"
	m_tEdtOffsetY.SetWindowText(L"0");
	m_tEdtPrintSpeed.SetWindowText(L"700");// 设置打印速度编辑框内容为 "700"
	m_tEdtPrintHei.SetWindowText(L"25");
	m_tEdtLightVol.SetWindowText(L"200");
	m_tEdtFeature.SetWindowText(L"0");

	m_tCmbLayer.SetCurSel(0);
	m_tCmbSearchType.SetCurSel(0);
	m_tCheckShapeMatch.SetCheck(FALSE);
	m_tSliderContrastRatio.SetPos(100);

	if (m_ptTemplateManager != nullptr) {// 如果模板管理器指针不为空
		int iIndex = m_tListBoxTemplate.GetCurSel();// 获取列表框的当前选中项索引

		if (iIndex != LB_ERR) {// 如果索引不为 LB_ERR
			TemplateManager::PT_Template ptTemplate = m_ptTemplateManager->getTemplateParam(iIndex);// 获取模板参数指针

			if (ptTemplate != nullptr) {// 如果模板参数指针不为空
				CString tText;// 定义字符串变量 tText

				m_tEdtName.SetWindowText(String2Wstring(ptTemplate->tName).c_str());// 将模板名称设置到名称编辑框中
				m_tEdtProduct.SetWindowText(String2Wstring(ptTemplate->tProductFile).c_str());// 将产品文件路径设置到产品编辑框中
				m_tEdtTempFile.SetWindowText(String2Wstring(ptTemplate->tTemplateFile).c_str());
				m_tEdtMaskFile.SetWindowText(String2Wstring(ptTemplate->tMaskFile).c_str());
				m_tEdtImageFile.SetWindowText(String2Wstring(ptTemplate->tImageFile).c_str());

				tText.Format(_T("%d"), ptTemplate->iMaxOverLap);// 将最大重叠值格式化为字符串，并设置到最大重叠编辑框中
				m_tEdtMaxOverLap.SetWindowText(tText);

				tText.Format(_T("%d"), ptTemplate->iScore);
				m_tEdtScore.SetWindowText(tText);

				tText.Format(_T("%d"), ptTemplate->iCenterOffsetX);
				m_tEdtOffsetX.SetWindowText(tText);

				tText.Format(_T("%d"), ptTemplate->iCenterOffsetY);
				m_tEdtOffsetY.SetWindowText(tText);

				tText.Format(_T("%d"), ptTemplate->iModelFeature);
				m_tEdtFeature.SetWindowText(tText);

				tText.Format(_T("%d"), ptTemplate->iPrintSpeed);
				m_tEdtPrintSpeed.SetWindowText(tText);

				tText.Format(_T("%d"), ptTemplate->iPrintHei);
				m_tEdtPrintHei.SetWindowText(tText);

				tText.Format(_T("%d"), ptTemplate->iLightVoltage);
				m_tEdtLightVol.SetWindowText(tText);

				m_tCmbLayer.SetCurSel(ptTemplate->iLayer - 2);// 设置金字塔层数组合框的当前选中项为模板参数中的金字塔层数减去 2
				m_tCmbSearchType.SetCurSel(ptTemplate->iSearchType);// 设置搜索类型组合框的当前选中项为模板参数中的搜索类型
				m_tCheckShapeMatch.SetCheck(1 == ptTemplate->iMethod);
				m_tSliderContrastRatio.SetPos(ptTemplate->iContrastRatio);

				Mat tTempMat = imread(ptTemplate->tTemplateFile, cv::IMREAD_GRAYSCALE);

				m_tVideoTemplateStatic.setContrastRatio((float)ptTemplate->iContrastRatio / 100.0f);
				m_tVideoTemplateStatic.setImage(tTempMat);

				Mat tMaskMat = imread(ptTemplate->tMaskFile, cv::IMREAD_GRAYSCALE);// 读取掩码文件，将其转换为灰度图像
				m_tVideoMaskStatic.setImage(tMaskMat);// 将掩码图像设置到掩码静态控件中

				if (BST_CHECKED == m_tCheckBoxImageShow.GetCheck()) {
					StepDialog tStepDialog(2);

					tStepDialog.Create(IDD_STEP_DIALOG, this);// 创建步骤对话框
					tStepDialog.ShowWindow(SW_SHOW);
					tStepDialog.UpdateWindow();
					tStepDialog.setStepText(L"正在读取采集图，请稍候...");

					m_tImageMat = imread(ptTemplate->tImageFile, cv::IMREAD_GRAYSCALE);

					m_tVideoImageStatic.setImage(m_tImageMat);

					tStepDialog.setStepText(L"读取完成");
					tStepDialog.DestroyWindow();
				}

				if ((!m_tMainDlgMat.empty()) && (ptTemplate->tImageFile.length() == 0) 
					 && (iIndex == (m_ptTemplateManager->getTemplateListSize() - 1))) {
					fillTemplateFeature(Size(tTempMat.cols, tTempMat.rows), ptTemplate->iLayer);

					m_tVideoImageStatic.setImage(m_tMainDlgMat);
				}
			}
		}
	}
}


/***********************************************************************
 * 功能: 设置模版列表信息提示
 * 参数:
 *     无
 * 返回值：
 *	   无
************************************************************************/
void TemplateManagerDlg::fillTemplateInfo()
{
	int iTempListSize = m_ptTemplateManager->getTemplateListSize();
	int iCurSel = m_tListBoxTemplate.GetCurSel() + 1;
	CString tTempListInfo;

	if (iTempListSize > 0) {
		tTempListInfo.Format(_T("模板列表: 共 %d 条，当前项: %d."), iTempListSize, iCurSel);
	}
	else {
		tTempListInfo.Format(_T("当前模版列表为空。"));
	}

	CStatic* pTempListStatic = (CStatic*)GetDlgItem(IDC_STATIC_TEMPLISTINFO);
	if (pTempListStatic != nullptr) {
		pTempListStatic->SetWindowText(tTempListInfo);

		CRect tRect;
		pTempListStatic->GetWindowRect(tRect);
		ScreenToClient(tRect);
		InvalidateRect(tRect);
	}
}

/***********************************************************************
 * 功能: 设置模版特征值(层数改变、模版改变时更新)
 * 参数:
 *     tTemplatesize 模版大小
 * 返回值：
 *	   无
************************************************************************/
void TemplateManagerDlg::fillTemplateFeature(Size tTemplatesize, int iPyrLayer)
{
	int iPyrWidth = tTemplatesize.width / pow(2, iPyrLayer - 1);
	int iPyrHeight = tTemplatesize.height / pow(2, iPyrLayer - 1);

	int iMinLenth = std::min(iPyrWidth, iPyrHeight);
	int iModelFeature = std::max(iMinLenth / 3, 50);

	m_tSpinBtnFeature.SetPos(iModelFeature);

	CString tFeature;
	tFeature.Format(_T("%d"), iModelFeature);
	m_tEdtFeature.SetWindowText(tFeature);
}

/***********************************************************************
 * 功能: 删除模板掩码参数（只对显示）
 * 参数:
 *     无
 * 返回值：
 *	   无
************************************************************************/
void TemplateManagerDlg::OnBnClickedBtnMaskDel()
{
	m_tEdtMaskFile.SetWindowText(L"");
}

/***********************************************************************
 * 功能: 创建模板
 * 参数:
 *     无
 * 返回值：
 *	   无
************************************************************************/
void TemplateManagerDlg::OnBnClickedButtonCreate()
{
	m_tListBoxTemplate.SetCurSel(-1);

	m_tEdtName.SetWindowTextW(L"");
}

/***********************************************************************
 * 功能: 保存模板，如果没有就创建
 * 参数:
 *     无
 * 返回值：
 *	   无
************************************************************************/
void TemplateManagerDlg::OnBnClickedButtonSave()
{
	string tNewName = WinUtil::getEditText(m_tEdtName);

	if (tNewName.empty()) {
		MessageBox(L"模板名不能为空!", L"保存模板参数", MB_OK | MB_ICONWARNING);

		return;
	}
	else if (!WinUtil::checkIllegalCharacters(tNewName)) {
		MessageBox(L"模板名包含非法字符!", L"保存模板参数", MB_OK | MB_ICONWARNING);

		return;
	}

	if (m_tEdtTempFile.GetWindowTextLength() <= 0) {
		MessageBox(L"模板不能为空!", L"保存模板参数", MB_OK | MB_ICONWARNING);

		return;
	}

	if (m_tEdtProduct.GetWindowTextLength() <= 0) {
		MessageBox(L"产品贴图不能为空!", L"保存模板参数", MB_OK | MB_ICONWARNING);

		return;
	}

	int iSelIndex = m_tListBoxTemplate.GetCurSel();

	bool bReFillList	  = false;
	bool bCreateModel	  = false;
	bool bUseShapeMatch   = (BST_CHECKED == m_tCheckShapeMatch.GetCheck());
	string tMaskFile	  = WinUtil::getEditText(m_tEdtMaskFile);
	string tTemplateFile  = WinUtil::getEditText(m_tEdtTempFile);
	string tProductFile   = WinUtil::getEditText(m_tEdtProduct);

	int iCenterOffsetX	  = WinUtil::getEditTextToInt(m_tEdtOffsetX);
	int iCenterOffsetY	  = WinUtil::getEditTextToInt(m_tEdtOffsetY);
	int iCurContrastRatio = m_tSliderContrastRatio.GetPos();
	
	int iCulFeature		  = WinUtil::getEditTextToInt(m_tEdtFeature);
	iCulFeature			  = iCulFeature > 2000 ? 2000 : iCulFeature;
	iCulFeature			  = iCulFeature < 0 ? 0 : iCulFeature;

	if (iSelIndex < 0) {															//添加
		if (m_ptTemplateManager->isExist(tNewName)) {
			MessageBox(L"模版名称不能重复，请重新命名!", L"保存模板参数", MB_OK | MB_ICONWARNING);

			return;
		}

		TemplateManager::T_Template tTemplate;

		tTemplate.tName         = tNewName;
		tTemplate.tTemplateFile = tTemplateFile;
		tTemplate.tProductFile  = tProductFile;
		tTemplate.tMaskFile     = tMaskFile;
		tTemplate.tImageFile    = WinUtil::getEditText(m_tEdtImageFile);

		tTemplate.iMethod        = bUseShapeMatch ? 1 : 0;
		tTemplate.iLayer         = m_tCmbLayer.GetCurSel() + 2;
		tTemplate.iSearchType    = m_tCmbSearchType.GetCurSel();
		tTemplate.iCenterOffsetX = iCenterOffsetX;
		tTemplate.iCenterOffsetY = iCenterOffsetY;
		tTemplate.iScore		 = WinUtil::getEditTextToInt(m_tEdtScore);
		tTemplate.iPrintSpeed	 = WinUtil::getEditTextToInt(m_tEdtPrintSpeed);
		tTemplate.iPrintHei		 = WinUtil::getEditTextToInt(m_tEdtPrintHei);
		tTemplate.iLightVoltage  = WinUtil::getEditTextToInt(m_tEdtLightVol);
		tTemplate.iMaxOverLap	 = WinUtil::getEditTextToInt(m_tEdtMaxOverLap);
		tTemplate.iModelFeature  = iCulFeature;
		tTemplate.iContrastRatio = iCurContrastRatio;

		if (!m_ptTemplateManager->addTemplate(&tTemplate)) {
			MessageBox(L"添加模板失败!", L"保存模板参数", MB_OK | MB_ICONWARNING);

			return;
		}

		iSelIndex = m_ptTemplateManager->getTemplateList()->size() - 1;
		bReFillList = true;
		bCreateModel = true;
	}
	else {																			// 修改
		TemplateManager::PT_Template ptTemplate = m_ptTemplateManager->getTemplateParam(iSelIndex);
		if (nullptr == ptTemplate) {
			MessageBox(L"找不到该模板参数，请检查后再操作!", L"保存模板参数", MB_OK | MB_ICONWARNING);

			return;
		}
		
		string tOldName = ptTemplate->tName;
		int iOldMethod = ptTemplate->iMethod;
		
		// 判断名称是否重复
		if (tOldName != tNewName) {
			int iFindIndex = m_ptTemplateManager->findNameIndex(tNewName);

			if ((iFindIndex != -1) && (iFindIndex != iSelIndex)) {
				MessageBox(L"模版名称重复，保存失败!", L"保存模板参数", MB_OK | MB_ICONWARNING);

				return;
			}
			else {
#ifndef USE_HALCON
				// 删除旧名称创建的模型(NCC转为形状匹配且改名，则不删除，因为无模型)
				if ((iOldMethod == bUseShapeMatch) && bUseShapeMatch) {	
					m_ptTemplateManager->deleteModel(tOldName);
				}
#else
				m_ptTemplateManager->deleteModel(tOldName);
#endif
			}

			bReFillList = true;
		}

		// 形状匹配转为NCC匹配，将模型删除
		if ((iOldMethod == (!bUseShapeMatch)) && (!bUseShapeMatch)) {
			m_ptTemplateManager->deleteModel(tOldName); 
		}

		// 判断是否需要重新生成模型
		int iOldLayer			= ptTemplate->iLayer;
		int iCurLayer			= m_tCmbLayer.GetCurSel() + 2;
		int iOldContrastRatio	= ptTemplate->iContrastRatio;
		string tOldTemplateName	= ptTemplate->tTemplateFile;
		string tOldMaskName		= ptTemplate->tMaskFile;

		if ((iOldLayer != iCurLayer) || (tTemplateFile != tOldTemplateName)
			|| (tMaskFile != tOldMaskName) || (iOldContrastRatio != iCurContrastRatio)) {
			bCreateModel = true;
		}

		ptTemplate->tName         = tNewName;
		ptTemplate->tTemplateFile = tTemplateFile;
		ptTemplate->tProductFile  = tProductFile;
		ptTemplate->tMaskFile     = tMaskFile;
		ptTemplate->tImageFile    = WinUtil::getEditText(m_tEdtImageFile);

		ptTemplate->iMethod        = bUseShapeMatch ? 1 : 0;
		ptTemplate->iLayer         = m_tCmbLayer.GetCurSel() + 2;
		ptTemplate->iSearchType    = m_tCmbSearchType.GetCurSel();
		ptTemplate->iCenterOffsetX = iCenterOffsetX;
		ptTemplate->iCenterOffsetY = iCenterOffsetY;
		ptTemplate->iScore         = WinUtil::getEditTextToInt(m_tEdtScore);
		ptTemplate->iPrintSpeed    = WinUtil::getEditTextToInt(m_tEdtPrintSpeed);
		ptTemplate->iPrintHei	   = WinUtil::getEditTextToInt(m_tEdtPrintHei);
		ptTemplate->iLightVoltage  = WinUtil::getEditTextToInt(m_tEdtLightVol);
		ptTemplate->iMaxOverLap    = WinUtil::getEditTextToInt(m_tEdtMaxOverLap);
		ptTemplate->iModelFeature  = iCulFeature;
		ptTemplate->iContrastRatio = iCurContrastRatio;

		if (!m_ptTemplateManager->saveTemplateToFile()) {
			MessageBox(L"保存失败!", L"保存模板参数", MB_OK | MB_ICONWARNING);

			return;
		}
	}

#ifndef USE_HALCON
	if (bUseShapeMatch) {
		if (!m_ptTemplateManager->isModelExist(tNewName)) {
			bCreateModel = true;
		}

		if (bCreateModel) {
            pq_matchSetParam("match_features", to_string(iCulFeature).c_str(), true);

			StepDialog tStepDialog(2);

			tStepDialog.Create(IDD_STEP_DIALOG, this);
			tStepDialog.ShowWindow(SW_SHOW);
			tStepDialog.UpdateWindow();
			tStepDialog.setStepText(L"开始训练模板，请稍候...");

			Mat tTempMat = imread(tTemplateFile, IMREAD_GRAYSCALE);

			if (!tTempMat.empty()) {
				char acValue[32];
				Mat tMaskMat = imread(tMaskFile, IMREAD_GRAYSCALE);
				string tModeName = CString2String(FileUtil::getAppPath(TEMPLATE_MODEL)).c_str() + string("\\") + tNewName;

				sprintf(acValue, "%d", m_tCmbLayer.GetCurSel() + 2);
				pq_matchSetParam("match_layer", acValue, true);

				if (pq_createTemplate(tTempMat, tMaskMat, tModeName.c_str())) {
					tStepDialog.DestroyWindow();

					MessageBox(L"保存成功!", L"保存模板参数", MB_OK | MB_ICONINFORMATION);
				}
				else {
					tStepDialog.DestroyWindow();

					MessageBox(L"训练模板失败!", L"保存模板参数", MB_OK | MB_ICONWARNING);

					// 训练模型失败后如果还存在该模型，则删除
					if (m_ptTemplateManager->isModelExist(tNewName)) {
						m_ptTemplateManager->deleteModel(tNewName);
					}
				}
			}
			else {
				tStepDialog.DestroyWindow();

				MessageBox(L"保存失败: 模板文件为空!", L"保存模板参数", MB_OK | MB_ICONWARNING);
			}
		}
		else {
			MessageBox(L"保存成功!", L"保存模板参数", MB_OK | MB_ICONINFORMATION);
		}
	}
	else {
		MessageBox(L"保存成功!", L"保存模板参数", MB_OK | MB_ICONINFORMATION);
	}
#else
	if (!m_ptTemplateManager->isModelExist(tNewName, bUseShapeMatch)) {
		bCreateModel = true;
	}

	PQ_SetParam("match_method", (bUseShapeMatch ? "1" : "0"), false);
	PQ_SetParam("match_sub_pixel", "1", false);
	PQ_SetParam("match_max_level", "8", false);
	PQ_SetParam("match_grediness", "0.8", false);
	PQ_SetParam("match_tiff_path", tProductFile.c_str(), false);

	std::string middleParam = "";

	middleParam = std::to_string((m_tCmbLayer.GetCurSel() + 2));
	PQ_SetParam("match_select_level", middleParam.c_str(), false);

	middleParam = std::to_string((iCurContrastRatio / 100.0f));
	PQ_SetParam("match_contrast_ratio", middleParam.c_str(), false);

	middleParam = std::to_string((WinUtil::getEditTextToInt(m_tEdtScore) / 100.0f));
	PQ_SetParam("match_min_score", middleParam.c_str(), false);

	middleParam = std::to_string((WinUtil::getEditTextToInt(m_tEdtMaxOverLap) / 100.0f));
	PQ_SetParam("match_max_over_lap", middleParam.c_str(), false);

	middleParam = std::to_string(iCenterOffsetX);
	PQ_SetParam("match_offset_x", middleParam.c_str(), false);

	middleParam = std::to_string(iCenterOffsetY);
	PQ_SetParam("match_offset_y", middleParam.c_str(), false);

	if (bCreateModel) {
		StepDialog tStepDialog(2);

		tStepDialog.Create(IDD_STEP_DIALOG, this);
		tStepDialog.ShowWindow(SW_SHOW);
		tStepDialog.UpdateWindow();
		tStepDialog.setStepText(L"开始训练模板，请稍候...");

		if(PQ_CreateModel(tTemplateFile.c_str(), tNewName.c_str(), tMaskFile.c_str(), tProductFile.c_str(), iCenterOffsetX, iCenterOffsetY)) {
			tStepDialog.setStepText(L"训练完成！");

			MessageBox(L"保存成功!", L"保存模板参数", MB_OK | MB_ICONINFORMATION);
		}
		else {
			tStepDialog.setStepText(L"训练失败！");

			MessageBox(L"保存失败!", L"保存模板参数", MB_OK | MB_ICONERROR);
		}
	}
	else {
		if(PQ_SetParam("", tNewName.c_str(), true)){
			MessageBox(L"保存成功!", L"保存模板参数", MB_OK | MB_ICONINFORMATION);
		}
		else {
			MessageBox(L"保存失败!", L"保存模板参数", MB_OK | MB_ICONERROR);
		}
	}
#endif

	if (bReFillList) {
		fillTemplateList();
	}

	m_tListBoxTemplate.SetCurSel(iSelIndex);
	fillTemplateParam();
	fillTemplateInfo();
}

/***********************************************************************
 * 功能: 模板列表选中变化事件
 * 参数:
 *     无
 * 返回值：
 *	   无
************************************************************************/
void TemplateManagerDlg::OnLbnSelchangeTemplateList()
{
	//std::cout << "TemplateManagerDlg::OnLbnSelchangeTemplateList" << endl;
	m_tCheckMat = Mat();

	Mat tEmptyMat;
	m_tVideoImageStatic.setImage(tEmptyMat);

	m_tListCtrlMatchResult.DeleteAllItems();

	fillTemplateParam();
	fillTemplateInfo();

#ifdef USE_HALCON
	string tNewName = WinUtil::getEditText(m_tEdtName);

	if (tNewName.length() > 0) {
		PQ_InitParam(tNewName.c_str());
	}
#endif
}

//选择产品贴图按钮
void TemplateManagerDlg::OnBnClickedBtnProductChoose()
{
	CString tCfgFile = FileUtil::getAppPath(UV_MATCH_CFG);

	CFileDialog tFileDialog(TRUE, NULL, IniUtil::readStringValue(tCfgFile, L"template", L"open_folder", NULL),
		                    OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, L"product file(*.tif)|*.tif||");

	if (IDOK == tFileDialog.DoModal()) {
		m_tEdtProduct.SetWindowText(tFileDialog.GetPathName());
	}
}

// 选择模板按钮
void TemplateManagerDlg::OnBnClickedBtnTempChoose()
{
	CString tCfgFile = FileUtil::getAppPath(UV_MATCH_CFG);

	CFileDialog tFileDialog(TRUE, NULL, IniUtil::readStringValue(tCfgFile, L"template", L"open_folder", NULL),
							OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, L"template file(*.png)|*.png||");

	if (IDOK == tFileDialog.DoModal()) {
		m_tEdtTempFile.SetWindowText(tFileDialog.GetPathName());
		
		//如果模板名称为空，则使用文件名
		if (m_tEdtName.GetWindowTextLength() <= 0) {
			m_tEdtName.SetWindowText(FileUtil::getFileName(tFileDialog.GetPathName(), false));
		}

		Mat tTempMat = imread(WinUtil::getEditText(m_tEdtTempFile), cv::IMREAD_GRAYSCALE);
		m_tVideoTemplateStatic.setImage(tTempMat);

		int nSel = m_tCmbLayer.GetCurSel() + 2;
		fillTemplateFeature(tTempMat.size(), nSel);
	}
}

//选择模板掩码按钮
void TemplateManagerDlg::OnBnClickedBtnMaskChoose()
{
	CString tCfgFile = FileUtil::getAppPath(UV_MATCH_CFG);

	CFileDialog tFileDialog(TRUE, NULL, IniUtil::readStringValue(tCfgFile, L"template", L"open_folder", NULL),
							OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, L"template mask file(*.png)|*.png||");

	if (IDOK == tFileDialog.DoModal()) {
		m_tEdtMaskFile.SetWindowText(tFileDialog.GetPathName());

		Mat tMaskMat = imread(WinUtil::getEditText(m_tEdtMaskFile), cv::IMREAD_GRAYSCALE);
		m_tVideoMaskStatic.setImage(tMaskMat);
	}
}

//选择采集图按钮
void TemplateManagerDlg::OnBnClickedBtnImageChoose()
{
	CString tCfgFile = FileUtil::getAppPath(UV_MATCH_CFG);

	CFileDialog tFileDialog(TRUE, NULL, IniUtil::readStringValue(tCfgFile, L"template", L"open_folder", NULL),
		                    OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, L"image file(*.png)|*.png||");

	if (IDOK == tFileDialog.DoModal()) {
		m_tEdtImageFile.SetWindowText(tFileDialog.GetPathName());
	}
}

//删除模板按钮
void TemplateManagerDlg::OnBnClickedButtonDelete()
{
	int iSelIndex = m_tListBoxTemplate.GetCurSel();

	if (iSelIndex < 0) {
		MessageBox(L"请先选中要删除的模板!", L"删除模板", MB_OK | MB_ICONWARNING);

		return;
	}

	if (IDOK == MessageBox(L"确定要删除该模板吗!", L"删除模板", MB_OKCANCEL | MB_ICONQUESTION)) {
		if ((!m_tMainDlgMat.empty()) && (iSelIndex == (m_ptTemplateManager->getTemplateListSize() - 1))) {
			m_tMainDlgMat = Mat();
		}

		m_tImageMat = Mat();
		m_tVideoImageStatic.setImage(m_tImageMat);

		if (m_ptTemplateManager->delTemplate(iSelIndex)) {
			if (iSelIndex > 0) {
				setSelListItem(iSelIndex - 1);
			}

			MessageBox(L"删除成功!", L"删除模板", MB_OK | MB_ICONINFORMATION);
		}
		else {
			MessageBox(L"删除失败!", L"删除模板", MB_OK | MB_ICONWARNING);
		}
	}

	fillTemplateList();
	fillTemplateInfo();
}

//匹配检测
void TemplateManagerDlg::OnBnClickedBtnMatchCheck()
{
	m_tCheckMat = Mat();
	Mat tEmptyMat;

	m_tVideoImageStatic.setImage(tEmptyMat);

	int iSelIndex = m_tListBoxTemplate.GetCurSel();

	if (iSelIndex < 0) {
		MessageBox(L"请先选中要检测的模板!", L"模板检测", MB_OK | MB_ICONWARNING);

		return;
	}

	if (m_tEdtTempFile.GetWindowTextLength() <= 0) {
		MessageBox(L"模板不能为空!", L"模板检测", MB_OK | MB_ICONWARNING);

		return;
	}

	if (m_tEdtProduct.GetWindowTextLength() <= 0) {
		MessageBox(L"产品贴图不能为空!", L"模板检测", MB_OK | MB_ICONWARNING);

		return;
	}

	/*if (m_tEdtImageFile.GetWindowTextLength() <= 0) {
		MessageBox(L"采集图不能为空!", L"模板检测", MB_OK | MB_ICONWARNING);

		return;
	}*/

#ifndef USE_HALCON
	StepDialog tStepDialog(7);

	tStepDialog.Create(IDD_STEP_DIALOG, this);
	tStepDialog.ShowWindow(SW_SHOW);
	tStepDialog.UpdateWindow();
	tStepDialog.setStepText(L"开始模板检测，请稍候...");

	Mat tTempMat = imread(WinUtil::getEditText(m_tEdtTempFile), IMREAD_GRAYSCALE);
	if (tTempMat.empty()) {
		tStepDialog.DestroyWindow();
		MessageBox(L"模板文件不存在!", L"模板检测", MB_OK | MB_ICONWARNING);
	
		return;
	}
	
	tStepDialog.setStepText(L"模板检测: 读取采集图，请稍候...");

	Mat tImageMat;
	
	if ((!m_tMainDlgMat.empty()) && (m_tEdtImageFile.GetWindowTextLength() <= 0)
		&& (m_tListBoxTemplate.GetCurSel() == (m_ptTemplateManager->getTemplateListSize() - 1))) {
		tImageMat = m_tMainDlgMat.clone();
	}
	else if (m_tEdtImageFile.GetWindowTextLength() > 0) {
		tImageMat = imread(WinUtil::getEditText(m_tEdtImageFile), IMREAD_GRAYSCALE);
	}
	else {
		tStepDialog.DestroyWindow();
		MessageBox(L"采集图不能为空!", L"模板检测", MB_OK | MB_ICONWARNING);

		return;
	}

	if (tImageMat.empty()) {
		tStepDialog.DestroyWindow();
		MessageBox(L"采集图文件不存在!", L"模板检测", MB_OK | MB_ICONWARNING);
	
		return;
	}

	if ((tImageMat.cols < tTempMat.cols) || (tImageMat.rows < tTempMat.rows )) {
		MessageBox(L"采集图或模板图有错误，请确定后再操作!", L"模板检测", MB_OK | MB_ICONERROR);
	}
	
	tStepDialog.setStepText(L"模板检测: 读取产品图，请稍候...");

	TIFF* ptProductTiff = TIFFOpen(WinUtil::getEditText(m_tEdtProduct).c_str(), "rb");
	if (nullptr == ptProductTiff) {
		tStepDialog.DestroyWindow();
		MessageBox(L"产品贴图文件不存在!", L"模板检测", MB_OK | MB_ICONWARNING);
	
		return;
	}

	m_tListCtrlMatchResult.DeleteAllItems();

	Mat tMaskMat = imread(WinUtil::getEditText(m_tEdtMaskFile), IMREAD_GRAYSCALE);

	char acValue[32];
	CString tText;

	sprintf(acValue, "%d", (BST_CHECKED == m_tCheckShapeMatch.GetCheck()) ? 1 : 0);
	pq_matchSetParam("match_method", acValue, false);

	if (BST_CHECKED == m_tCheckShapeMatch.GetCheck()) {
		tStepDialog.setStepText(L"模板检测: 模型检测中，请稍候...");

		if (!m_ptTemplateManager->isModelExist(WinUtil::getEditText(m_tEdtName))) {
			MessageBox(L"模版模型不存在，请重新创建模型!", L"模板检测", MB_OK | MB_ICONWARNING);

			return;
		}
	}
	else {
		tStepDialog.setStepText(L"模板检测: 匹配处理中，请稍候...");
	}
	
	sprintf(acValue, "%d", m_tCmbLayer.GetCurSel() + 2);
	pq_matchSetParam("match_layer", acValue, false);
	
	sprintf(acValue, "%d", WinUtil::getEditTextToInt(m_tEdtScore));
	pq_matchSetParam("match_score", acValue, false);
	
	switch (m_tCmbSearchType.GetCurSel()) {
		case 1:		pq_matchSetParam("match_max_pos", "500", false);	break;
		case 2:		pq_matchSetParam("match_max_pos", "1000", false);	break;
		case 3:		pq_matchSetParam("match_max_pos", "2000", false);	break;
		default:	pq_matchSetParam("match_max_pos", "75", false);		break;
	}
	
	sprintf(acValue, "%d", WinUtil::getEditTextToInt(m_tEdtOffsetX));
	pq_matchSetParam("match_center_x_offset", acValue, false);
	
	sprintf(acValue, "%d", WinUtil::getEditTextToInt(m_tEdtOffsetY));
	pq_matchSetParam("match_center_y_offset", acValue, false);
	
	sprintf(acValue, "%d", m_tSliderContrastRatio.GetPos());
	pq_matchSetParam("match_contrast_ratio", acValue, false);
	
	sprintf(acValue, "%d", WinUtil::getEditTextToInt(m_tEdtMaxOverLap));
	pq_matchSetParam("match_max_over_lap", acValue, false);

	sprintf(acValue, "%d", WinUtil::getEditTextToInt(m_tEdtFeature));
	pq_matchSetParam("match_features", acValue, false);

	string tModeName = CString2String(FileUtil::getAppPath(TEMPLATE_MODEL)).c_str() + string("\\") + WinUtil::getEditText(m_tEdtName);
	pq_matchSetParam("match_model", tModeName.c_str(), true);

	vector<tuple<Mat, Rect, double, double>> tVecPrintImage;

	tStepDialog.setStepText(L"模板检测: 正在识别，请稍候...");

	if (pq_matchCheck(m_tCheckMat, tVecPrintImage, tImageMat, tTempMat, tMaskMat, ptProductTiff)) {
		tStepDialog.setStepText(L"模板检测: 识别成功，请稍候...");

		m_tVideoImageStatic.setImage(m_tCheckMat);

		// 按匹配得分排序
		sort(tVecPrintImage.begin(), tVecPrintImage.end(),
			[](const tuple<Mat, Rect, double, double>& i1, const tuple<Mat, Rect, double, double>& i2) { return get<2>(i1) < get<2>(i2); });

		int iRowIndex = 0;

		for (const auto& tItem : tVecPrintImage) {
			tText.Format(_T("%d"), iRowIndex + 1);
			iRowIndex = m_tListCtrlMatchResult.InsertItem(m_tListCtrlMatchResult.GetItemCount(), tText);

			tText.Format(_T("%.2f"), (double)std::get<2>(tItem) * 100);
			m_tListCtrlMatchResult.SetItemText(iRowIndex, 1, tText);

			tText.Format(_T("%.3f"), (double)std::get<3>(tItem));
			m_tListCtrlMatchResult.SetItemText(iRowIndex, 2, tText);

			iRowIndex++;
		}
	}
	else {
		MessageBox(L"检测失败!", L"模板检测", MB_OK | MB_ICONWARNING);
	}
	
	tStepDialog.setStepText(L"模板检测完成!");
	TIFFClose(ptProductTiff);

	tStepDialog.DestroyWindow();
#else
	StepDialog tStepDialog(6);

	tStepDialog.Create(IDD_STEP_DIALOG, this);
	tStepDialog.ShowWindow(SW_SHOW);
	tStepDialog.UpdateWindow();
	tStepDialog.setStepText(L"开始模板检测，请稍候...");

	tStepDialog.setStepText(L"模板检测: 读取采集图，请稍候...");

	Mat tImageMat;

	if ((!m_tMainDlgMat.empty()) && (m_tEdtImageFile.GetWindowTextLength() <= 0)
		&& (m_tListBoxTemplate.GetCurSel() == (m_ptTemplateManager->getTemplateListSize() - 1))) {
		tImageMat = m_tMainDlgMat.clone();
	}
	else if (m_tEdtImageFile.GetWindowTextLength() > 0) {
		tImageMat = imread(WinUtil::getEditText(m_tEdtImageFile), IMREAD_GRAYSCALE);
	}
	else {
		tStepDialog.DestroyWindow();
		MessageBox(L"采集图不能为空!", L"模板检测", MB_OK | MB_ICONWARNING);

		return;
	}

	if (tImageMat.empty()) {
		tStepDialog.DestroyWindow();
		MessageBox(L"采集图文件不存在!", L"模板检测", MB_OK | MB_ICONWARNING);

		return;
	}

	/*Mat tImageMat = imread(WinUtil::getEditText(m_tEdtImageFile), IMREAD_GRAYSCALE);
	if (tImageMat.empty()) {
		tStepDialog.DestroyWindow();
		MessageBox(L"采集图文件不存在!", L"模板检测", MB_OK | MB_ICONWARNING);

		return;
	}*/

	tStepDialog.setStepText(L"模板检测: 读取模型，请稍候...");
	if (!PQ_MatchBegin(WinUtil::getEditText(m_tEdtName).c_str())) {
		tStepDialog.DestroyWindow();
		MessageBox(L"模型读取失败!", L"模板检测", MB_OK | MB_ICONWARNING);

		return;
	}

	tStepDialog.setStepText(L"模板检测: 检测参数，请稍候...");
	char buffer[256];                                 
	memset(buffer, 0, sizeof(buffer));
	PQ_GetParam("match_tiff_path", buffer);

	TIFF* ptProductTiff = TIFFOpen(buffer, "rb");
	if (nullptr == ptProductTiff) {
		tStepDialog.DestroyWindow();
		MessageBox(L"产品贴图文件不存在!", L"模板检测", MB_OK | MB_ICONWARNING);

		return;
	}
	TIFFClose(ptProductTiff);

	m_tListCtrlMatchResult.DeleteAllItems();
	vector<tuple<Mat, Rect, double, double>> tVecPrintImage;

	tStepDialog.setStepText(L"模板检测: 正在识别，请稍候...");
	if (PQ_MatchCheck(m_tCheckMat, tVecPrintImage, tImageMat)) {
		tStepDialog.setStepText(L"模板检测: 识别成功，请稍候...");

		m_tVideoImageStatic.setImage(m_tCheckMat);

		// 按匹配得分排序
		sort(tVecPrintImage.begin(), tVecPrintImage.end(),
			[](const tuple<Mat, Rect, double, double>& i1, const tuple<Mat, Rect, double, double>& i2) { return get<2>(i1) < get<2>(i2); });

		int iRowIndex = 0;
		CString tText;

		for (const auto& tItem : tVecPrintImage) {
			tText.Format(_T("%d"), iRowIndex + 1);
			iRowIndex = m_tListCtrlMatchResult.InsertItem(m_tListCtrlMatchResult.GetItemCount(), tText);

			tText.Format(_T("%.2f"), (double)std::get<2>(tItem) * 100);
			m_tListCtrlMatchResult.SetItemText(iRowIndex, 1, tText);

			tText.Format(_T("%.3f"), (double)std::get<3>(tItem));
			m_tListCtrlMatchResult.SetItemText(iRowIndex, 2, tText);

			iRowIndex++;
		}		
	}
	else {
		MessageBox(L"检测失败!", L"模板检测", MB_OK | MB_ICONWARNING);
	}

	PQ_MatchEnd();
	tStepDialog.setStepText(L"模板检测完成!");
	tStepDialog.DestroyWindow();
#endif
}

void TemplateManagerDlg::OnNMCustomdrawSliderConstrastRatio(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);

	int iPos = m_tSliderContrastRatio.GetPos();
	CString tText;

	tText.Format(L"%d", iPos);
	m_tStaticContrastRatio.SetWindowText(tText);

	CRect tRect;
	m_tStaticContrastRatio.GetWindowRect(tRect);
	ScreenToClient(tRect);
	InvalidateRect(tRect);

	m_tVideoTemplateStatic.setContrastRatio((float)iPos/100.0f);

	*pResult = 0;
}


void TemplateManagerDlg::OnBnClickedTempMaxButton()
{
	if (!m_tCheckMat.empty()) {
		cv::namedWindow("Match Result", cv::WINDOW_KEEPRATIO);
		
		CRect tScreenRect;
		::SystemParametersInfo(SPI_GETWORKAREA, 0, &tScreenRect, 0);

		float fScaleX = 1.0f;
		float fScaleY = 1.0f;

		if (tScreenRect.Width() < m_tCheckMat.cols) {
			fScaleX = (float)tScreenRect.Width() / m_tCheckMat.cols;
		}

		if (tScreenRect.Height() < m_tCheckMat.rows) {
			fScaleY = (float)tScreenRect.Height() / m_tCheckMat.rows;
		}

		float fScale = (fScaleX < fScaleY) ? fScaleX : fScaleY;

		int iWid = (int)(m_tCheckMat.cols * fScale);
		int iHei = (int)(m_tCheckMat.rows * fScale);
		int iDx = (tScreenRect.Width() - iWid) / 2;
		int iDy = (tScreenRect.Height() - iHei) / 2;

		cv::resizeWindow("Match Result", Size(iWid, iHei));
		cv::moveWindow("Match Result", iDx, iDy);
		cv::imshow("Match Result", m_tCheckMat);
	}
	else {
		MessageBox(L"结果图为空!", L"模板检测", MB_OK | MB_ICONWARNING);
	}	
}


void TemplateManagerDlg::OnDeltaposSpinFeature(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);

	// TODO: 在此添加控件通知处理程序代码
	int iMaxFeature = 2000;
	int iMinFeature = 0;
	int iSpinStep = 50;

	int iCurFeature = WinUtil::getEditTextToInt(m_tEdtFeature);

	if (pNMUpDown->iDelta == 1){
		iCurFeature = ((iCurFeature + iSpinStep) > iMaxFeature) ? iMaxFeature : (iCurFeature + iSpinStep);
	}
	if (pNMUpDown->iDelta == -1){
		iCurFeature = ((iCurFeature - iSpinStep) < iMinFeature) ? iMinFeature : (iCurFeature - iSpinStep);
	}

	m_tSpinBtnFeature.SetPos(iCurFeature);

	CString tFeature;
	tFeature.Format(_T("%d"), iCurFeature);
	m_tEdtFeature.SetWindowText(tFeature);

	*pResult = 0;
}


void TemplateManagerDlg::OnSelchangeComboLayer()
{
	// TODO: 在此添加控件通知处理程序代码
	int nSel = m_tCmbLayer.GetCurSel() + 2;

	try {
		Mat tTempMat = imread(WinUtil::getEditText(m_tEdtTempFile), IMREAD_GRAYSCALE);

		fillTemplateFeature(tTempMat.size(), nSel);
	}
	catch (cv::Exception& e){
	}
}

//制作模板掩码
void TemplateManagerDlg::OnBnClickedBtnMaskMake()
{
	if (m_tEdtTempFile.GetWindowTextLength() <= 0) {
		MessageBox(L"模板文件不存在!", L"制作模板掩码", MB_OK | MB_ICONWARNING);

		m_tEdtTempFile.SetFocus();
		return;
	}

	string tTemplateFile = WinUtil::getEditText(m_tEdtTempFile);
	CString tTempFile = String2Wstring(tTemplateFile).c_str();
	if (!PathFileExists(tTempFile)) {
		MessageBox(TEXT("模板文件不存在！"), TEXT("制作模板掩码"), MB_OK | MB_ICONWARNING);

		return;
	}

	MakeTemplateMaskDialog tMakeTemplateMaskDlg(this, tTemplateFile);
	
	if (IDOK == tMakeTemplateMaskDlg.DoModal()) {
		CString tFileName = FileUtil::getFileNameWithoutSuffix(tTempFile);

		tFileName += "_mask.png";

		m_tEdtMaskFile.SetWindowText(tFileName);

		Mat tMaskMat = imread(WinUtil::getEditText(m_tEdtMaskFile), cv::IMREAD_GRAYSCALE);
		m_tVideoMaskStatic.setImage(tMaskMat);
	}
}

//创建模板
void TemplateManagerDlg::OnBnClickedBtnTemplateMake()
{
	m_tVideoImageStatic.setImage(m_tImageMat);
	//_cwprintf(L"%s begin...\r\n", __FUNCTIONW__);

	if (m_tImageMat.empty()) {
		MessageBox(L"采集图为空，无法制作模板!", L"制作模板", MB_OK | MB_ICONWARNING);

		return;
	}

	if (m_tEdtProduct.GetWindowTextLength() <= 0) {
		MessageBox(L"产品图为空，无法制作模板!", L"制作模板", MB_OK | MB_ICONWARNING);
		m_tEdtProduct.SetFocus();

		return;
	}

	Mat tProductMat;

	if (!TiffUtil::readTiffFileToMat(tProductMat, WinUtil::getEditText(m_tEdtProduct))) {
		MessageBox(L"读产品图失败，无法制作模板!", L"制作模板", MB_OK | MB_ICONWARNING);

		return;
	}

	cv::Rect tSelectRect = m_tVideoImageStatic.getSelectRect();
	cv::Rect tImgRect(0, 0, m_tImageMat.cols, m_tImageMat.rows);

	tSelectRect &= tImgRect;

	if (tSelectRect.empty()) {
		MessageBox(L"没有选取区域，无法制作模板!", L"制作模板", MB_OK | MB_ICONWARNING);

		return;
	}

	Mat tTempMat = m_tImageMat(tSelectRect);

	_cwprintf(L"%s ready to make\r\n", __FUNCTIONW__);

	CString tProductFile;

	m_tEdtProduct.GetWindowText(tProductFile);
	/*CString tProductPath = FileUtil::getPath(tProductFile);

	std::string tSavePath = CString2String(tProductPath);*/

	_cwprintf(L"%s tProductPath: %s\r\n", __FUNCTIONW__, tProductFile);

	MakeTemplateDialog tDlg(this, tTempMat, tProductMat, tProductFile);

	tDlg.DoModal();
	_cwprintf(L"%s end\r\n", __FUNCTIONW__);
}
//重置模板
LRESULT TemplateManagerDlg::OnResetTemplate(WPARAM wparam, LPARAM lparam)
{
	_cwprintf(L"%s begin\r\n", __FUNCTIONW__);//输出调试信息，用于调试程序，输出函数名和参数值，用于调试程序，输出函数名和参数值

	std::string *ptTemplateFile = (std::string*)wparam;//获取模板文件路径
	std::string *ptProductFile  = (std::string*)lparam;//获取产品贴图文件路径

	if ((ptTemplateFile != nullptr) && (ptProductFile != nullptr)) {
		m_tEdtProduct.SetWindowText(String2Wstring(*ptProductFile).c_str());//设置产品贴图文件路径
		m_tEdtTempFile.SetWindowText(String2Wstring(*ptTemplateFile).c_str());//设置模板文件路径

		Mat tTempMat = imread(WinUtil::getEditText(m_tEdtTempFile), cv::IMREAD_GRAYSCALE);//读取模板文件
		m_tVideoTemplateStatic.setImage(tTempMat);//显示模板图像
	}

	return 0;
}
//BOOL用于Windows API 和MFC编程中，表示函数的返回值类型。BOOL类型通常用于表示函数的执行结果，通常取值为TRUE或FALSE。
//实现鼠标滚轮事件增加放大/缩小功能，并且满足按下Alt+滚动鼠标时进行放大/缩小操作，放大时显示图片最原始效果，图像过大时以当前鼠标为中心进行局部显示可见区
BOOL TemplateManagerDlg::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt){//鼠标滚轮事件处理函数，nFlags为鼠标事件标志，zDelta为滚轮滚动的距离，pt为鼠标指针的位置
	if(GetAsyncKeyState(VK_MENU) & 0x8000){//判断是否按下了Alt键getAsyncKeyState(VK_MENU) & 0x8000表示Alt键是否按下，0x8000表示Alt键的状态，如果Alt键按下，则返回非0值，否则返回0值
    double scaleFactor = (zDelta > 0) ? 1.1 : 0.9; // 放大或缩小的比例因子，zDelta > 0表示滚轮向上滚动，scaleFactor为1.1，否则为0.9
    m_tVideoImageStatic.zoomout(scaleFactor, pt); // 调用setScale函数进行放大或缩小操作传入的参数为scaleFactor和pt，scaleFactor为放大或缩小的比例因子，pt为鼠标指针的位置
    return TRUE; // 处理完鼠标滚轮事件后返回TRUE，表示已经处理	
	}
	return StyleDialog::OnMouseWheel(nFlags, zDelta, pt);//调用基类的OnMouseWheel函数进行默认处理，传入的参数为nFlags、zDelta和pt
}
