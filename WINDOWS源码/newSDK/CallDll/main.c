#include "ipaynowSDK.h"
#include<stdio.h>


#ifndef _WIN32
#include <dlfcn.h> 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
void test_printout(SDKOUTTRADE const *out)  //打印结果
{
	printf("\n收到结果:\n");
	printf("respCode  :  %s \n", out->respCode);
	printf("mchTransETime  :  %s \n", out->mchTransETime);
	printf("transId  :  %s \n", out->transId);
	printf("accInfo  :  %s \n", out->accInfo);
	printf("txnAmt  :  %d \n", out->txnAmt);
	printf("cretiId  :  %s \n", out->cretiId);
	printf("cusInfo  :  %s \n", out->cusInfo);
	printf("mchTransId  :  %s \n", out->mchTransId);

}

void test_printHeader()
{
	printf("========== SDK TEST  =====================\n");
	printf(" 22 : Alipay - Pay            23: Alipay - Flush \n");
	printf(" 25 : Alipay - Refund                      \n");
	printf(" 32: WX - PAY                33: WX - FLUSH     \n");
	printf(" 35: WX - REFUND                          \n");
	printf(" 12: Union - Pay             13: Union - Flush  \n");
	printf(" 14: Union - Revoke          15: Union - Refund \n");
	printf(" 16: Union - PrePay          17: Union - PrePayRevoke \n");
	printf(" 18: Union - PrePayFinish    19: Union - Query \n");
	printf(" 20: Union - PrePayFlush     21: Union - PrePayFinishRevoke \n");
	printf("Please input your choice ! enter 0 exit   \n");
}

int test_Alipay_Pay()
{
	void *handle;  
	handle=dlopen("./libipaynowSDK.so",RTLD_LAZY);//open lib file   
	ipaynowSDKTrade pay = dlsym(handle,"ipaynowSDKTrade");//call dlsym function   
	
	SDKINTRADE sdkin;
	memset(&sdkin, 0, sizeof(SDKINTRADE));
	SDKOUTTRADE sdkout;
	memset(&sdkout, 0, sizeof(SDKOUTTRADE));

	sdkin.txnAmt = 1;								//金额
	char *msgType = "022";
	char *mKey = "12345678901234561234567890123456";

	char mchTransId[30];
	memset(mchTransId, 0, sizeof(mchTransId));
	printf("Please input mchTransId: \n");
	scanf("%s", &mchTransId);

	//char *mchTransId = "1231231239530";				//交易号
	char *mchTransSTime = "20150812150420";
	char *cretiId = "11111111";
	char *orgMchTransId = "";
	char cardInfo[2048];

	char QRCode[30];
	memset(QRCode, 0, sizeof(QRCode));
	printf("Please input QRCode: \n");
	scanf("%s", &QRCode);

	//char *QRCode = "289491986291525291";			//二维码
	memset(cardInfo, 0, sizeof(cardInfo));
	sprintf(cardInfo, "{\"QRCode\":\"%s\"}", QRCode);
	//printf(cardInfo);
	strcpy(sdkin.msgType, msgType);
	strcpy(sdkin.mKey, mKey);
	strcpy(sdkin.mchTransId, mchTransId);
	strcpy(sdkin.mchTransSTime, mchTransSTime);
	strcpy(sdkin.cretiId, cretiId);
	strcpy(sdkin.cardInfo, cardInfo);
	//printf(sdkin.cardInfo);
	strcpy(sdkin.orgMchTransId, orgMchTransId);
	int ret = pay(&sdkin, &sdkout);
	test_printout(&sdkout);
	dlclose(handle); 
	return ret;
}

int test_Alipay_Flush()
{
	void *handle;  
	handle=dlopen("./libipaynowSDK.so",RTLD_LAZY);//open lib file   
	ipaynowSDKTrade pay = dlsym(handle,"ipaynowSDKTrade");//call dlsym function   


	SDKINTRADE sdkin;
	memset(&sdkin, 0, sizeof(SDKINTRADE));
	SDKOUTTRADE sdkout;
	memset(&sdkout, 0, sizeof(SDKOUTTRADE));

	sdkin.txnAmt = 1;								//金额
	char *msgType = "023";
	char *mKey = "12345678901234561234567890123456";
	char *mchTransId = "1231231239526";	
	char *mchTransSTime = "20150812150420";
	char *cretiId = "11111111";

	char orgMchTransId[30];
	memset(orgMchTransId, 0, sizeof(orgMchTransId));
	printf("Please input orgMchTransId: \n");
	scanf("%s", &orgMchTransId);

	//char *orgMchTransId = "1231231239527";			//原交易号
	char cardInfo[2048];
	char *QRCode = "282481639281824266";			//二维码
	memset(cardInfo, 0, sizeof(cardInfo));
	sprintf(cardInfo, "{\"QRCode\":\"%s\"}", QRCode);
	
	strcpy(sdkin.msgType, msgType);
	strcpy(sdkin.mKey, mKey);
	strcpy(sdkin.mchTransId, mchTransId);
	strcpy(sdkin.mchTransSTime, mchTransSTime);
	strcpy(sdkin.cretiId, cretiId);
	strcpy(sdkin.cardInfo, cardInfo);
	
	strcpy(sdkin.orgMchTransId, orgMchTransId);
	int ret = pay(&sdkin, &sdkout);
	test_printout(&sdkout);
	return ret;
}

int test_Alipay_Refund()
{
	void *handle;  
	handle=dlopen("./libipaynowSDK.so",RTLD_LAZY);//open lib file   
	ipaynowSDKTrade pay = dlsym(handle,"ipaynowSDKTrade");//call dlsym function   


	SDKINTRADE sdkin;
	memset(&sdkin, 0, sizeof(SDKINTRADE));
	SDKOUTTRADE sdkout;
	memset(&sdkout, 0, sizeof(SDKOUTTRADE));

	sdkin.txnAmt = 1;								//金额
	char *msgType = "025";
	char *mKey = "12345678901234561234567890123456";
	char *mchTransId = "1231231239526";
	char *mchTransSTime = "20150812150420";
	char *cretiId = "11111111";

	char orgMchTransId[30];
	memset(orgMchTransId, 0, sizeof(orgMchTransId));
	printf("Please input orgMchTransId: \n");
	scanf("%s", &orgMchTransId);
	//char *orgMchTransId = "1231231239530";			//原交易号

	char cardInfo[2048];
	char *QRCode = "282481639281824266";			//二维码
	memset(cardInfo, 0, sizeof(cardInfo));
	sprintf(cardInfo, "{\"QRCode\":\"%s\"}", QRCode);
	//printf(cardInfo);
	strcpy(sdkin.msgType, msgType);
	strcpy(sdkin.mKey, mKey);
	strcpy(sdkin.mchTransId, mchTransId);
	strcpy(sdkin.mchTransSTime, mchTransSTime);
	strcpy(sdkin.cretiId, cretiId);
	strcpy(sdkin.cardInfo, cardInfo);
	//printf(sdkin.cardInfo);
	strcpy(sdkin.orgMchTransId, orgMchTransId);
	int ret = pay(&sdkin, &sdkout);
	test_printout(&sdkout);
	return ret;
}
int test_WX_Pay()
{
	void *handle;  
	handle=dlopen("./libipaynowSDK.so",RTLD_LAZY);//open lib file   
	ipaynowSDKTrade pay = dlsym(handle,"ipaynowSDKTrade");//call dlsym function   


	SDKINTRADE sdkin;
	memset(&sdkin, 0, sizeof(SDKINTRADE));
	SDKOUTTRADE sdkout;
	memset(&sdkout, 0, sizeof(SDKOUTTRADE));

	sdkin.txnAmt = 1;								//金额
	char *msgType = "032";
	char *mKey = "12345678901234561234567890123456";

	char mchTransId[30];
	memset(mchTransId, 0, sizeof(mchTransId));
	printf("Please input mchTransId: \n");
	scanf("%s", &mchTransId);

	//char *mchTransId = "1231231230006";				//交易号

	char *mchTransSTime = "20150812150420";
	char *cretiId = "11111111";
	char *orgMchTransId = "";
	char cardInfo[2048];

	char QRCode[30];
	memset(QRCode, 0, sizeof(QRCode));
	printf("Please input QRCode: \n");
	scanf("%s", &QRCode);
	//char *QRCode = "130821146969264761";			//二维码

	memset(cardInfo, 0, sizeof(cardInfo));
	sprintf(cardInfo, "{\"QRCode\":\"%s\"}", QRCode);
	//printf(cardInfo);
	strcpy(sdkin.msgType, msgType);
	strcpy(sdkin.mKey, mKey);
	strcpy(sdkin.mchTransId, mchTransId);
	strcpy(sdkin.mchTransSTime, mchTransSTime);
	strcpy(sdkin.cretiId, cretiId);
	strcpy(sdkin.cardInfo, cardInfo);
	//printf(sdkin.cardInfo);
	strcpy(sdkin.orgMchTransId, orgMchTransId);
	int ret = pay(&sdkin, &sdkout);
	test_printout(&sdkout);
	return ret;
}

int test_WX_Flush()
{
	void *handle;  
	handle=dlopen("./libipaynowSDK.so",RTLD_LAZY);//open lib file   
	ipaynowSDKTrade pay = dlsym(handle,"ipaynowSDKTrade");//call dlsym function   

	SDKINTRADE sdkin;
	memset(&sdkin, 0, sizeof(SDKINTRADE));
	SDKOUTTRADE sdkout;
	memset(&sdkout, 0, sizeof(SDKOUTTRADE));

	sdkin.txnAmt = 1;								//金额
	char *msgType = "033";
	char *mKey = "12345678901234561234567890123456";
	char *mchTransId = "1231231230000";
	char *mchTransSTime = "20150812150420";
	char *cretiId = "11111111";

	char orgMchTransId[30];
	memset(orgMchTransId, 0, sizeof(orgMchTransId));
	printf("Please input orgMchTransId: \n");
	scanf("%s", &orgMchTransId);
	//char *orgMchTransId = "1231231230003";			//原交易号

	char cardInfo[2048];
	char *QRCode = "284208031008006906";			//二维码
	memset(cardInfo, 0, sizeof(cardInfo));
	sprintf(cardInfo, "{\"QRCode\":\"%s\"}", QRCode);
	//printf(cardInfo);
	strcpy(sdkin.msgType, msgType);
	strcpy(sdkin.mKey, mKey);
	strcpy(sdkin.mchTransId, mchTransId);
	strcpy(sdkin.mchTransSTime, mchTransSTime);
	strcpy(sdkin.cretiId, cretiId);
	strcpy(sdkin.cardInfo, cardInfo);
	//printf(sdkin.cardInfo);
	strcpy(sdkin.orgMchTransId, orgMchTransId);
	int ret = pay(&sdkin, &sdkout);
	test_printout(&sdkout);
	return ret;
}

