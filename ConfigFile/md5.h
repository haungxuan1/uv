// 防止头文件被重复包含
#pragma once
// 包含标准字符串库
#include <string>
 
// 使用标准命名空间
using namespace std;

/**
 * @brief 用于计算MD5哈希值的类
 */
class CMD5 
{
public:
    // 构造函数
    CMD5();

public:
    /**
     * @brief 计算指定缓冲区的MD5哈希值
     * @param lpBuffer 输入缓冲区的指针
     * @param nOffset 缓冲区的起始偏移量
     * @param nSize 缓冲区的大小
     * @return 计算得到的MD5哈希值的宽字符串表示
     */
    wstring GetMd5(unsigned char *lpBuffer, long nOffset, long nSize);
    /**
     * @brief 计算指定文件的MD5哈希值
     * @param strFilePath 文件的路径
     * @param nOffset 文件的起始偏移量
     * @return 计算得到的MD5哈希值的宽字符串表示
     */
    wstring GetMd5(wstring strFilePath, long nOffset);

private:
    // 重置MD5计算的状态
    void reset();
    // 将MD5摘要转换为字符串
    string toString();
    // 更新MD5计算的输入数据
    void update(const void* input, size_t length);
    // 获取MD5摘要
    const unsigned char* digest();
    // 更新MD5计算的输入数据（处理无符号字符数组）
    void update(const unsigned char* input, size_t length);
    // 完成MD5计算
    void final();
    // 对64字节的数据块进行MD5转换
    void transform(const unsigned char block[64]);
    // 将无符号整数数组编码为无符号字符数组
    void encode(const unsigned int* input, unsigned char* output, size_t length);
    // 将无符号字符数组解码为无符号整数数组
    void decode(const unsigned char* input, unsigned int* output, size_t length);
    // 将无符号字符数组转换为十六进制字符串
    string bytesToHexString(const unsigned char* input, size_t length);
 
private:
    // MD5状态（ABCD）
    unsigned int _state[4]; 
    // 输入数据的位数，模2^64（低位在前）
    unsigned int _count[2]; 
    // 输入缓冲区
    unsigned char _buffer[64]; 
    // 消息摘要
    unsigned char _digest[16]; 
    // 标记MD5计算是否完成
    bool _finished;   
    // 用于填充计算的常量数组
    static const unsigned char PADDING[64]; 
    // 十六进制字符表
    static const char HEX[16];
    // 缓冲区大小
    enum { BUFFER_SIZE = 1024 };
};
