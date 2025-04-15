#include "pch.h"
#include "def.h"
#include "TemplateManager.h"
#include "../cJSON.h"
#include "../utils/FileUtil.h"
#include "../utils/JsonUtil.h"
#include "../Local Setting/Transform.h"

using namespace Transform;

TemplateManager* TemplateManager::g_ptTemplateManager = nullptr;

TemplateManager::TemplateManager()
{
	m_tVecTemplate.clear();
}

TemplateManager::~TemplateManager()
{
	m_tVecTemplate.clear();
}

TemplateManager* TemplateManager::instance()
{
	if (nullptr == g_ptTemplateManager) {
		g_ptTemplateManager = new TemplateManager();

		if (g_ptTemplateManager != nullptr) {
			g_ptTemplateManager->loadTemplateFrmFile();
		}
	}

	return g_ptTemplateManager;
}

void TemplateManager::destroy()
{
	if (g_ptTemplateManager != nullptr) {
		delete g_ptTemplateManager;

		g_ptTemplateManager = nullptr;
	}
}

/***********************************************************************
 * 功能: 从文件读取模板列表
 * 参数:
 *     无
 * 返回值：
 *	   true:  成功
 *     false: 失败
************************************************************************/
bool TemplateManager::loadTemplateFrmFile()
{
	bool bRet = false;
	CFile tFile;
	CString tTemplateFile = FileUtil::getAppPath(TEMPLATE_FILE);

	m_tVecTemplate.clear();

	if (tFile.Open(tTemplateFile, CFile::modeRead | CFile::typeText)) {
		DWORD iFileSize = tFile.GetLength();

		if (iFileSize > 0) {
			char* pcBuf = new char[iFileSize + 10];

			if (pcBuf != nullptr) {
				UINT uLen = tFile.Read(pcBuf, iFileSize);

				if (uLen > 0) {
					cJSON *ptRootJson = cJSON_Parse(pcBuf);

					if (ptRootJson != nullptr) {
						cJSON* ptTmpJson;
						cJSON* ptItemJson;

						for (int i = 0; i < cJSON_GetArraySize(ptRootJson); i++) {
							ptItemJson = cJSON_GetArrayItem(ptRootJson, i);
							if (ptItemJson != nullptr) {
								T_Template tTemplate;

								tTemplate.tTemplateFile = JsonUtil::getStringValue(ptItemJson,"template_file", "");
								tTemplate.tImageFile    = JsonUtil::getStringValue(ptItemJson, "image_file", "");
								tTemplate.tProductFile  = JsonUtil::getStringValue(ptItemJson, "product_file", "");
								tTemplate.tMaskFile     = JsonUtil::getStringValue(ptItemJson, "mask_file", "");
								tTemplate.tName         = JsonUtil::getStringValue(ptItemJson, "name", "");

								tTemplate.iMethod        = JsonUtil::getIntValue(ptItemJson, "method", 0);
								tTemplate.iLayer         = JsonUtil::getIntValue(ptItemJson, "layer", 2);
								tTemplate.iSearchType    = JsonUtil::getIntValue(ptItemJson, "search_type", 75);
								tTemplate.iCenterOffsetX = JsonUtil::getIntValue(ptItemJson, "center_offset_x", 0);
								tTemplate.iCenterOffsetY = JsonUtil::getIntValue(ptItemJson, "center_offset_y", 0);

								tTemplate.iScore         = JsonUtil::getIntValue(ptItemJson, "score", 60);
								tTemplate.iMaxOverLap    = JsonUtil::getIntValue(ptItemJson, "max_over_lap", 50);
								tTemplate.iContrastRatio = JsonUtil::getIntValue(ptItemJson, "contrast_ratio", 100);
								tTemplate.iPrintSpeed    = JsonUtil::getIntValue(ptItemJson, "print_speed", 0);
								tTemplate.iPrintHei      = JsonUtil::getIntValue(ptItemJson, "print_hei", 0);
								tTemplate.iLightVoltage  = JsonUtil::getIntValue(ptItemJson, "light_voltage", 0);
								tTemplate.iModelFeature  = JsonUtil::getIntValue(ptItemJson, "model_feature", 0);

								m_tVecTemplate.push_back(tTemplate);
							}
						}

						cJSON_Delete(ptRootJson);
					}

					bRet = true;
				}

				delete[] pcBuf;
			}
		}

		tFile.Close();
	}

	return bRet;
}