int test_WX_Refund()
{
	void *handle;  
	handle=dlopen("./libipaynowSDK.so",RTLD_LAZY);//open lib file   
	ipaynowSDKTrade pay = dlsym(handle,"ipaynowSDKTrade");//call dlsym function   


	SDKINTRADE sdkin;
	memset(&sdkin, 0, sizeof(SDKINTRADE));
	SDKOUTTRADE sdkout;
	memset(&sdkout, 0, sizeof(SDKOUTTRADE));

	sdkin.txnAmt = 1;								//金额
	char *msgType = "035";
	char *mKey = "12345678901234561234567890123456";

	
	char *mchTransId = "1231231239526";
	char *mchTransSTime = "20150812150420";
	char *cretiId = "11111111";

	char orgMchTransId[30];
	memset(orgMchTransId, 0, sizeof(orgMchTransId));
	printf("Please input orgMchTransId: \n");
	scanf("%s", &orgMchTransId);

	//char *orgMchTransId = "1231231230006";			//原交易号
	char cardInfo[2048];
	char *QRCode = "130666745428966245";			//二维码
	memset(cardInfo, 0, sizeof(cardInfo));
	sprintf(cardInfo, "{\"QRCode\":\"%s\"}", QRCode);
	//printf(cardInfo);
	strcpy(sdkin.msgType, msgType);
	strcpy(sdkin.mKey, mKey);
	strcpy(sdkin.mchTransId, mchTransId);
	strcpy(sdkin.mchTransSTime, mchTransSTime);
	strcpy(sdkin.cretiId, cretiId);
	strcpy(sdkin.cardInfo, cardInfo);
	//printf(sdkin.cardInfo);
	strcpy(sdkin.orgMchTransId, orgMchTransId);
	int ret = pay(&sdkin, &sdkout);
	test_printout(&sdkout);
	return ret;
}


int test_Union_Pay()
{
	void *handle;  
	handle=dlopen("./libipaynowSDK.so",RTLD_LAZY);//open lib file   
	ipaynowSDKTrade pay = dlsym(handle,"ipaynowSDKTrade");//call dlsym function   

	SDKINTRADE sdkin;
	memset(&sdkin, 0, sizeof(SDKINTRADE));
	SDKOUTTRADE sdkout;
	memset(&sdkout, 0, sizeof(SDKOUTTRADE));

	sdkin.txnAmt = 1;								//金额
	char *msgType = "012";
	char *mKey = "12345678901234561234567890123456";
	//char *mchTransId = "1231231239526";
	char *mchTransSTime = "20150812150420";
	char *cretiId = "11111111";

	char mchTransId[30];
	memset(mchTransId, 0, sizeof(mchTransId));
	printf("Please input mchTransId: \n");
	scanf("%s", &mchTransId);

	char *orgMchTransId = "";			//原交易号
	char cardInfo[2048];

	//char *entCode = "02";
	char *csn = "";
	char *trk2 = "4392268307661786d200610111333579";
	char *trk3 = "";
	char *pin_data = "";
	char *exp_date = "";
	char *pan = "";
	char *ic_data = "";
	//char *ic_data = "9f3303e0e1c800006aec28129f2608d454f553538f555e9f360204f182027c009c01009f1a0201569a031508149f02060000000000015f2a0201569f03060000000000009f3501229f34030203009f370403b253b39f2701809f410400001527";
	char entCode[5];
	memset(entCode, 0, sizeof(entCode));
	if (strlen(ic_data) == 0)		// 有IC域,写入 05 否则按磁条卡02对待
	{
		sprintf(entCode, "02");
	}
	else
	{
		sprintf(entCode, "05");
	}
	if (strlen(pan) != 0)			// 如果有手输卡号.选择 01
	{
		sprintf(entCode, "01");
	}
	sprintf(cardInfo, "{\"entCode\":\"%s\" , \
						\"csn\":\"%s\",		 \
						\"trk2\":\"%s\",	\
						\"trk3\":\"%s\",	\
						\"pin_data\":\"%s\",\
						\"exp_date\":\"%s\",\
						\"pan\":\"%s\",\
						\"ic_data\":\"%s\"}",
						entCode, csn, trk2, trk3, pin_data, exp_date, pan, ic_data);
	//printf(cardInfo);
	strcpy(sdkin.msgType, msgType);
	strcpy(sdkin.mKey, mKey);
	strcpy(sdkin.mchTransId, mchTransId);
	strcpy(sdkin.mchTransSTime, mchTransSTime);
	strcpy(sdkin.cretiId, cretiId);
	strcpy(sdkin.cardInfo, cardInfo);
	//printf(sdkin.cardInfo);
	strcpy(sdkin.orgMchTransId, orgMchTransId);
	int ret = pay(&sdkin, &sdkout);
	test_printout(&sdkout);
	return ret;
}

int test_Union_Flush()
{
	void *handle;  
	handle=dlopen("./libipaynowSDK.so",RTLD_LAZY);//open lib file   
	ipaynowSDKTrade pay = dlsym(handle,"ipaynowSDKTrade");//call dlsym function   

	SDKINTRADE sdkin;
	memset(&sdkin, 0, sizeof(SDKINTRADE));
	SDKOUTTRADE sdkout;
	memset(&sdkout, 0, sizeof(SDKOUTTRADE));

	sdkin.txnAmt = 1;								//金额
	char *msgType = "013";
	char *mKey = "12345678901234561234567890123456";
	char *mchTransId = "1231231239526";
	char *mchTransSTime = "20150812150420";
	char *cretiId = "11111111";

	char orgMchTransId[30];
	memset(orgMchTransId, 0, sizeof(orgMchTransId));
	printf("Please input orgMchTransId: \n");
	scanf("%s", &orgMchTransId);
	//char *orgMchTransId = "1231231230006";			//原交易号
	char cardInfo[2048];
	//char *entCode = "02";
	char *csn = "";
	//char *trk2 = "6228480018449666876d2412221554000";
	char *trk2 = "4392268307661786d200610111333579";
	char *trk3 = "";
	char *pin_data = "";
	char *exp_date = "";
	char *pan = "";
	char *ic_data = "";
	//char *ic_data = "9f3303e0e1c895050000006aec28129f26000001527";
	char entCode[5];
	memset(entCode, 0, sizeof(entCode));
	if (strlen(ic_data) == 0)		// 有IC域,写入 05 否则按磁条卡02对待
	{
		sprintf(entCode, "02");
	}
	else
	{
		sprintf(entCode, "05");
	}
	if (strlen(pan) != 0)			// 如果有手输卡号.选择 01
	{
		sprintf(entCode, "01");
	}
	sprintf(cardInfo, "{\"entCode\":\"%s\" , \"csn\":\"%s\",\"trk2\":\"%s\",\"trk3\":\"%s\",\"pin_data\":\"%s\",\"exp_date\":\"%s\",\"pan\":\"%s\",\"ic_data\":\"%s\"}",
		entCode, csn, trk2, trk3, pin_data, exp_date, pan, ic_data);
	//printf(cardInfo);
	strcpy(sdkin.msgType, msgType);
	strcpy(sdkin.mKey, mKey);
	strcpy(sdkin.mchTransId, mchTransId);
	strcpy(sdkin.mchTransSTime, mchTransSTime);
	strcpy(sdkin.cretiId, cretiId);
	strcpy(sdkin.cardInfo, cardInfo);
	//printf(sdkin.cardInfo);
	strcpy(sdkin.orgMchTransId, orgMchTransId);
	int ret = pay(&sdkin, &sdkout);
	test_printout(&sdkout);
	return ret;
}
int test_Union_Revoke()
{
	void *handle;  
	handle=dlopen("./libipaynowSDK.so",RTLD_LAZY);//open lib file   
	ipaynowSDKTrade pay = dlsym(handle,"ipaynowSDKTrade");//call dlsym function   

	SDKINTRADE sdkin;
	memset(&sdkin, 0, sizeof(SDKINTRADE));
	SDKOUTTRADE sdkout;
	memset(&sdkout, 0, sizeof(SDKOUTTRADE));

	sdkin.txnAmt = 1;								//金额
	char *msgType = "014";
	char *mKey = "12345678901234561234567890123456";
	char *mchTransId = "1231231239526";
	char *mchTransSTime = "20150812150420";
	char *cretiId = "11111111";

	char orgMchTransId[30];
	memset(orgMchTransId, 0, sizeof(orgMchTransId));
	printf("Please input orgMchTransId: \n");
	scanf("%s", &orgMchTransId);

	//char *orgMchTransId = "1231231230006";			//原交易号
	char cardInfo[2048];

	//char *entCode = "02";
	char *csn = "";
	//char *trk2 = "6228480018449666876d24122215543510000";
	char *trk2 = "4392268307661786d200610111333579";
	char *trk3 = "";
	char *pin_data = "357478";
	char *exp_date = "";
	char *pan = "";
	char *ic_data = "";
	//char *ic_data = "9f3303e0e1c89505080004e0009f1e0833443533383733379f101307080103a0a800010a0100000000006aec28129f2608d454f553538f555e9f360204f182027c009c01009f1a0201569a031508149f02060000000000015f2a0201569f03060000000000009f3501229f34030203009f370403b253b39f2701809f410400001527";
	char entCode[5];
	memset(entCode, 0, sizeof(entCode));
	if (strlen(ic_data) == 0)		// 有IC域,写入 05 否则按磁条卡02对待
	{
		sprintf(entCode, "02");
	}
	else
	{
		sprintf(entCode, "05");
	}
	if (strlen(pan) != 0)			// 如果有手输卡号.选择 01
	{
		sprintf(entCode, "01");
	}

	sprintf(cardInfo, "{\"entCode\":\"%s\" , \"csn\":\"%s\",\"trk2\":\"%s\",\"trk3\":\"%s\",\"pin_data\":\"%s\",\"exp_date\":\"%s\",\"pan\":\"%s\",\"ic_data\":\"%s\"}",
		entCode, csn, trk2, trk3, pin_data, exp_date, pan, ic_data);
	//printf(cardInfo);
	strcpy(sdkin.msgType, msgType);
	strcpy(sdkin.mKey, mKey);
	strcpy(sdkin.mchTransId, mchTransId);
	strcpy(sdkin.mchTransSTime, mchTransSTime);
	strcpy(sdkin.cretiId, cretiId);
	strcpy(sdkin.cardInfo, cardInfo);
	//printf(sdkin.cardInfo);
	strcpy(sdkin.orgMchTransId, orgMchTransId);
	int ret = pay(&sdkin, &sdkout);
	test_printout(&sdkout);
	return ret;
}


