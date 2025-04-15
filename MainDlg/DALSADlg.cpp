// 包含预编译头文件
#include "pch.h"
// 包含DALSADlg类的头文件
#include "DALSADlg.h"
// 包含PQPrinter类的头文件
#include "PQPrinter.h"
// 包含PQMsg类的头文件
#include "PQMsg.h"
// 包含Transform命名空间的头文件
#include "Transform.h"

// 包含Helper类的头文件
#include "Helper.h"
// 包含标准输入输出流库
#include <iostream>
// 包含文件输入输出流库
#include <fstream>
// 包含OpenCV的特征2D模块
#include <opencv2/features2d.hpp>
//#include "FastMatch.h"

// 使用cv命名空间
using namespace cv;
// 使用std命名空间
using namespace std;
// 使用Transform命名空间
using namespace Transform;

// 实现CDALSADlg类的动态创建机制，基类为StyleDialog
//IMPLEMENT_DYNAMIC(CDALSADlg, CDialog)
IMPLEMENT_DYNAMIC(CDALSADlg, StyleDialog)

// 开始CDALSADlg类的消息映射，基类为StyleDialog
//BEGIN_MESSAGE_MAP(CDALSADlg, CDialog)
BEGIN_MESSAGE_MAP(CDALSADlg, StyleDialog)
	// 处理窗口销毁消息
	ON_WM_DESTROY()
	// 处理相机到达消息
	ON_MESSAGE(PQM_CAMERA_ARRIVAL, OnArrivalMsg)
	// 处理相机移除消息
	ON_MESSAGE(PQM_CAMERA_REMOVE, OnRemoveMsg)
	// 处理相机连接消息
	ON_MESSAGE(PQM_CAMERA_CONNECT, OnConnectMsg)
	// 处理相机断开连接消息
	ON_MESSAGE(PQM_CAMERA_DISCONNECT, OnDisconnectMsg)
	// 处理相机抓取消息
	ON_MESSAGE(PQM_CAMERA_GRAB, OnGrabMsg)
	// 处理相机冻结消息
	ON_MESSAGE(PQM_CAMERA_FREEZE, OnFreezeMsg)
	// 处理相机捕获消息
	ON_MESSAGE(PQM_CAMERA_CAPTURE, OnCaptureMsg)
	// 处理帧索引重置消息
	ON_MESSAGE(PQM_FRAME_RESET_IDX, OnResetFrameIdxMsg)
//	ON_BN_CLICKED(IDC_BT_MAIN_GRAB, OnClickedGrab)
//	ON_BN_CLICKED(IDC_BT_MAIN_FREEZE, OnClickedFreeze)
// 结束消息映射
END_MESSAGE_MAP()
// 构造函数，初始化CDALSADlg对象
CDALSADlg::CDALSADlg(CWnd* pParent) :
	//CDialog(CDALSADlg::IDD, pParent)
	StyleDialog(CDALSADlg::IDD, pParent)
{
	// 20240909
	// N_No_Name_Default_Default
	m_rConfig = L"D:\\Teledyne DALSA\\Sapera\\CamFiles\\User\\N_No_Name_Default_Default.ccf";

	setMainDialogFlag(true);
}

// 析构函数，销毁CDALSADlg对象
CDALSADlg::~CDALSADlg()
{

}

// 数据交换函数，用于对话框和控件之间的数据交换
void CDALSADlg::DoDataExchange(CDataExchange* pDX)
{
	//CDialog::DoDataExchange(pDX);
	StyleDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_CB_MAIN_BOARD, m_cbBoard);

	DDX_Control(pDX, IDC_CB_MAIN_CAMERA, m_cbCamera);

	//	DDX_Control(pDX, IDC_BT_MAIN_GRAB, m_btGrab);

	//	DDX_Control(pDX, IDC_BT_MAIN_FREEZE, m_btFreeze);
}

