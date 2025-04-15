#pragma once
#include <string>
#include <functional>

// 文件头
namespace Filer
{
	// 用于软件文件体检测
	typedef struct _tagFileHeader
	{
		unsigned short uDllVersion;	// 文件版本号，可能会存在的兼容问题
		unsigned char uFileType;	// 针对不同类型的文件定义不同的类型用以核实
		wchar_t szMD5[32];
		_tagFileHeader();
	}tagFileHeader, * lpFileHeader;

	enum 
	{
		eOT_ReadCamParam = 0x81,
		eOT_WriteCamParam = 0x1,	//	相机校准参数
				//	模板
				//	贴图
				//  
	}; // OperatorType

	typedef struct _tagFiler
	{
		typedef std::function<void(bool, _tagFiler*)> CallbackFunc;
		unsigned short uVersion;
		unsigned char uOperatorType;
		std::wstring strPath;
		CallbackFunc fCallback;
		_tagFiler();
		_tagFiler(unsigned char _uOperatorType, CallbackFunc _fCallback);
	}tagFiler, * lpFiler;
}