int test_Union_Refund()
{
	void *handle;  
	handle=dlopen("./libipaynowSDK.so",RTLD_LAZY);//open lib file   
	ipaynowSDKTrade pay = dlsym(handle,"ipaynowSDKTrade");//call dlsym function   

	SDKINTRADE sdkin;
	memset(&sdkin, 0, sizeof(SDKINTRADE));
	SDKOUTTRADE sdkout;
	memset(&sdkout, 0, sizeof(SDKOUTTRADE));

	sdkin.txnAmt = 1;								//金额
	char *msgType = "015";
	char *mKey = "12345678901234561234567890123456";
	char *mchTransId = "1231231239526";
	char *mchTransSTime = "20150812150420";
	char *cretiId = "11111111";

	char orgMchTransId[30];
	memset(orgMchTransId, 0, sizeof(orgMchTransId));
	printf("Please input orgMchTransId: \n");
	scanf("%s", &orgMchTransId);

	//char *orgMchTransId = "1231231230006";			//原交易号
	char cardInfo[2048];

	//char *entCode = "02";
	char *csn = "";
	//char *trk2 = "6228480018449666876d24122215543510000";
	char *trk2 = "4392268307661786d200610111333579";
	char *trk3 = "";
	char *pin_data = "357478";
	char *exp_date = "";
	char *pan = "";
	char *ic_data = "";
	//char *ic_data = "9f3303e0e1c89505080004e0009f1e0833443533383733379f101307080103a0a800010a0100000000006aec28129f2608d454f553538f555e9f360204f182027c009c01009f1a0201569a031508149f02060000000000015f2a0201569f03060000000000009f3501229f34030203009f370403b253b39f2701809f410400001527";
	char entCode[5];
	memset(entCode, 0, sizeof(entCode));
	if (strlen(ic_data) == 0)		// 有IC域,写入 05 否则按磁条卡02对待
	{
		sprintf(entCode, "02");
	}
	else
	{
		sprintf(entCode, "05");
	}
	if (strlen(pan) != 0)			// 如果有手输卡号.选择 01
	{
		sprintf(entCode, "01");
	}


	sprintf(cardInfo, "{\"entCode\":\"%s\" , \"csn\":\"%s\",\"trk2\":\"%s\",\"trk3\":\"%s\",\"pin_data\":\"%s\",\"exp_date\":\"%s\",\"pan\":\"%s\",\"ic_data\":\"%s\"}",
		entCode, csn, trk2, trk3, pin_data, exp_date, pan, ic_data);
	//printf(cardInfo);
	strcpy(sdkin.msgType, msgType);
	strcpy(sdkin.mKey, mKey);
	strcpy(sdkin.mchTransId, mchTransId);
	strcpy(sdkin.mchTransSTime, mchTransSTime);
	strcpy(sdkin.cretiId, cretiId);
	strcpy(sdkin.cardInfo, cardInfo);
	//printf(sdkin.cardInfo);
	strcpy(sdkin.orgMchTransId, orgMchTransId);
	int ret = pay(&sdkin, &sdkout);
	test_printout(&sdkout);
	return ret;
}



int test_Union_Query()
{
	void *handle;  
	handle=dlopen("./libipaynowSDK.so",RTLD_LAZY);//open lib file   
	ipaynowSDKTrade pay = dlsym(handle,"ipaynowSDKTrade");//call dlsym function   

	SDKINTRADE sdkin;
	memset(&sdkin, 0, sizeof(SDKINTRADE));
	SDKOUTTRADE sdkout;
	memset(&sdkout, 0, sizeof(SDKOUTTRADE));

	sdkin.txnAmt = 1;								//金额
	char *msgType = "019";
	char *mKey = "12345678901234561234567890123456";
	//char *mchTransId = "1231231239526";
	char *mchTransSTime = "20150812150420";
	char *cretiId = "11111111";

	char mchTransId[30];
	memset(mchTransId, 0, sizeof(mchTransId));
	printf("Please input mchTransId: \n");
	scanf("%s", &mchTransId);

	//char *orgMchTransId = "1231231230006";			//原交易号
	char cardInfo[2048];

	//char *entCode = "02";
	char *csn = "";
	char *trk2 = "6228480018449666876d24122215543510000";
	char *trk3 = "";
	char *pin_data = "357478";
	char *orgMchTransId = "";
	char *exp_date = "";
	char *pan = "";
	//char *ic_data = "";
	char *ic_data = "9f3303e0e1c89505080004e0009f1e0833443533383733379f101307080103a0a800010a0100000000006aec28129f2608d454f553538f555e9f360204f182027c009c01009f1a0201569a031508149f02060000000000015f2a0201569f03060000000000009f3501229f34030203009f370403b253b39f2701809f410400001527";
	char entCode[5];
	memset(entCode, 0, sizeof(entCode));
	if (strlen(ic_data) == 0)		// 有IC域,写入 05 否则按磁条卡02对待
	{
		sprintf(entCode, "02");
	}
	else
	{
		sprintf(entCode, "05");
	}
	if (strlen(pan) != 0)			// 如果有手输卡号.选择 01
	{
		sprintf(entCode, "01");
	}
	sprintf(cardInfo, "{\"entCode\":\"%s\" , \
						\"csn\":\"%s\",		 \
						\"trk2\":\"%s\",	\
						\"trk3\":\"%s\",	\
						\"pin_data\":\"%s\",\
						\"exp_date\":\"%s\",\
						\"pan\":\"%s\",\
						\"ic_data\":\"%s\"}",
						entCode, csn, trk2, trk3, pin_data, exp_date, pan, ic_data);
	//printf(cardInfo);
	strcpy(sdkin.msgType, msgType);
	strcpy(sdkin.mKey, mKey);
	strcpy(sdkin.mchTransId, mchTransId);
	strcpy(sdkin.mchTransSTime, mchTransSTime);
	strcpy(sdkin.cretiId, cretiId);
	strcpy(sdkin.cardInfo, cardInfo);
	//printf(sdkin.cardInfo);
	strcpy(sdkin.orgMchTransId, orgMchTransId);
	int ret = pay(&sdkin, &sdkout);
	test_printout(&sdkout);
	return ret;
}

int test_Union_Pre_Pay()
{
	void *handle;  
	handle=dlopen("./libipaynowSDK.so",RTLD_LAZY);//open lib file   
	ipaynowSDKTrade pay = dlsym(handle,"ipaynowSDKTrade");//call dlsym function   

	SDKINTRADE sdkin;
	memset(&sdkin, 0, sizeof(SDKINTRADE));
	SDKOUTTRADE sdkout;
	memset(&sdkout, 0, sizeof(SDKOUTTRADE));

	sdkin.txnAmt = 1;								//金额
	char *msgType = "016";
	char *mKey = "12345678901234561234567890123456";
	//char *mchTransId = "1231231239526";
	char *mchTransSTime = "20150812150420";
	char *cretiId = "11111111";

	char mchTransId[30];
	memset(mchTransId, 0, sizeof(mchTransId));
	printf("Please input mchTransId: \n");
	scanf("%s", &mchTransId);

	char *orgMchTransId = "";			//原交易号
	char cardInfo[2048];

	//char *entCode = "02";
	char *csn = "000";
	//char *trk2 = "6228480018449666876d24122215543510000";
	char *trk2 = "4392268307661786d200610111333579";
	char *trk3 = "";
	char *pin_data = "357478";
	char *exp_date = "2412";
	char *pan = "";
	char *ic_data = "";
	//char *ic_data = "9f26087db9ad03747884969f2701809f101307080103a0a000010a010000000000f90e3ab89f3704803a4a7e9f3602050c9505080004e8009a031508189c01039f02060000000000015f2a02015682027c009f1a0201569f03060000000000009f3303e0e1c89f34030203009f3501229f1e0820202020202020208408a0000003330101019f090200209f410400000363";
	char entCode[5];
	memset(entCode, 0, sizeof(entCode));
	if (strlen(ic_data) == 0)		// 有IC域,写入 05 否则按磁条卡02对待
	{
		sprintf(entCode, "02");
	}
	else
	{
		sprintf(entCode, "05");
	}
	if (strlen(pan) != 0)			// 如果有手输卡号.选择 01
	{
		sprintf(entCode, "01");
	}
	sprintf(cardInfo, "{\"entCode\":\"%s\" , \
						\"csn\":\"%s\",		 \
						\"trk2\":\"%s\",	\
						\"trk3\":\"%s\",	\
						\"pin_data\":\"%s\",\
						\"exp_date\":\"%s\",\
						\"pan\":\"%s\",\
						\"ic_data\":\"%s\"}",
						entCode, csn, trk2, trk3, pin_data, exp_date, pan, ic_data);
	//printf(cardInfo);
	strcpy(sdkin.msgType, msgType);
	strcpy(sdkin.mKey, mKey);
	strcpy(sdkin.mchTransId, mchTransId);
	strcpy(sdkin.mchTransSTime, mchTransSTime);
	strcpy(sdkin.cretiId, cretiId);
	strcpy(sdkin.cardInfo, cardInfo);
	//printf(sdkin.cardInfo);
	strcpy(sdkin.orgMchTransId, orgMchTransId);
	int ret = pay(&sdkin, &sdkout);
	test_printout(&sdkout);
	return ret;
}


int test_Union_Pre_Pay_Revoke()
{
	void *handle;  
	handle=dlopen("./libipaynowSDK.so",RTLD_LAZY);//open lib file   
	ipaynowSDKTrade pay = dlsym(handle,"ipaynowSDKTrade");//call dlsym function   

	SDKINTRADE sdkin;
	memset(&sdkin, 0, sizeof(SDKINTRADE));
	SDKOUTTRADE sdkout;
	memset(&sdkout, 0, sizeof(SDKOUTTRADE));

	sdkin.txnAmt = 1;								//金额
	char *msgType = "017";
	char *mKey = "12345678901234561234567890123456";
	char *mchTransId = "1231231239526";
	char *mchTransSTime = "20150812150420";
	char *cretiId = "11111111";

	char orgMchTransId[30];
	memset(orgMchTransId, 0, sizeof(orgMchTransId));
	printf("Please input orgMchTransId: \n");
	scanf("%s", &orgMchTransId);

	//char *orgMchTransId = "1231231230006";			//原交易号
	char cardInfo[2048];

	//char *entCode = "02";
	char *csn = "";
	//char *trk2 = "6228480018449666876d24122215543510000";
	char *trk2 = "4392268307661786d200610111333579";
	char *trk3 = "";
	char *pin_data = "357478";
	char *exp_date = "";
	char *pan = "";
	char *ic_data = "";
	//char *ic_data = "9f3303e0e1c89505080004e0009f1e0833443533383733379f101307080103a0a800010a0100000000006aec28129f2608d454f553538f555e9f360204f182027c009c01009f1a0201569a031508149f02060000000000015f2a0201569f03060000000000009f3501229f34030203009f370403b253b39f2701809f410400001527";
	char entCode[5];
	memset(entCode, 0, sizeof(entCode));
	if (strlen(ic_data) == 0)		// 有IC域,写入 05 否则按磁条卡02对待
	{
		sprintf(entCode, "02");
	}
	else
	{
		sprintf(entCode, "05");
	}
	if (strlen(pan) != 0)			// 如果有手输卡号.选择 01
	{
		sprintf(entCode, "01");
	}
	sprintf(cardInfo, "{\"entCode\":\"%s\" , \
						\"csn\":\"%s\",		 \
						\"trk2\":\"%s\",	\
						\"trk3\":\"%s\",	\
						\"pin_data\":\"%s\",\
						\"exp_date\":\"%s\",\
						\"pan\":\"%s\",\
						\"ic_data\":\"%s\"}",
						entCode, csn, trk2, trk3, pin_data, exp_date, pan, ic_data);

	strcpy(sdkin.msgType, msgType);
	strcpy(sdkin.mKey, mKey);
	strcpy(sdkin.mchTransId, mchTransId);
	strcpy(sdkin.mchTransSTime, mchTransSTime);
	strcpy(sdkin.cretiId, cretiId);
	strcpy(sdkin.cardInfo, cardInfo);
	//printf(sdkin.cardInfo);
	strcpy(sdkin.orgMchTransId, orgMchTransId);
	int ret = pay(&sdkin, &sdkout);
	test_printout(&sdkout);
	return ret;
}

