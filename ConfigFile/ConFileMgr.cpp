#include "pch.h"
#include "ConfigFile.h"
//#include "Log\Log.h"
//
//CConfig::CConfig()
//{
//	BindEventFunc(ReadSkin, &CConfig::OnReadSkin);
//
//	BindEventFunc(ReadInformation, &CConfig::OnReadInformation);
//	BindEventFunc(WriteInformation, &CConfig::OnWriteInformation);
//
//	BindEventFunc(ReadVCFTool, &CConfig::OnReadVCFTool);
//	BindEventFunc(WriteVCFTool, &CConfig::OnWriteVCFTool);
//
//	BindEventFunc(ReadVPUParam, &CConfig::OnReadVPUParam);
//	BindEventFunc(WriteVPUParam, &CConfig::OnWriteVPUParam);
//
//	BindEventFunc(ReadTemperatureCalibrationCode, &CConfig::OnReadTemperatureCalibrationCode);
//	BindEventFunc(WriteTemperatureCalibrationCode, &CConfig::OnWriteTemperatureCalibrationCode);
//
//	BindEventFunc(ReadOperationLog, &CConfig::OnReadOperationLog);
//	BindEventFunc(WriteOperationLog, &CConfig::OnWriteOperationLog);
//	
//	BindEventFunc(ReadCustomizeStimulateParam, &CConfig::OnReadCustomizeStimulateParam);
//	BindEventFunc(WriteCustomizeStimulateParam, &CConfig::OnWriteCustomizeStimulateParam);
//
//	BindEventFunc(ReadVpuOperationLog, &CConfig::OnReadVpuOperationLog);
//	BindEventFunc(WriteVpuOperationLog, &CConfig::OnWriteVpuOperationLog);
//}
//
//CConfig::~CConfig()
//{
//
//}
//
//void CConfig::Regist()
//{
//	CModuleBase::Regist(0x11, 1000);
//}
//
//void CConfig::Unregist()
//{
//	CModuleBase::Unregist(std::bind(&CConfig::ResetEvent, this));
//}
//
//void CConfig::BindEventFunc(enumConfigParamType eType, fRWFile func)
//{
//	m_vFunc.insert(std::pair<enumConfigParamType, std::function<bool(lpConfigParam, FILE *&)>>(eType, std::bind(func, this, placeholders::_1, placeholders::_2)));
//}
//
//void CConfig::WaitEvent()
//{
//	m_cConfig.wait();
//}
//
//void CConfig::ResetEvent()
//{
//	m_cConfig.signal();
//}
//
//void CConfig::AddEvent(lpConfigParam lpParam)
//{
//	{
//		CSWLock lock(&m_lConfig);
//		m_vConfigParam.push_back(lpParam);
//	}
//
//	ResetEvent(); // 这里触发频率 ≈ 单次，因此这里没有加片段锁
//}
//
//bool CConfig::EventEmpty()
//{
//	bool bEmpty = false;
//
//	{
//		CSWLock lock(&m_lConfig);
//		bEmpty = m_vConfigParam.empty();
//	}
//
//	return bEmpty;
//}
//
//void CConfig::OnEvent()
//{
//	CSWLock lock(&m_lConfig);
//	while (!m_vConfigParam.empty()) // 这里换成数组效率更好一些，保持代码一致性，这里沿用了std容器
//	{
//		std::vector<lpConfigParam>::iterator iter = m_vConfigParam.begin();
//		((*iter)->eType & 0x80) ? OnReadEvent(*iter) : OnWriteEvent(*iter);
//		iter = m_vConfigParam.erase(iter);
//	}
//}
//
//void CConfig::Run()
//{
//	if (EventEmpty())
//	{
//		WaitEvent();
//	}
//	else
//	{
//		OnEvent();
//	}
//}
//
//void CConfig::Ontimer(UINT uTimerID)
//{
//	OnEvent();
//}
//
//void CConfig::OnReadEvent(lpConfigParam lpParam)
//{
//	FILE *lpFile = NULL; bool bRes = false;
//
//	if (CheckFileHeader(lpParam))
//	{
//		_wfopen_s(&lpFile, lpParam->strPath.c_str(), L"rb");
//
//		if (lpFile)
//		{
//			FseekFileHeader(lpFile);
//
//			bRes = m_vFunc[lpParam->eType](lpParam, lpFile);
//
//			fclose(lpFile);
//		}
//	}
//	lpParam->CallbackFunc(bRes, lpParam);
//}
//
//void CConfig::OnWriteEvent(lpConfigParam lpParam)
//{
//	FILE *lpFile = NULL; bool bRes = false;
//
//	_wfopen_s(&lpFile, lpParam->strPath.c_str(), L"wb");
//
//	if (lpFile)
//	{
//		bRes = m_vFunc[lpParam->eType](lpParam, lpFile);
//
//		fclose(lpFile);
//
//		bRes = bRes ? FillHeader(lpParam) : bRes;
//	}
//
//	lpParam->CallbackFunc(bRes, lpParam);
//}
//
//void CConfig::FseekFileHeader(FILE *&lpFile)
//{
//	fseek(lpFile, sizeof(tagFileHeader), SEEK_SET);
//}
//
//long CConfig::FileCount(const std::wstring &strPath)
//{
//	FILE *lpFile = nullptr; long nSize = -1;
//	_wfopen_s(&lpFile, strPath.c_str(), L"rb");
//	if (lpFile)
//	{
//		fseek(lpFile, 0, SEEK_END);
//		nSize = ftell(lpFile);
//		fseek(lpFile, 0, SEEK_SET);
//		fclose(lpFile);
//	}
//	return nSize;
//}
//
//long CConfig::ReadFile(const std::wstring &strPath, wchar_t *&lpBuffer)
//{
//	long nSize = FileCount(strPath);
//	if (0 < nSize)
//	{
//		FILE *lpFile = nullptr;
//		_wfopen_s(&lpFile, strPath.c_str(), L"rb");
//		if (lpFile)
//		{
//			double dSize = nSize / (double)sizeof(wchar_t);
//			long nBufferSize = ceil(dSize);
//			lpBuffer = new wchar_t[nBufferSize];
//			memset(lpBuffer, 0, nBufferSize * sizeof(wchar_t));
//			bool bResult = (fread(lpBuffer, nSize * sizeof(unsigned char), 1, lpFile) == 1);
//			fclose(lpFile); lpFile = nullptr;
//			return bResult ? nSize : (-1);
//		}
//	}
//	return nSize;
//}
//
//bool CConfig::FillHeader(lpConfigParam lpParam)
//{
//	wchar_t *lpbuffer = nullptr; bool bResult = false;
//	long nSize = ReadFile(lpParam->strPath, lpbuffer);// 这里兼容之前代码，不改动原有代码，仅添加一个文件头，因此这里采取先读取文件内容，计算文件头后重新写入
//	if (nSize > 0)
//	{
//		tagFileHeader tHeader;
//		FILE *lpFile = nullptr;
//		_wfopen_s(&lpFile, lpParam->strPath.c_str(), L"wb");
//		if (lpFile)
//		{
//			tHeader.uVersion = lpParam->uVersion;
//			tHeader.uCheckType = (unsigned char)(lpParam->eType);
//			std::wstring strMd5 = m_cMd5.GetMd5((unsigned char *)lpbuffer, 0, nSize);
//			memcpy(tHeader.szMD5, strMd5.c_str(), sizeof(wchar_t) * strMd5.size());
//			bResult = (fwrite(&tHeader, sizeof(tagFileHeader), 1, lpFile));
//			bResult = bResult ? (fwrite(lpbuffer, sizeof(unsigned char) * nSize, 1, lpFile)) : bResult;
//			fclose(lpFile);
//		}
//	}
//	if (lpbuffer)
//	{
//		delete[] lpbuffer;
//	}
//	return bResult;
//}
//
//bool CConfig::CheckFileHeader(lpConfigParam lpParam)
//{
//	bool bResult = false;
//	wchar_t *lpbuffer = nullptr; 
//	const long nHeaderSize = sizeof(tagFileHeader);
//	long nSize = ReadFile(lpParam->strPath, lpbuffer);
//	if (nSize > nHeaderSize)
//	{
//		tagFileHeader tHeader;
//		memcpy(&tHeader, lpbuffer, sizeof(tagFileHeader));
//		std::wstring strMd5 = m_cMd5.GetMd5((unsigned char *)lpbuffer, sizeof(tagFileHeader), nSize - sizeof(tagFileHeader));
//		bResult = ((strMd5 == std::wstring(tHeader.szMD5, 32)) && (tHeader.uVersion == lpParam->uVersion) &&(tHeader.uCheckType == (((unsigned char)lpParam->eType) & 0x7f))); // 根据定义取出类型校验特征
//	}
//	if (lpbuffer)
//	{
//		delete[] lpbuffer;
//	}
//	return bResult;
//}
//
//bool CConfig::OnReadSkin(lpConfigParam lpParam, FILE *&lpFile)
//{
//	lpReadSkinParam lpSkin = (lpReadSkinParam)lpParam;
//
//	bool bRes = (fread(&lpSkin->rtPos, sizeof(CRect), 1, lpFile) == 1);
//
//	bRes = bRes ? (fread(&lpSkin->nSize, sizeof(int), 1, lpFile) == 1) : bRes;
//
//	if (bRes)
//	{
//		lpSkin->lpControl = new tagReadSkinParam::tagControlSkin[lpSkin->nSize];
//		bRes = (fread(lpSkin->lpControl, sizeof(tagReadSkinParam::tagControlSkin) * lpSkin->nSize, 1, lpFile) == 1);
//	}
//
//	return bRes;
//}
//
//bool CConfig::OnReadInformation(lpConfigParam lpParam, FILE *&lpFile)
//{
//	lpUserInfoParam lpInfo = (lpUserInfoParam)lpParam;
//
//	return (fread(&lpInfo->info, sizeof(lpInfo->info), 1, lpFile) == 1);
//}
//
//bool CConfig::OnWriteInformation(lpConfigParam lpParam, FILE *&lpFile)
//{
//	lpUserInfoParam lpInfo = (lpUserInfoParam)lpParam;
//
//	return (fwrite(&lpInfo->info, sizeof(lpInfo->info), 1, lpFile) == 1);
//}
//
//bool CConfig::OnWriteVCFTool(lpConfigParam lpParam, FILE *&lpFile)
//{
//	lpVCFToolParam lpTool = (lpVCFToolParam)lpParam;
//
//	const tagVCFParam &tVcfParam = lpTool->tVcfParam;
//
//	bool bRes = (fwrite(&tVcfParam.uStimulateMode, sizeof(unsigned char), 1, lpFile) == 1);
//
//	bRes = bRes ? (fwrite(&tVcfParam.uArmMode, sizeof(unsigned char), 1, lpFile) == 1) : bRes;
//
//	bRes = bRes ? (fwrite(&tVcfParam.uDegree, sizeof(unsigned short), 1, lpFile) == 1) : bRes;
//
//	bRes = bRes ? (fwrite(&tVcfParam.config, sizeof(tagGlobalConfig), 1, lpFile) == 1) : bRes;
//
//	bRes = bRes ? (fwrite(&tVcfParam.imParam, sizeof(tagImageConfigParam), 1, lpFile) == 1) : bRes;
//
//	bRes = bRes ? (fwrite(tVcfParam.tEParam, sizeof(tagVCFParam::tagEParam) * ELECTRODE_ARRAY, 1, lpFile) == 1) : bRes;
//
//	return bRes;
//}
//
//bool CConfig::OnReadVCFTool(lpConfigParam lpParam, FILE *&lpFile)
//{
//	lpVCFToolParam lpTool = (lpVCFToolParam)lpParam;
//
//	tagVCFParam &tVcfParam = lpTool->tVcfParam;
//
//	bool bRes = (fread(&tVcfParam.uStimulateMode, sizeof(unsigned char), 1, lpFile) == 1);
//
//	bRes = bRes ? (fread(&tVcfParam.uArmMode, sizeof(unsigned char), 1, lpFile) == 1) : bRes;
//
//	bRes = bRes ? (fread(&tVcfParam.uDegree, sizeof(unsigned short), 1, lpFile) == 1) : bRes;
//
//	bRes = bRes ? (fread(&tVcfParam.config, sizeof(tagGlobalConfig), 1, lpFile) == 1) : bRes;
//
//	bRes = bRes ? (fread(&tVcfParam.imParam, sizeof(tagImageConfigParam), 1, lpFile) == 1) : bRes;
//
//	bRes = bRes ? (fread(tVcfParam.tEParam, sizeof(tagVCFParam::tagEParam) * ELECTRODE_ARRAY, 1, lpFile) == 1) : bRes;
//
//	return bRes;
//}
//
//bool CConfig::OnWriteVPUParam(lpConfigParam lpParam, FILE *&lpFile)
//{
//	lpCloneVPUParam lpClone = (lpCloneVPUParam)lpParam;
//
//	bool bRes = (fwrite(&lpClone->uSize, sizeof(unsigned short), 1, lpFile) == 1);
//
//	bRes = bRes ? (fwrite(lpClone->lpBuffer, sizeof(tagCloneVPUParam::tagCommandParam) * lpClone->uSize, 1, lpFile) == 1) : bRes;
//
//	return bRes;
//}
//
//bool CConfig::OnReadVPUParam(lpConfigParam lpParam, FILE *&lpFile)
//{
//	lpCloneVPUParam lpClone = (lpCloneVPUParam)lpParam;
//
//	bool bRes = (fread(&lpClone->uSize, sizeof(unsigned short), 1, lpFile) == 1);
//
//	lpClone->lpBuffer = new tagCloneVPUParam::tagCommandParam[lpClone->uSize];
//
//	return (bRes = bRes ? (fread(lpClone->lpBuffer, sizeof(tagCloneVPUParam::tagCommandParam) * lpClone->uSize, 1, lpFile) == 1) : bRes);
//}
//
//bool CConfig::OnReadTemperatureCalibrationCode(lpConfigParam lpParam, FILE *&lpFile)
//{
//	lpTemperatureCalibrationCodeParam lpCalibrationCode = (lpTemperatureCalibrationCodeParam)lpParam;
//
//	return (fread(&(lpCalibrationCode->uCode), sizeof(unsigned char), 1, lpFile) == 1);
//}
//
//bool CConfig::OnWriteTemperatureCalibrationCode(lpConfigParam lpParam, FILE *&lpFile)
//{
//	lpTemperatureCalibrationCodeParam lpCalibrationCode = (lpTemperatureCalibrationCodeParam)lpParam;
//
//	return (fwrite(&lpCalibrationCode->uCode, sizeof(unsigned char), 1, lpFile) == 1);
//}
//
//bool CConfig::OnReadOperationLog(lpConfigParam lpParam, FILE *&lpFile)
//{
//	lpOperationLogParam lpTool = (lpOperationLogParam)lpParam;
//	fseek(lpFile, 0, SEEK_END);
//	long nSize = ftell(lpFile) - sizeof(tagFileHeader);
//	double dSize = nSize / (double)sizeof(char);
//	long nBufferSize = ceil(dSize) + 1;
//	FseekFileHeader(lpFile);
//	char *lpBuffer = new char[nBufferSize];
//	memset(lpBuffer, 0, nBufferSize * sizeof(char));
//	bool bResult = (fread(lpBuffer, nSize * sizeof(unsigned char), 1, lpFile) == 1);
//	lpTool->szInfo = lpBuffer;
//	return bResult;
//}
//
//bool CConfig::OnWriteOperationLog(lpConfigParam lpParam, FILE *&lpFile)
//{
//	lpOperationLogParam lpLog = (lpOperationLogParam)lpParam;
//
//	return (fwrite(lpLog->szInfo, sizeof(char) * lpLog->uSize, 1, lpFile) == 1);
//}
//
//bool CConfig::OnReadVpuOperationLog(lpConfigParam lpParam, FILE *&lpFile)
//{
//	return OnReadOperationLog(lpParam, lpFile);
//}
//
//bool CConfig::OnWriteVpuOperationLog(lpConfigParam lpParam, FILE *&lpFile)
//{
//	return OnWriteOperationLog(lpParam, lpFile);
//}
//
//bool CConfig::OnReadCustomizeStimulateParam(lpConfigParam lpParam, FILE *&lpFile)
//{
//	lpCustomizeStimulateParam lpStimulateParam = (lpCustomizeStimulateParam)lpParam;
//
//	return (fread(lpStimulateParam->tInfo, sizeof(tagCustomizeStimulateParam::tagCustomizeStimulateInfo) * ELECTRODE_ARRAY, 1, lpFile) == 1);
//}
//
//bool CConfig::OnWriteCustomizeStimulateParam(lpConfigParam lpParam, FILE *&lpFile)
//{
//	lpCustomizeStimulateParam lpStimulateParam = (lpCustomizeStimulateParam)lpParam;
//
//	return (fwrite(lpStimulateParam->tInfo, sizeof(tagCustomizeStimulateParam::tagCustomizeStimulateInfo) * ELECTRODE_ARRAY, 1, lpFile) == 1);
//}