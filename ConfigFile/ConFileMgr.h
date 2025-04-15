// 防止头文件被重复包含
#pragma once
// 以下为被注释掉的头文件包含语句，可能后续需要使用
//#include "struct.h"
//#include "enum.h"
//#include "Module\ModuleBase.h"
//#include "LockModule\SRWLock.h"
//#include "LockModule\Condition.h"
//#include "md5.h"

/**
 * @brief CConfig类，继承自CModuleBase，用于管理配置文件的读写操作
 */
class CConfig: public CModuleBase
{
    // 定义一个函数指针类型，用于指向CConfig类的成员函数，该函数接受lpConfigParam和FILE*&作为参数，返回bool类型
    typedef bool(CConfig::*fRWFile)(lpConfigParam, FILE *&);
public:
    // 构造函数
    CConfig();
    // 析构函数
    ~CConfig();

public:
    // 注册函数，用于注册相关操作
    virtual void Regist();
    // 反注册函数，用于取消注册相关操作
    virtual void Unregist();

public:
    // 添加事件函数，用于添加配置参数事件
    void AddEvent(lpConfigParam lpParam);

private:
    // 填充文件头信息
    bool FillHeader(lpConfigParam lpParam); 
    // 检查文件头信息
    bool CheckFileHeader(lpConfigParam lpParam); 
    // 统计指定路径下的文件数量
    long FileCount(const std::wstring &strPath);
    // 读取指定路径下的文件内容到缓冲区
    long ReadFile(const std::wstring &strPath, wchar_t *&lpBuffer);
    // 将文件指针移动到文件头位置
    void FseekFileHeader(FILE *&lpFile);

private:
    // 等待事件发生
    void WaitEvent();
    // 重置事件状态
    void ResetEvent();

private:
    // 检查事件队列是否为空
    bool EventEmpty();
    // 处理事件
    void OnEvent();
    // 处理读取事件
    void OnReadEvent(lpConfigParam lpParam);
    // 处理写入事件
    void OnWriteEvent(lpConfigParam lpParam);

private:
    // 绑定事件处理函数到指定的事件类型
    void BindEventFunc(enumConfigParamType eType, fRWFile func);
    // 读取皮肤配置信息
    bool OnReadSkin(lpConfigParam lpParam, FILE *&lpFile);
    // 读取信息配置信息
    bool OnReadInformation(lpConfigParam lpParam, FILE *&lpFile);
    // 写入信息配置信息
    bool OnWriteInformation(lpConfigParam lpParam, FILE *&lpFile);
    // 写入VCF工具配置信息
    bool OnWriteVCFTool(lpConfigParam lpParam, FILE *&lpFile);
    // 读取VCF工具配置信息
    bool OnReadVCFTool(lpConfigParam lpParam, FILE *&lpFile);
    // 写入VPU参数配置信息
    bool OnWriteVPUParam(lpConfigParam lpParam, FILE *&lpFile);
    // 读取VPU参数配置信息
    bool OnReadVPUParam(lpConfigParam lpParam, FILE *&lpFile);
    // 读取温度校准代码配置信息
    bool OnReadTemperatureCalibrationCode(lpConfigParam lpParam, FILE *&lpFile);
    // 写入温度校准代码配置信息
    bool OnWriteTemperatureCalibrationCode(lpConfigParam lpParam, FILE *&lpFile);
    // 读取操作日志配置信息
    bool OnReadOperationLog(lpConfigParam lpParam, FILE *&lpFile);
    // 写入操作日志配置信息
    bool OnWriteOperationLog(lpConfigParam lpParam, FILE *&lpFile);
    // 读取自定义刺激参数配置信息
    bool OnReadCustomizeStimulateParam(lpConfigParam lpParam, FILE *&lpFile);
    // 写入自定义刺激参数配置信息
    bool OnWriteCustomizeStimulateParam(lpConfigParam lpParam, FILE *&lpFile);
    // 读取VPU操作日志配置信息
    bool OnReadVpuOperationLog(lpConfigParam lpParam, FILE *&lpFile);
    // 写入VPU操作日志配置信息
    bool OnWriteVpuOperationLog(lpConfigParam lpParam, FILE *&lpFile);

private:
    // 运行函数，用于执行核心操作
    virtual void Run();
    // 定时器回调函数，处理定时器事件
    virtual void Ontimer(UINT uTimerID);

private:
    // MD5计算对象
    CMD5 m_cMd5;
    // 存储配置参数的向量
    std::vector<lpConfigParam> m_vConfigParam;
    // 配置锁对象
    CSRWLockBase m_lConfig;
    // 条件对象
    CCondition m_cConfig;
    // 存储事件类型和对应的处理函数的映射表
    std::map<enumConfigParamType, std::function<bool(lpConfigParam, FILE *&)>> m_vFunc;
};

// 定义一个宏，用于获取CConfig类的单例实例
#define Config_Core (CSingleton<CConfig>::Install() ? CSingleton<CConfig>::Install() :	CSingleton<CConfig>::Install(new CConfig()))
// 定义一个宏，用于卸载CConfig类的单例实例
#define Config_Exit (CSingleton<CConfig>::Uninstall())