int test_Union_Pre_Pay_Finish()
{
	void *handle;  
	handle=dlopen("./libipaynowSDK.so",RTLD_LAZY);//open lib file   
	ipaynowSDKTrade pay = dlsym(handle,"ipaynowSDKTrade");//call dlsym function   

	SDKINTRADE sdkin;
	memset(&sdkin, 0, sizeof(SDKINTRADE));
	SDKOUTTRADE sdkout;
	memset(&sdkout, 0, sizeof(SDKOUTTRADE));

	sdkin.txnAmt = 1;								//金额
	char *msgType = "018";
	char *mKey = "12345678901234561234567890123456";
	//char *mchTransId = "1231231239526";
	char *mchTransSTime = "20150812150420";
	char *cretiId = "11111111";

	char mchTransId[30];
	memset(mchTransId, 0, sizeof(mchTransId));
	printf("Please input mchTransId: \n");
	scanf("%s", &mchTransId);

	char orgMchTransId[30];
	memset(orgMchTransId, 0, sizeof(orgMchTransId));
	printf("Please input orgMchTransId: \n");
	scanf("%s", &orgMchTransId);

	//char *orgMchTransId = "1231231230006";			//原交易号
	char cardInfo[2048];

	//char *entCode = "02";
	char *csn = "000";
	char *trk2 = "4392268307661786d200610111333579";
	//char *trk2 = "6228480018449666876d24122215543510000";
	char *trk3 = "";
	char *pin_data = "";
	char *exp_date = "";
	char *pan = "";
	char *ic_data = "";
	//char *ic_data = "9f26087db9ad03747884969f2701809f101307080103a0a000010a010000000000f90e3ab89f3704803a4a7e9f3602050c9505080004e8009a031508189c01039f02060000000000015f2a02015682027c009f1a0201569f03060000000000009f3303e0e1c89f34030203009f3501229f1e0820202020202020208408a0000003330101019f090200209f410400000363";
	char entCode[5];
	memset(entCode, 0, sizeof(entCode));
	if (strlen(ic_data) == 0)		// 有IC域,写入 05 否则按磁条卡02对待
	{
		sprintf(entCode, "02");
	}
	else
	{
		sprintf(entCode, "05");
	}
	if (strlen(pan) != 0)			// 如果有手输卡号.选择 01
	{
		sprintf(entCode, "01");
	}
	sprintf(cardInfo, "{\"entCode\":\"%s\" , \
						\"csn\":\"%s\",		 \
						\"trk2\":\"%s\",	\
						\"trk3\":\"%s\",	\
						\"pin_data\":\"%s\",\
						\"exp_date\":\"%s\",\
						\"pan\":\"%s\",\
							\"ic_data\":\"%s\"}",
						entCode, csn, trk2, trk3, pin_data, exp_date, pan, ic_data);
	//printf(cardInfo);
	strcpy(sdkin.msgType, msgType);
	strcpy(sdkin.mKey, mKey);
	strcpy(sdkin.mchTransId, mchTransId);
	strcpy(sdkin.mchTransSTime, mchTransSTime);
	strcpy(sdkin.cretiId, cretiId);
	strcpy(sdkin.cardInfo, cardInfo);
	//printf(sdkin.cardInfo);
	strcpy(sdkin.orgMchTransId, orgMchTransId);
	int ret = pay(&sdkin, &sdkout);
	test_printout(&sdkout);
	return ret;
}

int test_Union_Per_Pay_Flush()
{
	void *handle;  
	handle=dlopen("./libipaynowSDK.so",RTLD_LAZY);//open lib file   
	ipaynowSDKTrade pay = dlsym(handle,"ipaynowSDKTrade");//call dlsym function   

	SDKINTRADE sdkin;
	memset(&sdkin, 0, sizeof(SDKINTRADE));
	SDKOUTTRADE sdkout;
	memset(&sdkout, 0, sizeof(SDKOUTTRADE));

	sdkin.txnAmt = 1;								//金额
	char *msgType = "020";
	char *mKey = "12345678901234561234567890123456";
	char *mchTransId = "1231231239526";
	char *mchTransSTime = "20150812150420";
	char *cretiId = "11111111";

	char orgMchTransId[30];
	memset(orgMchTransId, 0, sizeof(orgMchTransId));
	printf("Please input orgMchTransId: \n");
	scanf("%s", &orgMchTransId);

	//char *orgMchTransId = "1231231230006";			//原交易号
	char cardInfo[2048];

	//char *entCode = "02";
	char *csn = "000";
	//char *trk2 = "6228480018449666876d24122215543510000";
	char *trk2 = "4392268307661786d200610111333579";
	char *trk3 = "";
	char *pin_data = "123";
	char *exp_date = "2412";
	char *pan = "";
	char *ic_data = "";
	//char *ic_data = "9f26087db9ad03747884969f2701809f101307080103a0a000010a010000000000f90e3ab89f3704803a4a7e9f3602050c9505080004e8009a031508189c01039f02060000000000015f2a02015682027c009f1a0201569f03060000000000009f3303e0e1c89f34030203009f3501229f1e0820202020202020208408a0000003330101019f090200209f410400000363";
	char entCode[5];
	memset(entCode, 0, sizeof(entCode));
	if (strlen(ic_data) == 0)		// 有IC域,写入 05 否则按磁条卡02对待
	{
		sprintf(entCode, "02");
	}
	else
	{
		sprintf(entCode, "05");
	}
	if (strlen(pan) != 0)			// 如果有手输卡号.选择 01
	{
		sprintf(entCode, "01");
	}
	sprintf(cardInfo, "{\"entCode\":\"%s\" , \
					  \"csn\":\"%s\",		 \
					  \"trk2\":\"%s\",	\
					  \"trk3\":\"%s\",	\
					  \"pin_data\":\"%s\",\
					  \"exp_date\":\"%s\",\
					  \"pan\":\"%s\",\
					  \"ic_data\":\"%s\"}",
					  entCode, csn, trk2, trk3, pin_data, exp_date, pan, ic_data);
	//printf(cardInfo);
	strcpy(sdkin.msgType, msgType);
	strcpy(sdkin.mKey, mKey);
	strcpy(sdkin.mchTransId, mchTransId);
	strcpy(sdkin.mchTransSTime, mchTransSTime);
	strcpy(sdkin.cretiId, cretiId);
	strcpy(sdkin.cardInfo, cardInfo);
	//printf(sdkin.cardInfo);
	strcpy(sdkin.orgMchTransId, orgMchTransId);
	int ret = pay(&sdkin, &sdkout);
	test_printout(&sdkout);
	return ret;
}
int test_Pre_Pay_Finsh_Revoke()
{
	void *handle;  
	handle=dlopen("./libipaynowSDK.so",RTLD_LAZY);//open lib file   
	ipaynowSDKTrade pay = dlsym(handle,"ipaynowSDKTrade");//call dlsym function   

	SDKINTRADE sdkin;
	memset(&sdkin, 0, sizeof(SDKINTRADE));
	SDKOUTTRADE sdkout;
	memset(&sdkout, 0, sizeof(SDKOUTTRADE));

	sdkin.txnAmt = 1;								//金额
	char *msgType = "021";
	char *mKey = "12345678901234561234567890123456";
	char *mchTransId = "1231231239526";
	char *mchTransSTime = "20150812150420";
	char *cretiId = "11111111";

	char orgMchTransId[30];
	memset(orgMchTransId, 0, sizeof(orgMchTransId));
	printf("Please input orgMchTransId: \n");
	scanf("%s", &orgMchTransId);

	//char *orgMchTransId = "1231231230006";			//原交易号
	char cardInfo[2048];

	//char *entCode = "02";
	char *csn = "";
	//char *trk2 = "6228480018449666876d24122215543510000";
	char *trk2 = "4392268307661786d200610111333579";
	char *trk3 = "";
	char *pin_data = "";
	char *exp_date = "";
	char *pan = "";
	char *ic_data = "";
	//char *ic_data = "9f3303e0e1c89505080004e0009f1e0833443533383733379f101307080103a0a800010a0100000000006aec28129f2608d454f553538f555e9f360204f182027c009c01009f1a0201569a031508149f02060000000000015f2a0201569f03060000000000009f3501229f34030203009f370403b253b39f2701809f410400001527";
	char entCode[5];
	memset(entCode, 0, sizeof(entCode));
	if (strlen(ic_data) == 0)		// 有IC域,写入 05 否则按磁条卡02对待
	{
		sprintf(entCode, "02");
	}
	else
	{
		sprintf(entCode, "05");
	}
	if (strlen(pan) != 0)			// 如果有手输卡号.选择 01
	{
		sprintf(entCode, "01");
	}
	sprintf(cardInfo, "{\"entCode\":\"%s\" , \
					  \"csn\":\"%s\",		 \
					  \"trk2\":\"%s\",	\
					  \"trk3\":\"%s\",	\
					  \"pin_data\":\"%s\",\
					  \"exp_date\":\"%s\",\
					  \"pan\":\"%s\",\
					  \"ic_data\":\"%s\"}",
					  entCode, csn, trk2, trk3, pin_data, exp_date, pan, ic_data);
	//printf(cardInfo);
	strcpy(sdkin.msgType, msgType);
	strcpy(sdkin.mKey, mKey);
	strcpy(sdkin.mchTransId, mchTransId);
	strcpy(sdkin.mchTransSTime, mchTransSTime);
	strcpy(sdkin.cretiId, cretiId);
	strcpy(sdkin.cardInfo, cardInfo);
	//printf(sdkin.cardInfo);
	strcpy(sdkin.orgMchTransId, orgMchTransId);
	int ret = pay(&sdkin, &sdkout);
	test_printout(&sdkout);
	return ret;
}
int main()
{
	//system("pause");
	test_printHeader();
	int i = 0;
	//scanf("%d", &i);
	while (scanf("%d", &i))
	{
		if (i == 0)
		{
			break;
		}
		switch (i)
		{
		case 22:
			test_Alipay_Pay();
			break;
		case 23:
			test_Alipay_Flush();
			break;
		case 25:
			test_Alipay_Refund();
			break;
		case 32:
			test_WX_Pay();
			break;
		case 33:
			test_WX_Flush();
			break;
		case 35:
			test_WX_Refund();
			break;
		case 12:
			test_Union_Pay();
			break;
		case 13:
			test_Union_Flush();
			break;
		case 14:
			test_Union_Revoke();
			break;
		case 15:
			test_Union_Refund();
			break;
		case 16:
			test_Union_Pre_Pay();
			break;
		case 17:
			test_Union_Pre_Pay_Revoke();
			break;
		case 18:
			test_Union_Pre_Pay_Finish();
			break;
		case 19:
			test_Union_Query();
			break;
		case 20:
			test_Union_Per_Pay_Flush();
			break;
		case 21:
			test_Pre_Pay_Finsh_Revoke();
			break;
		default:
			break;
		}
		test_printHeader();

	}
	system("pause");
	return 0;
}

