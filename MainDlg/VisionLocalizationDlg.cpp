#include "pch.h"
#include "def.h"
// 包含框架相关的头文件，提供基础的框架功能
#include "framework.h"
// 包含视觉定位主程序的头文件
#include "VisionLocalization.h"
// 包含视觉定位对话框的头文件
#include "VisionLocalizationDlg.h"
// 包含参数设置对话框的头文件
#include "ParamSetDialog.h"
// 包含变换相关的头文件，可能用于坐标变换等操作
#include "Transform.h"
// 包含PQ设置的头文件，用于配置相关设置
#include "PQSetting.h"
// 包含PQ消息的头文件，用于消息传递
#include "PQMsg.h"
// 包含辅助工具的头文件，提供一些辅助函数
#include "Helper.h"
// 包含cJSON库的头文件，用于处理JSON数据
#include "cJSON.h"
// 包含FlHeader头文件，具体功能取决于该头文件的实现
#include "FlHeader.h"
// 包含文件操作相关的头文件
#include "Filer.h"
// 包含自定义的Ini文件操作工具头文件
#include "../utils/IniUtil.h"
// 包含自定义的文件操作工具头文件
#include "../utils/FileUtil.h"
// 包含自定义的Tiff文件操作工具头文件
#include "../utils/TiffUtil.h"
// 包含模板管理对话框的头文件
#include "../Temp/TemplateManagerDlg.h"
// 包含制作模板对话框的头文件
#include "../Temp/MakeTemplateDialog.h"
// 包含屏幕截图对话框的头文件
#include "../CScreenShotDlg.h"
// 包含步骤对话框的头文件
#include "../StepDialog.h"

// 如果定义了USE_HALCON宏，则使用Halcon相关的匹配库
#ifdef USE_HALCON
// 包含FastHalMatch库的头文件
#include "FastHalMatch.h"
// 链接FastHalMatch库
#pragma comment(lib, "./FastHalMatch.lib")
// 否则使用普通的匹配库
#else
// 包含FastMatch库的头文件
#include "FastMatch.h"
// 链接FastMatch库
#pragma comment(lib, "./FastMatch.lib")
#endif

// 使用OpenCV命名空间
using namespace cv;
// 使用标准库命名空间
using namespace std;
// 使用Transform命名空间
using namespace Transform;

// 如果是调试模式
#ifdef _DEBUG
// 定义new操作符为DEBUG_NEW，用于调试内存分配
#define new DEBUG_NEW
#endif


// 实现动态创建类的功能，允许在运行时动态创建 CVisionLocalizationDlg 类的对象
// 第一个参数 CVisionLocalizationDlg 是要实现动态创建的类名
// 第二个参数 CFleeceDlg 是 CVisionLocalizationDlg 的基类名
IMPLEMENT_DYNAMIC(CVisionLocalizationDlg, CFleeceDlg)

// 开始消息映射，用于将消息与对应的处理函数关联起来
// 第一个参数 CVisionLocalizationDlg 是消息映射所属的类名
// 第二个参数 CFleeceDlg 是 CVisionLocalizationDlg 的基类名
BEGIN_MESSAGE_MAP(CVisionLocalizationDlg, CFleeceDlg)
	/*ON_WM_NCLBUTTONDOWN()
	ON_WM_NCMOUSEMOVE()
	ON_WM_ERASEBKGND()
	ON_WM_CREATE()
	ON_WM_CTLCOLOR()*/
	// 处理定时器消息，调用 OnTimer 函数
	ON_WM_TIMER()
	// 处理帮助信息消息，调用 OnHelpInfo 函数
	ON_WM_HELPINFO()
	// 处理自定义消息 PQM_MAIN_TEMP，调用 OnTempMsg 函数
	ON_MESSAGE(PQM_MAIN_TEMP, OnTempMsg)

	// 处理匹配帧消息 PQM_MATCH_FRAME，调用 OnFrameMsg 函数
	ON_MESSAGE(PQM_MATCH_FRAME, OnFrameMsg)
	// 处理匹配矩阵消息 PQM_MATCH_MATRIX，调用 OnMatrixMsg 函数
	ON_MESSAGE(PQM_MATCH_MATRIX, OnMatrixMsg)
	// 处理匹配转换消息 PQM_MATCH_CONVERT，调用 OnConvertMsg 函数
	ON_MESSAGE(PQM_MATCH_CONVERT, OnConvertMsg)
	// 处理匹配发送消息 PQM_MATCH_SENT，调用 OnSentMsg 函数
	ON_MESSAGE(PQM_MATCH_SENT, OnSentMsg)
	// 处理匹配调度消息 PQM_MATCH_SCHEDULE，调用 OnScheduleMsg 函数
	ON_MESSAGE(PQM_MATCH_SCHEDULE, OnScheduleMsg)
	// 处理匹配图像 TCP 完成消息 PQM_MATCH_IM_TCP_FINISH，调用 OnImTcpMsg 函数
	ON_MESSAGE(PQM_MATCH_IM_TCP_FINISH, OnImTcpMsg)
	// 处理羊毛发送停止结束消息 PQM_FLEECE_SEND_STOP_END，调用 OnImTcpMsg 函数
	ON_MESSAGE(PQM_FLEECE_SEND_STOP_END, OnImTcpMsg)
	// 处理匹配背景消息 PQM_MATCH_BACKGROUND，调用 OnSaveBackground 函数
	ON_MESSAGE(PQM_MATCH_BACKGROUND, OnSaveBackground)
	// 处理主更新配置参数消息 PQM_MAIN_UPDATE_CFG_PARAM，调用 OnUpadteCfgParamMsg 函数
	ON_MESSAGE(PQM_MAIN_UPDATE_CFG_PARAM, OnUpadteCfgParamMsg)

	// 处理工具提示需要文本通知消息，调用 SetTipText 函数
	ON_NOTIFY_EX(TTN_NEEDTEXT, 0, SetTipText)

	// 处理按钮 IDC_BT_MAIN_TIFF 点击事件，调用 OnClickedTIFF 函数
	ON_BN_CLICKED(IDC_BT_MAIN_TIFF, OnClickedTIFF)

	// 处理按钮 IDC_BT_MAIN_PRINTER 点击事件，调用 OnClickedPrinter 函数
	ON_BN_CLICKED(IDC_BT_MAIN_PRINTER,        OnClickedPrinter)
	// 处理按钮 IDC_BT_MAIN_MAKE_FEATURE 点击事件，调用 OnClickedMakeFeature 函数
	ON_BN_CLICKED(IDC_BT_MAIN_MAKE_FEATURE,   OnClickedMakeFeature)
	// 处理按钮 IDC_BT_MAIN_GATHER 点击事件，调用 OnClickedGather 函数
	ON_BN_CLICKED(IDC_BT_MAIN_GATHER,         OnClickedGather)
	// 处理按钮 IDC_BTN_SYS_PARAM 点击事件，调用 OnClickedSysParamBtn 函数
	ON_BN_CLICKED(IDC_BTN_SYS_PARAM,          OnClickedSysParamBtn)
	// 处理按钮 IDC_BT_MAIN_NEWTEMP 点击事件，调用 OnBnClickedBtMainNewtemp 函数
	//ON_BN_CLICKED(IDC_BT_MAIN_CAPTURE_IMAGE,  OnBnClickedBtMainCaptureImage)
	ON_BN_CLICKED(IDC_BT_MAIN_NEWTEMP,		  OnBnClickedBtMainNewtemp)
	// 处理按钮 IDC_BT_MAIN_SAVEBACKGROUND 点击事件，调用 OnClickedSaveBackground 函数
	ON_BN_CLICKED(IDC_BT_MAIN_SAVEBACKGROUND, OnClickedSaveBackground)
	// 处理按钮 IDC_BT_MAIN_CUT_IMAGE 点击事件，调用 OnBnClickedBtMainCutImage 函数
	//ON_BN_CLICKED(IDC_BT_MAIN_CUT_IMAGE,	  OnBnClickedBtMainCutImage)

	// 处理组合框 IDC_CB_MAIN_MODEL 选择改变事件，调用 OnCbnSelchangeCbMainModel 函数
	ON_CBN_SELCHANGE(IDC_CB_MAIN_MODEL, OnCbnSelchangeCbMainModel)
    // 结束消息映射，与 BEGIN_MESSAGE_MAP 配对使用，用于结束消息与处理函数的关联定义
    END_MESSAGE_MAP()

// 类 CVisionLocalizationDlg 的构造函数定义
// 参数 pParent 为指向父窗口的指针，默认为 nullptr
CVisionLocalizationDlg::CVisionLocalizationDlg(CWnd* pParent):
    // 调用基类 CFleeceDlg 的构造函数，传入父窗口指针
    CFleeceDlg(pParent),
    // 调用基类 CMatchTh 的构造函数，传入参数 0.1 和当前对象的指针
    CMatchTh(0.1, this), 
    // 初始化成员变量 m_dPrintInfos，初始值都为 0.0
    m_dPrintInfos{ 0.0, 0.0, 0.0 }
{
    // 从应用程序资源中加载指定 ID 的图标，并将其句柄赋值给成员变量 m_hIcon
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);//m_hIcom用于存储应用程序的图标资源的句柄。
}

