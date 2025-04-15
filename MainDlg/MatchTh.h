#pragma once  // 防止头文件重复包含

// 包含必要的头文件
#include "Position.h"       // 位置计算相关
#include "SRWLock.h"        // 同步读写锁
#include "Condition.h"      // 条件变量
#include <tuple>            // 元组支持
#include <vector>           // 向量容器
using namespace cv;         // OpenCV命名空间
using namespace std;        // 标准命名空间

// 模板匹配线程类
class CMatchTh
{
protected:
    // 枚举定义
    enum { eMI_FrameId = 0, eMI_Fleece, eMI_Timer, eMI_Param, eMI_ImPtr, eMI_Match }; // 匹配索引枚举
    enum { eSI_Score = 0, eSI_Pos, eSI_ImPtr, eSI_PosEx, eSI_ImPtrEx }; // 单目标索引枚举

protected:
    // 类型定义
    typedef tuple<int, bool, CPrinterBase::vTimer, array<int64, 3>, ImPtr::CImPtr, CPosition::vMatcher> Res; // 结果类型
    typedef tuple<int, bool, CPrinterBase::vTimer, array<int64, 3>> UserParam; // 用户参数类型
    typedef tuple<double, Rect, ImPtr::CImPtr, Rect, ImPtr::CImPtr> SingleTarget; // 单目标匹配结果
    typedef vector<Res> In;  // 输入队列类型
    typedef vector<tuple<UserParam, vector<SingleTarget>>> Out; // 输出队列类型

protected:
    CMatchTh(double dScale, CWnd* lpInitiator); // 构造函数
    virtual ~CMatchTh(); // 析构函数

protected:
    void Reg();    // 注册线程
    void Unreg();  // 注销线程

protected:
    void OnConvert(Res& vMatch); // 结果转换处理

protected:
    // 队列操作方法
    void Push(Res& vMatch); // 压入输入队列
    bool Pop(Res& vMatch);  // 弹出输入队列
    void Push(tuple<UserParam, vector<SingleTarget>>& vMatch); // 压入输出队列
    bool Pop(tuple<UserParam, vector<SingleTarget>>& vMatch);  // 弹出输出队列

private:
    void OnConvert(Res& vMatch, tuple<UserParam, vector<SingleTarget>>& vOut); // 内部转换实现

private:
    // 线程函数
    static unsigned int __stdcall OnCovExitThread(void* lpUesrParam); // 退出线程
    static unsigned int __stdcall OnConvertThread(void* lpUesrParam); // 转换线程

private:
    void SetTermination(bool bTermination); // 设置终止标志
    bool IsTermination(); // 检查终止标志

private:
    // 成员变量
    In m_vIn;               // 输入队列
    Out m_vOut;             // 输出队列
    CWnd* m_lpInitiator;    // 窗口句柄
    double m_dScale;        // 缩放比例

private:
    // 同步控制
    bool m_bTermination;    // 终止标志
    CSRWLockBase m_lLock;    // 读写锁
    CCondition m_cConvert;   // 转换条件变量
    CCondition m_cTermination; // 终止条件变量
};