#else
#include <windows.h>
void test_printout(SDKOUTTRADE const *out)  //打印结果
{
	printf("\n收到结果       :\n");
	printf("respCode       :  %s \n", out->respCode);
	printf("mchTransETime  :  %s \n", out->mchTransETime);
	printf("transId        :  %s \n", out->transId);
	printf("accInfo        :  %s \n", out->accInfo);
	printf("txnAmt         :  %d \n", out->txnAmt);
	printf("cretiId        :  %s \n", out->cretiId);
	printf("cusInfo        :  %s \n", out->cusInfo);
	printf("mchTransId     :  %s \n", out->mchTransId);

}
int test_Alipay_Pay()
{
	HINSTANCE Hint = LoadLibrary("newSDK.dll");//加载我们刚才生成的dll
	ipaynowSDKTrade pay = (ipaynowSDKTrade)GetProcAddress(Hint, "ipaynowSDKTrade");//取得dll导出的add方法

	SDKINTRADE sdkin;
	memset(&sdkin, 0, sizeof(SDKINTRADE));
	SDKOUTTRADE sdkout;
	memset(&sdkout, 0, sizeof(SDKOUTTRADE));

	sdkin.txnAmt = 1;								//金额
	char *msgType = "022";
	char *mKey = "12345678901234561234567890123456";

	char mchTransId[30];
	memset(mchTransId, 0, sizeof(mchTransId));
	printf("Please input mchTransId: \n");
	scanf("%s", &mchTransId);

	//char *mchTransId = "1231231239530";				//交易号
	char *mchTransSTime = "20150812150420";
	char *cretiId = "11111111";
	char *orgMchTransId = "";
	char cardInfo[2048];

	char QRCode[30];
	memset(QRCode, 0, sizeof(QRCode));
	printf("Please input QRCode: \n");
	scanf("%s", &QRCode);

	//char *QRCode = "289491986291525291";			//二维码
	memset(cardInfo, 0, sizeof(cardInfo));
	sprintf(cardInfo, "{\"QRCode\":\"%s\"}", QRCode);
	//printf(cardInfo);
	strcpy(sdkin.msgType, msgType);
	strcpy(sdkin.mKey, mKey);
	strcpy(sdkin.mchTransId, mchTransId);
	strcpy(sdkin.mchTransSTime, mchTransSTime);
	strcpy(sdkin.cretiId, cretiId);
	strcpy(sdkin.cardInfo, cardInfo);
	//printf(sdkin.cardInfo);
	strcpy(sdkin.orgMchTransId, orgMchTransId);
	int ret = pay(&sdkin, &sdkout);
	test_printout(&sdkout);
	return ret;
}

int test_Alipay_Flush()
{
	HINSTANCE Hint = LoadLibrary("newSDK.dll");										//加载我们刚才生成的dll
	ipaynowSDKTrade pay = (ipaynowSDKTrade)GetProcAddress(Hint, "ipaynowSDKTrade");	//取得dll导出的add方法

	SDKINTRADE sdkin;
	memset(&sdkin, 0, sizeof(SDKINTRADE));
	SDKOUTTRADE sdkout;
	memset(&sdkout, 0, sizeof(SDKOUTTRADE));

	sdkin.txnAmt = 1;																//金额
	char *msgType = "023";
	char *mKey = "12345678901234561234567890123456";
	char *mchTransId = "1231231239526";
	char *mchTransSTime = "20150812150420";
	char *cretiId = "201508130000114000002414394679138467";

	char orgMchTransId[30];
	memset(orgMchTransId, 0, sizeof(orgMchTransId));
	printf("Please input orgMchTransId: \n");
	scanf("%s", &orgMchTransId);

	//char *orgMchTransId = "";														//原交易号
	char cardInfo[2048];
	char *QRCode = "282481639281824266";											//二维码
	memset(cardInfo, 0, sizeof(cardInfo));
	sprintf(cardInfo, "{\"QRCode\":\"%s\"}", QRCode);
	//printf(cardInfo);
	strcpy(sdkin.msgType, msgType);
	strcpy(sdkin.mKey, mKey);
	strcpy(sdkin.mchTransId, mchTransId);
	strcpy(sdkin.mchTransSTime, mchTransSTime);
	strcpy(sdkin.cretiId, cretiId);
	strcpy(sdkin.cardInfo, cardInfo);
	//printf(sdkin.cardInfo);
	strcpy(sdkin.orgMchTransId, orgMchTransId);
	int ret = pay(&sdkin, &sdkout);
	test_printout(&sdkout);
	return ret;
}

int test_Alipay_Refund()
{
	HINSTANCE Hint = LoadLibrary("newSDK.dll");//加载我们刚才生成的dll
	ipaynowSDKTrade pay = (ipaynowSDKTrade)GetProcAddress(Hint, "ipaynowSDKTrade");//取得dll导出的add方法

	SDKINTRADE sdkin;
	memset(&sdkin, 0, sizeof(SDKINTRADE));
	SDKOUTTRADE sdkout;
	memset(&sdkout, 0, sizeof(SDKOUTTRADE));

	sdkin.txnAmt = 1;								//金额
	char *msgType = "025";
	char *mKey = "12345678901234561234567890123456";

	char *mchTransId = "1231231239526";
	char *mchTransSTime = "20150812150420";
	char *cretiId = "11111111";

	char orgMchTransId[30];
	memset(orgMchTransId, 0, sizeof(orgMchTransId));
	printf("Please input orgMchTransId: \n");
	scanf("%s", &orgMchTransId);

	//char *orgMchTransId = "1231231230006";			//原交易号
	char cardInfo[2048];
	char *QRCode = "130666745428966245";			//二维码
	memset(cardInfo, 0, sizeof(cardInfo));
	sprintf(cardInfo, "{\"QRCode\":\"%s\"}", QRCode);
	//printf(cardInfo);
	strcpy(sdkin.msgType, msgType);
	strcpy(sdkin.mKey, mKey);
	strcpy(sdkin.mchTransId, mchTransId);
	strcpy(sdkin.mchTransSTime, mchTransSTime);
	strcpy(sdkin.cretiId, cretiId);
	strcpy(sdkin.cardInfo, cardInfo);
	//printf(sdkin.cardInfo);
	strcpy(sdkin.orgMchTransId, orgMchTransId);
	int ret = pay(&sdkin, &sdkout);
	test_printout(&sdkout);
	return 0;
}
int test_WX_Pay()
{
	HINSTANCE Hint = LoadLibrary("newSDK.dll");//加载我们刚才生成的dll
	ipaynowSDKTrade pay = (ipaynowSDKTrade)GetProcAddress(Hint, "ipaynowSDKTrade");//取得dll导出的add方法

	SDKINTRADE sdkin;
	memset(&sdkin, 0, sizeof(SDKINTRADE));
	SDKOUTTRADE sdkout;
	memset(&sdkout, 0, sizeof(SDKOUTTRADE));

	sdkin.txnAmt = 2;																			//金额
	char *msgType = "032";
	char *mKey = "12345678901234561234567890123456";

	char mchTransId[30];
	memset(mchTransId, 0, sizeof(mchTransId));
	printf("Please input mchTransId: \n");
	scanf("%s", &mchTransId);
	//char *mchTransId = "1231231230006";				//交易号

	char *mchTransSTime = "20150812150420";
	char *cretiId = "11111111";
	char *orgMchTransId = "";
	char cardInfo[2048];

	char QRCode[30];
	memset(QRCode, 0, sizeof(QRCode));
	printf("Please input QRCode: \n");
	scanf("%s", &QRCode);
	//char *QRCode = "130821146969264761";													//二维码

	memset(cardInfo, 0, sizeof(cardInfo));
	sprintf(cardInfo, "{\"QRCode\":\"%s\"}", QRCode);
	//printf(cardInfo);
	strcpy(sdkin.msgType, msgType);
	strcpy(sdkin.mKey, mKey);
	strcpy(sdkin.mchTransId, mchTransId);
	strcpy(sdkin.mchTransSTime, mchTransSTime);
	strcpy(sdkin.cretiId, cretiId);
	strcpy(sdkin.cardInfo, cardInfo);
	//printf(sdkin.cardInfo);
	strcpy(sdkin.orgMchTransId, orgMchTransId);
	int ret = pay(&sdkin, &sdkout);
	test_printout(&sdkout);
	return ret;
}

int test_WX_Flush()
{
	HINSTANCE Hint = LoadLibrary("newSDK.dll");//加载我们刚才生成的dll

	ipaynowSDKTrade pay = (ipaynowSDKTrade)GetProcAddress(Hint, "ipaynowSDKTrade");//取得dll导出的add方法

	SDKINTRADE sdkin;
	memset(&sdkin, 0, sizeof(SDKINTRADE));
	SDKOUTTRADE sdkout;
	memset(&sdkout, 0, sizeof(SDKOUTTRADE));

	sdkin.txnAmt = 1;								//金额
	char *msgType = "033";
	char *mKey = "12345678901234561234567890123456";
	char *mchTransId = "1231231230000";
	char *mchTransSTime = "20150812150420";
	char *cretiId = "11111111";

	char orgMchTransId[30];
	memset(orgMchTransId, 0, sizeof(orgMchTransId));
	printf("Please input orgMchTransId: \n");
	scanf("%s", &orgMchTransId);
	//char *orgMchTransId = "1231231230003";			//原交易号
	
	char cardInfo[2048];
	char *QRCode = "284208031008006906";			//二维码
	memset(cardInfo, 0, sizeof(cardInfo));
	sprintf(cardInfo, "{\"QRCode\":\"%s\"}", QRCode);
	//printf(cardInfo);
	strcpy(sdkin.msgType, msgType);
	strcpy(sdkin.mKey, mKey);
	strcpy(sdkin.mchTransId, mchTransId);
	strcpy(sdkin.mchTransSTime, mchTransSTime);
	strcpy(sdkin.cretiId, cretiId);
	strcpy(sdkin.cardInfo, cardInfo);
	//printf(sdkin.cardInfo);
	strcpy(sdkin.orgMchTransId, orgMchTransId);
	int ret = pay(&sdkin, &sdkout);
	test_printout(&sdkout);
	return ret;
}

int test_WX_Refund()
{
	HINSTANCE Hint = LoadLibrary("newSDK.dll");//加载我们刚才生成的dll

	ipaynowSDKTrade pay = (ipaynowSDKTrade)GetProcAddress(Hint, "ipaynowSDKTrade");//取得dll导出的add方法

	SDKINTRADE sdkin;
	memset(&sdkin, 0, sizeof(SDKINTRADE));
	SDKOUTTRADE sdkout;
	memset(&sdkout, 0, sizeof(SDKOUTTRADE));

	sdkin.txnAmt = 1;								//金额
	char *msgType = "035";
	char *mKey = "12345678901234561234567890123456";
	char *mchTransId = "1231231239526";
	char *mchTransSTime = "20150812150420";
	char *cretiId = "11111111";

	char orgMchTransId[30];
	memset(orgMchTransId, 0, sizeof(orgMchTransId));
	printf("Please input orgMchTransId: \n");
	scanf("%s", &orgMchTransId);

	//char *orgMchTransId = "1231231230006";			//原交易号
	char cardInfo[2048];
	char *QRCode = "130666745428966245";			//二维码
	memset(cardInfo, 0, sizeof(cardInfo));
	sprintf(cardInfo, "{\"QRCode\":\"%s\"}", QRCode);
	//printf(cardInfo);
	strcpy(sdkin.msgType, msgType);
	strcpy(sdkin.mKey, mKey);
	strcpy(sdkin.mchTransId, mchTransId);
	strcpy(sdkin.mchTransSTime, mchTransSTime);
	strcpy(sdkin.cretiId, cretiId);
	strcpy(sdkin.cardInfo, cardInfo);
	//printf(sdkin.cardInfo);
	strcpy(sdkin.orgMchTransId, orgMchTransId);
	int ret = pay(&sdkin, &sdkout);
	test_printout(&sdkout);
	return ret;
}

