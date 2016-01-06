
#ifndef IPAYNOWSDK_H
#define IPAYNOWSDK_H
typedef struct sdkintrade
{
	char msgType[4];
	char cardInfo[2048];
	char mKey[33];
	int txnAmt;
	char mchTransId[60];
	char mchTransSTime[15];
	char cretiId[48];
	char orgMchTransId[60];
}SDKINTRADE;
typedef struct sdkouttrade
{
	char respCode[3];
	char mchTransETime[15];
	char transId[41];
	char accInfo[60];
	int txnAmt;
	char cretiId[48];
	char cusInfo[74];
	char mchTransId[60];
}SDKOUTTRADE;

typedef int(*ipaynowSDKTrade)(SDKINTRADE const *in, SDKOUTTRADE *out);


typedef int(*ipaynowSDKTrade_Json)(char const *in, char *out);


#endif