// 确保头文件只被编译一次
#pragma once
// 包含CImView头文件
#include "CImView.h"
// 包含CImParam头文件
#include "CImParam.h"
// 包含ImLayer头文件
#include "ImLayer.h"

// 命名空间ImControl
namespace ImControl
{
    /**
     * @brief CImControl类，继承自ImCtrl::CImCtrl
     * 
     * 该类提供了对图像控件的管理功能，包括图层的创建、销毁、属性设置等。
     */
    class CImControl : public ImCtrl::CImCtrl
    {
        // 声明动态创建类的宏
        DECLARE_DYNAMIC(CImControl)

    public:
        /**
         * @brief 构造函数
         */
        CImControl();
        /**
         * @brief 析构函数
         */
        virtual ~CImControl();

    public:
        /**
         * @brief 设置图标
         * @param strIconDir 图标的目录路径
         */
        void SetIcon(std::wstring strIconDir);
        /**
         * @brief 设置用户参数
         * @param lpUserParam 用户参数指针
         */                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            
        void SetUserParam(void* lpUserParam) { m_lpUserParam = lpUserParam; }
        /**
         * @brief 获取用户参数
         * @return 用户参数指针
         */
        void* GetUserParam() { return m_lpUserParam; }

    public:
        /**
         * @brief 创建图层（默认）
         */
        void OnCreateLayer() { m_cLayers.OnCreateLayer(); }
        /**
         * @brief 创建指定索引的图层
         * @param nLayer 图层索引
         */
        void OnCreateLayer(int nLayer) { m_cLayers.OnCreateLayer(nLayer); }
        /**
         * @brief 销毁所有图层
         */
        void OnDestoryLayer() { m_cLayers.OnDestoryLayer(); }
        /**
         * @brief 销毁指定索引的图层
         * @param nIndex 图层索引
         */
        void OnDestoryLayer(int nIndex) { m_cLayers.OnDestoryLayer(nIndex); }
        /**
         * @brief 销毁指定索引列表的图层
         * @param vIndex 图层索引列表
         */
        void OnDestoryLayer(std::vector<int> vIndex) { m_cLayers.OnDestoryLayer(vIndex); }
        /**
         * @brief 获取图层数量
         * @return 图层数量
         */
        int GetLayerCount() { return m_cLayers.GetLayerCount(); }
        /**
         * @brief 获取当前聚焦的图层索引
         * @return 图层索引
         */
        int GetFocus() { return m_cLayers.GetFocus(); }
        /**
         * @brief 设置当前聚焦的图层索引
         * @param nIndex 图层索引
         */
        void SetFocus(int nIndex) { m_cLayers.SetFocus(nIndex); }
        /**
         * @brief 判断图层是否为空
         * @return 如果为空返回true，否则返回false
         */
        bool Empty() { return m_cLayers.Empty(); }
        /**
         * @brief 设置背景图层索引
         * @param nLayer 图层索引
         */
        void SetBackgroundLayer(int nLayer) { m_cLayers.SetBackgroundLayer(nLayer); }
        /**
         * @brief 获取背景图层索引
         * @return 背景图层索引
         */
        int GetBackgroundLayer() { return m_cLayers.GetBackgroundLayer(); }

    public:
        /**
         * @brief 设置指定图层的缩放比例
         * @param nLayer 图层索引
         * @param nScaling 缩放比例
         */
        void SetPrScaling(int nLayer, int nScaling) { m_cLayers.SetPrScaling(nLayer, nScaling); }
        /**
         * @brief 设置指定图层的图像
         * @param nLayer 图层索引
         * @param cImage 图像对象
         */
        void SetImage(int nLayer, ImPtr::CImPtr& cImage) { m_cLayers.SetImage(nLayer, cImage); }
        /**
         * @brief 设置指定图层的左上角坐标
         * @param nLayer 图层索引
         * @param vTL 左上角坐标数组
         */
        void SetTL(int nLayer, std::array<int, 2> vTL) { m_cLayers.SetTL(nLayer, vTL); }
        /**
         * @brief 设置指定图层的宽高
         * @param nLayer 图层索引
         * @param vWH 宽高数组
         */
        void SetWH(int nLayer, std::array<int, 2> vWH) { m_cLayers.SetWH(nLayer, vWH); }
        /**
         * @brief 设置指定图层的启用状态
         * @param nLayer 图层索引
         * @param bEnable 启用状态
         */
        void SetEnable(int nLayer, bool bEnable) { m_cLayers.SetEnable(nLayer, bEnable); }
        /**
         * @brief 设置指定图层的图像名称
         * @param nLayer 图层索引
         * @param rName 图像名称
         */
        void SetImName(int nLayer, CString rName) { m_cLayers.SetImName(nLayer, rName); }
        /**
         * @brief 设置指定图层的透明度
         * @param nLayer 图层索引
         * @param nOpacity 透明度
         */
        void SetOpacity(int nLayer, int nOpacity) { m_cLayers.SetOpacity(nLayer, nOpacity); }
        /**
         * @brief 设置指定图层的用户参数
         * @param nLayer 图层索引
         * @param lpUserParam 用户参数指针
         */
        void SetUserParam(int nLayer, void* lpUserParam) { m_cLayers.SetUserParam(nLayer, lpUserParam); }