// 初始化对话框函数，在对话框显示前执行
BOOL CDALSADlg::OnInitDialog()
{
	//CDialog::OnInitDialog();
	StyleDialog::OnInitDialog();

	AllocConsole(); 
	
	freopen("CONOUT$", "w", stdout);	

	PQCamera().Reg(GetSafeHwnd());

	return TRUE;
}

// 处理相机到达消息的函数
LRESULT CDALSADlg::OnArrivalMsg(WPARAM wparam, LPARAM lparam)
{
	// 将lparam转换为map<string, vector<string>>指针
	map<string, vector<string>>* lpArrival = (map<string, vector<string>>*)lparam;

	// 遍历lpArrival中的每个元素
	for (auto& iBoard : *lpArrival)
	{
		// 在m_cbBoard中查找与当前板卡名称匹配的项
		int nBoard = m_cbBoard.FindStringExact(0, String2Wstring(iBoard.first).c_str());

		// 如果未找到匹配项
		if (nBoard == (-1))
		{
			// 获取m_cbBoard的当前项数
			int idx = m_cbBoard.GetCount();

			// 创建一个新的vector<string>对象，存储当前板卡的相机列表
			vector<string>* lpCamera = new vector<string>(iBoard.second);

			// 将当前板卡名称插入到m_cbBoard中
			m_cbBoard.InsertString(idx, String2Wstring(iBoard.first).c_str());

			// 将存储相机列表的指针与当前板卡项关联
			m_cbBoard.SetItemDataPtr(idx, lpCamera);

			// 继续下一个板卡
			continue;
		}
		// 获取当前板卡关联的相机列表指针
		vector<string>* lpCamera = (vector<string>*)m_cbBoard.GetItemDataPtr(nBoard);

		// 遍历当前板卡的相机列表
		for (auto& iCamera : iBoard.second)
		{
			// 如果相机列表中不包含当前相机
			if (find(lpCamera->begin(), lpCamera->end(), iCamera) == lpCamera->end()) lpCamera->push_back(iCamera);
		}
	} // 注释内容保持不变

	// 如果m_cbBoard中没有项，则返回0
	if (m_cbBoard.GetCount() <= 0) return 0;

	// 获取当前选中的板卡索引，确保不小于0
	int nSel = max(0, m_cbBoard.GetCurSel());

	// 设置m_cbBoard的当前选中项
	m_cbBoard.SetCurSel(nSel);

	// 调用OnBoardsSelChange函数
	OnBoardsSelChange();

	// 释放lpArrival指向的内存
	delete lpArrival;

	return 0;
}

// 处理板卡选择改变的函数
void CDALSADlg::OnBoardsSelChange()
{
    // 存储当前选中的相机名称
    CString rCamera;
    // 获取当前选中的板卡索引
    int nBoard = m_cbBoard.GetCurSel();
    // 获取当前选中的相机索引
    int nCamera = m_cbCamera.GetCurSel();
    // 如果有选中的相机，获取其名称
    nCamera != (-1) ? m_cbCamera.GetLBText(nCamera, rCamera) : []() {}();
    // 获取当前板卡关联的相机列表指针
    vector<string>* lpCamera = (vector<string>*)m_cbBoard.GetItemDataPtr(nBoard);
    // 清空相机下拉列表
    m_cbCamera.ResetContent();
    // 将当前板卡关联的相机列表添加到相机下拉列表中
    for (auto& iCamera : *lpCamera) m_cbCamera.InsertString(m_cbCamera.GetCount(), String2Wstring(iCamera).c_str());
    // 在相机下拉列表中查找之前选中的相机
    nCamera = m_cbCamera.FindStringExact(0, rCamera);
    // 确保相机索引不超过相机列表的最大索引
    nCamera = min(nCamera, m_cbCamera.GetCount() - 1);
    // 确保相机索引不小于0
    nCamera = max(0, nCamera);
    // 如果相机索引在相机列表范围内，设置为当前选中项
    nCamera < m_cbCamera.GetCount() ? m_cbCamera.SetCurSel(nCamera) : []()->int {return 0; }();
    // 如果相机索引在相机列表范围内，调用连接函数
    nCamera < m_cbCamera.GetCount() ? OnConnect() : [](){}();
}