/***********************************************************************
 * 功能: 保存模板列表到文件
 * 参数:
 *     无
 * 返回值：
 *	   true:  成功
 *     false: 失败
************************************************************************/
bool TemplateManager::saveTemplateToFile()
{
	bool bRet = false;
	CFile tFile;
	CString tTemplateFile = FileUtil::getAppPath(TEMPLATE_FILE);

	if (tFile.Open(tTemplateFile, CFile::modeCreate|CFile::modeWrite|CFile::typeText)) {
		cJSON* ptRootJson = cJSON_CreateArray();

		if (ptRootJson != nullptr) {
			cJSON* ptItemJson;

			for (auto tTemplate : m_tVecTemplate) {
				ptItemJson = cJSON_CreateObject();
				if (ptItemJson != nullptr) {
					cJSON_AddItemToArray(ptRootJson, ptItemJson);

					cJSON_AddStringToObject(ptItemJson, "template_file", tTemplate.tTemplateFile.c_str());
					cJSON_AddStringToObject(ptItemJson, "image_file",    tTemplate.tImageFile.c_str());
					cJSON_AddStringToObject(ptItemJson, "product_file",  tTemplate.tProductFile.c_str());
					cJSON_AddStringToObject(ptItemJson, "mask_file",     tTemplate.tMaskFile.c_str());
					cJSON_AddStringToObject(ptItemJson, "name",          tTemplate.tName.c_str());

					cJSON_AddNumberToObject(ptItemJson, "method",          tTemplate.iMethod);
					cJSON_AddNumberToObject(ptItemJson, "layer",           tTemplate.iLayer);
					cJSON_AddNumberToObject(ptItemJson, "search_type",     tTemplate.iSearchType);
					cJSON_AddNumberToObject(ptItemJson, "center_offset_x", tTemplate.iCenterOffsetX);
					cJSON_AddNumberToObject(ptItemJson, "center_offset_y", tTemplate.iCenterOffsetY);
					cJSON_AddNumberToObject(ptItemJson, "model_feature",   tTemplate.iModelFeature);

					cJSON_AddNumberToObject(ptItemJson, "score",          tTemplate.iScore);
					cJSON_AddNumberToObject(ptItemJson, "max_over_lap",   tTemplate.iMaxOverLap);
					cJSON_AddNumberToObject(ptItemJson, "contrast_ratio", tTemplate.iContrastRatio);
					cJSON_AddNumberToObject(ptItemJson, "print_speed",    tTemplate.iPrintSpeed);
					cJSON_AddNumberToObject(ptItemJson, "print_hei",      tTemplate.iPrintHei);
					cJSON_AddNumberToObject(ptItemJson, "light_voltage",  tTemplate.iLightVoltage);
				}
			}

			char* pcJsonText = cJSON_Print(ptRootJson);

			if (pcJsonText != nullptr) {
				tFile.Write(pcJsonText, strlen(pcJsonText));

				free(pcJsonText);
			}

			cJSON_Delete(ptRootJson);

			bRet = true;
		}

		tFile.Close();
	}

	return bRet;
}

/***********************************************************************
 * 功能: 获取模板列表
 * 参数:
 *     无
 * 返回值：
 *	   模板列表地址
************************************************************************/
std::vector<TemplateManager::T_Template>* TemplateManager::getTemplateList()
{
	return &m_tVecTemplate;
}

/***********************************************************************
 * 功能: 获取模板列表中某一项参数
 * 参数:
 *     iIndex: 序号
 * 返回值：
 *	   模板参数地址
************************************************************************/
TemplateManager::PT_Template TemplateManager::getTemplateParam(int iIndex)
{
	TemplateManager::PT_Template ptTemplate = nullptr;

	if ((m_tVecTemplate.size() > 0) && (iIndex >= 0) && (iIndex < m_tVecTemplate.size())) {
		ptTemplate = &m_tVecTemplate[iIndex];
	}

	return ptTemplate;
}