int test_Union_Pay()
{
	HINSTANCE Hint = LoadLibrary("ipaynowSDK.dll");//加载我们刚才生成的dll
	ipaynowSDKTrade pay = (ipaynowSDKTrade)GetProcAddress(Hint, "ipaynowSDKTrade");//取得dll导出的add方法

	SDKINTRADE sdkin;
	memset(&sdkin, 0, sizeof(SDKINTRADE));
	SDKOUTTRADE sdkout;
	memset(&sdkout, 0, sizeof(SDKOUTTRADE));

	sdkin.txnAmt = 1;								//金额
	char *msgType = "012";
	char *mKey = "12345678901234561234567890123456";
	//char *mchTransId = "1231231239526";
	char *mchTransSTime = "20150812150420";
	char *cretiId = "11111111";

	char mchTransId[30];
	memset(mchTransId, 0, sizeof(mchTransId));
	printf("Please input mchTransId: \n");
	scanf("%s", &mchTransId);

	char *orgMchTransId = "";			//原交易号
	char cardInfo[2048];

	//char *entCode = "02";
	char *csn = "";
	char *trk2 = "4392268307661786d200610111333579";
	char *trk3 = "";
	char *pin_data = "";
	char *exp_date = "";
	char *pan = "";
	char *ic_data = "";
	//char *ic_data = "9f3303e0e1c89505080004e0009f1e0833443533383733379f101307080103a0a800010a0100000000006aec28129f2608d454f553538f555e9f360204f182027c009c01009f1a0201569a031508149f02060000000000015f2a0201569f03060000000000009f3501229f34030203009f370403b253b39f2701809f410400001527";
	char entCode[5];
	memset(entCode, 0, sizeof(entCode));
	if (strlen(ic_data) == 0)		// 有IC域,写入 05 否则按磁条卡02对待
	{
		sprintf(entCode, "02");
	}
	else
	{
		sprintf(entCode, "05");
	}
	if (strlen(pan) != 0)			// 如果有手输卡号.选择 01
	{
		sprintf(entCode, "01");
	}
	sprintf(cardInfo, "{\"entCode\":\"%s\" , \
					  	\"csn\":\"%s\",		 \
						\"trk2\":\"%s\",	\
						\"trk3\":\"%s\",	\
						\"pin_data\":\"%s\",\
						\"exp_date\":\"%s\",\
						\"pan\":\"%s\",\
						\"ic_data\":\"%s\"}",
						entCode, csn, trk2, trk3, pin_data, exp_date, pan, ic_data);
	//printf(cardInfo);
	strcpy(sdkin.msgType, msgType);
	strcpy(sdkin.mKey, mKey);
	strcpy(sdkin.mchTransId, mchTransId);
	strcpy(sdkin.mchTransSTime, mchTransSTime);
	strcpy(sdkin.cretiId, cretiId);
	strcpy(sdkin.cardInfo, cardInfo);
	//printf(sdkin.cardInfo);
	strcpy(sdkin.orgMchTransId, orgMchTransId);
	int ret = pay(&sdkin, &sdkout);
	test_printout(&sdkout);
	return ret;
}

int test_Union_Flush()
{
	HINSTANCE Hint = LoadLibrary("ipaynowSDK.dll");//加载我们刚才生成的dll
	ipaynowSDKTrade pay = (ipaynowSDKTrade)GetProcAddress(Hint, "ipaynowSDKTrade");//取得dll导出的add方法

	SDKINTRADE sdkin;
	memset(&sdkin, 0, sizeof(SDKINTRADE));
	SDKOUTTRADE sdkout;
	memset(&sdkout, 0, sizeof(SDKOUTTRADE));

	sdkin.txnAmt = 1;								//金额
	char *msgType = "013";
	char *mKey = "12345678901234561234567890123456";
	char *mchTransId = "1231231239526";
	char *mchTransSTime = "20150812150420";
	char *cretiId = "11111111";

	char orgMchTransId[30];
	memset(orgMchTransId, 0, sizeof(orgMchTransId));
	printf("Please input orgMchTransId: \n");
	scanf("%s", &orgMchTransId);
	//char *orgMchTransId = "1231231230006";			//原交易号
	char cardInfo[2048];
	//char *entCode = "02";
	char *csn = "";
	//char *trk2 = "6228480018449666876d24122215543510000";
	char *trk2 = "4392268307661786d200610111333579";
	char *trk3 = "";
	char *pin_data = "";
	char *exp_date = "";
	char *pan = "";
	char *ic_data = "";
	//char *ic_data = "9f3303e0e1c89505080004e0009f1e0833443533383733379f101307080103a0a800010a0100000000006aec28129f2608d454f553538f555e9f360204f182027c009c01009f1a0201569a031508149f02060000000000015f2a0201569f03060000000000009f3501229f34030203009f370403b253b39f2701809f410400001527";
	char entCode[5];
	memset(entCode, 0, sizeof(entCode));
	if (strlen(ic_data) == 0)		// 有IC域,写入 05 否则按磁条卡02对待
	{
		sprintf(entCode, "02");
	}
	else
	{
		sprintf(entCode, "05");
	}
	if (strlen(pan) != 0)			// 如果有手输卡号.选择 01
	{
		sprintf(entCode, "01");
	}
	sprintf(cardInfo, "{\"entCode\":\"%s\" , \"csn\":\"%s\",\"trk2\":\"%s\",\"trk3\":\"%s\",\"pin_data\":\"%s\",\"exp_date\":\"%s\",\"pan\":\"%s\",\"ic_data\":\"%s\"}",
					entCode, csn, trk2, trk3, pin_data, exp_date, pan, ic_data);
	//printf(cardInfo);
	strcpy(sdkin.msgType, msgType);
	strcpy(sdkin.mKey, mKey);
	strcpy(sdkin.mchTransId, mchTransId);
	strcpy(sdkin.mchTransSTime, mchTransSTime);
	strcpy(sdkin.cretiId, cretiId);
	strcpy(sdkin.cardInfo, cardInfo);
	//printf(sdkin.cardInfo);
	strcpy(sdkin.orgMchTransId, orgMchTransId);
	int ret = pay(&sdkin, &sdkout);
	test_printout(&sdkout);
	return ret;
}
int test_Union_Revoke()
{
	HINSTANCE Hint = LoadLibrary("ipaynowSDK.dll");//加载我们刚才生成的dll
	ipaynowSDKTrade pay = (ipaynowSDKTrade)GetProcAddress(Hint, "ipaynowSDKTrade");//取得dll导出的add方法

	SDKINTRADE sdkin;
	memset(&sdkin, 0, sizeof(SDKINTRADE));
	SDKOUTTRADE sdkout;
	memset(&sdkout, 0, sizeof(SDKOUTTRADE));

	sdkin.txnAmt = 1;								//金额
	char *msgType = "014";
	char *mKey = "12345678901234561234567890123456";
	char *mchTransId = "1231231239526";
	char *mchTransSTime = "20150812150420";
	char *cretiId = "11111111";

	char orgMchTransId[30];
	memset(orgMchTransId, 0, sizeof(orgMchTransId));
	printf("Please input orgMchTransId: \n");
	scanf("%s", &orgMchTransId);

	//char *orgMchTransId = "1231231230006";			//原交易号
	char cardInfo[2048];

	//char *entCode = "02";
	char *csn = "";
	//char *trk2 = "6228480018449666876d24122215543510000";
	char *trk2 = "4392268307661786d200610111333579";
	char *trk3 = "";
	char *pin_data = "357478";
	char *exp_date = "";
	char *pan = "";
	char *ic_data = "";
	//char *ic_data = "9f3303e0e1c89505080004e0009f1e0833443533383733379f101307080103a0a800010a0100000000006aec28129f2608d454f553538f555e9f360204f182027c009c01009f1a0201569a031508149f02060000000000015f2a0201569f03060000000000009f3501229f34030203009f370403b253b39f2701809f410400001527";
	char entCode[5];
	memset(entCode, 0, sizeof(entCode));
	if (strlen(ic_data) == 0)		// 有IC域,写入 05 否则按磁条卡02对待
	{
		sprintf(entCode, "02");
	}
	else
	{
		sprintf(entCode, "05");
	}
	if (strlen(pan) != 0)			// 如果有手输卡号.选择 01
	{
		sprintf(entCode, "01");
	}

	sprintf(cardInfo, "{\"entCode\":\"%s\" , \"csn\":\"%s\",\"trk2\":\"%s\",\"trk3\":\"%s\",\"pin_data\":\"%s\",\"exp_date\":\"%s\",\"pan\":\"%s\",\"ic_data\":\"%s\"}",
		entCode, csn, trk2, trk3, pin_data, exp_date, pan, ic_data);
	//printf(cardInfo);
	strcpy(sdkin.msgType, msgType);
	strcpy(sdkin.mKey, mKey);
	strcpy(sdkin.mchTransId, mchTransId);
	strcpy(sdkin.mchTransSTime, mchTransSTime);
	strcpy(sdkin.cretiId, cretiId);
	strcpy(sdkin.cardInfo, cardInfo);
	//printf(sdkin.cardInfo);
	strcpy(sdkin.orgMchTransId, orgMchTransId);
	int ret = pay(&sdkin, &sdkout);
	test_printout(&sdkout);
	return ret;
}


int test_Union_Refund()
{
	HINSTANCE Hint = LoadLibrary("ipaynowSDK.dll");//加载我们刚才生成的dll
	ipaynowSDKTrade pay = (ipaynowSDKTrade)GetProcAddress(Hint, "ipaynowSDKTrade");//取得dll导出的方法

	SDKINTRADE sdkin;
	memset(&sdkin, 0, sizeof(SDKINTRADE));
	SDKOUTTRADE sdkout;
	memset(&sdkout, 0, sizeof(SDKOUTTRADE));

	sdkin.txnAmt = 1;								//金额
	char *msgType = "015";
	char *mKey = "12345678901234561234567890123456";
	char *mchTransId = "1231231239526";
	char *mchTransSTime = "20150812150420";
	char *cretiId = "11111111";

	char orgMchTransId[30];
	memset(orgMchTransId, 0, sizeof(orgMchTransId));
	printf("Please input orgMchTransId: \n");
	scanf("%s", &orgMchTransId);

	//char *orgMchTransId = "1231231230006";			//原交易号
	char cardInfo[2048];

	//char *entCode = "02";
	char *csn = "";
	//char *trk2 = "6228480018449666876d24122215543510000";
	char *trk2 = "4392268307661786d200610111333579";
	char *trk3 = "";
	char *pin_data = "357478";
	char *exp_date = "";
	char *pan = "";
	char *ic_data = "";
	//char *ic_data = "9f3303e0e1c89505080004e0009f1e0833443533383733379f101307080103a0a800010a0100000000006aec28129f2608d454f553538f555e9f360204f182027c009c01009f1a0201569a031508149f02060000000000015f2a0201569f03060000000000009f3501229f34030203009f370403b253b39f2701809f410400001527";
	char entCode[5];
	memset(entCode, 0, sizeof(entCode));
	if (strlen(ic_data) == 0)		// 有IC域,写入 05 否则按磁条卡02对待
	{
		sprintf(entCode, "02");
	}
	else
	{
		sprintf(entCode, "05");
	}
	if (strlen(pan) != 0)			// 如果有手输卡号.选择 01
	{
		sprintf(entCode, "01");
	}


	sprintf(cardInfo, "{\"entCode\":\"%s\" , \"csn\":\"%s\",\"trk2\":\"%s\",\"trk3\":\"%s\",\"pin_data\":\"%s\",\"exp_date\":\"%s\",\"pan\":\"%s\",\"ic_data\":\"%s\"}",
		entCode, csn, trk2, trk3, pin_data, exp_date, pan, ic_data);
	//printf(cardInfo);
	strcpy(sdkin.msgType, msgType);
	strcpy(sdkin.mKey, mKey);
	strcpy(sdkin.mchTransId, mchTransId);
	strcpy(sdkin.mchTransSTime, mchTransSTime);
	strcpy(sdkin.cretiId, cretiId);
	strcpy(sdkin.cardInfo, cardInfo);
	//printf(sdkin.cardInfo);
	strcpy(sdkin.orgMchTransId, orgMchTransId);
	int ret = pay(&sdkin, &sdkout);
	test_printout(&sdkout);
	return ret;
}



