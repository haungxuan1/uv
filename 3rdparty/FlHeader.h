#pragma once
#include <string>
#include <functional>

// �ļ�ͷ
namespace Filer
{
	// ��������ļ�����
	typedef struct _tagFileHeader
	{
		unsigned short uDllVersion;	// �ļ��汾�ţ����ܻ���ڵļ�������
		unsigned char uFileType;	// ��Բ�ͬ���͵��ļ����岻ͬ���������Ժ�ʵ
		wchar_t szMD5[32];
		_tagFileHeader();
	}tagFileHeader, * lpFileHeader;

	enum 
	{
		eOT_ReadCamParam = 0x81,
		eOT_WriteCamParam = 0x1,	//	���У׼����
				//	ģ��
				//	��ͼ
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