/***********************************************************************
 * 功能: 检测模板是否存在
 * 参数:
 *     tName: 模板名
 * 返回值：
 *	   true:  存在
 *     false: 不存在
************************************************************************/
bool TemplateManager::isExist(std::string tName)
{
	bool bRet = false;

	if (!tName.empty() && (m_tVecTemplate.size() > 0)) {
		for (auto tTemplate : m_tVecTemplate) {
			if (tTemplate.tName == tName) {
				bRet = true;

				break;
			}
		}
	}

	return bRet;
}

int TemplateManager::findNameIndex(std::string tName)
{
	if (!tName.empty() && (m_tVecTemplate.size() > 0)) {
		for (int i = 0; i < m_tVecTemplate.size(); i++) {
			if (m_tVecTemplate[i].tName == tName) {
				return i;
			}
		}
	}

	return -1;
}


/***********************************************************************
 * 功能: 获取模版列表的大小
 * 参数:
 *     无
 * 返回值：
 *	   模版列表长度
************************************************************************/
int TemplateManager::getTemplateListSize()
{
	return m_tVecTemplate.size();
}


/***********************************************************************
 * 功能: 添加模板
 * 参数:
 *     ptTemplate: 模板参数
 * 返回值：
 *	   true:  成功
 *     false: 失败
************************************************************************/
bool TemplateManager::addTemplate(PT_Template ptTemplate)
{
	bool bRet = false;

	if (ptTemplate != nullptr) {
		if (!isExist(ptTemplate->tName)) {
			m_tVecTemplate.push_back(*ptTemplate);

			saveTemplateToFile();
			bRet = true;
		}
	}

	return bRet;
}

/***********************************************************************
 * 功能: 删除模板
 * 参数:
 *     iIndex: 序号
 * 返回值：
 *	   true:  成功
 *     false: 失败
************************************************************************/
bool TemplateManager::delTemplate(int iIndex)
{
	bool bRet = false;

	if ((iIndex >= 0) && (iIndex < m_tVecTemplate.size())) {
		deleteModel(m_tVecTemplate[iIndex].tName);

		m_tVecTemplate.erase(m_tVecTemplate.begin() + iIndex);

		bRet = saveTemplateToFile();
	}

	return bRet;
}

/***********************************************************************
 * 功能: 判断模板模型文件是否存在
 * 参数:
 *     tName: 模板名称
 * 返回值：
 *	   true:  存在
 *     false: 不存在
************************************************************************/
bool TemplateManager::isModelExist(std::string tName, bool bShapeModel)
{
	bool bRet = false;

#ifndef USE_HALCON
	CString tModeName = FileUtil::getAppPath(TEMPLATE_MODEL) + L"\\" + String2Wstring(tName).c_str();

	if (PathFileExists(tModeName + ".yaml") && PathFileExists(tModeName + "_cfg.yaml")) {
		bRet = true;
	}
#else
	CString tModeName = FileUtil::getAppPath(HALCON_MODEL) + L"\\" + String2Wstring(tName).c_str();

	if ((bShapeModel && PathFileExists(tModeName + ".shm")) || (!bShapeModel) && PathFileExists(tModeName + ".ncm")) {
		bRet = true;
	}
#endif

	return bRet;
}

/***********************************************************************
 * 功能: 删除模板模型
 * 参数:
 *     tName: 模板名称
 * 返回值：
 *	   无
************************************************************************/
void TemplateManager::deleteModel(std::string tName)
{
#ifndef USE_HALCON
	CString tModeName = FileUtil::getAppPath(TEMPLATE_MODEL) + L"\\" + String2Wstring(tName).c_str();

	DeleteFile(tModeName + ".yaml");
	DeleteFile(tModeName + "_cfg.yaml");
#else
	CString tModeName = FileUtil::getAppPath(HALCON_MODEL) + L"\\" + String2Wstring(tName).c_str();

	DeleteFile(tModeName + ".cfg");

	if(PathFileExists(tModeName + ".shm")) DeleteFile(tModeName + ".shm");
	if (PathFileExists(tModeName + ".ncm")) DeleteFile(tModeName + ".ncm");
#endif
}