int test_Union_Query()
{
	HINSTANCE Hint = LoadLibrary("ipaynowSDK.dll");//加载我们刚才生成的dll
	ipaynowSDKTrade pay = (ipaynowSDKTrade)GetProcAddress(Hint, "ipaynowSDKTrade");//取得dll导出的方法

	SDKINTRADE sdkin;
	memset(&sdkin, 0, sizeof(SDKINTRADE));
	SDKOUTTRADE sdkout;
	memset(&sdkout, 0, sizeof(SDKOUTTRADE));

	sdkin.txnAmt = 1;								//金额
	char *msgType = "019";
	char *mKey = "12345678901234561234567890123456";
	//char *mchTransId = "1231231239526";
	char *mchTransSTime = "20150812150420";
	char *cretiId = "11111111";

	char mchTransId[30];
	memset(mchTransId, 0, sizeof(mchTransId));
	printf("Please input mchTransId: \n");
	scanf("%s", &mchTransId);

	//char *orgMchTransId = "1231231230006";			//原交易号
	char cardInfo[2048];

	//char *entCode = "02";
	char *csn = "";
	char *trk2 = "6228480018449666876d24122215543510000";
	char *trk3 = "";
	char *pin_data = "357478";
	char *orgMchTransId = "";
	char *exp_date = "";
	char *pan = "";
	//char *ic_data = "";
	char *ic_data = "9f3303e0e1c89505080004e0009f1e0833443533383733379f101307080103a0a800010a0100000000006aec28129f2608d454f553538f555e9f360204f182027c009c01009f1a0201569a031508149f02060000000000015f2a0201569f03060000000000009f3501229f34030203009f370403b253b39f2701809f410400001527";
	char entCode[5];
	memset(entCode, 0, sizeof(entCode));
	if (strlen(ic_data) == 0)		// 有IC域,写入 05 否则按磁条卡02对待
	{
		sprintf(entCode, "02");
	}
	else
	{
		sprintf(entCode, "05");
	}
	if (strlen(pan) != 0)			// 如果有手输卡号.选择 01
	{
		sprintf(entCode, "01");
	}
	sprintf(cardInfo, "{\"entCode\":\"%s\" , \
						\"csn\":\"%s\",		 \
						\"trk2\":\"%s\",	\
						\"trk3\":\"%s\",	\
						\"pin_data\":\"%s\",\
						\"exp_date\":\"%s\",\
						\"pan\":\"%s\",\
						\"ic_data\":\"%s\"}",
						entCode, csn, trk2, trk3, pin_data, exp_date, pan, ic_data);
	//printf(cardInfo);
	strcpy(sdkin.msgType, msgType);
	strcpy(sdkin.mKey, mKey);
	strcpy(sdkin.mchTransId, mchTransId);
	strcpy(sdkin.mchTransSTime, mchTransSTime);
	strcpy(sdkin.cretiId, cretiId);
	strcpy(sdkin.cardInfo, cardInfo);
	//printf(sdkin.cardInfo);
	strcpy(sdkin.orgMchTransId, orgMchTransId);
	int ret = pay(&sdkin, &sdkout);
	test_printout(&sdkout);
	return ret;
}

int test_Union_Pre_Pay()
{
	HINSTANCE Hint = LoadLibrary("ipaynowSDK.dll");//加载我们刚才生成的dll
	ipaynowSDKTrade pay = (ipaynowSDKTrade)GetProcAddress(Hint, "ipaynowSDKTrade");//取得dll导出的方法

	SDKINTRADE sdkin;
	memset(&sdkin, 0, sizeof(SDKINTRADE));
	SDKOUTTRADE sdkout;
	memset(&sdkout, 0, sizeof(SDKOUTTRADE));

	sdkin.txnAmt = 1;								//金额
	char *msgType = "016";
	char *mKey = "12345678901234561234567890123456";
	//char *mchTransId = "1231231239526";
	char *mchTransSTime = "20150812150420";
	char *cretiId = "11111111";

	char mchTransId[30];
	memset(mchTransId, 0, sizeof(mchTransId));
	printf("Please input mchTransId: \n");
	scanf("%s", &mchTransId);

	char *orgMchTransId = "";			//原交易号
	char cardInfo[2048];

	//char *entCode = "02";
	char *csn = "000";
	//char *trk2 = "6228480018449666876d24122215543510000";
	char *trk2 = "4392268307661786d200610111333579";
	char *trk3 = "";
	char *pin_data = "357478";
	char *exp_date = "2412";
	char *pan = "";
	char *ic_data = "";
	//char *ic_data = "9f26087db9ad03747884969f2701809f101307080103a0a000010a010000000000f90e3ab89f3704803a4a7e9f3602050c9505080004e8009a031508189c01039f02060000000000015f2a02015682027c009f1a0201569f03060000000000009f3303e0e1c89f34030203009f3501229f1e0820202020202020208408a0000003330101019f090200209f410400000363";
	char entCode[5];
	memset(entCode, 0, sizeof(entCode));
	if (strlen(ic_data) == 0)		// 有IC域,写入 05 否则按磁条卡02对待
	{
		sprintf(entCode, "02");
	}
	else
	{
		sprintf(entCode, "05");
	}
	if (strlen(pan) != 0)			// 如果有手输卡号.选择 01
	{
		sprintf(entCode, "01");
	}
	sprintf(cardInfo, "{\"entCode\":\"%s\" , \
						\"csn\":\"%s\",		 \
						\"trk2\":\"%s\",	\
						\"trk3\":\"%s\",	\
						\"pin_data\":\"%s\",\
						\"exp_date\":\"%s\",\
						\"pan\":\"%s\",\
						\"ic_data\":\"%s\"}",
						entCode, csn, trk2, trk3, pin_data, exp_date, pan, ic_data);
	//printf(cardInfo);
	strcpy(sdkin.msgType, msgType);
	strcpy(sdkin.mKey, mKey);
	strcpy(sdkin.mchTransId, mchTransId);
	strcpy(sdkin.mchTransSTime, mchTransSTime);
	strcpy(sdkin.cretiId, cretiId);
	strcpy(sdkin.cardInfo, cardInfo);
	//printf(sdkin.cardInfo);
	strcpy(sdkin.orgMchTransId, orgMchTransId);
	int ret = pay(&sdkin, &sdkout);
	test_printout(&sdkout);
	return ret;
}


int test_Union_Pre_Pay_Revoke()
{
	HINSTANCE Hint = LoadLibrary("ipaynowSDK.dll");//加载我们刚才生成的dll
	ipaynowSDKTrade pay = (ipaynowSDKTrade)GetProcAddress(Hint, "ipaynowSDKTrade");//取得dll导出的方法

	SDKINTRADE sdkin;
	memset(&sdkin, 0, sizeof(SDKINTRADE));
	SDKOUTTRADE sdkout;
	memset(&sdkout, 0, sizeof(SDKOUTTRADE));

	sdkin.txnAmt = 1;								//金额
	char *msgType = "017";
	char *mKey = "12345678901234561234567890123456";
	char *mchTransId = "1231231239526";
	char *mchTransSTime = "20150812150420";
	char *cretiId = "11111111";

	char orgMchTransId[30];
	memset(orgMchTransId, 0, sizeof(orgMchTransId));
	printf("Please input orgMchTransId: \n");
	scanf("%s", &orgMchTransId);

	//char *orgMchTransId = "1231231230006";			//原交易号
	char cardInfo[2048];

	//char *entCode = "02";
	char *csn = "";
	//char *trk2 = "6228480018449666876d24122215543510000";
	char *trk2 = "4392268307661786d200610111333579";
	char *trk3 = "";
	char *pin_data = "357478";
	char *exp_date = "";
	char *pan = "";
	char *ic_data = "";
	//char *ic_data = "9f3303e0e1c89505080004e0009f1e0833443533383733379f101307080103a0a800010a0100000000006aec28129f2608d454f553538f555e9f360204f182027c009c01009f1a0201569a031508149f02060000000000015f2a0201569f03060000000000009f3501229f34030203009f370403b253b39f2701809f410400001527";
	char entCode[5];
	memset(entCode, 0, sizeof(entCode));
	if (strlen(ic_data) == 0)		// 有IC域,写入 05 否则按磁条卡02对待
	{
		sprintf(entCode, "02");
	}
	else
	{
		sprintf(entCode, "05");
	}
	if (strlen(pan) != 0)			// 如果有手输卡号.选择 01
	{
		sprintf(entCode, "01");
	}
	sprintf(cardInfo, "{\"entCode\":\"%s\" , \
						\"csn\":\"%s\",		 \
						\"trk2\":\"%s\",	\
						\"trk3\":\"%s\",	\
						\"pin_data\":\"%s\",\
						\"exp_date\":\"%s\",\
						\"pan\":\"%s\",\
						\"ic_data\":\"%s\"}",
						entCode, csn, trk2, trk3, pin_data, exp_date, pan, ic_data);
	//printf(cardInfo);
	strcpy(sdkin.msgType, msgType);
	strcpy(sdkin.mKey, mKey);
	strcpy(sdkin.mchTransId, mchTransId);
	strcpy(sdkin.mchTransSTime, mchTransSTime);
	strcpy(sdkin.cretiId, cretiId);
	strcpy(sdkin.cardInfo, cardInfo);
	//printf(sdkin.cardInfo);
	strcpy(sdkin.orgMchTransId, orgMchTransId);
	int ret = pay(&sdkin, &sdkout);
	test_printout(&sdkout);
	return ret;
}