// 检测配置目录是否存在 config image log template template_model
// 检查并创建必要的目录
void CVisionLocalizationDlg::checkAndCreateDirectory()
{
    // 获取可执行文件的路径
    CString tExePath = FileUtil::getExePath();//FileUtil::getExePath()用于获取当前可执行文件的路径。

    // 构建配置文件目录的路径
    CString tConfigPath = tExePath + TEXT("\\config");//TEXT("\\config")用于构建配置文件目录的路径。
    // 构建采集图像目录的路径
    CString tCollectImagePath = tExePath + TEXT("\\image");//TEXT("\\image")用于构建采集图像目录的路径。
    // 构建日志文件目录的路径
    CString tLogPath = tExePath + TEXT("\\log");
    // 构建模板文件目录的路径
    CString tTemplatePath = tExePath + TEXT("\\template");
    // 构建模板模型文件目录的路径
    CString tTemplateModelPath = tExePath + TEXT("\\template_model");
    //目录就是文件夹，文件夹就是目录。
    // 检查配置文件目录是否存在，如果不存在则创建
    if (!PathIsDirectory(tConfigPath)) CreateDirectory(tConfigPath, NULL);//PathIsDirectory用于检查指定路径是否为目录。CreateDirectory用于创建指定路径的目录。
    // 检查采集图像目录是否存在，如果不存在则创建
    if (!PathIsDirectory(tCollectImagePath)) CreateDirectory(tCollectImagePath, NULL);
    // 检查日志文件目录是否存在，如果不存在则创建
    if (!PathIsDirectory(tLogPath)) CreateDirectory(tLogPath, NULL);
    // 检查模板文件目录是否存在，如果不存在则创建
    if (!PathIsDirectory(tTemplatePath)) CreateDirectory(tTemplatePath, NULL);
    // 检查模板模型文件目录是否存在，如果不存在则创建
    if (!PathIsDirectory(tTemplateModelPath)) CreateDirectory(tTemplateModelPath, NULL);
}

// 刷新模板列表
void CVisionLocalizationDlg::fillData()
{
    // 获取配置文件的路径
    CString tCfgFile = FileUtil::getAppPath(UV_MATCH_CFG);

    // 更新对话框数据到变量
    UpdateData(TRUE);

    // 读取上次设置的打印宽度参数，默认值为900
    m_dPrintInfos[0] = IniUtil::readIntValue(tCfgFile, L"print", L"width", 900);
    // 读取上次设置的打印高度参数，默认值为600
    m_dPrintInfos[1] = IniUtil::readIntValue(tCfgFile, L"print", L"height", 600);
    // 读取上次设置的相机参数，默认值为"G:\\12.04\\param.png"
    m_rCameraParam   = IniUtil::readStringValue(tCfgFile, L"camera", L"param", L"G:\\12.04\\param.png");
    // 读取上次设置的校准器参数，默认值为空
    m_rCalibratorParam = IniUtil::readStringValue(tCfgFile, L"camera", L"calibrator_param", L"");
    
    // 输出当前函数名、相机参数、打印宽度和高度信息
    _cwprintf(L"%s: camera_param=%s, %.1f, %.1f\r\n", __FUNCTIONW__, m_rCameraParam, m_dPrintInfos[0], m_dPrintInfos[1]);

    // 发送自定义消息，通知相机参数更新
    PostMessage(PQM_MAIN_CAMERA_PARAM, 0, 0);

    // 读取上次选择的模板索引，默认值为0
    int iSelIndex = IniUtil::readIntValue(tCfgFile, L"template", L"current_index", 0);

    // 清空模板列表框的内容
    m_tCmbTemplateList.ResetContent();

    // 检查模板管理器是否存在
    if (m_ptTemplateManager != nullptr) {
        // 获取模板列表的指针
        vector<TemplateManager::T_Template>* ptVecTemplateList = m_ptTemplateManager->getTemplateList();

        // 检查模板列表指针是否有效
        if (ptVecTemplateList != nullptr) {
            // 遍历模板列表
            for (int i = 0; i < ptVecTemplateList->size(); i++) {
                // 将模板名称插入到模板列表框中
                m_tCmbTemplateList.InsertString(i, String2Wstring(ptVecTemplateList->at(i).tName).c_str());
            }

            // 设置模板列表框的当前选中项
            m_tCmbTemplateList.SetCurSel(iSelIndex);

#ifdef USE_HALCON
            // 获取当前选中的模板参数
            TemplateManager::PT_Template ptTemplate = m_ptTemplateManager->getTemplateParam(iSelIndex);

            // 检查模板参数是否有效
            if (ptTemplate != nullptr) {
                // 获取模板名称
                string tNewName = ptTemplate->tName;

                // 检查模板名称是否为空
                if (tNewName.length() > 0) {
                    // 初始化Halcon匹配参数
                    PQ_InitParam(tNewName.c_str());
                }
            }
#endif
        }
    }

    // 更新变量数据到对话框
    UpdateData(FALSE);

    // 输出当前函数名和相机参数信息
    _cwprintf(L"%s end: camera_param=%s\r\n", __FUNCTIONW__, m_rCameraParam);
    // 注释掉的代码，用于输出设置的相机参数信息
    //_cwprintf(L"%s end: camera_param=%s\r\n", __FUNCTIONW__, m_tSettingCameraParam);
}

// 发送匹配参数到匹配模块
/**
 * @brief 更新匹配参数
 *
 * 此函数用于根据当前选择的模板更新匹配参数。它从模板管理器中获取当前选中的模板，
 * 并根据模板的属性设置匹配参数。根据是否定义了USE_HALCON宏，使用不同的函数来设置参数。
 *
 * @param 无
 * @return 无
 */
 //模板管理器实际上是一个类，用于管理模板的加载、保存和操作。
 //在这个函数中，m_ptTemplateManager是一个指向模板管理器实例的指针。
void CVisionLocalizationDlg::updateMatchParam()
{
    // 检查模板管理器是否存在
    if (m_ptTemplateManager != nullptr) {
        // 获取当前选中的模板参数
        TemplateManager::PT_Template ptTemplate = m_ptTemplateManager->getTemplateParam(m_tCmbTemplateList.GetCurSel());//m_tCmbTemplateList.GetCurSel()用于获取当前在组合框中选中的模板索引。

        // 检查模板参数是否有效
        if (ptTemplate != nullptr) {
            // 获取配置文件的路径
            CString tCfgFile = FileUtil::getAppPath(UV_MATCH_CFG);
            // 读取调试启用标志，默认值为0
            int iDebug = IniUtil::readIntValue(tCfgFile, L"template", L"debug_enable", 0);
            // 如果未定义USE_HALCON宏，使用普通匹配库
            #ifndef USE_HALCON
                // 设置调试启用标志
                pq_matchSetParam("debug_enable", iDebug ? "1" : "0", false);

                // 用于存储转换后的参数值
                char acValue[32];
                // 将匹配方法转换为字符串并设置参数
                sprintf(acValue, "%d", ptTemplate->iMethod);
                pq_matchSetParam("match_method", acValue, false);
                // 将匹配层数转换为字符串并设置参数
                sprintf(acValue, "%d", ptTemplate->iLayer);
                pq_matchSetParam("match_layer", acValue, false);

                // 将匹配分数转换为字符串并设置参数
                sprintf(acValue, "%d", ptTemplate->iScore);
                pq_matchSetParam("match_score", acValue, false);

                // 根据搜索类型设置最大匹配位置
                switch (ptTemplate->iSearchType) {
                    case 1:     pq_matchSetParam("match_max_pos", "500",  false); break;
                    case 2:     pq_matchSetParam("match_max_pos", "1000", false); break;
                    case 3:     pq_matchSetParam("match_max_pos", "2000", false); break;
                    default:    pq_matchSetParam("match_max_pos", "75",   false); break;
                }

                // 将中心X偏移量转换为字符串并设置参数
                sprintf(acValue, "%d", ptTemplate->iCenterOffsetX);
                pq_matchSetParam("match_center_x_offset", acValue, false);

                // 将中心Y偏移量转换为字符串并设置参数
                sprintf(acValue, "%d", ptTemplate->iCenterOffsetY);
                pq_matchSetParam("match_center_y_offset", acValue, false);

                // 将对比度比率转换为字符串并设置参数
                sprintf(acValue, "%d", ptTemplate->iContrastRatio);
                pq_matchSetParam("match_contrast_ratio", acValue, false);

                // 将最大重叠率转换为字符串并设置参数
                sprintf(acValue, "%d", ptTemplate->iMaxOverLap);
                pq_matchSetParam("match_max_over_lap", acValue, false);

                // 构建匹配模型的路径并设置参数
                string tModeName = CString2String(FileUtil::getAppPath(TEMPLATE_MODEL)).c_str() + string("\\") + ptTemplate->tName;
                pq_matchSetParam("match_model", tModeName.c_str(), true);
            // 如果定义了USE_HALCON宏，使用Halcon匹配库
            #else
                // 设置调试启用标志
                PQ_SetParam("debug_enable", iDebug ? "1" : "0");

                // 开始Halcon匹配过程
                PQ_MatchBegin(ptTemplate->tName.c_str());
            #endif
        }
    }
}

// 保存打印相关参数
// 该函数用于将打印相关的参数保存到配置文件中
void CVisionLocalizationDlg::saveConfigParam()
{
    // 获取配置文件的路径
    CString tCfgFile = FileUtil::getAppPath(UV_MATCH_CFG);

    // 将打印宽度参数写入配置文件
    IniUtil::writeIntValue(tCfgFile,    L"print",  L"width",  (int)m_dPrintInfos[0]);
    // 将打印高度参数写入配置文件
    IniUtil::writeIntValue(tCfgFile,    L"print",  L"height", (int)m_dPrintInfos[1]);
    // 将相机参数写入配置文件
    IniUtil::writeStringValue(tCfgFile, L"camera", L"param",   m_rCameraParam);
    // 将校准器参数写入配置文件
    IniUtil::writeStringValue(tCfgFile, L"camera", L"calibrator_param", m_rCalibratorParam);

    // 注释掉的代码，原本用于将另一个相机参数写入配置文件
    // IniUtil::writeStringValue(tCfgFile, L"camera", L"param", m_tSettingCameraParam);
}

// 更新打印相关参数
/**
 * @brief 更新打印相关参数
 *
 * 此函数用于从配置文件中读取并更新打印相关的参数，包括相机参数、校准器参数和调试启用标志。
 * 根据是否定义了USE_HALCON宏，使用不同的函数来设置调试启用标志。
 *
 * @param 无
 * @return 无
 */
