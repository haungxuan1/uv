#pragma once
#include <functional>
#include "FlHeader.h"

using namespace std;
// 将文件编辑封装于此
namespace Filer
{	
	// 功能注册 - 通用接口
	// 创建线程
	void Register();

	// 功能注销 - 通用接口
	// 终止线程
	void Unregister();

	// 注册失败
	void SetRegErrorCallback(function<void()> fRegError);

	// 注册成功
	void SetRegSucceedCallback(function<void()> fRegSucceed);

	// 注销失败
	void SetUnregErrorCallback(function<void()> fUnregError);

	// 注销成功
	void SetUnregSucceedCallback(function<void()> fUnregSucceed);

	// 编辑文件
	void Edit(lpFiler lpParam);

	// 增加内存与结构体互转api
	void Memory2Struct(unsigned char uOperatorType, function<void(lpFiler, unsigned char*&, int&)> fuOperatorFunc);
}
