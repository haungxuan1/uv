#pragma once

// 定义一个名为 TemplateManager 的类，用于管理模板信息
class TemplateManager
{
public:
    // 定义一个结构体 T_Template，用于存储模板的各种信息
    typedef struct {
        // 模板文件路径
        std::string tTemplateFile;
        // 图像文件路径
        std::string tImageFile;
        // 产品文件路径
        std::string tProductFile;
        // 掩码文件路径
        std::string tMaskFile;
        // 模板名称
        std::string tName;
        // 模板匹配方法
        int iMethod;
        // 模板所在图层
        int iLayer;
        // 搜索类型
        int iSearchType;
        // 中心偏移 X 坐标
        int iCenterOffsetX;
        // 中心偏移 Y 坐标
        int iCenterOffsetY;
        // 对比度
        int iContrastRatio;
        // 匹配得分
        int iScore;
        // 打印速度
        int iPrintSpeed;
        // 打印高度
        int iPrintHei;
        // 灯光电压
        int iLightVoltage;
        // 最大重叠度
        int iMaxOverLap;
        // 模型特征
        int iModelFeature;
    }T_Template, *PT_Template;

    // 获取 TemplateManager 类的单例实例
    static TemplateManager* instance();

    // 销毁单例实例
    void destroy();

    // 从文件中加载模板信息
    bool loadTemplateFrmFile();
    // 将模板信息保存到文件中
    bool saveTemplateToFile();

    // 获取模板列表
    std::vector<T_Template>* getTemplateList();
    // 根据索引获取模板参数
    PT_Template getTemplateParam(int iIndex);

    // 检查指定名称的模板是否存在
    bool isExist(std::string tName);
    // 查找指定名称的模板在列表中的索引
    int findNameIndex(std::string tName);
    // 获取模板列表的大小
    int getTemplateListSize();

    // 添加一个模板到列表中
    bool addTemplate(PT_Template ptTemplate);
    // 根据索引删除一个模板
    bool delTemplate(int iIndex);

    // 检查指定名称的模型是否存在
    bool isModelExist(std::string tName, bool bShapeModel = false);
    // 删除指定名称的模型
    void deleteModel(std::string tName);

private:
    // 私有构造函数，确保只能通过 instance() 方法获取实例
    TemplateManager();
    // 私有析构函数
    ~TemplateManager();

    // 存储模板信息的向量
    std::vector<T_Template> m_tVecTemplate;
    // 静态成员变量，用于存储单例实例
    static TemplateManager* g_ptTemplateManager;
};