void CVisionLocalizationDlg::updateConfigParam()
{
    // 获取配置文件的路径
    CString tCfgFile = FileUtil::getAppPath(UV_MATCH_CFG);

    // 更新对话框数据到变量
    UpdateData(TRUE);
    // 从配置文件中读取相机参数，若不存在则使用默认值
    m_rCameraParam        = IniUtil::readStringValue(tCfgFile, L"camera", L"param", L"G:\\12.04\\param.png");
    // 从配置文件中读取校准器参数，若不存在则使用默认值
    m_rCalibratorParam  = IniUtil::readStringValue(tCfgFile, L"camera", L"calibrator_param", L"");
    // 更新变量数据到对话框
    UpdateData(FALSE);

    // 从配置文件中读取调试启用标志，若不存在则使用默认值0
    int iDebug = IniUtil::readIntValue(tCfgFile, L"template", L"debug_enable", 0);

    // 如果未定义USE_HALCON宏，使用普通匹配库设置调试启用标志
    #ifndef USE_HALCON
        pq_matchSetParam("debug_enable", iDebug ? "1" : "0", true);
    // 如果定义了USE_HALCON宏，使用Halcon匹配库设置调试启用标志
    #else
        PQ_SetParam("debug_enable", iDebug ? "1" : "0");
    #endif
}

/**
 * @brief 初始化应用程序实例，确保软件单实例运行
 * 
 * 此函数创建一个互斥体，用于检查软件是否已经启动。
 * 如果互斥体已经存在，则表示软件已经在运行，此时会弹出提示框并退出应用程序。
 * 
 * @return BOOL 如果互斥体创建成功且软件未启动，则返回TRUE；否则退出应用程序。
 */
BOOL CVisionLocalizationDlg::InitInstance()
{
    // 创建一个互斥体，用于确保软件单实例运行
    HANDLE hMutex = CreateMutex(NULL, TRUE, L"uv视觉打印软件");

    // 检查互斥体是否已经存在
    // 如果互斥体已经存在，则GetLastError()会返回ERROR_ALREADY_EXISTS
    if (GetLastError() == ERROR_ALREADY_EXISTS) {
        // 关闭互斥体句柄，释放资源
        CloseHandle(hMutex);

        // 弹出消息框，提示用户软件已启动
        AfxMessageBox(L"软件已启动");
        // 退出应用程序
        exit(0);
    }

    // 如果互斥体创建成功且软件未启动，则返回TRUE
    return TRUE;
}

/**
 * @brief 设置打印机相关控件的启用状态
 * 
 * 此函数用于根据传入的布尔值参数 `ret` 来启用或禁用与打印机操作相关的一系列控件。
 * 
 * @param ret 布尔值，`true` 表示启用控件，`false` 表示禁用控件。
 */
void CVisionLocalizationDlg::SetPrinterEnable(bool ret)
{
    // 启用或禁用加载相机参数的按钮
    GetDlgItem(IDC_BT_MAIN_CAMERA_PARAM)->EnableWindow(ret);        // 加载按钮
    // 启用或禁用校准按钮
    GetDlgItem(IDC_BT_MAIN_CALIBRATOR)->EnableWindow(ret);         // 校准按钮
	    // 启用或禁用模板管理按钮
	GetDlgItem(IDC_BT_MAIN_MAKE_FEATURE)->EnableWindow(ret);       // 模版管理按钮

    // 启用或禁用采集按钮
    GetDlgItem(IDC_BT_MAIN_GATHER)->EnableWindow(ret);             // 采集按钮
    // 启用或禁用系统参数设置按钮
    GetDlgItem(IDC_BTN_SYS_PARAM)->EnableWindow(ret);              // 参数设置按钮
    // 启用或禁用保存采集图像的按钮
    GetDlgItem(IDC_BT_MAIN_SAVEBACKGROUND)->EnableWindow(ret);     // 保存采集图按钮

    // 注释掉的代码，原本用于启用或禁用更改特征的按钮
    //GetDlgItem(IDC_BT_MAIN_CHANGE_FEATURE)->EnableWindow(ret);

    // 启用或禁用打印面积宽度的编辑框
    GetDlgItem(IDC_ET_MAIN_WIDTH)->EnableWindow(ret);              // 打印面积宽 编辑框
    // 启用或禁用打印面积高度的编辑框
    GetDlgItem(IDC_ET_MAIN_HEIGHT)->EnableWindow(ret);             // 打印面积高 编辑框
    // 启用或禁用模板管理的下拉框
    GetDlgItem(IDC_CB_MAIN_MODEL)->EnableWindow(ret);              // 模版管理下拉框
    // 启用或禁用相机板卡的下拉框
    GetDlgItem(IDC_CB_MAIN_BOARD)->EnableWindow(ret);              // 相机板卡下拉框
    // 启用或禁用相机设备的下拉框
    GetDlgItem(IDC_CB_MAIN_CAMERA)->EnableWindow(ret);             // 相机设备下拉框
}

/**
 * @brief 设置采集相关控件的启用状态
 * 
 * 此函数用于根据传入的布尔值参数 `ret` 来启用或禁用与采集操作相关的一系列控件。
 * 
 * @param ret 布尔值，`true` 表示启用控件，`false` 表示禁用控件。
 */
void CVisionLocalizationDlg::SetGatherEnable(bool ret)
{
    // 启用或禁用加载相机参数的按钮
    GetDlgItem(IDC_BT_MAIN_CAMERA_PARAM)->EnableWindow(ret);        // 加载按钮
    // 启用或禁用校准按钮
    GetDlgItem(IDC_BT_MAIN_CALIBRATOR)->EnableWindow(ret);         // 校准按钮
    // 启用或禁用打印按钮
    GetDlgItem(IDC_BT_MAIN_PRINTER)->EnableWindow(ret);             // 打印按钮
    // 启用或禁用系统参数设置按钮
    GetDlgItem(IDC_BTN_SYS_PARAM)->EnableWindow(ret);              // 参数设置按钮
    // 启用或禁用保存采集图像的按钮
    GetDlgItem(IDC_BT_MAIN_SAVEBACKGROUND)->EnableWindow(ret);     // 保存采集图按钮
    // 启用或禁用模板管理按钮
    GetDlgItem(IDC_BT_MAIN_MAKE_FEATURE)->EnableWindow(ret);       // 模版管理按钮
    // 启用或禁用打印面积宽度的编辑框
    GetDlgItem(IDC_ET_MAIN_WIDTH)->EnableWindow(ret);              // 打印面积宽 编辑框
    // 启用或禁用打印面积高度的编辑框
    GetDlgItem(IDC_ET_MAIN_HEIGHT)->EnableWindow(ret);             // 打印面积高 编辑框
    // 启用或禁用模板管理的下拉框
    GetDlgItem(IDC_CB_MAIN_MODEL)->EnableWindow(ret);              // 模版管理下拉框
    // 启用或禁用相机板卡的下拉框
    GetDlgItem(IDC_CB_MAIN_BOARD)->EnableWindow(ret);              // 相机板卡下拉框
    // 启用或禁用相机设备的下拉框
    GetDlgItem(IDC_CB_MAIN_CAMERA)->EnableWindow(ret);             // 相机设备下拉框
}

/**
 * @brief 执行数据交换操作，用于将对话框控件与成员变量进行绑定
 *
 * 此函数继承自 CCameraParamDlg，负责将对话框中的控件与 CVisionLocalizationDlg 类的成员变量进行数据交换。
 * 通过 DDX_Text 和 DDX_Control 宏，实现了文本框、按钮、组合框、进度条等控件与对应成员变量的双向绑定。
 *
 * @param pDX 指向 CDataExchange 对象的指针，用于管理数据交换过程
 */
void CVisionLocalizationDlg::DoDataExchange(CDataExchange* pDX)
{
    // 调用基类 CCameraParamDlg 的 DoDataExchange 函数，进行基类的数据交换操作
    CCameraParamDlg::DoDataExchange(pDX);
    // 注释掉的代码，原本用于调用 CFleeceDlg 的 DoDataExchange 函数
    //CFleeceDlg::DoDataExchange(pDX);

    // 将 IDC_ET_MAIN_TIFF 文本框与成员变量 m_tProductName 进行数据交换
    DDX_Text(pDX, IDC_ET_MAIN_TIFF,    m_tProductName);
    // 将 IDC_ET_MAIN_WIDTH 文本框与成员变量 m_dPrintInfos[0] 进行数据交换
    DDX_Text(pDX, IDC_ET_MAIN_WIDTH,   m_dPrintInfos[0]);
    // 将 IDC_ET_MAIN_HEIGHT 文本框与成员变量 m_dPrintInfos[1] 进行数据交换
    DDX_Text(pDX, IDC_ET_MAIN_HEIGHT,  m_dPrintInfos[1]);
    // 将 IDC_STATIC_DISTANCE 静态文本框与成员变量 m_tPrintHeiStatic 进行数据交换
    DDX_Text(pDX, IDC_STATIC_DISTANCE, m_tPrintHeiStatic);
    // 将 IDC_STATIC_CARSPEED 静态文本框与成员变量 m_tPrintSpeedStatic 进行数据交换
    DDX_Text(pDX, IDC_STATIC_CARSPEED, m_tPrintSpeedStatic);
    // 将 IDC_STATIC_VOLTAGE 静态文本框与成员变量 m_tLightVoltageStatic 进行数据交换
    DDX_Text(pDX, IDC_STATIC_VOLTAGE,  m_tLightVoltageStatic);

    // 将 IDC_BT_MAIN_PRINTER 按钮控件与成员变量 m_tBtnPrint 进行关联
    DDX_Control(pDX, IDC_BT_MAIN_PRINTER, m_tBtnPrint);
    // 将 IDC_BT_MAIN_GATHER 按钮控件与成员变量 m_tBtnGather 进行关联
    DDX_Control(pDX, IDC_BT_MAIN_GATHER,  m_tBtnGather);
    // 将 IDC_BT_MAIN_MAKE_FEATURE 按钮控件与成员变量 m_tModelBtn 进行关联
    DDX_Control(pDX, IDC_BT_MAIN_MAKE_FEATURE, m_tModelBtn);
    // 将 IDC_CB_MAIN_MODEL 组合框控件与成员变量 m_tCmbTemplateList 进行关联
    DDX_Control(pDX, IDC_CB_MAIN_MODEL,   m_tCmbTemplateList);
    // 将 IDC_PROGRESS_PRINT 进度条控件与成员变量 m_tProgressPrint 进行关联
    DDX_Control(pDX, IDC_PROGRESS_PRINT,  m_tProgressPrint);

    // 将 IDC_BTN_SYS_PARAM 按钮控件与成员变量 m_tSysParamBtn 进行关联
    DDX_Control(pDX, IDC_BTN_SYS_PARAM, m_tSysParamBtn);
    // 将 IDC_BT_MAIN_SAVEBACKGROUND 按钮控件与成员变量 m_tSaveBgBtn 进行关联
    DDX_Control(pDX, IDC_BT_MAIN_SAVEBACKGROUND, m_tSaveBgBtn);
    // 注释掉的代码，原本用于将 IDC_BT_MAIN_CAPTURE_IMAGE 按钮控件与成员变量 m_tCaptureImageBtn 进行关联
    //DDX_Control(pDX, IDC_BT_MAIN_CAPTURE_IMAGE, m_tCaptureImageBtn);
    // 注释掉的代码，原本用于将 IDC_BT_MAIN_CUT_IMAGE 按钮控件与成员变量 m_tCutImageBtn 进行关联
    //DDX_Control(pDX, IDC_BT_MAIN_CUT_IMAGE, m_tCutImageBtn);
    // 将 IDC_BT_MAIN_NEWTEMP 按钮控件与成员变量 m_tNewTempBtn 进行关联
    DDX_Control(pDX, IDC_BT_MAIN_NEWTEMP, m_tNewTempBtn);
}

