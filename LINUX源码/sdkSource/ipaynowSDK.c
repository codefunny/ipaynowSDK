#include "innerInterface.h"
#include "log.h"
#ifndef _WIN32
int ipaynow_JSON_INTERFACE(char const *input, char *out)
{
	LOG_WriteLog("=====Enter Json Interface Function====");
	int ret = INNER_JSON_INTERFACE(input, out);
	LOG_WriteLog("=====End Json Interface Function  ====");
	return ret;
}

int ipaynowSDKTrade(SDKINTRADE const *in, SDKOUTTRADE *out)
{
	LOG_WriteLog("=====Enter Json Interface Function====");
	int ret = INNER_STRUCT_INTERFACE(in, out);
	LOG_WriteLog("=====End Json Interface Function  ====");
	return ret;
}

#else
int __declspec(dllexport) ipaynow_JSON_INTERFACE(char const *input, char *out)
{
	LOG_WriteLog("===== Enter Json Interface Function ====");
	int ret = INNER_JSON_INTERFACE(input, out);
	LOG_WriteLog("===== End Json Interface Function   ====");
	return ret;
}

int __declspec(dllexport) ipaynowSDKTrade(SDKINTRADE const *in, SDKOUTTRADE *out)
{
	LOG_WriteLog("===== Enter Json Interface Function ====");
	int ret = INNER_STRUCT_INTERFACE(in, out);
	LOG_WriteLog("===== End Json Interface Function   ====");
	return ret;
}

#endif