    public:
        /**
         * @brief 获取指定图层的启用状态
         * @param nLayer 图层索引
         * @return 启用状态
         */
        bool GetEnable(int nLayer) { return m_cLayers.GetEnable(nLayer); }
        /**
         * @brief 获取指定图层的左上角坐标
         * @param nLayer 图层索引
         * @return 左上角坐标数组
         */
        std::array<int, 2> GetTL(int nLayer) { return m_cLayers.GetTL(nLayer); }
        /**
         * @brief 获取指定图层的宽高
         * @param nLayer 图层索引
         * @return 宽高数组
         */
        std::array<int, 2> GetWH(int nLayer) { return m_cLayers.GetWH(nLayer); }
        /**
         * @brief 获取指定图层的图像
         * @param nLayer 图层索引
         * @return 图像对象
         */
        ImPtr::CImPtr GetImage(int nLayer) { return m_cLayers.GetImage(nLayer); }
        /**
         * @brief 获取指定图层的用户参数
         * @param nLayer 图层索引
         * @return 用户参数指针
         */
        void* GetUserParam(int nLayer) { return m_cLayers.GetUserParam(nLayer); }

    private:
        /**
         * @brief 创建控件时的扩展处理函数
         * @param lpCreateStruct 创建结构指针
         */
        virtual void OnCreateEx(LPCREATESTRUCT& lpCreateStruct);
        /**
         * @brief 初始化控件（指定宽高）
         * @param cx 宽度
         * @param cy 高度
         */
        void OnInitCtrl(const int& cx, const int& cy);
        /**
         * @brief 初始化控件
         */
        void OnInitCtrl();

    private:
        // 图像视图对象
        ImView::CImView m_cView;
        // 图像参数对象
        ImParam::CImParam m_cImParam;
        // 图层管理对象
        ImLayer::CLayers m_cLayers;
        // 用户参数指针
        void* m_lpUserParam;

    private:
        /**
         * @brief 处理图层聚焦改变消息
         * @param wparam 消息参数
         * @param lparam 消息参数
         * @return 处理结果
         */
        afx_msg LRESULT OnLayerFocusChanged(WPARAM wparam, LPARAM lparam);
        /**
         * @brief 处理图层启用状态改变消息
         * @param wparam 消息参数
         * @param lparam 消息参数
         * @return 处理结果
         */
        afx_msg LRESULT OnLayerEnableChanged(WPARAM wparam, LPARAM lparam);
        /**
         * @brief 处理图层缩放比例改变消息
         * @param wparam 消息参数
         * @param lparam 消息参数
         * @return 处理结果
         */
        afx_msg LRESULT OnLayerScalingChanged(WPARAM wparam, LPARAM lparam);

    private:
        /**
         * @brief 处理透明度改变消息
         * @param wparam 消息参数
         * @param lparam 消息参数
         * @return 处理结果
         */
        afx_msg LRESULT OnOpacityChangedMsg(WPARAM wparam, LPARAM lparam);
        /**
         * @brief 处理缩放比例改变消息
         * @param wparam 消息参数
         * @param lparam 消息参数
         * @return 处理结果
         */
        afx_msg LRESULT OnScalingChangedMsg(WPARAM wparam, LPARAM lparam);
        /**
         * @brief 处理左上角坐标改变消息
         * @param wparam 消息参数
         * @param lparam 消息参数
         * @return 处理结果
         */
        afx_msg LRESULT OnTLChangedMsg(WPARAM wparam, LPARAM lparam);
        /**
         * @brief 处理宽高改变消息
         * @param wparam 消息参数
         * @param lparam 消息参数
         * @return 处理结果
         */
        afx_msg LRESULT OnWHChangedMsg(WPARAM wparam, LPARAM lparam);

    private:
        /**
         * @brief 处理视图拖动中消息
         * @param wparam 消息参数
         * @param lparam 消息参数
         * @return 处理结果
         */
        afx_msg LRESULT OnViewDraggingMsg(WPARAM wparam, LPARAM lparam);
        /**
         * @brief 处理视图拖动消息
         * @param wparam 消息参数
         * @param lparam 消息参数
         * @return 处理结果
         */
        afx_msg LRESULT OnViewDragMsg(WPARAM wparam, LPARAM lparam);
        /**
         * @brief 处理视图缩放消息
         * @param wparam 消息参数
         * @param lparam 消息参数
         * @return 处理结果
         */
        afx_msg LRESULT OnViewScalingMsg(WPARAM wparam, LPARAM lparam);

    private:
        /**
         * @brief 处理擦除背景消息
         * @param pDC 设备上下文指针
         * @return 处理结果
         */
        afx_msg BOOL OnEraseBkgnd(CDC* pDC);

        // 声明消息映射宏
        DECLARE_MESSAGE_MAP()
    };
}