int test_Union_Pre_Pay_Finish()
{
	HINSTANCE Hint = LoadLibrary("ipaynowSDK.dll");//加载我们刚才生成的dll
	ipaynowSDKTrade pay = (ipaynowSDKTrade)GetProcAddress(Hint, "ipaynowSDKTrade");//取得dll导出的add方法

	SDKINTRADE sdkin;
	memset(&sdkin, 0, sizeof(SDKINTRADE));
	SDKOUTTRADE sdkout;
	memset(&sdkout, 0, sizeof(SDKOUTTRADE));

	sdkin.txnAmt = 1;								//金额
	char *msgType = "018";
	char *mKey = "12345678901234561234567890123456";
	//char *mchTransId = "1231231239526";
	char *mchTransSTime = "20150812150420";
	char *cretiId = "11111111";

	char mchTransId[30];
	memset(mchTransId, 0, sizeof(mchTransId));
	printf("Please input mchTransId: \n");
	scanf("%s", &mchTransId);

	char orgMchTransId[30];
	memset(orgMchTransId, 0, sizeof(orgMchTransId));
	printf("Please input orgMchTransId: \n");
	scanf("%s", &orgMchTransId);

	//char *orgMchTransId = "1231231230006";			//原交易号
	char cardInfo[2048];

	//char *entCode = "02";
	char *csn = "000";
	char *trk2 = "4392268307661786d200610111333579";
	//char *trk2 = "6228480018449666876d24122215543510000";
	char *trk3 = "";
	char *pin_data = "";
	char *exp_date = "";
	char *pan = "";
	char *ic_data = "";
	//char *ic_data = "9f26087db9ad03747884969f2701809f101307080103a0a000010a010000000000f90e3ab89f3704803a4a7e9f3602050c9505080004e8009a031508189c01039f02060000000000015f2a02015682027c009f1a0201569f03060000000000009f3303e0e1c89f34030203009f3501229f1e0820202020202020208408a0000003330101019f090200209f410400000363";
	char entCode[5];
	memset(entCode, 0, sizeof(entCode));
	if (strlen(ic_data) == 0)		// 有IC域,写入 05 否则按磁条卡02对待
	{
		sprintf(entCode, "02");
	}
	else
	{
		sprintf(entCode, "05");
	}
	if (strlen(pan) != 0)			// 如果有手输卡号.选择 01
	{
		sprintf(entCode, "01");
	}
	sprintf(cardInfo, "{\"entCode\":\"%s\" , \
						\"csn\":\"%s\",		 \
						\"trk2\":\"%s\",	\
						\"trk3\":\"%s\",	\
						\"pin_data\":\"%s\",\
						\"exp_date\":\"%s\",\
						\"pan\":\"%s\",\
						\"ic_data\":\"%s\"}",
						entCode, csn, trk2, trk3, pin_data, exp_date, pan, ic_data);
	//printf(cardInfo);
	strcpy(sdkin.msgType, msgType);
	strcpy(sdkin.mKey, mKey);
	strcpy(sdkin.mchTransId, mchTransId);
	strcpy(sdkin.mchTransSTime, mchTransSTime);
	strcpy(sdkin.cretiId, cretiId);
	strcpy(sdkin.cardInfo, cardInfo);
	//printf(sdkin.cardInfo);
	strcpy(sdkin.orgMchTransId, orgMchTransId);
	int ret = pay(&sdkin, &sdkout);
	test_printout(&sdkout);
	return ret;
}

int test_Union_Per_Pay_Flush()
{
	HINSTANCE Hint = LoadLibrary("ipaynowSDK.dll");//加载我们刚才生成的dll
	ipaynowSDKTrade pay = (ipaynowSDKTrade)GetProcAddress(Hint, "ipaynowSDKTrade");//取得dll导出的add方法

	SDKINTRADE sdkin;
	memset(&sdkin, 0, sizeof(SDKINTRADE));
	SDKOUTTRADE sdkout;
	memset(&sdkout, 0, sizeof(SDKOUTTRADE));

	sdkin.txnAmt = 1;								//金额
	char *msgType = "020";
	char *mKey = "12345678901234561234567890123456";
	char *mchTransId = "1231231239526";
	char *mchTransSTime = "20150812150420";
	char *cretiId = "11111111";

	char orgMchTransId[30];
	memset(orgMchTransId, 0, sizeof(orgMchTransId));
	printf("Please input orgMchTransId: \n");
	scanf("%s", &orgMchTransId);

	//char *orgMchTransId = "1231231230006";			//原交易号
	char cardInfo[2048];

	//char *entCode = "02";
	char *csn = "000";
	//char *trk2 = "6228480018449666876d24122215543510000";
	char *trk2 = "4392268307661786d200610111333579";
	char *trk3 = "";
	char *pin_data = "123";
	char *exp_date = "2412";
	char *pan = "";
	char *ic_data = "";
	//char *ic_data = "9f26087db9ad03747884969f2701809f101307080103a0a000010a010000000000f90e3ab89f3704803a4a7e9f3602050c9505080004e8009a031508189c01039f02060000000000015f2a02015682027c009f1a0201569f03060000000000009f3303e0e1c89f34030203009f3501229f1e0820202020202020208408a0000003330101019f090200209f410400000363";
	char entCode[5];
	memset(entCode, 0, sizeof(entCode));
	if (strlen(ic_data) == 0)		// 有IC域,写入 05 否则按磁条卡02对待
	{
		sprintf(entCode, "02");
	}
	else
	{
		sprintf(entCode, "05");
	}
	if (strlen(pan) != 0)			// 如果有手输卡号.选择 01
	{
		sprintf(entCode, "01");
	}
	sprintf(cardInfo, "{\"entCode\":\"%s\" , \
\"csn\":\"%s\",		 \
\"trk2\":\"%s\",	\
\"trk3\":\"%s\",	\
\"pin_data\":\"%s\",\
\"exp_date\":\"%s\",\
\"pan\":\"%s\",\
\"ic_data\":\"%s\"}",
entCode, csn, trk2, trk3, pin_data, exp_date, pan, ic_data);
	//printf(cardInfo);
	strcpy(sdkin.msgType, msgType);
	strcpy(sdkin.mKey, mKey);
	strcpy(sdkin.mchTransId, mchTransId);
	strcpy(sdkin.mchTransSTime, mchTransSTime);
	strcpy(sdkin.cretiId, cretiId);
	strcpy(sdkin.cardInfo, cardInfo);
	//printf(sdkin.cardInfo);
	strcpy(sdkin.orgMchTransId, orgMchTransId);
	int ret = pay(&sdkin, &sdkout);
	test_printout(&sdkout);
	return ret;
}
int test_Pre_Pay_Finsh_Revoke()
{
	HINSTANCE Hint = LoadLibrary("ipaynowSDK.dll");//加载我们刚才生成的dll
	ipaynowSDKTrade pay = (ipaynowSDKTrade)GetProcAddress(Hint, "ipaynowSDKTrade");//取得dll导出的add方法

	SDKINTRADE sdkin;
	memset(&sdkin, 0, sizeof(SDKINTRADE));
	SDKOUTTRADE sdkout;
	memset(&sdkout, 0, sizeof(SDKOUTTRADE));

	sdkin.txnAmt = 1;								//金额
	char *msgType = "021";
	char *mKey = "12345678901234561234567890123456";
	char *mchTransId = "1231231239526";
	char *mchTransSTime = "20150812150420";
	char *cretiId = "11111111";

	char orgMchTransId[30];
	memset(orgMchTransId, 0, sizeof(orgMchTransId));
	printf("Please input orgMchTransId: \n");
	scanf("%s", &orgMchTransId);

	//char *orgMchTransId = "1231231230006";			//原交易号
	char cardInfo[2048];

	//char *entCode = "02";
	char *csn = "";
	//char *trk2 = "6228480018449666876d24122215543510000";
	char *trk2 = "4392268307661786d200610111333579";
	char *trk3 = "";
	char *pin_data = "";
	char *exp_date = "";
	char *pan = "";
	char *ic_data = "";
	//char *ic_data = "9f3303e0e1c89505080004e0009f1e0833443533383733379f101307080103a0a800010a0100000000006aec28129f2608d454f553538f555e9f360204f182027c009c01009f1a0201569a031508149f02060000000000015f2a0201569f03060000000000009f3501229f34030203009f370403b253b39f2701809f410400001527";
	char entCode[5];
	memset(entCode, 0, sizeof(entCode));
	if (strlen(ic_data) == 0)		// 有IC域,写入 05 否则按磁条卡02对待
	{
		sprintf(entCode, "02");
	}
	else
	{
		sprintf(entCode, "05");
	}
	if (strlen(pan) != 0)			// 如果有手输卡号.选择 01
	{
		sprintf(entCode, "01");
	}
	sprintf(cardInfo, "{\"entCode\":\"%s\" , \
\"csn\":\"%s\",		 \
\"trk2\":\"%s\",	\
\"trk3\":\"%s\",	\
\"pin_data\":\"%s\",\
\"exp_date\":\"%s\",\
\"pan\":\"%s\",\
\"ic_data\":\"%s\"}",
entCode, csn, trk2, trk3, pin_data, exp_date, pan, ic_data);
	//printf(cardInfo);
	strcpy(sdkin.msgType, msgType);
	strcpy(sdkin.mKey, mKey);
	strcpy(sdkin.mchTransId, mchTransId);
	strcpy(sdkin.mchTransSTime, mchTransSTime);
	strcpy(sdkin.cretiId, cretiId);
	strcpy(sdkin.cardInfo, cardInfo);
	//printf(sdkin.cardInfo);
	strcpy(sdkin.orgMchTransId, orgMchTransId);
	int ret = pay(&sdkin, &sdkout);
	test_printout(&sdkout);
	return ret;
}
void test_printHeader()
{
	printf("========== SDK TEST  =====================\n");
	printf(" 22 : Alipay - Pay            23: Alipay - Flush \n");
	printf(" 25 : Alipay - Refund                      \n");
	printf(" 32: WX - PAY                33: WX - FLUSH     \n");
	printf(" 35: WX - REFUND                          \n");
	printf(" 12: Union - Pay             13: Union - Flush  \n");
	printf(" 14: Union - Revoke          15: Union - Refund \n");
	printf(" 16: Union - PrePay          17: Union - PrePayRevoke \n");
	printf(" 18: Union - PrePayFinish    19: Union - Query \n");
	printf(" 20: Union - PrePayFlush     21: Union - PrePayFinishRevoke \n");
	printf("Please input your choice ! enter 0 exit   \n");
}

int main()
{

	//system("pause");
	test_printHeader();
	int i = 0;
	//scanf("%d", &i);
	while (scanf("%d", &i))
	{
		if (i == 0)
		{
			break;
		}
		switch (i)
		{
		case 22:
			test_Alipay_Pay();
			break;
		case 23:
			test_Alipay_Flush();
			break;
		case 25:
			test_Alipay_Refund();
			break;
		case 32:
			test_WX_Pay();
			break;
		case 33:
			test_WX_Flush();
			break;
		case 35:
			test_WX_Refund();
			break;
		case 12:
			test_Union_Pay();
			break;
		case 13:
			test_Union_Flush();
			break;
		case 14:
			test_Union_Revoke();
			break;
		case 15:
			test_Union_Refund();
			break;
		case 16:
			test_Union_Pre_Pay();
			break;
		case 17:
			test_Union_Pre_Pay_Revoke();
			break;
		case 18:
			test_Union_Pre_Pay_Finish();
			break;
		case 19:
			test_Union_Query();
			break;
		case 20:
			test_Union_Per_Pay_Flush();
			break;
		case 21:
			test_Pre_Pay_Finsh_Revoke();
			break;
		default:
			break;
		}
		test_printHeader();
		
	}

	system("pause");
	return 0;
}



#endif 