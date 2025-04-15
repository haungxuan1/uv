#pragma once

#include "../cJSON.h"

class JsonUtil
{
public:
	static int getIntValue(cJSON *ptJson, const char* pcKeyName, int iDefVal);
	static double getDoubleValue(cJSON* ptJson, const char* pcKeyName, double dDefVal);
	static const char* getStringValue(cJSON* ptJson, const char* pcKeyName, const char* pcDefVal);

private:
	JsonUtil();
	~JsonUtil();
};