// 处理相机连接的函数
void CDALSADlg::OnConnect()
{
    // 获取当前选中的板卡索引
    int nBoard = m_cbBoard.GetCurSel();
    // 获取当前选中的相机索引
    int nCamera = m_cbCamera.GetCurSel();
    // 如果板卡或相机未选中，直接返回
    if (nBoard == (-1) || nCamera == (-1)) return;
    // 存储当前选中的板卡名称
    CString rBoard;
    // 获取当前选中的板卡名称
    m_cbBoard.GetLBText(nBoard, rBoard);
    // 调用PQCamera的Connect方法进行连接
    PQCamera().Connect(Wstring2String(rBoard.GetBuffer()), nCamera, Wstring2String(m_rConfig.GetBuffer()), GetSafeHwnd());
}

// 处理相机连接消息的函数
LRESULT CDALSADlg::OnConnectMsg(WPARAM wparam, LPARAM lparam)
{
//    m_btGrab.EnableWindow(TRUE);
//    m_btFreeze.EnableWindow(FALSE);
    return 0;
}

// 处理点击抓取按钮的函数
void CDALSADlg::OnClickedGrab()
{
    // 重置帧索引
    m_nFrameIdx = 0;
    // 存储外部触发帧计数
    int nExtTriggerFrameCount = 0;
    // 获取外部触发帧计数
    ExtTriggerFrameCount(nExtTriggerFrameCount);
    // 调用PQCamera的Grab方法进行抓取
    PQCamera().Grab(nExtTriggerFrameCount, GetSafeHwnd());
}

// 处理点击冻结按钮的函数
void CDALSADlg::OnClickedFreeze()
{
    // 调用PQCamera的Freeze方法进行冻结
    PQCamera().Freeze(GetSafeHwnd());
}

// 处理相机抓取消息的函数
LRESULT CDALSADlg::OnGrabMsg(WPARAM wparam, LPARAM lparam)
{
//    m_btGrab.EnableWindow(FALSE);
//    m_btFreeze.EnableWindow(TRUE);
    return 0;
}

// 处理相机冻结消息的函数
LRESULT CDALSADlg::OnFreezeMsg(WPARAM wparam, LPARAM lparam)
{
//    m_btGrab.EnableWindow(TRUE);
//    m_btFreeze.EnableWindow(FALSE);
    return 0;
}

// 处理相机移除消息的函数
LRESULT CDALSADlg::OnRemoveMsg(WPARAM wparam, LPARAM lparam)
{
//    m_btGrab.EnableWindow(FALSE);
//    m_btFreeze.EnableWindow(FALSE);
    // 弹出消息框提示相机移除
    AfxMessageBox(L"�������");
    return 0;
}

// 处理相机断开连接消息的函数
LRESULT CDALSADlg::OnDisconnectMsg(WPARAM wparam, LPARAM lparam)
{
    return 0;
}

// 处理相机捕获消息的函数
LRESULT CDALSADlg::OnCaptureMsg(WPARAM wparam, LPARAM lparam)
{
    // 增加帧索引
    ++m_nFrameIdx;
    // 发送消息进行帧匹配
    PostMessage(PQM_MATCH_FRAME, (WPARAM)m_nFrameIdx, lparam);
    return 0;
}

// 处理帧索引重置消息的函数
LRESULT CDALSADlg::OnResetFrameIdxMsg(WPARAM wparam, LPARAM lparam)
{
    // 重置帧索引
    m_nFrameIdx = 0;
    return 0;
}

// 处理窗口销毁的函数
void CDALSADlg::OnDestroy()
{
    // 调用基类的OnDestroy方法
    CDialog::OnDestroy();
    // 取消注册相机消息
    PQCamera().Unreg(GetSafeHwnd());
    // 取消注册打印机消息
    PQPrinter().Unreg();
}
