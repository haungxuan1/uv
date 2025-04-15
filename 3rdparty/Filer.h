#pragma once
#include <functional>
#include "FlHeader.h"

using namespace std;
// ���ļ��༭��װ�ڴ�
namespace Filer
{	
	// ����ע�� - ͨ�ýӿ�
	// �����߳�
	void Register();

	// ����ע�� - ͨ�ýӿ�
	// ��ֹ�߳�
	void Unregister();

	// ע��ʧ��
	void SetRegErrorCallback(function<void()> fRegError);

	// ע��ɹ�
	void SetRegSucceedCallback(function<void()> fRegSucceed);

	// ע��ʧ��
	void SetUnregErrorCallback(function<void()> fUnregError);

	// ע���ɹ�
	void SetUnregSucceedCallback(function<void()> fUnregSucceed);

	// �༭�ļ�
	void Edit(lpFiler lpParam);

	// �����ڴ���ṹ�廥תapi
	void Memory2Struct(unsigned char uOperatorType, function<void(lpFiler, unsigned char*&, int&)> fuOperatorFunc);
}