/**
 * @brief 设置工具提示文本
 *
 * 此函数用于根据控件ID和当前选中的模板设置工具提示文本。
 * 当鼠标悬停在特定控件上时，会显示相应的提示信息。
 *
 * @param id 工具提示的ID
 * @param ptNmHdr 指向NMHDR结构的指针，包含通知消息的信息
 * @param pResult 指向LRESULT的指针，用于返回处理结果
 * @return BOOL 如果成功设置工具提示文本，则返回TRUE；否则返回FALSE
 */
BOOL CVisionLocalizationDlg::SetTipText(UINT id, NMHDR* ptNmHdr, LRESULT* pResult)
{
    // 检查ptNmHdr指针是否为空
    if (ptNmHdr != nullptr) {
        // 将ptNmHdr转换为TOOLTIPTEXT指针，用于处理工具提示文本
        TOOLTIPTEXT* ptTooltipText = (TOOLTIPTEXT*)ptNmHdr;

        // 获取通知消息的来源控件ID
        UINT nID = ptNmHdr->idFrom;

        // 检查工具提示的标志是否包含TTF_IDISHWND，表示ID是窗口句柄
        if (ptTooltipText->uFlags & TTF_IDISHWND) {
            // 获取当前选中的模板参数
            TemplateManager::PT_Template ptTemplate = m_ptTemplateManager->getTemplateParam(m_tCmbTemplateList.GetCurSel());

            // 检查模板参数是否有效
            if (ptTemplate != nullptr) {
                // 将窗口句柄转换为对话框控件ID
                nID = ::GetDlgCtrlID((HWND)nID);

                // 根据控件ID设置工具提示文本
                switch (nID) {
                // 当控件ID为IDC_CB_MAIN_MODEL时
                case IDC_CB_MAIN_MODEL:
                    // 将模板文件路径转换为宽字符串并复制到工具提示文本中
                    _tcscpy(ptTooltipText->lpszText, String2Wstring(ptTemplate->tTemplateFile).c_str());
                    break;

                // 当控件ID为IDC_ET_MAIN_TIFF时
                case IDC_ET_MAIN_TIFF:
                    // 将产品文件路径转换为宽字符串并复制到工具提示文本中
                    _tcscpy(ptTooltipText->lpszText, String2Wstring(ptTemplate->tProductFile).c_str());
                    break;

                // 当控件ID为IDC_ET_MAIN_CAMERA_PARAM时
                case IDC_ET_MAIN_CAMERA_PARAM:
                    // 将相机参数复制到工具提示文本中
                    _tcscpy(ptTooltipText->lpszText, m_rCameraParam);
                    break;

                // 其他情况
                default:
                    break;
                }
            }

            // 返回TRUE表示成功处理工具提示文本
            return TRUE;
        }
    }

    // 返回FALSE表示未成功处理工具提示文本
    return FALSE;
}

/**
 * @brief 初始化对话框
 *
 * 此函数在对话框初始化时调用，负责设置对话框的图标、检查配置文件目录、创建工具提示、初始化模板管理器、注册设置、填充数据、初始化进度条、创建图像控件、设置按钮图标以及注册匹配线程等操作。
 *
 * @return BOOL 初始化成功返回 TRUE
 */
BOOL CVisionLocalizationDlg::OnInitDialog()
{
    // 从应用程序资源中加载指定 ID 的图标
    HICON hIcon = AfxGetApp()->LoadIcon(IDI_ICON);//hIcon用于存储应用程序的图标资源的句柄。AfxGetApp()用于获取当前应用程序的实例指针，LoadIcon()用于从应用程序资源中加载指定 ID 的图标。IDI_ICON是一个预定义的图标资源 ID。

    // 设置对话框的大图标
    SetClassLongPtr(m_hWnd, GCLP_HICON, (LONG_PTR)hIcon);//SetClassLongPtr()用于设置窗口类的长指针值。m_hWnd是对话框窗口的句柄，GCLP_HICON是一个预定义的窗口类长指针索引，表示窗口类的大图标。(LONG_PTR)hIcon将图标资源的句柄转换为长指针类型。
    // 设置对话框的小图标
    SetClassLongPtr(m_hWnd, GCLP_HICONSM, (LONG_PTR)hIcon);//SetClassLongPtr()用于设置窗口类的长指针值。m_hWnd是对话框窗口的句柄，GCLP_HICONSM是一个预定义的窗口类长指针索引，表示窗口类的小图标。(LONG_PTR)hIcon将图标资源的句柄转换为长指针类型。

    // 检查并创建必要的配置文件目录
    checkAndCreateDirectory();

    // 创建工具提示控件
    m_tToolTip.Create(this);//m_tToolTip是一个工具提示控件对象。Create()用于创建工具提示控件。this表示当前对话框对象的指针。
    // 为模板列表组合框添加工具提示，使用回调函数提供提示文本
    m_tToolTip.AddTool(GetDlgItem(IDC_CB_MAIN_MODEL), LPSTR_TEXTCALLBACK);//GetDlgItem()用于获取对话框中的控件。IDC_CB_MAIN_MODEL是一个预定义的控件 ID，表示模板列表组合框。AddTool()用于向工具提示控件添加工具。LPSTR_TEXTCALLBACK是一个预定义的回调函数指针，表示工具提示文本的回调函数。
    // 为 TIFF 文件输入框添加工具提示，使用回调函数提供提示文本
    m_tToolTip.AddTool(GetDlgItem(IDC_ET_MAIN_TIFF), LPSTR_TEXTCALLBACK);
    // 为相机参数输入框添加工具提示，使用回调函数提供提示文本
    m_tToolTip.AddTool(GetDlgItem(IDC_ET_MAIN_CAMERA_PARAM), LPSTR_TEXTCALLBACK);

    // 获取模板管理器的单例实例
    m_ptTemplateManager = TemplateManager::instance();

    // 注册 PQ 设置
    PQSetting::Reg();

    // 调用基类的对话框初始化函数
    CFleeceDlg::OnInitDialog();

    // 填充模板列表和相关数据
    fillData();
    // 触发模板列表选择改变事件，以显示预览图
    OnCbnSelchangeCbMainModel();

    // 设置进度条的范围为 0 到 100
    m_tProgressPrint.SetRange(0, 100);
    // 将进度条的位置设置为 0
    m_tProgressPrint.SetPos(0);

    // 定义一个矩形对象，用于存储窗口的位置和大小
    CRect rt;

    // 获取视频显示控件的窗口矩形，并将其转换为客户端坐标
    ::GetWindowRect(GetDlgItem(IDC_ST_VIDEO)->GetSafeHwnd(), rt);//GetDlgItem()用于获取对话框中的控件。IDC_ST_VIDEO是一个预定义的控件 ID，表示视频显示控件。GetSafeHwnd()用于获取控件的安全句柄。
    ScreenToClient(rt);//ScreenToClient()用于将屏幕坐标转换为客户端坐标。
//m_cImControl是一个图像控件对象，用于显示图像。
    // 设置图像控件的图标路径
    m_cImControl.SetIcon(L"D:\\ImControl");//SetIcon()用于设置图像控件的图标路径。L"D:\\ImControl"是一个字符串常量，表示图像控件的图标路径。
    // 设置图像控件的初始图片为空
    m_cImControl.SetPic(L"");//SetPic()用于设置图像控件的初始图片。L""是一个空字符串，表示图像控件的初始图片为空。
    // 创建图像控件，设置其样式和位置
    //Create()用于创建图像控件。L""是一个空字符串，表示图像控件的初始图片为空。
    //WS_CHILD | WS_VISIBLE | SS_NOTIFY是一个窗口样式，用于指定图像控件的样式。WS_CHILD表示图像控件是一个子窗口，WS_VISIBLE表示图像控件是可见的，SS_NOTIFY表示图像控件支持通知消息。
    m_cImControl.Create(L"", WS_CHILD | WS_VISIBLE | SS_NOTIFY, CRect(rt.left, rt.top, rt.right, rt.bottom), this, IDC_ST_VIDEO);

    // 加载打印按钮的图标资源
    //LoadImage()用于从指定的资源中加载图像。AfxGetInstanceHandle()用于获取当前应用程序的实例句柄。MAKEINTRESOURCE(IDI_PRINT_BTN_ICON)是一个宏，用于将资源 ID 转换为资源句柄。
    //IMAGE_ICON表示要加载的资源是一个图标。28, 28表示要加载的图标的宽度和高度。LR_DEFAULTCOLOR | LR_CREATEDIBSECTION是一个加载标志，用于指定加载图像的方式。
    HICON hBtnIcon = (HICON)LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_PRINT_BTN_ICON), IMAGE_ICON, 28, 28, LR_DEFAULTCOLOR | LR_CREATEDIBSECTION);
    // 设置打印按钮的图标
    m_tBtnPrint.setIcon(hBtnIcon, 28);

    // 加载采集按钮的图标资源
    hBtnIcon = (HICON)LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_COLLECT_BTN_ICON), IMAGE_ICON, 28, 28, LR_DEFAULTCOLOR | LR_CREATEDIBSECTION);
    // 设置采集按钮的图标
    m_tBtnGather.setIcon(hBtnIcon, 28);

    // 加载模板管理按钮的图标资源
    hBtnIcon = (HICON)LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_MODEL_BTN_ICON), IMAGE_ICON, 28, 28, LR_DEFAULTCOLOR | LR_CREATEDIBSECTION);
    // 设置模板管理按钮的图标
    m_tModelBtn.setIcon(hBtnIcon, 28);

    // 加载新建模板按钮的图标资源
    hBtnIcon = (HICON)LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_NEW_TEMP_ICON), IMAGE_ICON, 28, 28, LR_DEFAULTCOLOR | LR_CREATEDIBSECTION);
    // 设置新建模板按钮的图标
    m_tNewTempBtn.setIcon(hBtnIcon, 28);

    // 加载设置按钮的图标资源
    hBtnIcon = (HICON)LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_SETTING_BTN_ICON), IMAGE_ICON, 28, 28, LR_DEFAULTCOLOR | LR_CREATEDIBSECTION);
    // 设置设置按钮的图标
    m_tSysParamBtn.setIcon(hBtnIcon, 28);

    // 加载保存按钮的图标资源
    hBtnIcon = (HICON)LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_SAVE_BTN_ICON), IMAGE_ICON, 28, 28, LR_DEFAULTCOLOR | LR_CREATEDIBSECTION);
    // 设置保存按钮的图标
    m_tSaveBgBtn.setIcon(hBtnIcon, 28);

    // 注册匹配线程
    CMatchTh::Reg();

    // 以下是测试代码，用于加载图像并在图像控件中显示
    // 定义一个指向 Mat 对象的指针
    /*m_ptImageMat = new Mat();
    // 从文件中读取图像，以灰度模式读取
    *m_ptImageMat = imread("D:/tmp/match-tmp/2024-11-28/image.png", cv::IMREAD_GRAYSCALE);

    // 创建一个图像指针对象
    ImPtr::CImPtr tImPtr(m_ptImageMat);

    // 清除图像控件的所有图层
    m_cImControl.ClearLayer();

    // 在图像控件中创建一个新的图层
    m_cImControl.CreateLayer(true, false, false, "", true,
        1.0,
        WString2CString(L""),
        tImPtr,
        true, 1.0f, 0, 0,
        nullptr);

    // 克隆图像并保存到采集图像矩阵中
    m_tCollectImgMat = m_ptImageMat->clone();*/

    // 初始化成功，返回 TRUE
    return TRUE;
}

