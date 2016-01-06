#include "innerInterface.h"

#ifdef linux

int ipaynow_JSON_INTERFACE(char const *input, char *out);

int ipaynowSDKTrade(SDKINTRADE const *in, SDKOUTTRADE *out);

#else

int __declspec(dllexport) ipaynow_JSON_INTERFACE(char const *input, char *out);

int __declspec(dllexport) ipaynowSDKTrade(SDKINTRADE const *in, SDKOUTTRADE *out);

#endif