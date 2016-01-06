#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "iso8583.h"
#include "connect.h"
#include "config.h"
#include "trans.h"
#include "innerInterface.h"
/*
int main(int argc, char *argv[])
{
	int i;
	unsigned char encrypt[] = "admin";//21232f297a57a5a743894a0e4a801fc3
	unsigned char decrypt[16];
	MD5_CTX md5;
	MD5Init(&md5);
	MD5Update(&md5, encrypt, strlen((char *)encrypt));
	MD5Final(&md5, decrypt);
	printf("加密前:%s\n加密后:", encrypt);
	for (i = 0; i<16; i++)
	{
		printf("%02x", decrypt[i]);
	}

	getchar();

	return 0;
}

*/
/*
int main()
{
	printf("ISO8583_COMBINE_LENGTH   函数测试 \n");
	int len = 123;
	char temp[5];
	memset(temp, 0, sizeof(temp));
	int bitLen = 0;
	bitLen = ISO8583_COMBINE_LENGTH(len, ISO8583_LENGTH_FIX, temp);
	printf("输出长度: %d  输出内容: %s \n", bitLen, temp);
	memset(temp, 0, sizeof(temp));
	bitLen = ISO8583_COMBINE_LENGTH(len, ISO8583_LENGTH_2_CHANGE, temp);
	printf("输出长度: %d  输出内容: %s \n", bitLen, temp);
	memset(temp, 0, sizeof(temp));
	bitLen = ISO8583_COMBINE_LENGTH(len, ISO8583_LENGTH_3_CHANGE, temp);
	printf("输出长度: %d  输出内容: %s \n", bitLen, temp);
	memset(temp, 0, sizeof(temp));
	bitLen = ISO8583_COMBINE_LENGTH(len, ISO8583_LENGTH_4_CHANGE, temp);
	printf("输出长度: %d  输出内容: %s \n", bitLen, temp);

	system("pause");
}
*/
/*
int main()
{
	char test[2048];
	memset(test, 0, sizeof(test));
	char format[100];
	memset(format, 0, sizeof(format));
	sprintf(format, "%%0%ds", 100);
	sprintf(test, "%sp", "1001");
	printf(test);

	char* input = "123     abc";
	char output[200];
	memset(output, 0, sizeof(output));
	int ret = ISO8583_Str2BcdStr(input, strlen(input), output);
	printf("转化后的长度:%d \n 转化后的信息:%s \n", ret, output);
	printf("\n\n\n\n\n\n\n8583组包实验:\n");
	PACKSOURCE ps;
	ISO_FILEDS_INIT(&ps);

	printf("加密试验:\n");
	char *test1 = "12345678";
	char *key = "12345678";
	char result[9];
	memset(result, 0, sizeof(result));
	ISO8583_DES8(ENCRYPT, test1, (char *)&result, key);
	char output1[17];
	memset(output1, 0, sizeof(output1));
	ISO8583_Str2BcdStr(result, 8, output1);
	printf("加密结果为:%s \n", output1);

	system("pause");
}
*/
/*
int main()
{
	PACKSOURCE ps;
	ISO_FILEDS_INIT(&ps);
	char result[ISO8583_MAX_DATA_LENGTH];
	memset(result, 0, sizeof(result));
	
	char test[10];
	char* test1 = "12345678";
	memcpy(test, test1, strlen(test1));
	printf("%s\n", test + 2);

	//printf("%d", TEST);
	system("pause");
}
*/
/**/
/*
int TEST_ALIPAY()
{
	cJSON *json = NULL;
	json = cJSON_CreateObject();
	cJSON *jsonCardInfo = NULL;
	jsonCardInfo = cJSON_CreateObject();
	char *code = "281223808023708483";
	cJSON_AddStringToObject(jsonCardInfo, "QRCode", code);

	cJSON_AddStringToObject(json, "msgType", "022");
	cJSON_AddItemToObject(json, "cardInfo", jsonCardInfo);
	char *key = "12345678901234561234567890123456";
	cJSON_AddStringToObject(json, "mkey", key);
	int txnAmt = 1;
	cJSON_AddNumberToObject(json, "txnAmt", txnAmt);
	char *mchTransId = "20009527";
	cJSON_AddStringToObject(json, "mchTransId", mchTransId);
	char *mchTransSTime = "20150811153940";
	cJSON_AddStringToObject(json, "mchTransSTime", mchTransSTime);
	char *cretiId = "11111111";
	cJSON_AddStringToObject(json, "cretiId", cretiId);
	char *orgMchTransId = "222222";
	cJSON_AddStringToObject(json, "orgMchTransId", orgMchTransId);

	char *in = NULL;
	in = cJSON_Print(json);
	printf(in);
	char result[MAX_LEN];
	memset(result, 0, sizeof(result));
	int ret = INNER_JSON_INTERFACE(in, result);
	printf(result);
	return ret;
}

int TEST_ALIPAY_FLUSH()
{
	cJSON *json = NULL;
	json = cJSON_CreateObject();
	cJSON *jsonCardInfo = NULL;
	jsonCardInfo = cJSON_CreateObject();
	char *code = "281033278833789420";
	cJSON_AddStringToObject(jsonCardInfo, "QRCode", code);

	cJSON_AddStringToObject(json, "msgType", "023");
	cJSON_AddItemToObject(json, "cardInfo", jsonCardInfo);
	char *key = "12345678901234561234567890123456";
	cJSON_AddStringToObject(json, "mkey", key);
	int txnAmt = 1;
	cJSON_AddNumberToObject(json, "txnAmt", txnAmt);
	char *mchTransId = "20009527";
	cJSON_AddStringToObject(json, "mchTransId", mchTransId);
	char *mchTransSTime = "20150811153940";
	cJSON_AddStringToObject(json, "mchTransSTime", mchTransSTime);
	char *cretiId = "11111111";
	cJSON_AddStringToObject(json, "cretiId", cretiId);
	char *orgMchTransId = "222222";
	cJSON_AddStringToObject(json, "orgMchTransId", orgMchTransId);

	char *in = NULL;
	in = cJSON_Print(json);
	printf(in);
	char result[MAX_LEN];
	memset(result, 0, sizeof(result));
	int ret = INNER_JSON_INTERFACE(in, result);
	printf(result);
	return ret;
}

int TEST_ALIPAY_REFUND()
{
	cJSON *json = NULL;
	json = cJSON_CreateObject();
	cJSON *jsonCardInfo = NULL;
	jsonCardInfo = cJSON_CreateObject();
	char *code = "281033278833789420";
	cJSON_AddStringToObject(jsonCardInfo, "QRCode", code);

	cJSON_AddStringToObject(json, "msgType", "025");
	cJSON_AddItemToObject(json, "cardInfo", jsonCardInfo);
	char *key = "12345678901234561234567890123456";
	cJSON_AddStringToObject(json, "mkey", key);
	int txnAmt = 1;
	cJSON_AddNumberToObject(json, "txnAmt", txnAmt);
	char *mchTransId = "20009527";
	cJSON_AddStringToObject(json, "mchTransId", mchTransId);
	char *mchTransSTime = "20150811153940";
	cJSON_AddStringToObject(json, "mchTransSTime", mchTransSTime);
	char *cretiId = "11111111";
	cJSON_AddStringToObject(json, "cretiId", cretiId);
	char *orgMchTransId = "222222";
	cJSON_AddStringToObject(json, "orgMchTransId", orgMchTransId);

	char *in = NULL;
	in = cJSON_Print(json);
	printf(in);
	char result[MAX_LEN];
	memset(result, 0, sizeof(result));
	int ret = INNER_JSON_INTERFACE(in, result);
	printf(result);
	return ret;
}

int TEST_WX_PAY()
{
	cJSON *json = NULL;
	json = cJSON_CreateObject();
	cJSON *jsonCardInfo = NULL;
	jsonCardInfo = cJSON_CreateObject();
	char *code = "281223808023708483";
	cJSON_AddStringToObject(jsonCardInfo, "QRCode", code);

	cJSON_AddStringToObject(json, "msgType", "032");
	cJSON_AddItemToObject(json, "cardInfo", jsonCardInfo);
	char *key = "12345678901234561234567890123456";
	cJSON_AddStringToObject(json, "mkey", key);
	int txnAmt = 1;
	cJSON_AddNumberToObject(json, "txnAmt", txnAmt);
	char *mchTransId = "20009527";
	cJSON_AddStringToObject(json, "mchTransId", mchTransId);
	char *mchTransSTime = "20150811153940";
	cJSON_AddStringToObject(json, "mchTransSTime", mchTransSTime);
	char *cretiId = "11111111";
	cJSON_AddStringToObject(json, "cretiId", cretiId);
	char *orgMchTransId = "222222";
	cJSON_AddStringToObject(json, "orgMchTransId", orgMchTransId);

	char *in = NULL;
	in = cJSON_Print(json);
	printf(in);
	char result[MAX_LEN];
	memset(result, 0, sizeof(result));
	int ret = INNER_JSON_INTERFACE(in, result);
	printf(result);
	return ret;
}

int TEST_WX_FLUSH()
{
	cJSON *json = NULL;
	json = cJSON_CreateObject();
	cJSON *jsonCardInfo = NULL;
	jsonCardInfo = cJSON_CreateObject();
	char *code = "281033278833789420";
	cJSON_AddStringToObject(jsonCardInfo, "QRCode", code);

	cJSON_AddStringToObject(json, "msgType", "033");
	cJSON_AddItemToObject(json, "cardInfo", jsonCardInfo);
	char *key = "12345678901234561234567890123456";
	cJSON_AddStringToObject(json, "mkey", key);
	int txnAmt = 1;
	cJSON_AddNumberToObject(json, "txnAmt", txnAmt);
	char *mchTransId = "20009527";
	cJSON_AddStringToObject(json, "mchTransId", mchTransId);
	char *mchTransSTime = "20150811153940";
	cJSON_AddStringToObject(json, "mchTransSTime", mchTransSTime);
	char *cretiId = "11111111";
	cJSON_AddStringToObject(json, "cretiId", cretiId);
	char *orgMchTransId = "222222";
	cJSON_AddStringToObject(json, "orgMchTransId", orgMchTransId);

	char *in = NULL;
	in = cJSON_Print(json);
	printf(in);
	char result[MAX_LEN];
	memset(result, 0, sizeof(result));
	int ret = INNER_JSON_INTERFACE(in, result);
	printf(result);
	return ret;
}

int TEST_WX_REFUND()
{
	cJSON *json = NULL;
	json = cJSON_CreateObject();
	cJSON *jsonCardInfo = NULL;
	jsonCardInfo = cJSON_CreateObject();
	char *code = "281033278833789420";
	cJSON_AddStringToObject(jsonCardInfo, "QRCode", code);

	cJSON_AddStringToObject(json, "msgType", "035");
	cJSON_AddItemToObject(json, "cardInfo", jsonCardInfo);
	char *key = "12345678901234561234567890123456";
	cJSON_AddStringToObject(json, "mkey", key);
	int txnAmt = 1;
	cJSON_AddNumberToObject(json, "txnAmt", txnAmt);
	char *mchTransId = "20009527";
	cJSON_AddStringToObject(json, "mchTransId", mchTransId);
	char *mchTransSTime = "20150811153940";
	cJSON_AddStringToObject(json, "mchTransSTime", mchTransSTime);
	char *cretiId = "11111111";
	cJSON_AddStringToObject(json, "cretiId", cretiId);
	char *orgMchTransId = "222222";
	cJSON_AddStringToObject(json, "orgMchTransId", orgMchTransId);

	char *in = NULL;
	in = cJSON_Print(json);
	printf(in);
	char result[MAX_LEN];
	memset(result, 0, sizeof(result));
	int ret = INNER_JSON_INTERFACE(in, result);
	printf(result);
	return ret;
}


void main()
{
	//	签到过程
	//printf("签到过程:\n");
	//TRANS_WAIT(5000);
	char *key = "12345678901234561234567890123456";
	char *source = "0200302404c030c0981100001100000000000100008349120210000";
	char *mak = "3132333435363738";
	char out[20];
	memset(out, 0, sizeof(out));
	//ISO8583_CALCMAC(source,strlen(source),out,mak);
	//TRANS_SIGNUP(key);
	TEST_ALIPAY();
	TEST_ALIPAY_FLUSH();
	system("pause");
}
*/