/**
 * @brief 预处理消息函数，用于处理特定的键盘和鼠标消息
 * 
 * 此函数会在消息被分发到窗口过程之前进行预处理。
 * 它会检查键盘按键和鼠标移动消息，并执行相应的操作。
 * 
 * @param ptMsg 指向MSG结构体的指针，包含了消息的详细信息
 * @return BOOL 如果消息被处理，则返回TRUE；否则调用基类的预处理函数
 */
BOOL CVisionLocalizationDlg::PreTranslateMessage(MSG* ptMsg)
{
    // 检查消息类型是否为按键按下消息
    if (ptMsg->message == WM_KEYDOWN) {
        // 检查按下的键是否为回车键
        if (ptMsg->wParam == VK_RETURN) {
            // 如果按下回车键，直接返回TRUE，阻止消息继续传递
            return TRUE;
        }
        /*
        // 检查按下的键是否为ESC键
        else if (ptMsg->wParam == VK_ESCAPE) {
            // 弹出确认对话框，询问用户是否确定要退出程序
            int iRet = MessageBox(TEXT("确定要退出程序吗？"), TEXT("提示"), MB_ICONINFORMATION | MB_OKCANCEL);
            // 如果用户点击了OK按钮
            if (iRet == IDOK) {
                // 调用OnCancel函数，执行退出操作
                OnCancel();
            }
            // 返回TRUE，阻止消息继续传递
            return TRUE;
        }
        */
        // 检查按下的键是否为P键
        else if ('P' == ptMsg->wParam) {
            // 检查Ctrl键是否被按下
            if (GetKeyState(VK_CONTROL) & 0x8000) {
                // 如果Ctrl+P被按下，调用截图函数
                OnBnClickedBtMainCutImage();
            }
        }
        // 检查按下的键是否为W键
        else if ('W' == ptMsg->wParam) {
            // 检查Ctrl键是否被按下
            if (GetKeyState(VK_CONTROL) & 0x8000) {
                // 如果Ctrl+W被按下，调用捕获图片函数
                OnBnClickedBtMainCaptureImage();
            }
        }
    }
    // 检查消息类型是否为鼠标移动消息
    else if (ptMsg->message == WM_MOUSEMOVE) {
        // 将鼠标移动消息传递给工具提示控件进行处理
        m_tToolTip.RelayEvent(ptMsg);
    }
    // 如果消息未被处理，调用基类的预处理函数
    return CFleeceDlg::PreTranslateMessage(ptMsg);
}

/**
 * @brief 处理帮助信息消息
 * 
 * 此函数用于处理帮助信息消息，当用户请求帮助信息时会调用该函数。
 * 目前该函数简单地返回 TRUE，表示帮助信息处理完成。
 * 
 * @param pHelpInfo 指向 HELPINFO 结构体的指针，包含帮助信息的相关数据
 * @return BOOL 总是返回 TRUE，表示帮助信息处理完成
 */
BOOL CVisionLocalizationDlg::OnHelpInfo(HELPINFO* pHelpInfo)
{
    // 返回 TRUE 表示帮助信息处理完成
    return TRUE;
}

/**
 * @brief 处理匹配帧消息
 * 
 * 此函数用于处理匹配帧消息，当接收到匹配帧消息时会调用该函数。
 * 它从消息参数中提取帧 ID 和帧指针，调用 PQPrinter 的 ImCapture 方法进行图像捕获操作，
 * 然后释放帧指针的内存，并返回 0 表示消息处理完成。
 * 
 * @param wparam 消息的 WPARAM 参数，包含帧 ID
 * @param lparam 消息的 LPARAM 参数，包含帧指针
 * @return LRESULT 总是返回 0，表示消息处理完成
 */
// 处理匹配帧消息的函数，当接收到匹配帧消息时会调用该函数
LRESULT CVisionLocalizationDlg::OnFrameMsg(WPARAM wparam, LPARAM lparam)
{
    // 从 wparam 中提取帧 ID
    int nFrameId = (int)wparam;

    // 从 lparam 中提取帧指针
    ImPtr::CImPtr* lpFrame = (ImPtr::CImPtr*)lparam;

    // 调用 PQPrinter 的 ImCapture 方法进行图像捕获操作
    // 传入帧ID、布尔值false、包含函数名和当前时间的元组、空指针和帧指针
    PQPrinter().ImCapture({ nFrameId, false, { {__FUNCTION__, clock()} }, nullptr, *lpFrame });

    // 释放帧指针的内存，避免内存泄漏
    delete lpFrame;

    // 返回 0 表示消息处理完成
    return 0;
}

LRESULT CVisionLocalizationDlg::OnMatrixMsg(WPARAM wparam, LPARAM lparam)
{
	//收到图像，渲染前清除
	m_cImControl.ClearLayer();

	tuple<int, bool, CPrinterBase::vTimer, array<int64, 3>, ImPtr::CImPtr, CPosition::vMatcher>* lpParam =
		(tuple<int, bool, CPrinterBase::vTimer, array<int64, 3>, ImPtr::CImPtr, CPosition::vMatcher>*)lparam;

	OnConvert(*lpParam); 
	
	delete lpParam;
	return 0;
}

// 当前采取缩小匹配显示修正由内存不足引发旋转图像崩溃
LRESULT CVisionLocalizationDlg::OnConvertMsg(WPARAM wparam, LPARAM lparam)
{
	if (!m_cImControl.Empty()) return 0;

	tuple<UserParam, vector<SingleTarget>> vMatch;

	if (!Pop(vMatch)) return 0;

	UpdateData(TRUE);

	double dScore = 0.0;//(bAuto ? 0.4 : 0.0);
	auto& iUserParam = get<0>(vMatch);
	auto& iTarget = get<1>(vMatch);

	m_cImControl.SetUserParam(new UserParam(iUserParam));

	array<int, 2> vImControlRenderSize = m_cImControl.GetImControlRenderSize();

	auto& iRtBg = get<eSI_PosEx>(iTarget[0]); // 背景图层
	double dx = (double)vImControlRenderSize[0] / iRtBg.width;
	double dy = (double)vImControlRenderSize[1] / iRtBg.height;
	double ds = min(dx, dy); // 缩放倍数

	for (int i = 0; i < iTarget.size(); ++i){
		auto& iScore = get<eSI_Score>(iTarget[i]);
		auto& iPos = get<eSI_Pos>(iTarget[i]);
		auto& iImPtr = get<eSI_ImPtr>(iTarget[i]);
		auto& iPosEx = get<eSI_PosEx>(iTarget[i]);
		auto& iImPtrEx = get<eSI_ImPtrEx>(iTarget[i]);
		double cx = 0.5 * iPosEx.width + iPosEx.x;
		double cy = 0.5 * iPosEx.height + iPosEx.y; // 中心点坐标

		if (iScore < dScore && i != 0) continue;
	
		m_cImControl.CreateLayer(!!!i, !!!i, false, "", true,
			1.0 - !!i * 0.5,
			WString2CString(i ? to_wstring(iScore) : L""),
			iImPtrEx,
			false, ds, ds * cx, ds * cy,
			(void*)new tuple<Rect, ImPtr::CImPtr>(iPos, iImPtr)); // 用户参数 = 匹配输出
	}

	SetTimer(0, 300, nullptr);

	return 0;
}

