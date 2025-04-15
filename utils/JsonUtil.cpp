#include "pch.h"
#include "JsonUtil.h"

int JsonUtil::getIntValue(cJSON* ptJson, const char* pcKeyName, int iDefVal)
{
    int iRet = iDefVal;

    if ((ptJson != nullptr) && (pcKeyName != nullptr) && (*pcKeyName != '\0')) {
        cJSON* ptFindJson = cJSON_GetObjectItem(ptJson, pcKeyName);

        if (ptFindJson != nullptr) {
            iRet = ptFindJson->valueint;
        }
    }

    return iRet;
}

double JsonUtil::getDoubleValue(cJSON* ptJson, const char* pcKeyName, double dDefVal)
{
    double dRet = dDefVal;

    if ((ptJson != nullptr) && (pcKeyName != nullptr) && (*pcKeyName != '\0')) {
        cJSON* ptFindJson = cJSON_GetObjectItem(ptJson, pcKeyName);

        if (ptFindJson != nullptr) {
            dRet = ptFindJson->valuedouble;
        }
    }

    return dRet;
}

const char* JsonUtil::getStringValue(cJSON* ptJson, const char* pcKeyName, const char* pcDefVal)
{
    const char* pcRet = pcDefVal;

    if ((ptJson != nullptr) && (pcKeyName != nullptr) && (*pcKeyName != '\0')) {
        cJSON* ptFindJson = cJSON_GetObjectItem(ptJson, pcKeyName);

        if ((ptFindJson != NULL) && (ptFindJson->valuestring != nullptr)) {
            pcRet = ptFindJson->valuestring;
        }
    }

    return pcRet;
}