LRESULT CVisionLocalizationDlg::OnScheduleMsg(WPARAM wparam, LPARAM lparam)
{
	int64 nPrintRows = wparam, nPrtHeader = lparam;

	m_tProgressPrint.SetPos(((double)nPrintRows) / ((double)nPrtHeader) * 100);

//	double dpi = 1200; double dPixInMM = dpi / 25.4;

//	m_rSchedule.Format(L"%0.2f // %0.2f mm", (double)nPrintRows / dPixInMM, (double)nPrtHeader / dPixInMM);

	return 0;
}

LRESULT CVisionLocalizationDlg::OnImTcpMsg(WPARAM wparam, LPARAM lparam)
{
	m_tBtnPrint.SetWindowText(L"打印");

	SetPrinterEnable(true);

	m_tProgressPrint.SetPos(0);

	OnClickedFreeze();

	PQPrinter().Unreg();

	UserParam* lpUserParam = (UserParam*)m_cImControl.GetUserParam();

	!!lpUserParam ? [&lpUserParam]() { delete lpUserParam; }() : []() {}();

//	m_cImControl.ClearLayer();

	m_tBtnPrint.EnableWindow(TRUE);

	return 0;
}

LRESULT CVisionLocalizationDlg::OnSaveBackground(WPARAM wparam, LPARAM lparam)
{
	if (lparam != NULL) {
		Mat newImage = *((Mat*)lparam);

		if (!newImage.empty()) {
			m_tCollectImgMat = newImage.clone();

			if (m_bCollectFlag) {							//只有采集时才保存
				CTime t = CTime::GetCurrentTime();

				TCHAR szPath[MAX_PATH];

				DWORD dwResult = GetCurrentDirectory(MAX_PATH, szPath);

				CString strFile = szPath;

				CString str = t.Format(L"\\image\\%Y%m%d%H%M%S.png");
				strFile = strFile + str;

				// imwrite(CString2String(strFile).c_str(), newImage);

				m_iCollectNum++;

				if (m_iCollectNum == 2) {
					MessageBox(TEXT("采集完成！"), TEXT("提示"), MB_OK | MB_ICONINFORMATION);

					OnClickedGather();
				}
			}
		}
		else {
			m_tCollectImgMat = Mat();
		}
	}

	return 0;
}

LRESULT CVisionLocalizationDlg::OnUpadteCfgParamMsg(WPARAM wparam, LPARAM lparam)
{
	updateConfigParam();

	return 0;
}

void CVisionLocalizationDlg::OnClickedSend()
{
//	m_btSend.EnableWindow(FALSE);

	UserParam* lpUserParam = (UserParam*)m_cImControl.GetUserParam();

	if (m_cImControl.Empty() || !!!lpUserParam) return;

	vector<long long> vSign;

	m_cImControl.GetSign(vSign);

	CPrtMatrix::vTarget vTarget(vSign.size() - 1);// 排除目标主体

	for (int64 i = 1; i < vSign.size(); ++i) // 排除目标主体
	{
		ImLayer::tagLayer tParam;

		auto& iEnable = get<eSI_Score>(vTarget[i - 1]);

		auto& iPos = get<eSI_Pos>(vTarget[i - 1]);

		auto& iImPtr = get<eSI_ImPtr>(vTarget[i - 1]);

		m_cImControl.Get(vSign[i], tParam);

		tuple<Rect, ImPtr::CImPtr>* lpParam = (tuple<Rect, ImPtr::CImPtr>*)tParam.lpUserParam;

		iEnable = tParam.bEnable;

		iPos = get<0>(*lpParam);

		iImPtr = get<1>(*lpParam);

		delete lpParam;
	}

	get<eMI_Timer>(*lpUserParam).push_back({ __FUNCTION__, clock() });

	tuple<CPrtMatrix::UserParam, CPrtMatrix::vTarget>* lpMatch =
		new tuple<CPrtMatrix::UserParam, CPrtMatrix::vTarget>(*lpUserParam, vTarget);

	PQPrinter().PrtMatrix({ 0, 0, {}, lpMatch, ImPtr::CImPtr() });

	m_cImControl.SetUserParam(nullptr);// m_cImControl.ClearLayer(); 
	
	delete lpUserParam;

	PostMessage(PQM_MATCH_SENT); 
}

LRESULT CVisionLocalizationDlg::OnSentMsg(WPARAM wparam, LPARAM lparam)
{
	PostMessage(PQM_MATCH_CONVERT);

	return 0;
}

void CVisionLocalizationDlg::OnTimer(UINT_PTR nIDEvent)
{
	KillTimer(nIDEvent);

	if (!m_bCollectFlag) {								//采集时不发送匹配后的数据
		OnClickedSend();
	}

	CDialog::OnTimer(nIDEvent);
}

// 打印 - 贴图选择
void CVisionLocalizationDlg::OnClickedTIFF()
{
	UpdateData(TRUE);

	CFileDialog dlg(TRUE, NULL, L"F:\\f\\VisionLocalization\\x64\\Release\\temp\\", OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, L"Print (*.tif)|*.tif||");

	if (dlg.DoModal() != IDOK) return;

	m_tProductFile = dlg.GetPathName();
	m_tProductName = FileUtil::getFileName(m_tProductFile, false);

	UpdateData(FALSE);
}

// 打印
void CVisionLocalizationDlg::OnClickedPrinter()
{
	int iSelIndex = m_tCmbTemplateList.GetCurSel();

	if (iSelIndex < 0) {
		MessageBox(TEXT("请先选择模版再打印！"), TEXT("打印"), MB_OK | MB_ICONERROR);
		return;
	}

	TemplateManager::PT_Template ptTemplate = m_ptTemplateManager->getTemplateParam(iSelIndex);

	if (ptTemplate != nullptr) {
		if (!PathFileExists(String2Wstring(ptTemplate->tTemplateFile).c_str())) {
			MessageBox(TEXT("当前模版文件不存在！"), TEXT("打印"), MB_OK | MB_ICONERROR);
			return;
		}

		if (!PathFileExists(m_tProductFile)) {
			MessageBox(TEXT("当前产品贴图文件不存在！"), TEXT("打印"), MB_OK | MB_ICONERROR);
			return;
		}

		if (1 == ptTemplate->iMethod) {												//如果是形状匹配，则要确保有模型文件
			if (!m_ptTemplateManager->isModelExist(ptTemplate->tName)) {
				MessageBox(TEXT("当前模型文件不存在！"), TEXT("打印"), MB_OK | MB_ICONERROR);
				return;
			}
		}

		UpdateData(TRUE);

		CString str;

		m_tBtnPrint.GetWindowText(str);

		m_bCollectFlag = false;
		m_iCollectNum = 0;

		if (str == L"停止") {
			m_tBtnPrint.SetWindowText(L"打印");

			CFleeceDlg::Stop();

			SetPrinterEnable(true);
		}
		else{
		
			SetPrinterEnable(false);

			m_tBtnPrint.SetWindowText(L"停止");

			//判断模板，传入匹配参数
			updateMatchParam();

			updateConfigParam();

			//		m_tProgressPrint.SetPos(0);
			//		m_tBtnPrint.EnableWindow(FALSE);

			setSkipExtSize(0);					//y方向跳白
			OnClickedGrab();
			m_vCollectImgMat.clear();

			Mat M(3, 3, CV_64FC1);

			memcpy(M.data, m_tCameraParam.M.data(), sizeof(double) * m_tCameraParam.M.size());

			array<double, 2> vPixInMM = { m_tCameraParam.vDPI1[0] / 25.4, m_tCameraParam.vDPI1[1] / 25.4 }; // 打印 像素/mm

			PQPrinter().SetImInLine(m_tCameraParam.nExtTriggerFrameCount);
			PQPrinter().SetForegroundThreshold(m_tCameraParam.nForeground);
			PQPrinter().SetTransparentThreshold(m_tCameraParam.nForeground);
			PQPrinter().SetSkipFrame({ PQPrinter().GetImInLine(), 2 * PQPrinter().GetImInLine() });// 针对光电传感器补丁
			PQPrinter().SetSkipLine(m_tCameraParam.nPassInStride);
			PQPrinter().SetImWH(m_tCameraParam.vWH1);
			PQPrinter().SetImWH2(m_tCameraParam.vWH2);
			PQPrinter().SetImWH2Ex(m_tCameraParam.vWH2Ex);
			PQPrinter().SetImHomography(M);
			PQPrinter().SetImCompensation(m_tCameraParam.vFill);	// 透视变换参数
			PQPrinter().SetAligParam({ m_tCameraParam.nForeground, 0, m_tCameraParam.nPixelInPass, 32, m_tCameraParam.nErode, m_tCameraParam.nDilate }); // 设置参照物对齐参数
			PQPrinter().SetCmeraDpi(m_tCameraParam.vDPI2);
			PQPrinter().SetPrinteDpi(m_tCameraParam.vDPI1);
			PQPrinter().SetOffset(m_tCameraParam.vDist); // 偏移距离 // 该值为相机dpi
			PQPrinter().SetPrintHeader({ (int)(vPixInMM[0] * m_dPrintInfos[0]),  (int)(vPixInMM[1] * m_dPrintInfos[1]) });	// 打印dpi

			auto iCaDpi = PQPrinter().GetCmeraDpi();
			auto iPrDpi = PQPrinter().GetPrinteDpi();
			array<double, 2> dPr2Camer = { iCaDpi[0] / iPrDpi[0], iCaDpi[1] / iPrDpi[1] }; // 相机原始采图 // 此时相机dpi尚未做对齐
			array<int, 2> vHeader = PQPrinter().GetPrintHeader();

			// 转相机dpi
			array<double, 2> vArea = { dPr2Camer[0] * vHeader[0], dPr2Camer[1] * vHeader[1] };
			double dFrameInLine = (double)(vArea[0] - m_tCameraParam.vDist[0]) / 30000.0;
			double dX2Y = iCaDpi[1] / iCaDpi[0];

			cout << "area: " << vArea[0] << ", " << vArea[1] << " | " << dFrameInLine << endl;

			// 视野y在测试过程中采取20cm
			array<int, 2> vSight = { (int)(dX2Y * vArea[0]) - m_tCameraParam.vDist[0], (int)(13056 * 2.0) }; // 相机实际需要提供的视野填充 = prt.w - 填充.w // 此时相机dpi需要对齐

			Mat tTiffMat;

			ReadTif(CString2String(m_tProductFile), tTiffMat);
			PQPrinter().SetSight(vSight);
			PQPrinter().Enable1bitDll(false);
			PQPrinter().SetPrinterType(CPrinterBase::ePT_FastPrint);
			PQPrinter().SetInitiator(this);

			// 输出测试过程
			PQPrinter().SetBaseDir(CString2String(L"G:\\11.15\\"));

			PQPrinter().SetTIFF(CString2String(m_tProductFile), tTiffMat.channels()); // m_rTIFF
			//	PQPrinter().SetTIFF(CString2String(m_rTIFF), cTIFF.channels()); // m_rTIFF

			Mat tTemplateMat = imread(ptTemplate->tTemplateFile, IMREAD_GRAYSCALE);
			PQPrinter().SetTemplate(tTemplateMat);

			Mat tMaskMask = imread(ptTemplate->tMaskFile, IMREAD_GRAYSCALE);
			if (!tMaskMask.empty())
				PQPrinter().SetMask(tMaskMask); // mask

			PQPrinter().Reg(false);
		}
	}

	m_tProgressPrint.SetPos(0);

	#ifdef USE_HALCON
		PQ_MatchEnd();
	#endif
	return;
}

LRESULT CVisionLocalizationDlg::OnTempMsg(WPARAM wparam, LPARAM lparam)
{

	return 0;
}


// 制作模板
void CVisionLocalizationDlg::OnClickedMakeFeature()
{
	TemplateManagerDlg tDlg(this);

	tDlg.setSelListItem(m_tCmbTemplateList.GetCurSel());
	tDlg.DoModal();

	//重新刷新模板列表
	fillData();
	OnCbnSelchangeCbMainModel();
}

//采集
void CVisionLocalizationDlg::OnClickedGather()
{
	UpdateData(TRUE);

	m_bCollectFlag = true;
	m_iCollectNum = 0;

	CString str;
	m_tBtnGather.GetWindowText(str);

	if (str == L"停止"){
		CFleeceDlg::Stop();

		SetGatherEnable(true);

		m_tBtnPrint.EnableWindow(true);
		m_tBtnGather.SetWindowText(L"采集");
	}
	else{
		TCHAR szPath[MAX_PATH];
		DWORD dwResult = GetCurrentDirectory(MAX_PATH, szPath);

		SetGatherEnable(false);
		m_vCollectImgMat.clear();

		// SendMessage(PQM_IM_LAYER_CLEAR);
		// m_cImControl.ClearLayer();

		m_tBtnPrint.EnableWindow(false);
		m_tBtnGather.SetWindowText(L"停止");

		setSkipExtSize(3);								//y方向跳白，采集时使用跳白加快速度

		//判断模板，传入匹配参数
		updateMatchParam();

		OnClickedGrab();

		Mat M(3, 3, CV_64FC1);

		memcpy(M.data, m_tCameraParam.M.data(), sizeof(double) * m_tCameraParam.M.size());

		array<double, 2> vPixInMM = { m_tCameraParam.vDPI1[0] / 25.4, m_tCameraParam.vDPI1[1] / 25.4 }; // 打印 像素/mm

		PQPrinter().SetImInLine(m_tCameraParam.nExtTriggerFrameCount);
		PQPrinter().SetForegroundThreshold(m_tCameraParam.nForeground);
		PQPrinter().SetTransparentThreshold(m_tCameraParam.nForeground);
		PQPrinter().SetSkipFrame({ PQPrinter().GetImInLine(), 2 * PQPrinter().GetImInLine() });// 针对光电传感器补丁
		PQPrinter().SetSkipLine(m_tCameraParam.nPassInStride);
		PQPrinter().SetImWH(m_tCameraParam.vWH1);
		PQPrinter().SetImWH2(m_tCameraParam.vWH2);
		PQPrinter().SetImWH2Ex(m_tCameraParam.vWH2Ex);
		PQPrinter().SetImHomography(M);
		PQPrinter().SetImCompensation(m_tCameraParam.vFill);	// 透视变换参数
		PQPrinter().SetAligParam({ m_tCameraParam.nForeground, 0, m_tCameraParam.nPixelInPass, 32, m_tCameraParam.nErode, m_tCameraParam.nDilate }); // 设置参照物对齐参数
		PQPrinter().SetCmeraDpi(m_tCameraParam.vDPI2);
		PQPrinter().SetPrinteDpi(m_tCameraParam.vDPI1);
		PQPrinter().SetOffset(m_tCameraParam.vDist); // 偏移距离 // 该值为相机dpi
		PQPrinter().SetPrintHeader({ (int)(vPixInMM[0] * m_dPrintInfos[0]),  (int)(vPixInMM[1] * m_dPrintInfos[1]) });	// 打印dpi

		auto iCaDpi = PQPrinter().GetCmeraDpi();
		auto iPrDpi = PQPrinter().GetPrinteDpi();
		array<double, 2> dPr2Camer = { iCaDpi[0] / iPrDpi[0], iCaDpi[1] / iPrDpi[1] }; // 相机原始采图 // 此时相机dpi尚未做对齐
		array<int, 2> vHeader = PQPrinter().GetPrintHeader();

		// 转相机dpi
		array<double, 2> vArea = { dPr2Camer[0] * vHeader[0], dPr2Camer[1] * vHeader[1] };
		double dFrameInLine = (double)(vArea[0] - m_tCameraParam.vDist[0]) / 30000.0;
		double dX2Y = iCaDpi[1] / iCaDpi[0];

		cout << "area: " << vArea[0] << ", " << vArea[1] << " | " << dFrameInLine << endl;

		// 视野y在测试过程中采取20cm
		array<int, 2> vSight = { (int)(dX2Y * vArea[0]) - m_tCameraParam.vDist[0], (int)(13056 * 2.0) }; // 相机实际需要提供的视野填充 = prt.w - 填充.w // 此时相机dpi需要对齐

		Mat tTiffMat;

		// ReadTif(CString2String(rTiff), cTIFF);
		//	ReadTif(CString2String(m_rTIFF), cTIFF);

		PQPrinter().SetSight(vSight);
		PQPrinter().Enable1bitDll(false);
		PQPrinter().SetPrinterType(CPrinterBase::ePT_FastPrint);
		PQPrinter().SetInitiator(this);

		// 输出测试过程
		PQPrinter().SetBaseDir(CString2String(L"G:\\11.15\\"));
		PQPrinter().SetTIFF(CString2String(FileUtil::getAppPath(L"TempTiff.tif")), tTiffMat.channels()); // m_rTIFF

		TIFF* tempTif = TIFFOpen(CString2String(FileUtil::getAppPath(L"TempTiff.tif")).c_str(), "rb");
		uint16_t uTiffBitSample = 0;
		TIFFGetField(tempTif, TIFFTAG_BITSPERSAMPLE, &uTiffBitSample);

		if (uTiffBitSample != 8) {
			// _cwprintf(L"%s : MatchAndCreatPrintMat tiff bit sample is not 8, read is %d.\r\n", __FUNCTIONW__, uTiffBitSample);
			MessageBox(TEXT("贴图通道位数不等于8！"), TEXT("采集"), MB_ICONERROR | MB_OK);
		}
		TIFFClose(tempTif);

		Mat tTemplateMat = imread(CString2String(FileUtil::getAppPath(L"Temp.png")), 0);

		PQPrinter().SetTemplate(tTemplateMat);
		PQPrinter().Reg(true);
	}

	m_tProgressPrint.SetPos(0);

	#ifdef USE_HALCON
		PQ_MatchEnd();
	#endif
}

/*******************************************************
* 功能：捕获图片按钮事件
* 参数：无
* 返回值：无
********************************************************/
void CVisionLocalizationDlg::OnBnClickedBtMainCaptureImage()
{
	if (!m_tCollectImgMat.empty()) {
		m_vCollectImgMat.push_back(m_tCollectImgMat.clone());

		_cwprintf(L"%s Capture background size = %d.\r\n", __FUNCTIONW__, m_vCollectImgMat.size());

		MessageBox(TEXT("采集图捕获成功！"), TEXT("捕获图像"), MB_OK | MB_ICONINFORMATION);
	}
	else {
		MessageBox(TEXT("采集图为空，捕获失败！"), TEXT("捕获图像"), MB_OK | MB_ICONERROR);
	}
}

/*******************************************************
* 功能：保存图片按钮事件
* 参数：无
* 返回值：无
********************************************************/
void CVisionLocalizationDlg::OnClickedSaveBackground()
{
	if (m_vCollectImgMat.size() > 0) {
		int iImageCount = m_vCollectImgMat.size();
		_cwprintf(L"%s save background size = %d.\r\n", __FUNCTIONW__, m_vCollectImgMat.size());

		StepDialog tStepDialog(iImageCount);

		tStepDialog.Create(IDD_STEP_DIALOG, this);
		tStepDialog.ShowWindow(SW_SHOW);
		tStepDialog.UpdateWindow();
		tStepDialog.setStepText(L"正在保存采集图，请稍候...");

		for (int i = 0; i < iImageCount; i++) {
			CString strFile = FileUtil::getAppPath(L"image");
			CTime t = CTime::GetCurrentTime();
			CString str = t.Format(L"\\%Y%m%d%H%M%S.png");

			strFile = strFile + str;
			imwrite(CString2String(strFile).c_str(), m_vCollectImgMat[i]);

			// _cwprintf(L"%s i=%d, value=%d, equal=%d.\r\n", __FUNCTIONW__, i, (iMatValue - 1), (i == (iMatValue - 1)));

			if (i == (iImageCount -2)) {
				tStepDialog.setStepText(L"正在保存最后一张图，请稍候...");
			}
			else {
				CString msg;
				msg.Format(TEXT("第%d张图保存完成，正在保存第%d张图，请稍候..."), (i+1), (i+2));

				tStepDialog.setStepText(msg);
			}
		}

		tStepDialog.DestroyWindow();
		MessageBox(TEXT("采集图保存成功！"), TEXT("保存图片"), MB_OK | MB_ICONINFORMATION);
	}
	else {
		MessageBox(TEXT("采集图为空，保存失败！"), TEXT("保存图片"), MB_OK | MB_ICONERROR);
	}
}

/*******************************************************
* 功能：截取图片
* 参数：无
* 返回值：无
********************************************************/
Mat CVisionLocalizationDlg::captureImage()
{
	Mat tCaptureImage;

	if (!m_tCollectImgMat.empty()) {
		CRect tRect;
		CRect tImViewRect = m_cImControl.GetImViewImageRect();
		double dRatio = (double)m_tCollectImgMat.cols / tImViewRect.Width();    //比例，长和宽是等比例放大

		::GetWindowRect(GetDlgItem(IDC_ST_VIDEO)->GetSafeHwnd(), tRect);

		CRect tDstRect(tRect.left, tRect.top, tRect.left + tImViewRect.Width(), tRect.top + tImViewRect.Height());

		CScreenShotDlg tDlg(this, tDstRect);

		if (tDlg.DoModal() == IDOK) {
			CRect tCaptureRect = tDlg.getCaptureRect();

			int iX = tCaptureRect.left * dRatio;
			int iY = tCaptureRect.top * dRatio;
			int iWidth = tCaptureRect.Width() * dRatio;
			int iHeight = tCaptureRect.Height() * dRatio;

			tCaptureImage = m_tCollectImgMat(Rect(iX, iY, iWidth, iHeight));
		}
	}

	return tCaptureImage;
}

/*******************************************************
* 功能：截取图片按钮事件
* 参数：无
* 返回值：无
********************************************************/
void CVisionLocalizationDlg::OnBnClickedBtMainCutImage()
{
	SetWindowText(_T("UV视觉打印软件--正在截取图像中"));

	Mat tCaptureMat = captureImage();

	if (!tCaptureMat.empty()) {
		CTime tTime = CTime::GetCurrentTime();
		CString tFileHead = FileUtil::getAppPath(L"image") + tTime.Format(L"\\%Y%m%d%H%M%S.png");
		std::string tSaveFile = CString2String(tFileHead);

		imwrite(tSaveFile, tCaptureMat);

		MessageBox(TEXT("截取成功！"), TEXT("截取图像"), MB_OK | MB_ICONINFORMATION);
	}
	else {
		MessageBox(TEXT("截取区域图像为空，截取失败！"), TEXT("截取图像"), MB_OK | MB_ICONERROR);
	}

	SetWindowText(_T("UV视觉打印软件"));
	InvalidateRect(NULL);
	//RedrawWindow(NULL, NULL, RDW_FRAME | RDW_INVALIDATE);
}

/*******************************************************
* 功能：新建模板按钮事件
* 参数：无
* 返回值：无
********************************************************/
// 定义新建模板按钮事件处理函数
void CVisionLocalizationDlg::OnBnClickedBtMainNewtemp()
{
    // 调用 captureImage 函数捕获图像
    Mat tCaptureMat = captureImage();

    // 检查捕获的图像是否为空
    if (!tCaptureMat.empty()) {
        // 获取配置文件的路径
        CString tCfgFile = FileUtil::getAppPath(UV_MATCH_CFG);

        // 创建一个文件对话框，用于选择产品文件
        CFileDialog tFileDialog(TRUE, NULL, IniUtil::readStringValue(tCfgFile, L"template", L"open_folder", NULL),
            OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, L"product file(*.tif)|*.tif||");

        // 显示文件对话框并检查用户是否点击了“确定”按钮
        if (IDOK == tFileDialog.DoModal()) {
            // 定义两个 Mat 对象，用于存储 ROI 图像和产品图像
            Mat tRoiMat, tProductMat;
            // 获取用户选择的产品文件的路径
            CString tProductPath = tFileDialog.GetPathName();

            // 尝试将产品文件读取到 Mat 对象中
            if (!TiffUtil::readTiffFileToMat(tProductMat, CString2String(tProductPath))) {
                // 如果读取失败，显示警告消息框
                MessageBox(L"读取产品图失败，请重新选择!", L"新建模版", MB_OK | MB_ICONWARNING);

                // 退出函数
                return;
            }

            // 注释掉的代码，原本用于获取 ROI 图像和保存路径
            /*tRoiMat = m_tCollectImgMat(m_tRoiRect);
            std::string tSavePath = CString2String(FileUtil::getPath(tProductPath));*/

            // 创建一个制作模板对话框实例，传入必要的参数
            MakeTemplateDialog tDlg(this, tCaptureMat, tProductMat, tProductPath, false);

            // 显示制作模板对话框并检查用户是否点击了“确定”按钮
            if (tDlg.DoModal() == IDOK) {
                // 创建一个模板管理对话框实例
                TemplateManagerDlg tTemplateManagerDlg(this);

                // 设置模板管理对话框中选中的列表项为最后一个模板
                tTemplateManagerDlg.setSelListItem(m_ptTemplateManager->getTemplateListSize() - 1);
                // 设置模板管理对话框中的采集图像矩阵
                tTemplateManagerDlg.setCollectImageMat(m_tCollectImgMat);
                // 显示模板管理对话框
                tTemplateManagerDlg.DoModal();

                // 刷新模板列表和相关数据
                fillData();
                // 触发模板列表选择改变事件，以显示预览图
                OnCbnSelchangeCbMainModel();
            }
        }
    }
    else {
        // 如果捕获的图像为空，显示错误消息框
        MessageBox(TEXT("选取图像为空，创建模板失败！"), TEXT("创建模板"), MB_OK | MB_ICONERROR);
    }
}

/*******************************************************
* 功能：参数设置按钮事件
* 参数：无
* 返回值：无
********************************************************/
/**
 * @brief 处理系统参数设置按钮的点击事件
 *
 * 此函数用于处理系统参数设置按钮的点击事件。当用户点击该按钮时，会创建一个参数设置对话框实例，
 * 并将当前对话框的句柄传递给该实例，然后以模态对话框的形式显示该对话框。
 *
 * @param 无
 * @return 无
 */
void CVisionLocalizationDlg::OnClickedSysParamBtn()
{
    // 创建一个参数设置对话框实例，传入当前对话框的指针作为父窗口
    ParamSetDialog tDlg(this);

    // 设置参数设置对话框的句柄为当前对话框的安全句柄
    tDlg.setLpHandle(GetSafeHwnd());

    // 以模态对话框的形式显示参数设置对话框
    tDlg.DoModal();
}

// 模板列表框变化事件
void CVisionLocalizationDlg::OnCbnSelchangeCbMainModel()
{
    // 变量数据到对话框
    UpdateData(TRUE);
    // 更新对话框数据到变量
// `UpdateData` 是 MFC（Microsoft Foundation Classes）中的一个成员函数，用于对话框数据的交换和验证。
// 它主要用于在对话框控件和对话框类的成员变量之间同步数据。
// 
// 具体来说，`UpdateData` 函数有两种调用方式：
// 1. `UpdateData(TRUE)`: 这个调用会将对话框控件中的数据复制到对话框类的成员变量中。
//    例如，如果对话框中有一个文本框，用户在文本框中输入了一些内容，调用 `UpdateData(TRUE)` 后，
//    这些内容会被复制到对应的成员变量中。
// 2. `UpdateData(FALSE)`: 这个调用会将对话框类的成员变量中的数据复制到对话框控件中。
//    例如，如果成员变量的值发生了改变，调用 `UpdateData(FALSE)` 后，对话框控件会显示更新后的值。
// 
// 在当前代码中，`UpdateData(TRUE);` 的作用是将对话框控件中的数据更新到对应的成员变量中，
// 以便后续代码可以使用这些最新的数据进行处理。

    // 清空产品名称
    m_tProductName = L"";//因为在这个函数中，我们需要根据用户选择的模板来更新界面上的显示信息。
    // 如果不清空产品名称，那么之前显示的信息可能会残留下来，影响用户的体验。

    // 检查模板管理器是否存在
    if (m_ptTemplateManager != nullptr) {
        // 获取当前选中的模板索引
        int iSelIndex = m_tCmbTemplateList.GetCurSel();
        // 获取当前选中的模板参数
        TemplateManager::PT_Template ptTemplate = m_ptTemplateManager->getTemplateParam(iSelIndex);

        // 检查模板参数是否有效
        if (ptTemplate != nullptr) {
            // 获取模板对应的产品文件路径
            m_tProductFile = String2Wstring(ptTemplate->tProductFile).c_str();
            // 获取产品文件名
            m_tProductName = FileUtil::getFileName(m_tProductFile, false);

            // 在图像控件中显示模板图片
            m_cImControl.SetPic(String2Wstring(ptTemplate->tTemplateFile).c_str());

            // 设置台面距离显示文本
            m_tPrintHeiStatic.Format(_T("台面距离： %d"), ptTemplate->iPrintHei);
            // 设置打印速度显示文本
            m_tPrintSpeedStatic.Format(_T("打印速度： %d"), ptTemplate->iPrintSpeed);
            // 设置灯源电压显示文本
            m_tLightVoltageStatic.Format(_T("灯源电压： %d"), ptTemplate->iLightVoltage);

            // 如果定义了USE_HALCON宏
            #ifdef USE_HALCON
                // 获取模板名称
                string tNewName = ptTemplate->tName;

                // 检查模板名称是否为空
                if (tNewName.length() > 0) {
                    // 初始化Halcon匹配参数
                    PQ_InitParam(tNewName.c_str());
                }
            #endif

            // 获取配置文件的路径
            CString tCfgFile = FileUtil::getAppPath(UV_MATCH_CFG);
            // 将当前选中的模板索引写入配置文件
            IniUtil::writeIntValue(tCfgFile, L"template", L"current_index", iSelIndex);
        }
    }

    // 更新变量数据到对话框
    UpdateData(FALSE);
}
