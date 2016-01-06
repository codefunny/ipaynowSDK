#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cJSON.h"
#include "innerInterface.h"
#include "trans.h"
#include "config.h"
#include "log.h"
typedef struct msgTypeMap
{
	char msgChar[4];
	int	 msgInt;
}MSG_MAP;
#define TRANS_NUM 20
MSG_MAP msg_map[TRANS_NUM] =
{
	{ "012", 12 },
	{ "013", 13 },
	{ "014", 14 },
	{ "015", 15 },
	{ "016", 16 },
	{ "017", 17 },
	{ "018", 18 },
	{ "019", 19 },
	{ "020", 20 },
	{ "021", 21 },
	{ "022", 22 },
	{ "023", 23 },
	{ "025", 25 },
	{ "032", 32 },
	{ "033", 33 },
	{ "035", 35 },
	{ "042", 42 },
	{ "043", 43 },
	{ "044", 44 },
	{ "045", 45 },
};

/* �õ���������ӳ�� */
int INNER_getMsgInt(char *msgtype)
{
	int ret = 0;
	for (int i = 0; i < TRANS_NUM; i++)
	{
		if (strcmp(msg_map[i].msgChar, msgtype) == 0)
		{
			ret = msg_map[i].msgInt;
			break;
		}
	}
	return ret;
}

/* ����Ψһ�ӿ� ����JSON��ʽ�ַ���,���JSON��ʽ�ַ��� �ɹ�����0 ʧ�ܷ��ظ���
	-1 JSON ����ʧ��

*/
int INNER_JSON_INTERFACE(char const *input, char *out)
{
	char jsonSource[MAX_LEN],jsonResult[MAX_LEN];
	memset(jsonSource, 0, sizeof(jsonSource));
	memset(jsonResult, 0, sizeof(jsonResult));
	memcpy(jsonSource, input, strlen(input));
	cJSON *json = NULL;
	json = cJSON_Parse(jsonSource);
	if (!json) 
	{ 
		LOG_WriteLog("input json string can't parse correctly.");
		INNER_packRetJson(NULL, NULL, NULL, NULL, 0, NULL, "json ����ʧ��", NULL, out);
		return -1;
	}
	/* ��������json�� */
	char msgType[4];
	char mKey[33];
	int txnAmt = 0;
	char mchTransId[64];
	char mchTransSTime[24];
	char cretiId[52];
	char orgMchTransId[100];
	memset(msgType, 0, sizeof(msgType));
	memset(mKey, 0, sizeof(mKey));
	memset(mchTransId, 0, sizeof(mchTransId));
	memset(mchTransSTime, 0, sizeof(mchTransSTime));
	memset(cretiId, 0, sizeof(cretiId));
	memset(orgMchTransId, 0, sizeof(orgMchTransId));

	cJSON* jsonfind = NULL;
	/* ��ȡmsgType*/
	jsonfind = cJSON_GetObjectItem(json, "msgType");
	if (jsonfind == NULL)
	{
		memset(msgType, 0, sizeof(msgType));
		LOG_WriteLog("input json string : msgType[must exist] parse failed");
		INNER_packRetJson(NULL, NULL, NULL, NULL, 0, NULL, "msgType[must exist] parse failed", NULL, out);
		return -1;
	}
	memcpy(msgType, jsonfind->valuestring, strlen(jsonfind->valuestring));
	/* ��ȡcard info */
	cJSON *jsoncard = NULL;
	jsoncard = cJSON_GetObjectItem(json, "cardInfo");
	if (jsoncard == NULL)
	{
		LOG_WriteLog("input json string : cardInfo[must exist] parse failed");
		INNER_packRetJson(NULL, NULL, NULL, NULL, 0, NULL, "cardInfo[must exist] parse failed", NULL, out);
		return -1;
	}
	/* ��ȡ mKey*/
	jsonfind = cJSON_GetObjectItem(json, "mKey");
	if (jsonfind == NULL)
	{
		memset(mKey, 0, sizeof(mKey));
		LOG_WriteLog("input json string : mKey[must exist] parse failed");
		INNER_packRetJson(NULL, NULL, NULL, NULL, 0, NULL, "mKey[must exist] parse failed", NULL, out);
		return -1;
	}
	memcpy(mKey, jsonfind->valuestring, strlen(jsonfind->valuestring));
	/* ��ȡ txnAmt */
	jsonfind = cJSON_GetObjectItem(json, "txnAmt");
	if (jsonfind == NULL)
	{
		txnAmt = 0;
		LOG_WriteLog("input json string : txnAmt parse failed");
		INNER_packRetJson(NULL, NULL, NULL, NULL, 0, NULL, "txnAmt parse failed", NULL, out);
		return -1;
	}
	txnAmt = jsonfind->valueint;
	/* ��ȡ mchTransId �Ǳ��� */
	jsonfind = cJSON_GetObjectItem(json, "mchTransId");
	if (jsonfind != NULL)		
	{
		memcpy(mchTransId, jsonfind->valuestring, strlen(jsonfind->valuestring));
	}
	/* ��ȡ mchTransSTime ����*/
	jsonfind = cJSON_GetObjectItem(json, "mchTransSTime");
	if (jsonfind == NULL)
	{
		LOG_WriteLog("input json string : mchTransSTime[must exist] parse failed");
		INNER_packRetJson(NULL, NULL, NULL, NULL, 0, NULL, "mchTransSTime[must exist] parse failed", NULL, out);
		return -1;
	}
	memcpy(mchTransSTime, jsonfind->valuestring, strlen(jsonfind->valuestring));
	/* ��ȡ cretiId �Ǳ���*/
	jsonfind = cJSON_GetObjectItem(json, "cretiId");
	if (jsonfind != NULL)
	{
		memcpy(cretiId, jsonfind->valuestring, strlen(jsonfind->valuestring));
	}
	/* ��ȡ orgMchTransId �Ǳ���*/
	jsonfind = cJSON_GetObjectItem(json, "orgMchTransId");
	if (jsonfind != NULL)
	{
		memcpy(orgMchTransId, jsonfind->valuestring, strlen(jsonfind->valuestring));
	}
	/* �Զ�ǩ�� */
	int ret = 0;
	ret = TRANS_AUTOSIGNUP(mKey);
	if (ret != 0)
	{
		LOG_WriteLog("AUTO SIGN UP : error");
		return ret;
	}
	/*��ȡ������Ϣ����. ��ʼ����*/
	int msgTypeInt = INNER_getMsgInt(msgType);
	char log[200];
	memset(log, 0, sizeof(log));
	sprintf(log, "Start process [%d] trans.", msgTypeInt);
	LOG_WriteLog(log);
	switch (msgTypeInt)
	{
	case 12://����������	
		ret = INNER_UNION_PAY(jsoncard, mKey, txnAmt, mchTransId, mchTransSTime, cretiId, orgMchTransId, out);
		break;
	case 13://����������
		ret = INNER_UNION_FLUSH(jsoncard, mKey, txnAmt, mchTransId, mchTransSTime, cretiId, orgMchTransId, out);
		break;
	case 14://����������
		ret = INNER_UNION_REVOKE(jsoncard, mKey, txnAmt, mchTransId, mchTransSTime, cretiId, orgMchTransId, out);
		break; 
	case 15://�������˻�
		ret = INNER_UNION_REFUND(jsoncard, mKey, txnAmt, mchTransId, mchTransSTime, cretiId, orgMchTransId, out);
		break;
	case 16://������Ԥ��Ȩ
		ret = INNER_UNION_PRE_PAY(jsoncard, mKey, txnAmt, mchTransId, mchTransSTime, cretiId, orgMchTransId, out);
		break;
	case 17://������Ԥ��Ȩ����
		ret = INNER_UNION_PRE_PAY_REVOKE(jsoncard, mKey, txnAmt, mchTransId, mchTransSTime, cretiId, orgMchTransId, out);
		break;
	case 18://������Ԥ��Ȩ���
		ret = INNER_UNION_PRE_PAY_FINISH(jsoncard, mKey, txnAmt, mchTransId, mchTransSTime, cretiId, orgMchTransId, out);
		break;
	case 19://������ ����ѯ
		ret = INNER_UNION_QUERY(jsoncard, mKey, txnAmt, mchTransId, mchTransSTime, cretiId, orgMchTransId, out);
		break;
	case 20://Ԥ��Ȩ����
		ret = INNER_UNION_PRE_PAY_FLUSH(jsoncard, mKey, txnAmt, mchTransId, mchTransSTime, cretiId, orgMchTransId, out);
		break;
	case 21://Ԥ��Ȩ��ɳ���
		ret = INNER_UNION_PRE_PAY_FINSHI_REVOKE(jsoncard, mKey, txnAmt, mchTransId, mchTransSTime, cretiId, orgMchTransId, out);
		break;
	case 22://֧����֧��
		ret = INNER_ALIPAY_PAY(jsoncard, mKey, txnAmt, mchTransId, mchTransSTime, cretiId, orgMchTransId, out);
		break;
	case 23://֧��������
		ret = INNER_ALIPAY_FLUSH(jsoncard, mKey, txnAmt, mchTransId, mchTransSTime, cretiId, orgMchTransId, out);
		break;
	case 25://֧�����˻�
		ret = INNER_ALIPAY_REFUND(jsoncard, mKey, txnAmt, mchTransId, mchTransSTime, cretiId, orgMchTransId, out);
		break;
	case 32://΢��֧��
		ret = INNER_WX_PAY(jsoncard, mKey, txnAmt, mchTransId, mchTransSTime, cretiId, orgMchTransId, out);
		break;
	case 33://΢�ų���
		ret = INNER_WX_FLUSH(jsoncard, mKey, txnAmt, mchTransId, mchTransSTime, cretiId, orgMchTransId, out);
		break;
	case 35://΢���˻�
		ret = INNER_WX_REFUND(jsoncard, mKey, txnAmt, mchTransId, mchTransSTime, cretiId, orgMchTransId, out);
		break;
	case 42://�⿨����
		ret = INNER_OUTSIDE_PAY(jsoncard, mKey, txnAmt, mchTransId, mchTransSTime, cretiId, orgMchTransId, out);
		break;
	case 43://�⿨���ѳ���
		ret = INNER_OUTSIDE_FLUSH(jsoncard, mKey, txnAmt, mchTransId, mchTransSTime, cretiId, orgMchTransId, out);
		break;
	case 44://�⿨����
		ret = INNER_OUTSIDE_REVOKE(jsoncard, mKey, txnAmt, mchTransId, mchTransSTime, cretiId, orgMchTransId, out);
		break;
	case 45://�⿨�˻�
		ret = INNER_OUTSIDE_REFUND(jsoncard, mKey, txnAmt, mchTransId, mchTransSTime, cretiId, orgMchTransId, out);
		break;
	default:
		break;
	}

	cJSON_Delete(json);
	return ret;
}
int INNER_STRUCT_INTERFACE(SDKINTRADE const *in, SDKOUTTRADE *out)
{
	cJSON *root = NULL;
	root = cJSON_CreateObject();

	cJSON *cardRoot = NULL;
	cardRoot = cJSON_Parse(in->cardInfo);
	if (cardRoot == NULL)
	{
		return -1;
	}

	cJSON_AddStringToObject(root, "msgType", in->msgType);
	cJSON_AddItemToObject(root, "cardInfo", cardRoot);
	cJSON_AddStringToObject(root, "mKey", in->mKey);
	cJSON_AddNumberToObject(root, "txnAmt", in->txnAmt);
	cJSON_AddStringToObject(root, "mchTransId", in->mchTransId);
	cJSON_AddStringToObject(root, "mchTransSTime", in->mchTransSTime);
	cJSON_AddStringToObject(root, "cretiId", in->cretiId);
	cJSON_AddStringToObject(root, "orgMchTransId", in->orgMchTransId);

	char input[MAX_LEN];
	memset(input, 0, sizeof(input));
	char *out1;
	out1 = cJSON_Print(root);
	//printf(out1);
	memcpy(input, out1, strlen(out1));
	free(out1);
	
	cJSON_Delete(root);

	char output[MAX_LEN];
	memset(output, 0, sizeof(output));
	int ret = 0;

	ret = INNER_JSON_INTERFACE(input, output);

	if (ret != 0)
	{
		return ret;
	}
	/* ��ʼ����output,�������д��ṹ��*/
	cJSON *outRoot = NULL;
	outRoot = cJSON_Parse(output);
	if (outRoot == NULL)
	{
		LOG_WriteLog("out put json parse : error");
		char *resp = "out put json prase failed";
		memcpy(out->cusInfo, resp, strlen(resp));
		return -1;
	}
	cJSON *findjson = NULL;
	findjson = cJSON_GetObjectItem(outRoot, "respCode");
	memcpy(out->respCode, findjson->valuestring, strlen(findjson->valuestring));
	findjson = cJSON_GetObjectItem(outRoot, "mchTransETime");
	memcpy(out->mchTransETime, findjson->valuestring, strlen(findjson->valuestring));
	findjson = cJSON_GetObjectItem(outRoot, "transId");
	memcpy(out->transId, findjson->valuestring, strlen(findjson->valuestring));
	findjson = cJSON_GetObjectItem(outRoot, "accInfo");
	memcpy(out->accInfo, findjson->valuestring, strlen(findjson->valuestring));
	findjson = cJSON_GetObjectItem(outRoot, "txnAmt");
	out->txnAmt = findjson->valueint;
	findjson = cJSON_GetObjectItem(outRoot, "cretiId"); 
	memcpy(out->cretiId, findjson->valuestring, strlen(findjson->valuestring));
	findjson = cJSON_GetObjectItem(outRoot, "cusInfo");
	memcpy(out->cusInfo, findjson->valuestring, strlen(findjson->valuestring));
	findjson = cJSON_GetObjectItem(outRoot, "mchTransId");
	memcpy(out->mchTransId, findjson->valuestring, strlen(findjson->valuestring));
	cJSON_Delete(outRoot);

	return 0;
}
/* ֧�������� */
int INNER_ALIPAY_PAY(cJSON *cardInfo, char const * mKey, int txnAmt,
	char const *mchTransId, char const *mchTransSTime, char const *cretiId,
	char const *orgMchTransId, char *out)
{
	char code[60];
	memset(code, 0, sizeof(code));
	INNER_parseCardInfo_CODE(cardInfo, code);
	int traceint = 0, batchint = 0;																/* ��ˮ ���� ��ѯ��ʱ����Ҫ*/
	PACKSOURCE ps;
	ISO_FILEDS_INIT(&ps);
	char *msgTypePay = "0200";
	memcpy(ps.content[0].data, msgTypePay, strlen(msgTypePay));
	ps.content[0].dataLen = strlen(msgTypePay);
	ps.content[0].is_exist = 1;

	char *processCodePay = "000025";
	memcpy(ps.content[3].data, processCodePay, strlen(processCodePay));
	ps.content[3].dataLen = strlen(processCodePay);
	ps.content[3].is_exist = 1;

	sprintf(ps.content[4].data, "%012d", txnAmt);
	ps.content[4].dataLen = 12;
	ps.content[4].is_exist = 1;

	sprintf(ps.content[25].data, "13", 2);
	ps.content[25].dataLen = 2;
	ps.content[25].is_exist = 1;
	/* д��56�� */
	char temp56[600];
	memset(temp56, 0, sizeof(temp56));
	INNER_PACK56(mchTransId, cretiId, orgMchTransId, temp56);
	memcpy(ps.content[56].data, temp56, strlen(temp56));
	ps.content[56].dataLen = strlen(temp56) / 2;
	ps.content[56].is_exist = 1;
	/* д������ 60 �� */
	char batch[7];
	memset(batch, 0, sizeof(batch));
	char temp[20];
	memset(temp, 0, sizeof(temp));
	CFG_GetProfileString(CFG_BATCHNO, temp);
	batchint = atoi(temp);
	char filed60[20];
	memset(filed60, 0, sizeof(filed60));
	sprintf(filed60, "22%06d", batchint);
	sprintf(ps.content[60].data, filed60, strlen(filed60));
	ps.content[60].dataLen = strlen(filed60);
	ps.content[60].is_exist = 1;
	/* д���ά����Ϣ */
	char codeStr[60];
	memset(codeStr, 0, sizeof(codeStr));
	ISO8583_Str2BcdStr(code, strlen(code), codeStr);
	memcpy(ps.content[62].data, codeStr, strlen(codeStr));
	ps.content[62].dataLen = strlen(code);
	ps.content[62].is_exist = 1;
//retry:;
	TRANS_RET tr;
	memset(&tr, 0, sizeof(TRANS_RET));
	UNPACKRESULT ur;
	memset(&ur, 0, sizeof(UNPACKRESULT));
	int ret = TRANS_COMMON(&ps, &tr, mKey,&ur);

	/* ��֧������ֵ���д��� */
	if (strcmp(tr.respCode, "IP") != 0)														/* ֧����֧���ɹ� */
	{
		//if (strcmp(tr.respCode, "94") == 0 || strcmp(tr.respCode, "A0") == 0 || strcmp(tr.respCode, "a0") == 0 )
		//{
		//	goto retry;
		//}
		ret = INNER_packRetJson(tr.respCode,tr.mchTransETime,tr.transId,
			tr.accInfo,tr.txnAmt,tr.cretiId,tr.cusInfo,tr.mchTransId,out);
		return ret;
	}
	
	/* �ȴ��û��������� ��ѯ*/
	/* �����ˮ*/
	char transIdTemp[100];
	memset(transIdTemp, 0, sizeof(transIdTemp));
	memcpy(transIdTemp, tr.transId, strlen(tr.transId));

	traceint = atoi(ur.content[11].data);
	int alipay_query_time = 0;
	int alipay_query_total_time = 0;
	char cfgtemp[60];
	memset(cfgtemp, 0, sizeof(cfgtemp));
	CFG_GetProfileString(CFG_ALIPAY_TIME, cfgtemp);
	alipay_query_time = atoi(cfgtemp);
	memset(cfgtemp, 0, sizeof(cfgtemp));
	CFG_GetProfileString(CFG_ALIPAY_TOTAL_TIME, cfgtemp);
	alipay_query_total_time = atoi(cfgtemp);

	int totalTime = 0;
	memset(&tr, 0, sizeof(TRANS_RET));
	
	while (totalTime < alipay_query_total_time)
	{
		TRANS_WAIT(alipay_query_time);
		/* ����ѯ */
		ret = INNER_ALIPAY_PAY_QUERY(cardInfo, mKey, txnAmt, mchTransId,
			mchTransSTime, cretiId, orgMchTransId, batchint,traceint, &tr);
		/* ��ѯ����*/
		if (ret == 0)
		{
			//��ѯ����,��������.
			memset(tr.transId, 0, sizeof(tr.transId));
			memcpy(tr.transId, transIdTemp, strlen(transIdTemp));
			INNER_packRetJson(tr.respCode, tr.mchTransETime, tr.transId,
				tr.accInfo, tr.txnAmt, tr.cretiId, tr.cusInfo, tr.mchTransId, out);
			return ret;
		}

		totalTime += alipay_query_time;
	}
	//memcpy(tr.respCode, "98", strlen("98"));
	ret = INNER_packRetJson(tr.respCode, tr.mchTransETime, tr.transId,
		tr.accInfo, tr.txnAmt, tr.cretiId, tr.cusInfo, tr.mchTransId, out);
	return ret;
}

int INNER_ALIPAY_PAY_QUERY(cJSON *cardInfo, char const * mKey, int txnAmt,
	char const *mchTransId, char const *mchTransSTime, char const *cretiId,
	char const *orgMchTransId, int batchintSrc, int traceintSrc,
	TRANS_RET *tr)
{
	char code[60];
	memset(code, 0, sizeof(code));
	INNER_parseCardInfo_CODE(cardInfo, code);
	int traceint = 0, batchint = 0;																/* ��ˮ ���� ��ѯ��ʱ����Ҫ*/
	PACKSOURCE ps;
	ISO_FILEDS_INIT(&ps);
	char *msgTypePay = "0200";
	memcpy(ps.content[0].data, msgTypePay, strlen(msgTypePay));
	ps.content[0].dataLen = strlen(msgTypePay);
	ps.content[0].is_exist = 1;

	char *processCodePay = "310000";
	memcpy(ps.content[3].data, processCodePay, strlen(processCodePay));
	ps.content[3].dataLen = strlen(processCodePay);
	ps.content[3].is_exist = 1;

	sprintf(ps.content[25].data, "13", 2);
	ps.content[25].dataLen = 2;
	ps.content[25].is_exist = 1;
	/* д������ 60 �� */
	char batch[7];
	memset(batch, 0, sizeof(batch));
	char temp[20];
	memset(temp, 0, sizeof(temp));
	CFG_GetProfileString(CFG_BATCHNO, temp);
	batchint = atoi(temp);
	char filed60[20];
	memset(filed60, 0, sizeof(filed60));
	sprintf(filed60, "01%06d", batchint);
	sprintf(ps.content[60].data, filed60, strlen(filed60));
	ps.content[60].dataLen = strlen(filed60);
	ps.content[60].is_exist = 1;
	/* д��ԭ���κ���ˮ �� 61�� */
	sprintf(ps.content[61].data, "%06d%06d", batchintSrc, traceintSrc);
	ps.content[61].dataLen = strlen(ps.content[61].data);
	ps.content[61].is_exist = 1;

	//TRANS_RET tr;
	int retryTime = 0;
retry:;
	memset(tr, 0, sizeof(TRANS_RET));
	UNPACKRESULT ur;
	memset(&ur, 0, sizeof(UNPACKRESULT));
	int ret = TRANS_COMMON(&ps, tr, mKey, &ur);
	/* ��֧������ֵ���д��� */
	if (strcmp(tr->respCode, "00") == 0 || strcmp(tr->respCode,"IF") == 0)														/* ֧����֧���ɹ� */
	{
		//ret = INNER_packRetJson(tr.respCode, tr.mchTransETime, tr.transId,
		//	tr.accInfo, tr.txnAmt, tr.cretiId, tr.cusInfo, tr.mchTransId, out);
		return 0;
	}
	if (strcmp(tr->respCode, "94") == 0 || strcmp(tr->respCode, "A0") == 0 || strcmp(tr->respCode, "a0") == 0)
	{
		retryTime++;
		if (retryTime < 4)
		{
			goto retry;
		}
	}
	//INNER_packRetJson(tr.respCode, tr.mchTransETime, tr.transId,
	//	tr.accInfo, tr.txnAmt, tr.cretiId, tr.cusInfo, tr.mchTransId, out);
	return -1;
}

/* ֧�������� */
int INNER_ALIPAY_FLUSH(cJSON *cardInfo, char const * mKey, int txnAmt,
	char const *mchTransId, char const *mchTransSTime, char const *cretiId,
	char const *orgMchTransId, char *out)
{
	char code[60];
	memset(code, 0, sizeof(code));
	INNER_parseCardInfo_CODE(cardInfo, code);
	int traceint = 0, batchint = 0;																/* ��ˮ ���� ��ѯ��ʱ����Ҫ*/
	PACKSOURCE ps;
	ISO_FILEDS_INIT(&ps);
	char *msgTypePay = "0700";
	memcpy(ps.content[0].data, msgTypePay, strlen(msgTypePay));
	ps.content[0].dataLen = strlen(msgTypePay);
	ps.content[0].is_exist = 1;

	char *processCodePay = "000025";
	memcpy(ps.content[3].data, processCodePay, strlen(processCodePay));
	ps.content[3].dataLen = strlen(processCodePay);
	ps.content[3].is_exist = 1;

	sprintf(ps.content[4].data, "%012d", txnAmt);
	ps.content[4].dataLen = 12;
	ps.content[4].is_exist = 1;

	sprintf(ps.content[25].data, "13", 2);
	ps.content[25].dataLen = 2;
	ps.content[25].is_exist = 1;

	/* 39 ����ԭ�� */
	char *flushReason = "06";
	sprintf(ps.content[39].data, flushReason, strlen(flushReason));
	ps.content[39].dataLen = strlen(flushReason);
	ps.content[39].is_exist = 1;
	
	/* д��56�� */
	char temp56[600];
	memset(temp56, 0, sizeof(temp56));
	INNER_PACK56(mchTransId, cretiId, orgMchTransId, temp56);
	memcpy(ps.content[56].data, temp56, strlen(temp56));
	ps.content[56].dataLen = strlen(temp56) / 2;
	ps.content[56].is_exist = 1; 
	/* д������ 60 �� */
	char batch[7];
	memset(batch, 0, sizeof(batch));
	char temp[20];
	memset(temp, 0, sizeof(temp));
	CFG_GetProfileString(CFG_BATCHNO, temp);
	batchint = atoi(temp);
	char filed60[20];
	memset(filed60, 0, sizeof(filed60));
	sprintf(filed60, "22%06d", batchint);
	sprintf(ps.content[60].data, filed60, strlen(filed60));
	ps.content[60].dataLen = strlen(filed60);
	ps.content[60].is_exist = 1;
	int retryTime = 0;
retry:;
	TRANS_RET tr;
	memset(&tr, 0, sizeof(TRANS_RET));
	UNPACKRESULT ur;
	memset(&ur, 0, sizeof(UNPACKRESULT));
	int ret = TRANS_COMMON(&ps, &tr, mKey, &ur);
	/* ��֧������ֵ���д��� */
	if (strcmp(tr.respCode, "00") == 0 || strcmp(tr.respCode, "25"))														/* ֧����֧���ɹ� */
	{
		ret = INNER_packRetJson(tr.respCode, tr.mchTransETime, tr.transId,
			tr.accInfo, tr.txnAmt, tr.cretiId, tr.cusInfo, tr.mchTransId, out);
		return 0;
	}
	if (strcmp(tr.respCode, "94") == 0 || strcmp(tr.respCode, "A0") == 0 || strcmp(tr.respCode, "a0") == 0)
	{
		retryTime++;
		if (retryTime < 4)
		{
			goto retry;
		}
	}
	INNER_packRetJson(tr.respCode, tr.mchTransETime, tr.transId,
		tr.accInfo, tr.txnAmt, tr.cretiId, tr.cusInfo, tr.mchTransId, out);
	return 0;
}

/* ֧�����˻� */
int INNER_ALIPAY_REFUND(cJSON *cardInfo, char const * mKey, int txnAmt,
	char const *mchTransId, char const *mchTransSTime, char const *cretiId,
	char const *orgMchTransId, char *out)
{
	char code[60];
	memset(code, 0, sizeof(code));
	INNER_parseCardInfo_CODE(cardInfo, code);
	int traceint = 0, batchint = 0;																/* ��ˮ ���� ��ѯ��ʱ����Ҫ*/
	PACKSOURCE ps;
	ISO_FILEDS_INIT(&ps);
	char *msgTypePay = "0720";
	memcpy(ps.content[0].data, msgTypePay, strlen(msgTypePay));
	ps.content[0].dataLen = strlen(msgTypePay);
	ps.content[0].is_exist = 1;

	char *processCodePay = "200025";
	memcpy(ps.content[3].data, processCodePay, strlen(processCodePay));
	ps.content[3].dataLen = strlen(processCodePay);
	ps.content[3].is_exist = 1;

	sprintf(ps.content[4].data, "%012d", txnAmt);
	ps.content[4].dataLen = 12;
	ps.content[4].is_exist = 1;

	sprintf(ps.content[25].data, "13", 2);
	ps.content[25].dataLen = 2;
	ps.content[25].is_exist = 1;

	/* д��56�� */
	char temp56[600];
	memset(temp56, 0, sizeof(temp56));
	INNER_PACK56(mchTransId, cretiId, orgMchTransId, temp56);
	memcpy(ps.content[56].data, temp56, strlen(temp56));
	ps.content[56].dataLen = strlen(temp56) / 2;
	ps.content[56].is_exist = 1;
	/* д������ 60 �� */
	char batch[7];
	memset(batch, 0, sizeof(batch));
	char temp[20];
	memset(temp, 0, sizeof(temp));
	CFG_GetProfileString(CFG_BATCHNO, temp);
	batchint = atoi(temp);
	char filed60[20];
	memset(filed60, 0, sizeof(filed60));
	sprintf(filed60, "25%06d", batchint);
	sprintf(ps.content[60].data, filed60, strlen(filed60));
	ps.content[60].dataLen = strlen(filed60);
	ps.content[60].is_exist = 1;

	int retryTime = 0;
retry:;
	TRANS_RET tr;
	memset(&tr, 0, sizeof(TRANS_RET));
	UNPACKRESULT ur;
	memset(&ur, 0, sizeof(UNPACKRESULT));
	int ret = TRANS_COMMON(&ps, &tr, mKey, &ur);
	/* ��֧������ֵ���д��� */
	if (strcmp(tr.respCode, "00") == 0 || strcmp(tr.respCode, "25") == 0 || strcmp(tr.respCode,"12") == 0 )														/* ֧����֧���ɹ� */
	{
		ret = INNER_packRetJson(tr.respCode, tr.mchTransETime, tr.transId,
			tr.accInfo, tr.txnAmt, tr.cretiId, tr.cusInfo, tr.mchTransId, out);
		return 0;
	}
	if (strcmp(tr.respCode, "94") == 0 || strcmp(tr.respCode, "A0") == 0 || strcmp(tr.respCode, "a0") == 0)
	{
		retryTime++;
		if (retryTime < 4)
		{
			goto retry;
		}
	}
	INNER_packRetJson(tr.respCode, tr.mchTransETime, tr.transId,
		tr.accInfo, tr.txnAmt, tr.cretiId, tr.cusInfo, tr.mchTransId, out);
	return 0;
}


/* ΢�Ž���*/
int INNER_WX_PAY(cJSON *cardInfo, char const * mKey, int txnAmt,
	char const *mchTransId, char const *mchTransSTime, char const *cretiId,
	char const *orgMchTransId, char *out)
{
	char code[60];
	memset(code, 0, sizeof(code));
	INNER_parseCardInfo_CODE(cardInfo, code);
	int traceint = 0, batchint = 0;																/* ��ˮ ���� ��ѯ��ʱ����Ҫ*/
	PACKSOURCE ps;
	ISO_FILEDS_INIT(&ps);
	char *msgTypePay = "0200";
	memcpy(ps.content[0].data, msgTypePay, strlen(msgTypePay));
	ps.content[0].dataLen = strlen(msgTypePay);
	ps.content[0].is_exist = 1;

	char *processCodePay = "000025";
	memcpy(ps.content[3].data, processCodePay, strlen(processCodePay));
	ps.content[3].dataLen = strlen(processCodePay);
	ps.content[3].is_exist = 1;

	sprintf(ps.content[4].data, "%012d", txnAmt);
	ps.content[4].dataLen = 12;
	ps.content[4].is_exist = 1;

	sprintf(ps.content[25].data, "16", 2);
	ps.content[25].dataLen = 2;
	ps.content[25].is_exist = 1;
	/* д��56�� */
	char temp56[600];
	memset(temp56, 0, sizeof(temp56));
	INNER_PACK56(mchTransId, cretiId, orgMchTransId, temp56);
	memcpy(ps.content[56].data, temp56, strlen(temp56));
	ps.content[56].dataLen = strlen(temp56) / 2;
	ps.content[56].is_exist = 1;
	/* д������ 60 �� */
	char batch[7];
	memset(batch, 0, sizeof(batch));
	char temp[20];
	memset(temp, 0, sizeof(temp));
	CFG_GetProfileString(CFG_BATCHNO, temp);
	batchint = atoi(temp);
	char filed60[20];
	memset(filed60, 0, sizeof(filed60));
	sprintf(filed60, "22%06d", batchint);
	sprintf(ps.content[60].data, filed60, strlen(filed60));
	ps.content[60].dataLen = strlen(filed60);
	ps.content[60].is_exist = 1;
	/* д���ά����Ϣ */
	char codeStr[60];
	memset(codeStr, 0, sizeof(codeStr));
	ISO8583_Str2BcdStr(code, strlen(code), codeStr);
	memcpy(ps.content[62].data, codeStr, strlen(codeStr));
	ps.content[62].dataLen = strlen(code);
	ps.content[62].is_exist = 1;

	TRANS_RET tr;
	memset(&tr, 0, sizeof(TRANS_RET));
	UNPACKRESULT ur;
	memset(&ur, 0, sizeof(UNPACKRESULT));
//retry:;
	int ret = TRANS_COMMON(&ps, &tr, mKey, &ur);
	/* ��֧������ֵ���д��� */
	if (strcmp(tr.respCode, "IP") != 0 )														/* ΢��֧���ɹ� */
	{
		//if (strcmp(tr.respCode, "94") == 0 || strcmp(tr.respCode, "A0") == 0 || strcmp(tr.respCode, "a0") == 0 )
		//{
		//	goto retry;
		//}
		ret = INNER_packRetJson(tr.respCode, tr.mchTransETime, tr.transId,
			tr.accInfo, tr.txnAmt, tr.cretiId, tr.cusInfo, tr.mchTransId, out);
		return ret;
	}
	/* �ȴ��û��������� ��ѯ*/
	/* �����ˮ*/
	// ��ʱ���ԭ���׵�transID
	char transIdTemp[100];
	memset(transIdTemp, 0, sizeof(transIdTemp));
	memcpy(transIdTemp, tr.transId, strlen(tr.transId));
	traceint = atoi(ur.content[11].data);
	int wxquery_time = 0;
	int wxquery_total_time = 0;
	char cfgtemp[60];
	memset(cfgtemp, 0, sizeof(cfgtemp));
	CFG_GetProfileString(CFG_WXQUERY_TIME, cfgtemp);
	wxquery_time = atoi(cfgtemp);
	memset(cfgtemp, 0, sizeof(cfgtemp));
	CFG_GetProfileString(CFG_ALIPAY_TOTAL_TIME, cfgtemp);
	wxquery_total_time = atoi(cfgtemp);

	int totalTime = 0;
	memset(&tr, 0, sizeof(TRANS_RET));

	while (totalTime < wxquery_total_time)
	{
		TRANS_WAIT(wxquery_time);
		/* ����ѯ */
		ret = INNER_WX_PAY_QUERY(cardInfo, mKey, txnAmt, mchTransId,
			mchTransSTime, cretiId, orgMchTransId, batchint, traceint, &tr);
		/* ��ѯ����*/
		if (ret == 0)
		{	// ��ѯ����,֧���ɹ�
			memset(tr.transId, 0, sizeof(tr.transId));
			memcpy(tr.transId, transIdTemp, strlen(transIdTemp));
			INNER_packRetJson(tr.respCode, tr.mchTransETime, tr.transId,
				tr.accInfo, tr.txnAmt, tr.cretiId, tr.cusInfo, tr.mchTransId, out);
			return ret;
		}
		if (strcmp(tr.respCode, "I5") != 0 )
		{	// ��ѯ����� I5,�˳���ѯ
			ret = INNER_packRetJson(tr.respCode, tr.mchTransETime, tr.transId,
				tr.accInfo, tr.txnAmt, tr.cretiId, tr.cusInfo, tr.mchTransId, out);
			return ret;
		}
		totalTime += wxquery_time;
	}
	memcpy(tr.respCode, "98", strlen("98"));
	ret = INNER_packRetJson(tr.respCode, tr.mchTransETime, tr.transId,
		tr.accInfo, tr.txnAmt, tr.cretiId, tr.cusInfo, tr.mchTransId, out);
	return ret;
}


/* ΢�Ž��ײ�ѯ*/
int INNER_WX_PAY_QUERY(cJSON *cardInfo, char const * mKey, int txnAmt,
	char const *mchTransId, char const *mchTransSTime, char const *cretiId,
	char const *orgMchTransId, int batchintSrc, int traceintSrc, TRANS_RET *tr)
{
	char code[60];
	memset(code, 0, sizeof(code));
	INNER_parseCardInfo_CODE(cardInfo, code);
	int traceint = 0, batchint = 0;																/* ��ˮ ���� ��ѯ��ʱ����Ҫ*/
	PACKSOURCE ps;
	ISO_FILEDS_INIT(&ps);
	char *msgTypePay = "0200";
	memcpy(ps.content[0].data, msgTypePay, strlen(msgTypePay));
	ps.content[0].dataLen = strlen(msgTypePay);
	ps.content[0].is_exist = 1;

	char *processCodePay = "310026";
	memcpy(ps.content[3].data, processCodePay, strlen(processCodePay));
	ps.content[3].dataLen = strlen(processCodePay);
	ps.content[3].is_exist = 1;

	sprintf(ps.content[25].data, "16", 2);
	ps.content[25].dataLen = 2;
	ps.content[25].is_exist = 1;
	/* д������ 60 �� */
	char batch[7];
	memset(batch, 0, sizeof(batch));
	char temp[20];
	memset(temp, 0, sizeof(temp));
	CFG_GetProfileString(CFG_BATCHNO, temp);
	batchint = atoi(temp);
	char filed60[20];
	memset(filed60, 0, sizeof(filed60));
	sprintf(filed60, "01%06d", batchint);
	sprintf(ps.content[60].data, filed60, strlen(filed60));
	ps.content[60].dataLen = strlen(filed60);
	ps.content[60].is_exist = 1;
	/* д��ԭ���κ���ˮ �� 61�� */
	sprintf(ps.content[61].data, "%06d%06d", batchintSrc, traceintSrc);
	ps.content[61].dataLen = strlen(ps.content[61].data);
	ps.content[61].is_exist = 1;

	//TRANS_RET tr;
	memset(tr, 0, sizeof(TRANS_RET));
	UNPACKRESULT ur;
	memset(&ur, 0, sizeof(UNPACKRESULT));
	int ret = TRANS_COMMON(&ps, tr, mKey, &ur);
	/* ��֧������ֵ���д��� */
	if (strcmp(tr->respCode, "00") == 0 || strcmp(tr->respCode, "IF") == 0)														/* ֧����֧���ɹ� */
	{
		//ret = INNER_packRetJson(tr.respCode, tr.mchTransETime, tr.transId,
		//	tr.accInfo, tr.txnAmt, tr.cretiId, tr.cusInfo, tr.mchTransId, out);
		return 0;
	}
	//INNER_packRetJson(tr.respCode, tr.mchTransETime, tr.transId,
	//	tr.accInfo, tr.txnAmt, tr.cretiId, tr.cusInfo, tr.mchTransId, out);
	return -1;
}
/* ΢�ų���*/
int INNER_WX_FLUSH(cJSON *cardInfo, char const * mKey, int txnAmt,
	char const *mchTransId, char const *mchTransSTime, char const *cretiId,
	char const *orgMchTransId, char *out)
{
	char code[60];
	memset(code, 0, sizeof(code));
	INNER_parseCardInfo_CODE(cardInfo, code);
	int traceint = 0, batchint = 0;																/* ��ˮ ���� ��ѯ��ʱ����Ҫ*/
	PACKSOURCE ps;
	ISO_FILEDS_INIT(&ps);
	char *msgTypePay = "0700";
	memcpy(ps.content[0].data, msgTypePay, strlen(msgTypePay));
	ps.content[0].dataLen = strlen(msgTypePay);
	ps.content[0].is_exist = 1;

	char *processCodePay = "000025";
	memcpy(ps.content[3].data, processCodePay, strlen(processCodePay));
	ps.content[3].dataLen = strlen(processCodePay);
	ps.content[3].is_exist = 1;

	sprintf(ps.content[4].data, "%012d", txnAmt);
	ps.content[4].dataLen = 12;
	ps.content[4].is_exist = 1;

	sprintf(ps.content[25].data, "16", 2);
	ps.content[25].dataLen = 2;
	ps.content[25].is_exist = 1;

	/* 39 ����ԭ�� */
	char *flushReason = "06";
	sprintf(ps.content[39].data, flushReason, strlen(flushReason));
	ps.content[39].dataLen = strlen(flushReason);
	ps.content[39].is_exist = 1;

	/* д��56�� */
	char temp56[600];
	memset(temp56, 0, sizeof(temp56));
	INNER_PACK56(mchTransId, cretiId, orgMchTransId, temp56);
	memcpy(ps.content[56].data, temp56, strlen(temp56));
	ps.content[56].dataLen = strlen(temp56) / 2;
	ps.content[56].is_exist = 1;
	/* д������ 60 �� */
	char batch[7];
	memset(batch, 0, sizeof(batch));
	char temp[20];
	memset(temp, 0, sizeof(temp));
	CFG_GetProfileString(CFG_BATCHNO, temp);
	batchint = atoi(temp);
	char filed60[20];
	memset(filed60, 0, sizeof(filed60));
	sprintf(filed60, "22%06d", batchint);
	sprintf(ps.content[60].data, filed60, strlen(filed60));
	ps.content[60].dataLen = strlen(filed60);
	ps.content[60].is_exist = 1;

	TRANS_RET tr;
	memset(&tr, 0, sizeof(TRANS_RET));
	UNPACKRESULT ur;
	memset(&ur, 0, sizeof(UNPACKRESULT));
	int ret = TRANS_COMMON(&ps, &tr, mKey, &ur);
	/* ��֧������ֵ���д��� */
	if (strcmp(tr.respCode, "00") == 0 || strcmp(tr.respCode, "25"))														/* ֧����֧���ɹ� */
	{
		ret = INNER_packRetJson(tr.respCode, tr.mchTransETime, tr.transId,
			tr.accInfo, tr.txnAmt, tr.cretiId, tr.cusInfo, tr.mchTransId, out);
		return 0;
	}
	INNER_packRetJson(tr.respCode, tr.mchTransETime, tr.transId,
		tr.accInfo, tr.txnAmt, tr.cretiId, tr.cusInfo, tr.mchTransId, out);
	return 0;
}
/* ΢���˻�*/
int INNER_WX_REFUND(cJSON *cardInfo, char const * mKey, int txnAmt,
	char const *mchTransId, char const *mchTransSTime, char const *cretiId,
	char const *orgMchTransId, char *out)
{
	char code[60];
	memset(code, 0, sizeof(code));
	INNER_parseCardInfo_CODE(cardInfo, code);
	int traceint = 0, batchint = 0;																/* ��ˮ ���� ��ѯ��ʱ����Ҫ*/
	PACKSOURCE ps;
	ISO_FILEDS_INIT(&ps);
	char *msgTypePay = "0720";
	memcpy(ps.content[0].data, msgTypePay, strlen(msgTypePay));
	ps.content[0].dataLen = strlen(msgTypePay);
	ps.content[0].is_exist = 1;

	char *processCodePay = "200025";
	memcpy(ps.content[3].data, processCodePay, strlen(processCodePay));
	ps.content[3].dataLen = strlen(processCodePay);
	ps.content[3].is_exist = 1;

	sprintf(ps.content[4].data, "%012d", txnAmt);
	ps.content[4].dataLen = 12;
	ps.content[4].is_exist = 1;

	sprintf(ps.content[25].data, "16", 2);
	ps.content[25].dataLen = 2;
	ps.content[25].is_exist = 1;

	/* д��56�� */
	char temp56[600];
	memset(temp56, 0, sizeof(temp56));
	INNER_PACK56(mchTransId, cretiId, orgMchTransId, temp56);
	memcpy(ps.content[56].data, temp56, strlen(temp56));
	ps.content[56].dataLen = strlen(temp56) / 2;
	ps.content[56].is_exist = 1;
	/* д������ 60 �� */
	char batch[7];
	memset(batch, 0, sizeof(batch));
	char temp[20];
	memset(temp, 0, sizeof(temp));
	CFG_GetProfileString(CFG_BATCHNO, temp);
	batchint = atoi(temp);
	char filed60[20];
	memset(filed60, 0, sizeof(filed60));
	sprintf(filed60, "25%06d", batchint);
	sprintf(ps.content[60].data, filed60, strlen(filed60));
	ps.content[60].dataLen = strlen(filed60);
	ps.content[60].is_exist = 1;

	TRANS_RET tr;
	memset(&tr, 0, sizeof(TRANS_RET));
	UNPACKRESULT ur;
	memset(&ur, 0, sizeof(UNPACKRESULT));
	int ret = TRANS_COMMON(&ps, &tr, mKey, &ur);
	/* ��֧������ֵ���д��� */
	if (strcmp(tr.respCode, "00") == 0 || strcmp(tr.respCode, "25") == 0 || strcmp(tr.respCode, "12") == 0)														/* ֧����֧���ɹ� */
	{
		ret = INNER_packRetJson(tr.respCode, tr.mchTransETime, tr.transId,
			tr.accInfo, tr.txnAmt, tr.cretiId, tr.cusInfo, tr.mchTransId, out);
		return 0;
	}
	INNER_packRetJson(tr.respCode, tr.mchTransETime, tr.transId,
		tr.accInfo, tr.txnAmt, tr.cretiId, tr.cusInfo, tr.mchTransId, out);
	return 0;
}



/* ��������*/
int INNER_UNION_PAY(cJSON *cardInfo, char const * mKey, int txnAmt,
	char const *mchTransId, char const *mchTransSTime, char const *cretiId,
	char const *orgMchTransId, char *out)
{ 
	char entCode[3], csn[5], trk2[39], trk3[106], pin_data[13], ic_data[1024], exp_date[5], pan[20];
	memset(entCode, 0, sizeof(entCode));
	memset(csn, 0, sizeof(csn));
	memset(trk2, 0, sizeof(trk2));
	memset(trk3, 0, sizeof(trk3));
	memset(pin_data, 0, sizeof(pin_data));
	memset(ic_data, 0, sizeof(ic_data));
	memset(exp_date, 0, sizeof(exp_date));
	memset(pan, 0, sizeof(pan));
	INNER_parseCardInfo_CARD(cardInfo, entCode, csn, trk2, trk3, pin_data, ic_data, exp_date,pan);
	/* ��ʼ�齨 �������ױ��� */
	PACKSOURCE ps;
	ISO_FILEDS_INIT(&ps);
	char *msgTypePay = "0200";
	memcpy(ps.content[0].data, msgTypePay, strlen(msgTypePay));
	ps.content[0].dataLen = strlen(msgTypePay);
	ps.content[0].is_exist = 1;

	/* ��� PAN Ϊ�� �ɶ��Ž�ȡ PAN .���PAN��Ϊ��,��ȡPAN.ʹ��ԭPAN */
	if (strlen(pan) == 0)
	{
		/* pan Ϊ�� trk2��Ϊ��*/
		if (strlen(trk2) != 0)
		{
			/* �� ���Ž�ȡ PAN */
			char trk2temp[60];
			memset(trk2temp, 0, sizeof(trk2temp));
			memcpy(trk2temp, trk2, strlen(trk2));
			//��ʼ���ݷָ�����ȡ����. " d" ���� "D" �ָ�
			char *pantemp = NULL;
			pantemp = strtok(trk2temp, "d,D");
			if (pantemp != NULL)
			{
				memcpy(pan, pantemp, strlen(pantemp));
			}
		}
	}
	/* ȡ pan ���*/
	
	/* ����  ������Ϣ */
	if (strlen(pan) != 0)
	{
		ps.content[2].is_exist = 1;
		memcpy(ps.content[2].data, pan, strlen(pan));
		ps.content[2].dataLen = strlen(pan);
	}
	/* ���״���*/
	char *processCodePay = "000012";
	memcpy(ps.content[3].data, processCodePay, strlen(processCodePay));
	ps.content[3].dataLen = strlen(processCodePay);
	ps.content[3].is_exist = 1;
	/* ��� */
	sprintf(ps.content[4].data, "%012d", txnAmt);
	ps.content[4].dataLen = 12;
	ps.content[4].is_exist = 1;

	/* 14 �� ����Ч��*/
	if (strlen(exp_date) != 0)
	{
		sprintf(ps.content[14].data, exp_date, strlen(exp_date));
		ps.content[14].dataLen = strlen(exp_date);
		ps.content[14].is_exist = 1;
	}
	/* 22 �� ��������뷽ʽ*/
	char field22[4];
	memset(field22, 0, sizeof(field22));
	strcpy(field22, entCode);
	if (strlen(pin_data) == 0)																/* ������Ϊ�� ,ѡ�������뷽ʽ*/
	{
		strcat(field22, "2");
	}
	else
	{
		strcat(field22, "1");
	}
	sprintf(ps.content[22].data, field22);
	ps.content[22].dataLen = strlen(field22);
	ps.content[22].is_exist = 1;

	/* 23 �� �����к�*/
	if (strlen(csn) != 0)
	{
		sprintf(ps.content[23].data, csn, strlen(csn));
		ps.content[23].dataLen = strlen(csn);
		ps.content[23].is_exist = 1;
	}
	/* 25 ��*/
	sprintf(ps.content[25].data, "00", 2);
	ps.content[25].dataLen = 2;
	ps.content[25].is_exist = 1;
	/* 26�� */
	if (strlen(pan) != 0)
	{
		sprintf(ps.content[26].data, "12", 2);
		ps.content[26].dataLen = 2;
		ps.content[26].is_exist = 1;
	}
	/* 35 ���� */
	if (strlen(trk2) != 0)
	{
		sprintf(ps.content[35].data, trk2, strlen(trk2));
		ps.content[35].dataLen = strlen(trk2);
		ps.content[35].is_exist = 1;
	}
	/* 36 ���� */
	if (strlen(trk3) != 0)
	{
		sprintf(ps.content[35].data, trk3, strlen(trk3));
		ps.content[35].dataLen = strlen(trk3);
		ps.content[35].is_exist = 1;
	}

	/* 52 ������ */
	if (strlen(pin_data) != 0)
	{
		/* TODO: ��Ҫ��������� - ��common �д��� TODO*/
		sprintf(ps.content[52].data, pin_data, strlen(pin_data));
		ps.content[52].dataLen = strlen(pin_data);
		ps.content[52].is_exist = 1;
	}
	/* 53 ��ȫ������Ϣ */
	if (strlen(pin_data) != 0)
	{
		char *filed53 = "2600000000000000";
		sprintf(ps.content[53].data, filed53, strlen(filed53));
		ps.content[53].dataLen = strlen(filed53);
		ps.content[53].is_exist = 1;
	}
	/* 55 �� IC��������*/
	if (strlen(ic_data) != 0)
	{
		/* TODO: ��Ҫ��������� - ��common �д��� TODO*/
		sprintf(ps.content[55].data, ic_data, strlen(ic_data));
		ps.content[55].dataLen = strlen(ic_data)/2;
		ps.content[55].is_exist = 1;
	}
	/* д��56�� */
	char temp56[600];
	memset(temp56, 0, sizeof(temp56));
	INNER_PACK56(mchTransId, cretiId, orgMchTransId, temp56);
	memcpy(ps.content[56].data, temp56, strlen(temp56));
	ps.content[56].dataLen = strlen(temp56) / 2;
	ps.content[56].is_exist = 1;//TODO:change there 
	/* д������ 60 �� */
	char batch[7];
	memset(batch, 0, sizeof(batch));
	char temp[20];
	memset(temp, 0, sizeof(temp));
	int batchint = 0;
	CFG_GetProfileString(CFG_BATCHNO, temp);
	batchint = atoi(temp);
	char filed60[20];
	memset(filed60, 0, sizeof(filed60));
	sprintf(filed60, "22%06d000501", batchint);
	sprintf(ps.content[60].data, filed60, strlen(filed60));
	ps.content[60].dataLen = strlen(filed60);
	ps.content[60].is_exist = 1;
	int retryTime = 0;
retry:;
	TRANS_RET tr;
	memset(&tr, 0, sizeof(TRANS_RET));
	UNPACKRESULT ur;
	memset(&ur, 0, sizeof(UNPACKRESULT));
	int ret = TRANS_COMMON(&ps, &tr, mKey, &ur);
	/* ��֧������ֵ���д��� */

	if (strcmp(tr.respCode, "94") == 0 || strcmp(tr.respCode, "A0") == 0 || strcmp(tr.respCode, "a0") == 0)
	{
		retryTime++;
		if (retryTime < 4)
		{
			goto retry;
		}
	}
	ret = INNER_packRetJson(tr.respCode, tr.mchTransETime, tr.transId,
		tr.accInfo, tr.txnAmt, tr.cretiId, tr.cusInfo, tr.mchTransId, out);
	return 0;
}

/* �������ѳ���*/
int INNER_UNION_FLUSH(cJSON *cardInfo, char const * mKey, int txnAmt,
	char const *mchTransId, char const *mchTransSTime, char const *cretiId,
	char const *orgMchTransId, char *out)
{
	char entCode[3], csn[5], trk2[39], trk3[106], pin_data[13], ic_data[1024], exp_date[5], pan[20];
	memset(entCode, 0, sizeof(entCode));
	memset(csn, 0, sizeof(csn));
	memset(trk2, 0, sizeof(trk2));
	memset(trk3, 0, sizeof(trk3));
	memset(pin_data, 0, sizeof(pin_data));
	memset(ic_data, 0, sizeof(ic_data));
	memset(exp_date, 0, sizeof(exp_date));
	memset(pan, 0, sizeof(pan));
	INNER_parseCardInfo_CARD(cardInfo, entCode, csn, trk2, trk3, pin_data, ic_data, exp_date, pan);
	/* ��ʼ�齨 �������ױ��� */
	PACKSOURCE ps;
	ISO_FILEDS_INIT(&ps);
	char *msgTypePay = "0700";
	memcpy(ps.content[0].data, msgTypePay, strlen(msgTypePay));
	ps.content[0].dataLen = strlen(msgTypePay);
	ps.content[0].is_exist = 1;

	/* ��� PAN Ϊ�� �ɶ��Ž�ȡ PAN .���PAN��Ϊ��,��ȡPAN.ʹ��ԭPAN */
	if (strlen(pan) == 0)
	{
		/* pan Ϊ�� trk2��Ϊ��*/
		if (strlen(trk2) != 0)
		{
			/* �� ���Ž�ȡ PAN */
			char trk2temp[60];
			memset(trk2temp, 0, sizeof(trk2temp));
			memcpy(trk2temp, trk2, strlen(trk2));
			//��ʼ���ݷָ�����ȡ����. " d" ���� "D" �ָ�
			char *pantemp = NULL;
			pantemp = strtok(trk2temp, "d,D");
			if (pantemp != NULL)
			{
				memcpy(pan, pantemp, strlen(pantemp));
			}
		}
	}
	/* ȡ pan ���*/
	/* ����  ������Ϣ */
	if (strlen(pan) != 0)
	{
		ps.content[2].is_exist = 1;
		memcpy(ps.content[2].data, pan, strlen(pan));
		ps.content[2].dataLen = strlen(pan);
	}
	/* ���״���*/
	char *processCodePay = "000012";
	memcpy(ps.content[3].data, processCodePay, strlen(processCodePay));
	ps.content[3].dataLen = strlen(processCodePay);
	ps.content[3].is_exist = 1;
	/* ��� */
	sprintf(ps.content[4].data, "%012d", txnAmt);
	ps.content[4].dataLen = 12;
	ps.content[4].is_exist = 1;
	/* 14 �� ����Ч��*/
	if (strlen(exp_date) != 0)
	{
		sprintf(ps.content[14].data, exp_date, strlen(exp_date));
		ps.content[14].dataLen = strlen(exp_date);
		ps.content[14].is_exist = 1;
	}
	/* 22 �� ��������뷽ʽ*/
	char field22[4];
	memset(field22, 0, sizeof(field22));
	strcpy(field22, entCode);
	if (strlen(pin_data) == 0)																/* ������Ϊ�� ,ѡ�������뷽ʽ*/
	{
		strcat(field22, "2");
	}
	else
	{
		strcat(field22, "1");
	}
	sprintf(ps.content[22].data, field22);
	ps.content[22].dataLen = strlen(field22);
	ps.content[22].is_exist = 1;
	/* 23 �� �����к�*/
	if (strlen(csn) != 0)
	{
		sprintf(ps.content[23].data, csn, strlen(csn));
		ps.content[23].dataLen = strlen(csn);
		ps.content[23].is_exist = 1;
	}
	/* 25 ��*/
	sprintf(ps.content[25].data, "00", 2);
	ps.content[25].dataLen = 2;
	ps.content[25].is_exist = 1;
	/* 26�� */
	if (strlen(pan) != 0)
	{
		sprintf(ps.content[26].data, "12", 2);
		ps.content[26].dataLen = 2;
		ps.content[26].is_exist = 1;
	}
	/* 35 ���� */
	if (strlen(trk2) != 0)
	{
		sprintf(ps.content[35].data, trk2, strlen(trk2));
		ps.content[35].dataLen = strlen(trk2);
		ps.content[35].is_exist = 1;
	}
	/* 36 ���� */
	if (strlen(trk3) != 0)
	{
		sprintf(ps.content[35].data, trk3, strlen(trk3));
		ps.content[35].dataLen = strlen(trk3);
		ps.content[35].is_exist = 1;
	}
	/* 39�� ����ԭ�� д��06 */
	sprintf(ps.content[39].data, "06", 2);
	ps.content[39].dataLen = 2;
	ps.content[39].is_exist = 1;
	/* 52 ������ */
	if (strlen(pin_data) != 0)
	{
		/* TODO: ��Ҫ��������� - ��common �д��� TODO*/
		sprintf(ps.content[52].data, pin_data, strlen(pin_data));
		ps.content[52].dataLen = strlen(pin_data);
		ps.content[52].is_exist = 1;
	}
	/* 53 ��ȫ������Ϣ */
	if (strlen(pin_data) != 0)
	{
		char *filed53 = "2600000000000000";
		sprintf(ps.content[53].data, filed53, strlen(filed53));
		ps.content[53].dataLen = strlen(filed53);
		ps.content[53].is_exist = 1;
	}
	/* 55 �� IC��������*/
	if (strlen(ic_data) != 0)
	{
		/* TODO: ��Ҫ��������� - ��common �д��� TODO*/
		sprintf(ps.content[55].data, ic_data, strlen(ic_data));
		ps.content[55].dataLen = strlen(ic_data) / 2;
		ps.content[55].is_exist = 1;
	}
	/* д��56�� */
	char temp56[600];
	memset(temp56, 0, sizeof(temp56));
	INNER_PACK56(mchTransId, cretiId, orgMchTransId, temp56);
	memcpy(ps.content[56].data, temp56, strlen(temp56));
	ps.content[56].dataLen = strlen(temp56) / 2;
	ps.content[56].is_exist = 1;//TODO:change there 
	/* д������ 60 �� */
	char batch[7];
	memset(batch, 0, sizeof(batch));
	char temp[20];
	memset(temp, 0, sizeof(temp));
	int batchint = 0;
	CFG_GetProfileString(CFG_BATCHNO, temp);
	batchint = atoi(temp);
	char filed60[20];
	memset(filed60, 0, sizeof(filed60));
	sprintf(filed60, "22%06d000501", batchint);
	sprintf(ps.content[60].data, filed60, strlen(filed60));
	ps.content[60].dataLen = strlen(filed60);
	ps.content[60].is_exist = 1;

	TRANS_RET tr;
	memset(&tr, 0, sizeof(TRANS_RET));
	UNPACKRESULT ur;
	memset(&ur, 0, sizeof(UNPACKRESULT));
	int ret = TRANS_COMMON(&ps, &tr, mKey, &ur);
	/* ��֧������ֵ���д��� */
	//if (strlen(tr.respCode) == 0)
	//{
	//	memcpy(tr.respCode, "98", strlen("98"));
	//}
	ret = INNER_packRetJson(tr.respCode, tr.mchTransETime, tr.transId,
		tr.accInfo, tr.txnAmt, tr.cretiId, tr.cusInfo, tr.mchTransId, out);
	return 0;
}

/* ���������˻�*/
int INNER_UNION_REFUND(cJSON *cardInfo, char const * mKey, int txnAmt, 
	char const *mchTransId, char const *mchTransSTime, char const *cretiId, 
	char const *orgMchTransId, char *out)
{
	char entCode[3], csn[5], trk2[39], trk3[106], pin_data[13], ic_data[1024], exp_date[5], pan[20];
	memset(entCode, 0, sizeof(entCode));
	memset(csn, 0, sizeof(csn));
	memset(trk2, 0, sizeof(trk2));
	memset(trk3, 0, sizeof(trk3));
	memset(pin_data, 0, sizeof(pin_data));
	memset(ic_data, 0, sizeof(ic_data));
	memset(exp_date, 0, sizeof(exp_date));
	memset(pan, 0, sizeof(pan));
	INNER_parseCardInfo_CARD(cardInfo, entCode, csn, trk2, trk3, pin_data, ic_data, exp_date, pan);
	/* ��ʼ�齨 �������ױ��� */
	PACKSOURCE ps;
	ISO_FILEDS_INIT(&ps);
	char *msgTypePay = "0720";
	memcpy(ps.content[0].data, msgTypePay, strlen(msgTypePay));
	ps.content[0].dataLen = strlen(msgTypePay);
	ps.content[0].is_exist = 1;

	/* ��� PAN Ϊ�� �ɶ��Ž�ȡ PAN .���PAN��Ϊ��,��ȡPAN.ʹ��ԭPAN */
	if (strlen(pan) == 0)
	{
		/* pan Ϊ�� trk2��Ϊ��*/
		if (strlen(trk2) != 0)
		{
			/* �� ���Ž�ȡ PAN */
			char trk2temp[60];
			memset(trk2temp, 0, sizeof(trk2temp));
			memcpy(trk2temp, trk2, strlen(trk2));
			//��ʼ���ݷָ�����ȡ����. " d" ���� "D" �ָ�
			char *pantemp = NULL;
			pantemp = strtok(trk2temp, "d,D");
			if (pantemp != NULL)
			{
				memcpy(pan, pantemp, strlen(pantemp));
			}
		}
	}
	/* ȡ pan ���*/
	/* ����  ������Ϣ */
	if (strlen(pan) != 0)
	{
		ps.content[2].is_exist = 1;
		memcpy(ps.content[2].data, pan, strlen(pan));
		ps.content[2].dataLen = strlen(pan);
	}
	/* ���״���*/
	char *processCodePay = "200125";
	memcpy(ps.content[3].data, processCodePay, strlen(processCodePay));
	ps.content[3].dataLen = strlen(processCodePay);
	ps.content[3].is_exist = 1;
	/* ��� */
	sprintf(ps.content[4].data, "%012d", txnAmt);
	ps.content[4].dataLen = 12;
	ps.content[4].is_exist = 1;
	/* 14 �� ����Ч��*/
	if (strlen(exp_date) != 0)
	{
		sprintf(ps.content[14].data, exp_date, strlen(exp_date));
		ps.content[14].dataLen = strlen(exp_date);
		ps.content[14].is_exist = 1;
	}
	/* 22 �� ��������뷽ʽ*/
	char field22[4];
	memset(field22, 0, sizeof(field22));
	strcpy(field22, entCode);
	if (strlen(pin_data) == 0)																/* ������Ϊ�� ,ѡ�������뷽ʽ*/
	{
		strcat(field22, "2");
	}
	else
	{
		strcat(field22, "1");
	}
	sprintf(ps.content[22].data, field22);
	ps.content[22].dataLen = strlen(field22);
	ps.content[22].is_exist = 1;
	/* 23 �� �����к�*/
	if (strlen(csn) != 0)
	{
		sprintf(ps.content[23].data, csn, strlen(csn));
		ps.content[23].dataLen = strlen(csn);
		ps.content[23].is_exist = 1;
	}
	/* 25 ��*/
	sprintf(ps.content[25].data, "00", 2);
	ps.content[25].dataLen = 2;
	ps.content[25].is_exist = 1;
	/* 26�� */
	if (strlen(pan) != 0)
	{
		sprintf(ps.content[26].data, "12", 2);
		ps.content[26].dataLen = 2;
		ps.content[26].is_exist = 1;
	}
	/* 35 ���� */
	if (strlen(trk2) != 0)
	{
		sprintf(ps.content[35].data, trk2, strlen(trk2));
		ps.content[35].dataLen = strlen(trk2);
		ps.content[35].is_exist = 1;
	}
	/* 36 ���� */
	if (strlen(trk3) != 0)
	{
		sprintf(ps.content[35].data, trk3, strlen(trk3));
		ps.content[35].dataLen = strlen(trk3);
		ps.content[35].is_exist = 1;
	}

	/* 52 ������ */
	if (strlen(pin_data) != 0)
	{
		/* TODO: ��Ҫ��������� - ��common �д��� TODO*/
		sprintf(ps.content[52].data, pin_data, strlen(pin_data));
		ps.content[52].dataLen = strlen(pin_data);
		ps.content[52].is_exist = 1;
	}
	/* 53 ��ȫ������Ϣ */
	if (strlen(pin_data) != 0)
	{
		char *filed53 = "2600000000000000";
		sprintf(ps.content[53].data, filed53, strlen(filed53));
		ps.content[53].dataLen = strlen(filed53);
		ps.content[53].is_exist = 1;
	}
	/* 55 �� IC��������*/
	if (strlen(ic_data) != 0)
	{
		/* TODO: ��Ҫ��������� - ��common �д��� TODO*/
		sprintf(ps.content[55].data, ic_data, strlen(ic_data));
		ps.content[55].dataLen = strlen(ic_data) / 2;
		ps.content[55].is_exist = 1;
	}
	/* д��56�� */
	char temp56[600];
	memset(temp56, 0, sizeof(temp56));
	INNER_PACK56(mchTransId, cretiId, orgMchTransId, temp56);
	memcpy(ps.content[56].data, temp56, strlen(temp56));
	ps.content[56].dataLen = strlen(temp56) / 2;
	ps.content[56].is_exist = 1;//TODO:change there 
	/* д������ 60 �� */
	char batch[7];
	memset(batch, 0, sizeof(batch));
	char temp[20];
	memset(temp, 0, sizeof(temp));
	int batchint = 0;
	CFG_GetProfileString(CFG_BATCHNO, temp);
	batchint = atoi(temp);
	char filed60[20];
	memset(filed60, 0, sizeof(filed60));
	
	sprintf(filed60, "25%06d000501", batchint);												/* �˻� */
	sprintf(ps.content[60].data, filed60, strlen(filed60));
	ps.content[60].dataLen = strlen(filed60);
	ps.content[60].is_exist = 1;
	/* 63��  ��ȫ 00 */
	ps.content[63].dataLen = strlen("000");
	sprintf(ps.content[63].data, "000", ps.content[63].dataLen);
	ps.content[63].is_exist = 1;

	TRANS_RET tr;
	memset(&tr, 0, sizeof(TRANS_RET));
	UNPACKRESULT ur;
	memset(&ur, 0, sizeof(UNPACKRESULT));
	int ret = TRANS_COMMON(&ps, &tr, mKey, &ur);
	/* ��֧������ֵ���д��� */
	//if (strlen(tr.respCode) == 0)
	//{
	//	memcpy(tr.respCode, "98", strlen("98"));
	//}
	ret = INNER_packRetJson(tr.respCode, tr.mchTransETime, tr.transId,
		tr.accInfo, tr.txnAmt, tr.cretiId, tr.cusInfo, tr.mchTransId, out);
	return 0;
}

/* �������ѳ���*/
int INNER_UNION_REVOKE(cJSON *cardInfo, char const * mKey, int txnAmt,
	char const *mchTransId, char const *mchTransSTime, char const *cretiId,
	char const *orgMchTransId, char *out)
{
	char entCode[3], csn[5], trk2[39], trk3[106], pin_data[13], ic_data[1024], exp_date[5], pan[20];
	memset(entCode, 0, sizeof(entCode));
	memset(csn, 0, sizeof(csn));
	memset(trk2, 0, sizeof(trk2));
	memset(trk3, 0, sizeof(trk3));
	memset(pin_data, 0, sizeof(pin_data));
	memset(ic_data, 0, sizeof(ic_data));
	memset(exp_date, 0, sizeof(exp_date));
	memset(pan, 0, sizeof(pan));
	INNER_parseCardInfo_CARD(cardInfo, entCode, csn, trk2, trk3, pin_data, ic_data, exp_date, pan);
	/* ��ʼ�齨 �������ױ��� */
	PACKSOURCE ps;
	ISO_FILEDS_INIT(&ps);
	char *msgTypePay = "0700";
	memcpy(ps.content[0].data, msgTypePay, strlen(msgTypePay));
	ps.content[0].dataLen = strlen(msgTypePay);
	ps.content[0].is_exist = 1;

	/* ��� PAN Ϊ�� �ɶ��Ž�ȡ PAN .���PAN��Ϊ��,��ȡPAN.ʹ��ԭPAN */
	if (strlen(pan) == 0)
	{
		/* pan Ϊ�� trk2��Ϊ��*/
		if (strlen(trk2) != 0)
		{
			/* �� ���Ž�ȡ PAN */
			char trk2temp[60];
			memset(trk2temp, 0, sizeof(trk2temp));
			memcpy(trk2temp, trk2, strlen(trk2));
			//��ʼ���ݷָ�����ȡ����. " d" ���� "D" �ָ�
			char *pantemp = NULL;
			pantemp = strtok(trk2temp, "d,D");
			if (pantemp != NULL)
			{
				memcpy(pan, pantemp, strlen(pantemp));
			}
		}
	}
	/* ȡ pan ���*/
	/* ����  ������Ϣ */
	if (strlen(pan) != 0)
	{
		ps.content[2].is_exist = 1;
		memcpy(ps.content[2].data, pan, strlen(pan));
		ps.content[2].dataLen = strlen(pan);
	}
	/* 3 ���״���*/
	char *processCodePay = "200014";
	memcpy(ps.content[3].data, processCodePay, strlen(processCodePay));
	ps.content[3].dataLen = strlen(processCodePay);
	ps.content[3].is_exist = 1;
	/* 4 ��� */
	sprintf(ps.content[4].data, "%012d", txnAmt);
	ps.content[4].dataLen = 12;
	ps.content[4].is_exist = 1;
	/* 14 �� ����Ч��*/
	if (strlen(exp_date) != 0)
	{
		sprintf(ps.content[14].data, exp_date, strlen(exp_date));
		ps.content[14].dataLen = strlen(exp_date);
		ps.content[14].is_exist = 1;
	}
	/* 22 �� ��������뷽ʽ*/
	char field22[4];
	memset(field22, 0, sizeof(field22));
	strcpy(field22, entCode);
	if (strlen(pin_data) == 0)																/* ������Ϊ�� ,ѡ�������뷽ʽ*/
	{
		strcat(field22, "2");
	}
	else
	{
		strcat(field22, "1");
	}
	sprintf(ps.content[22].data, field22);
	ps.content[22].dataLen = strlen(field22);
	ps.content[22].is_exist = 1;
	/* 23 �� �����к�*/
	if (strlen(csn) != 0)
	{
		sprintf(ps.content[23].data, csn, strlen(csn));
		ps.content[23].dataLen = strlen(csn);
		ps.content[23].is_exist = 1;
	}
	/* 25 ��*/
	sprintf(ps.content[25].data, "00", 2);
	ps.content[25].dataLen = 2;
	ps.content[25].is_exist = 1;
	/* 26�� */
	if (strlen(pan) != 0)
	{
		sprintf(ps.content[26].data, "12", 2);
		ps.content[26].dataLen = 2;
		ps.content[26].is_exist = 1;
	}
	/* 35 ���� */
	if (strlen(trk2) != 0)
	{
		sprintf(ps.content[35].data, trk2, strlen(trk2));
		ps.content[35].dataLen = strlen(trk2);
		ps.content[35].is_exist = 1;
	}
	/* 36 ���� */
	if (strlen(trk3) != 0)
	{
		sprintf(ps.content[35].data, trk3, strlen(trk3));
		ps.content[35].dataLen = strlen(trk3);
		ps.content[35].is_exist = 1;
	}
	
	/* 52 ������ */
	if (strlen(pin_data) != 0)
	{
		/* TODO: ��Ҫ��������� - ��common �д��� TODO*/
		sprintf(ps.content[52].data, pin_data, strlen(pin_data));
		ps.content[52].dataLen = strlen(pin_data);
		ps.content[52].is_exist = 1;
	}
	/* 53 ��ȫ������Ϣ */
	if (strlen(pin_data) != 0)
	{
		char *filed53 = "2600000000000000";
		sprintf(ps.content[53].data, filed53, strlen(filed53));
		ps.content[53].dataLen = strlen(filed53);
		ps.content[53].is_exist = 1;
	}
	/* 55 �� IC��������*/
	if (strlen(ic_data) != 0)
	{
		/* TODO: ��Ҫ��������� - ��common �д��� TODO*/
		sprintf(ps.content[55].data, ic_data, strlen(ic_data));
		ps.content[55].dataLen = strlen(ic_data) / 2;
		ps.content[55].is_exist = 1;
	}
	/* д��56�� */
	char temp56[600];
	memset(temp56, 0, sizeof(temp56));
	INNER_PACK56(mchTransId, cretiId, orgMchTransId, temp56);
	memcpy(ps.content[56].data, temp56, strlen(temp56));
	ps.content[56].dataLen = strlen(temp56) / 2;
	ps.content[56].is_exist = 1;//TODO:change there 
	/* д������ 60 �� */
	char batch[7];
	memset(batch, 0, sizeof(batch));
	char temp[20];
	memset(temp, 0, sizeof(temp));
	int batchint = 0;
	CFG_GetProfileString(CFG_BATCHNO, temp);
	batchint = atoi(temp);
	char filed60[20];
	memset(filed60, 0, sizeof(filed60));

	sprintf(filed60, "23%06d000501", batchint);												/* �˻� */
	sprintf(ps.content[60].data, filed60, strlen(filed60));
	ps.content[60].dataLen = strlen(filed60);
	ps.content[60].is_exist = 1;
	/* 63��  ��ȫ 00 */
	ps.content[63].dataLen = strlen("000");
	sprintf(ps.content[63].data, "000", ps.content[63].dataLen);
	ps.content[63].is_exist = 1;

	TRANS_RET tr;
	memset(&tr, 0, sizeof(TRANS_RET));
	UNPACKRESULT ur;
	memset(&ur, 0, sizeof(UNPACKRESULT));
	int ret = TRANS_COMMON(&ps, &tr, mKey, &ur);
	/* ��֧������ֵ���д��� */
	//if (strlen(tr.respCode) == 0)
	//{
	//	memcpy(tr.respCode, "98", strlen("98"));
	//}
	ret = INNER_packRetJson(tr.respCode, tr.mchTransETime, tr.transId,
		tr.accInfo, tr.txnAmt, tr.cretiId, tr.cusInfo, tr.mchTransId, out);
	return 0;
}

/* ����������ѯ*/
int INNER_UNION_QUERY(cJSON *cardInfo, char const * mKey, int txnAmt,
	char const *mchTransId, char const *mchTransSTime, char const *cretiId,
	char const *orgMchTransId, char *out)
{
	char entCode[3], csn[5], trk2[39], trk3[106], pin_data[13], ic_data[1024], exp_date[5], pan[20];
	memset(entCode, 0, sizeof(entCode));
	memset(csn, 0, sizeof(csn));
	memset(trk2, 0, sizeof(trk2));
	memset(trk3, 0, sizeof(trk3));
	memset(pin_data, 0, sizeof(pin_data));
	memset(ic_data, 0, sizeof(ic_data));
	memset(exp_date, 0, sizeof(exp_date));
	memset(pan, 0, sizeof(pan));
	INNER_parseCardInfo_CARD(cardInfo, entCode, csn, trk2, trk3, pin_data, ic_data, exp_date, pan);
	/* ��ʼ�齨 �������ױ��� */
	PACKSOURCE ps;
	ISO_FILEDS_INIT(&ps);
	char *msgTypePay = "0200";
	memcpy(ps.content[0].data, msgTypePay, strlen(msgTypePay));
	ps.content[0].dataLen = strlen(msgTypePay);
	ps.content[0].is_exist = 1;

	/* ��� PAN Ϊ�� �ɶ��Ž�ȡ PAN .���PAN��Ϊ��,��ȡPAN.ʹ��ԭPAN */
	if (strlen(pan) == 0)
	{
		/* pan Ϊ�� trk2��Ϊ��*/
		if (strlen(trk2) != 0)
		{
			/* �� ���Ž�ȡ PAN */
			char trk2temp[60];
			memset(trk2temp, 0, sizeof(trk2temp));
			memcpy(trk2temp, trk2, strlen(trk2));
			//��ʼ���ݷָ�����ȡ����. " d" ���� "D" �ָ�
			char *pantemp = NULL;
			pantemp = strtok(trk2temp, "d,D");
			if (pantemp != NULL)
			{
				memcpy(pan, pantemp, strlen(pantemp));
			}
		}
	}
	/* ȡ pan ���*/
	/* ����  ������Ϣ */
	if (strlen(pan) != 0)
	{
		ps.content[2].is_exist = 1;
		memcpy(ps.content[2].data, pan, strlen(pan));
		ps.content[2].dataLen = strlen(pan);
	}
	/* 3 ���״���*/
	char *processCodePay = "310000";
	memcpy(ps.content[3].data, processCodePay, strlen(processCodePay));
	ps.content[3].dataLen = strlen(processCodePay);
	ps.content[3].is_exist = 1;
	/* 4 ��� */
	sprintf(ps.content[4].data, "%012d", txnAmt);
	ps.content[4].dataLen = 12;
	ps.content[4].is_exist = 1;
	/* 14 �� ����Ч��*/
	if (strlen(exp_date) != 0)
	{
		sprintf(ps.content[14].data, exp_date, strlen(exp_date));
		ps.content[14].dataLen = strlen(exp_date);
		ps.content[14].is_exist = 1;
	}
	/* 22 �� ��������뷽ʽ*/
	char field22[4];
	memset(field22, 0, sizeof(field22));
	strcpy(field22, entCode);
	if (strlen(pin_data) == 0)																/* ������Ϊ�� ,ѡ�������뷽ʽ*/
	{
		strcat(field22, "2");
	}
	else
	{
		strcat(field22, "1");
	}
	sprintf(ps.content[22].data, field22);
	ps.content[22].dataLen = strlen(field22);
	ps.content[22].is_exist = 1;
	/* 23 �� �����к�*/
	if (strlen(csn) != 0)
	{
		sprintf(ps.content[23].data, csn, strlen(csn));
		ps.content[23].dataLen = strlen(csn);
		ps.content[23].is_exist = 1;
	}
	/* 25 ��*/
	sprintf(ps.content[25].data, "00", 2);
	ps.content[25].dataLen = 2;
	ps.content[25].is_exist = 1;
	/* 26�� */
	if (strlen(pan) != 0)
	{
		sprintf(ps.content[26].data, "12", 2);
		ps.content[26].dataLen = 2;
		ps.content[26].is_exist = 1;
	}
	/* 35 ���� */
	if (strlen(trk2) != 0)
	{
		sprintf(ps.content[35].data, trk2, strlen(trk2));
		ps.content[35].dataLen = strlen(trk2);
		ps.content[35].is_exist = 1;
	}
	/* 36 ���� */
	if (strlen(trk3) != 0)
	{
		sprintf(ps.content[35].data, trk3, strlen(trk3));
		ps.content[35].dataLen = strlen(trk3);
		ps.content[35].is_exist = 1;
	}

	/* 52 ������ */
	if (strlen(pin_data) != 0)
	{
		/* TODO: ��Ҫ��������� - ��common �д��� TODO*/
		sprintf(ps.content[52].data, pin_data, strlen(pin_data));
		ps.content[52].dataLen = strlen(pin_data);
		ps.content[52].is_exist = 1;
	}
	/* 53 ��ȫ������Ϣ */
	if (strlen(pin_data) != 0)
	{
		char *filed53 = "2600000000000000";
		sprintf(ps.content[53].data, filed53, strlen(filed53));
		ps.content[53].dataLen = strlen(filed53);
		ps.content[53].is_exist = 1;
	}
	/* 55 �� IC��������*/
	if (strlen(ic_data) != 0)
	{
		/* TODO: ��Ҫ��������� - ��common �д��� TODO*/
		sprintf(ps.content[55].data, ic_data, strlen(ic_data));
		ps.content[55].dataLen = strlen(ic_data) / 2;
		ps.content[55].is_exist = 1;
	}
	/* д��56�� */
/*	char temp56[600];
	memset(temp56, 0, sizeof(temp56));
	INNER_PACK56(mchTransId, cretiId, orgMchTransId, temp56);
	memcpy(ps.content[56].data, temp56, strlen(temp56));
	ps.content[56].dataLen = strlen(temp56) / 2;
	ps.content[56].is_exist = 1;//TODO:change there   */
	/* д������ 60 �� */
	char batch[7];
	memset(batch, 0, sizeof(batch));
	char temp[20];
	memset(temp, 0, sizeof(temp));
	int batchint = 0;
	CFG_GetProfileString(CFG_BATCHNO, temp);
	batchint = atoi(temp);
	char filed60[20];
	memset(filed60, 0, sizeof(filed60));

	sprintf(filed60, "01%06d000501", batchint);												/* �˻� */
	sprintf(ps.content[60].data, filed60, strlen(filed60));
	ps.content[60].dataLen = strlen(filed60);
	ps.content[60].is_exist = 1;
	/* 63��  ��ȫ 00 */
	ps.content[63].dataLen = strlen("000");
	sprintf(ps.content[63].data, "000", ps.content[63].dataLen);
	ps.content[63].is_exist = 1;

	TRANS_RET tr;
	memset(&tr, 0, sizeof(TRANS_RET));
	UNPACKRESULT ur;
	memset(&ur, 0, sizeof(UNPACKRESULT));
	int ret = TRANS_COMMON(&ps, &tr, mKey, &ur);
	/* ��֧������ֵ���д��� */
	//if (strlen(tr.respCode) == 0)
	//{
	//	memcpy(tr.respCode, "98", strlen("98"));
	//}
	/* ��ȡ��� ��λ�� */
	char amount[20];
	memset(amount, 0, sizeof(amount));
	int amountInt = 0;
	if (ur.content[54].is_exist == 1)
	{
		memcpy(amount, ur.content[54].data + 8, 12);
	}
	amountInt = atoi(amount);
	ret = INNER_packRetJson(tr.respCode, tr.mchTransETime, tr.transId,
		tr.accInfo, amountInt, tr.cretiId, tr.cusInfo, tr.mchTransId, out);
	return 0;
}

/* ������Ԥ��Ȩ*/
int INNER_UNION_PRE_PAY(cJSON *cardInfo, char const * mKey, int txnAmt,
	char const *mchTransId, char const *mchTransSTime, char const *cretiId,
	char const *orgMchTransId, char *out)
{
	char entCode[3], csn[5], trk2[39], trk3[106], pin_data[13], ic_data[1024], exp_date[5], pan[20];
	memset(entCode, 0, sizeof(entCode));
	memset(csn, 0, sizeof(csn));
	memset(trk2, 0, sizeof(trk2));
	memset(trk3, 0, sizeof(trk3));
	memset(pin_data, 0, sizeof(pin_data));
	memset(ic_data, 0, sizeof(ic_data));
	memset(exp_date, 0, sizeof(exp_date));
	memset(pan, 0, sizeof(pan));
	INNER_parseCardInfo_CARD(cardInfo, entCode, csn, trk2, trk3, pin_data, ic_data, exp_date, pan);
	/* ��ʼ�齨 �������ױ��� */
	PACKSOURCE ps;
	ISO_FILEDS_INIT(&ps);
	char *msgTypePay = "0100";
	memcpy(ps.content[0].data, msgTypePay, strlen(msgTypePay));
	ps.content[0].dataLen = strlen(msgTypePay);
	ps.content[0].is_exist = 1;

	/* ��� PAN Ϊ�� �ɶ��Ž�ȡ PAN .���PAN��Ϊ��,��ȡPAN.ʹ��ԭPAN */
	if (strlen(pan) == 0)
	{
		/* pan Ϊ�� trk2��Ϊ��*/
		if (strlen(trk2) != 0)
		{
			/* �� ���Ž�ȡ PAN */
			char trk2temp[60];
			memset(trk2temp, 0, sizeof(trk2temp));
			memcpy(trk2temp, trk2, strlen(trk2));
			//��ʼ���ݷָ�����ȡ����. " d" ���� "D" �ָ�
			char *pantemp = NULL;
			pantemp = strtok(trk2temp, "d,D");
			if (pantemp != NULL)
			{
				memcpy(pan, pantemp, strlen(pantemp));
			}
		}
	}
	/* ȡ pan ���*/
	/* ����  ������Ϣ */
	if (strlen(pan) != 0)
	{
		ps.content[2].is_exist = 1;
		memcpy(ps.content[2].data, pan, strlen(pan));
		ps.content[2].dataLen = strlen(pan);
	}
	/* 3 ���״���*/
	char *processCodePay = "030016";
	memcpy(ps.content[3].data, processCodePay, strlen(processCodePay));
	ps.content[3].dataLen = strlen(processCodePay);
	ps.content[3].is_exist = 1;
	/* 4 ��� */
	sprintf(ps.content[4].data, "%012d", txnAmt);
	ps.content[4].dataLen = 12;
	ps.content[4].is_exist = 1;
	/* 14 �� ����Ч��*/
	if (strlen(exp_date) != 0)
	{
		sprintf(ps.content[14].data, exp_date, strlen(exp_date));
		ps.content[14].dataLen = strlen(exp_date);
		ps.content[14].is_exist = 1;
	}
	/* 22 �� ��������뷽ʽ*/
	char field22[4];
	memset(field22, 0, sizeof(field22));
	strcpy(field22, entCode);
	if (strlen(pin_data) == 0)																/* ������Ϊ�� ,ѡ�������뷽ʽ*/
	{
		strcat(field22, "2");
	}
	else
	{
		strcat(field22, "1");
	}
	sprintf(ps.content[22].data, field22);
	ps.content[22].dataLen = strlen(field22);
	ps.content[22].is_exist = 1;
	/* 23 �� �����к�*/
	if (strlen(csn) != 0)
	{
		sprintf(ps.content[23].data, "%04s", csn);
		ps.content[23].dataLen = strlen(csn);
		ps.content[23].is_exist = 1;
	}
	/* 25 ��*/
	sprintf(ps.content[25].data, "06", 2);
	ps.content[25].dataLen = 2;
	ps.content[25].is_exist = 1;
	/* 26�� */
	if (strlen(pan) != 0)
	{
		sprintf(ps.content[26].data, "12", 2);
		ps.content[26].dataLen = 2;
		ps.content[26].is_exist = 1;
	}
	/* 35 ���� */
	if (strlen(trk2) != 0)
	{
		sprintf(ps.content[35].data, trk2, strlen(trk2));
		ps.content[35].dataLen = strlen(trk2);
		ps.content[35].is_exist = 1;
	}
	/* 36 ���� */
	if (strlen(trk3) != 0)
	{
		sprintf(ps.content[35].data, trk3, strlen(trk3));
		ps.content[35].dataLen = strlen(trk3);
		ps.content[35].is_exist = 1;
	}

	/* 52 ������ */
	if (strlen(pin_data) != 0)
	{
		/* TODO: ��Ҫ��������� - ��common �д��� TODO*/
		sprintf(ps.content[52].data, pin_data, strlen(pin_data));
		ps.content[52].dataLen = strlen(pin_data);
		ps.content[52].is_exist = 1;
	}
	/* 53 ��ȫ������Ϣ */
	if (strlen(pin_data) != 0)
	{
		char *filed53 = "2600000000000000";
		sprintf(ps.content[53].data, filed53, strlen(filed53));
		ps.content[53].dataLen = strlen(filed53);
		ps.content[53].is_exist = 1;
	}
	/* 55 �� IC��������*/
	if (strlen(ic_data) != 0)
	{
		/* TODO: ��Ҫ��������� - ��common �д��� TODO*/
		sprintf(ps.content[55].data, ic_data, strlen(ic_data));
		ps.content[55].dataLen = strlen(ic_data) / 2;
		ps.content[55].is_exist = 1;
	}
	/* д��56�� */
	char temp56[600];
	memset(temp56, 0, sizeof(temp56));
	INNER_PACK56(mchTransId, cretiId, orgMchTransId, temp56);
	memcpy(ps.content[56].data, temp56, strlen(temp56));
	ps.content[56].dataLen = strlen(temp56) / 2;
	ps.content[56].is_exist = 1;//TODO:change there 
	/* д������ 60 �� */
	char batch[7];
	memset(batch, 0, sizeof(batch));
	char temp[20];
	memset(temp, 0, sizeof(temp));
	int batchint = 0;
	CFG_GetProfileString(CFG_BATCHNO, temp);
	batchint = atoi(temp);
	char filed60[20];
	memset(filed60, 0, sizeof(filed60));

	sprintf(filed60, "10%06d000501", batchint);												/* Ԥ��Ȩ */
	sprintf(ps.content[60].data, filed60, strlen(filed60));
	ps.content[60].dataLen = strlen(filed60);
	ps.content[60].is_exist = 1;
	/* 63��  ��ȫ 00 */
	ps.content[63].dataLen = strlen("000");
	sprintf(ps.content[63].data, "000", ps.content[63].dataLen);
	ps.content[63].is_exist = 1;

	TRANS_RET tr;
	memset(&tr, 0, sizeof(TRANS_RET));
	UNPACKRESULT ur;
	memset(&ur, 0, sizeof(UNPACKRESULT));
	int ret = TRANS_COMMON(&ps, &tr, mKey, &ur);
	/* ��֧������ֵ���д��� */
	//if (strlen(tr.respCode) == 0)
	//{
	//	memcpy(tr.respCode, "98", strlen("98"));
	//}
	ret = INNER_packRetJson(tr.respCode, tr.mchTransETime, tr.transId,
		tr.accInfo, tr.txnAmt, tr.cretiId, tr.cusInfo, tr.mchTransId, out);
	return 0;
}

/* ������Ԥ��Ȩ����*/
int INNER_UNION_PRE_PAY_FLUSH(cJSON *cardInfo, char const * mKey, int txnAmt,
	char const *mchTransId, char const *mchTransSTime, char const *cretiId,
	char const *orgMchTransId, char *out)
{
	char entCode[3], csn[5], trk2[39], trk3[106], pin_data[13], ic_data[1024], exp_date[5], pan[20];
	memset(entCode, 0, sizeof(entCode));
	memset(csn, 0, sizeof(csn));
	memset(trk2, 0, sizeof(trk2));
	memset(trk3, 0, sizeof(trk3));
	memset(pin_data, 0, sizeof(pin_data));
	memset(ic_data, 0, sizeof(ic_data));
	memset(exp_date, 0, sizeof(exp_date));
	memset(pan, 0, sizeof(pan));
	INNER_parseCardInfo_CARD(cardInfo, entCode, csn, trk2, trk3, pin_data, ic_data, exp_date, pan);
	/* ��ʼ�齨 �������ױ��� */
	PACKSOURCE ps;
	ISO_FILEDS_INIT(&ps);
	char *msgTypePay = "0700";
	memcpy(ps.content[0].data, msgTypePay, strlen(msgTypePay));
	ps.content[0].dataLen = strlen(msgTypePay);
	ps.content[0].is_exist = 1;

	/* ��� PAN Ϊ�� �ɶ��Ž�ȡ PAN .���PAN��Ϊ��,��ȡPAN.ʹ��ԭPAN */
	if (strlen(pan) == 0)
	{
		/* pan Ϊ�� trk2��Ϊ��*/
		if (strlen(trk2) != 0)
		{
			/* �� ���Ž�ȡ PAN */
			char trk2temp[60];
			memset(trk2temp, 0, sizeof(trk2temp));
			memcpy(trk2temp, trk2, strlen(trk2));
			//��ʼ���ݷָ�����ȡ����. " d" ���� "D" �ָ�
			char *pantemp = NULL;
			pantemp = strtok(trk2temp, "d,D");
			if (pantemp != NULL)
			{
				memcpy(pan, pantemp, strlen(pantemp));
			}
		}
	}
	/* ȡ pan ���*/
	/* ����  ������Ϣ */
	if (strlen(pan) != 0)
	{
		ps.content[2].is_exist = 1;
		memcpy(ps.content[2].data, pan, strlen(pan));
		ps.content[2].dataLen = strlen(pan);
	}
	/* ���״���*/
	char *processCodePay = "030016";
	memcpy(ps.content[3].data, processCodePay, strlen(processCodePay));
	ps.content[3].dataLen = strlen(processCodePay);
	ps.content[3].is_exist = 1;
	/* ��� */
	sprintf(ps.content[4].data, "%012d", txnAmt);
	ps.content[4].dataLen = 12;
	ps.content[4].is_exist = 1;
	/* 14 �� ����Ч��*/
	if (strlen(exp_date) != 0)
	{
		sprintf(ps.content[14].data, exp_date, strlen(exp_date));
		ps.content[14].dataLen = strlen(exp_date);
		ps.content[14].is_exist = 1;
	}
	/* 22 �� ��������뷽ʽ*/
	char field22[4];
	memset(field22, 0, sizeof(field22));
	strcpy(field22, entCode);
	if (strlen(pin_data) == 0)																/* ������Ϊ�� ,ѡ�������뷽ʽ*/
	{
		strcat(field22, "2");
	}
	else
	{
		strcat(field22, "1");
	}
	sprintf(ps.content[22].data, field22);
	ps.content[22].dataLen = strlen(field22);
	ps.content[22].is_exist = 1;
	/* 23 �� �����к�*/
	if (strlen(csn) != 0)
	{
		sprintf(ps.content[23].data, "%04s", csn);
		ps.content[23].dataLen = strlen(csn);
		ps.content[23].is_exist = 1;
	}
	/* 25 ��*/
	sprintf(ps.content[25].data, "06", 2);
	ps.content[25].dataLen = 2;
	ps.content[25].is_exist = 1;
	/* 26�� */
	if (strlen(pan) != 0)
	{
		sprintf(ps.content[26].data, "12", 2);
		ps.content[26].dataLen = 2;
		ps.content[26].is_exist = 1;
	}
	/* 35 ���� */
	if (strlen(trk2) != 0)
	{
		sprintf(ps.content[35].data, trk2, strlen(trk2));
		ps.content[35].dataLen = strlen(trk2);
		ps.content[35].is_exist = 1;
	}
	/* 36 ���� */
	if (strlen(trk3) != 0)
	{
		sprintf(ps.content[35].data, trk3, strlen(trk3));
		ps.content[35].dataLen = strlen(trk3);
		ps.content[35].is_exist = 1;
	}
	/* 39�� ����ԭ�� д��06 */
	sprintf(ps.content[39].data, "98", 2);
	ps.content[39].dataLen = 2;
	ps.content[39].is_exist = 1;
	/* 52 ������ */
	//if (strlen(pin_data) != 0)
	//{
	//	/* TODO: ��Ҫ��������� - ��common �д��� TODO*/
	//	sprintf(ps.content[52].data, pin_data, strlen(pin_data));
	//	ps.content[52].dataLen = strlen(pin_data);
	//	ps.content[52].is_exist = 1;
	//}
	/* 53 ��ȫ������Ϣ */
	//if (strlen(pin_data) != 0)
	//{
	//	char *filed53 = "2600000000000000";
	//	sprintf(ps.content[53].data, filed53, strlen(filed53));
	//	ps.content[53].dataLen = strlen(filed53);
	//	ps.content[53].is_exist = 1;
	//}
	/* 55 �� IC��������*/
	if (strlen(ic_data) != 0)
	{
		/* TODO: ��Ҫ��������� - ��common �д��� TODO*/
		sprintf(ps.content[55].data, ic_data, strlen(ic_data));
		ps.content[55].dataLen = strlen(ic_data) / 2;
		ps.content[55].is_exist = 1;
	}
	/* д��56�� */
	char temp56[600];
	memset(temp56, 0, sizeof(temp56));
	INNER_PACK56(mchTransId, cretiId, orgMchTransId, temp56);
	memcpy(ps.content[56].data, temp56, strlen(temp56));
	ps.content[56].dataLen = strlen(temp56) / 2;
	ps.content[56].is_exist = 1;//TODO:change there 
	/* д������ 60 �� */
	char batch[7];
	memset(batch, 0, sizeof(batch));
	char temp[20];
	memset(temp, 0, sizeof(temp));
	int batchint = 0;
	CFG_GetProfileString(CFG_BATCHNO, temp);
	batchint = atoi(temp);
	char filed60[20];
	memset(filed60, 0, sizeof(filed60));
	sprintf(filed60, "10%06d00050", batchint);
	sprintf(ps.content[60].data, filed60, strlen(filed60));
	ps.content[60].dataLen = strlen(filed60);
	ps.content[60].is_exist = 1;

	TRANS_RET tr;
	memset(&tr, 0, sizeof(TRANS_RET));
	UNPACKRESULT ur;
	memset(&ur, 0, sizeof(UNPACKRESULT));
	int ret = TRANS_COMMON(&ps, &tr, mKey, &ur);
	/* ��֧������ֵ���д��� */
	//if (strlen(tr.respCode) == 0)
	//{
	//	memcpy(tr.respCode, "98", strlen("98"));
	//}
	ret = INNER_packRetJson(tr.respCode, tr.mchTransETime, tr.transId,
		tr.accInfo, tr.txnAmt, tr.cretiId, tr.cusInfo, tr.mchTransId, out);
	return 0;
}
/* ������Ԥ��Ȩ����*/
int INNER_UNION_PRE_PAY_REVOKE(cJSON *cardInfo, char const * mKey, int txnAmt,
	char const *mchTransId, char const *mchTransSTime, char const *cretiId,
	char const *orgMchTransId, char *out)
{
	char entCode[3], csn[5], trk2[39], trk3[106], pin_data[13], ic_data[1024], exp_date[5], pan[20];
	memset(entCode, 0, sizeof(entCode));
	memset(csn, 0, sizeof(csn));
	memset(trk2, 0, sizeof(trk2));
	memset(trk3, 0, sizeof(trk3));
	memset(pin_data, 0, sizeof(pin_data));
	memset(ic_data, 0, sizeof(ic_data));
	memset(exp_date, 0, sizeof(exp_date));
	memset(pan, 0, sizeof(pan));
	INNER_parseCardInfo_CARD(cardInfo, entCode, csn, trk2, trk3, pin_data, ic_data, exp_date, pan);
	/* ��ʼ�齨 �������ױ��� */
	PACKSOURCE ps;
	ISO_FILEDS_INIT(&ps);
	char *msgTypePay = "0700";
	memcpy(ps.content[0].data, msgTypePay, strlen(msgTypePay));
	ps.content[0].dataLen = strlen(msgTypePay);
	ps.content[0].is_exist = 1;

	/* ��� PAN Ϊ�� �ɶ��Ž�ȡ PAN .���PAN��Ϊ��,��ȡPAN.ʹ��ԭPAN */
	if (strlen(pan) == 0)
	{
		/* pan Ϊ�� trk2��Ϊ��*/
		if (strlen(trk2) != 0)
		{
			/* �� ���Ž�ȡ PAN */
			char trk2temp[60];
			memset(trk2temp, 0, sizeof(trk2temp));
			memcpy(trk2temp, trk2, strlen(trk2));
			//��ʼ���ݷָ�����ȡ����. " d" ���� "D" �ָ�
			char *pantemp = NULL;
			pantemp = strtok(trk2temp, "d,D");
			if (pantemp != NULL)
			{
				memcpy(pan, pantemp, strlen(pantemp));
			}
		}
	}
	/* ȡ pan ���*/
	/* ����  ������Ϣ */
	if (strlen(pan) != 0)
	{
		ps.content[2].is_exist = 1;
		memcpy(ps.content[2].data, pan, strlen(pan));
		ps.content[2].dataLen = strlen(pan);
	}
	/* 3 ���״���*/
	char *processCodePay = "200017";
	memcpy(ps.content[3].data, processCodePay, strlen(processCodePay));
	ps.content[3].dataLen = strlen(processCodePay);
	ps.content[3].is_exist = 1;
	/* 4 ��� */
	sprintf(ps.content[4].data, "%012d", txnAmt);
	ps.content[4].dataLen = 12;
	ps.content[4].is_exist = 1;
	/* 14 �� ����Ч��*/
	if (strlen(exp_date) != 0)
	{
		sprintf(ps.content[14].data, exp_date, strlen(exp_date));
		ps.content[14].dataLen = strlen(exp_date);
		ps.content[14].is_exist = 1;
	}
	/* 22 �� ��������뷽ʽ*/
	char field22[4];
	memset(field22, 0, sizeof(field22));
	strcpy(field22, entCode);
	if (strlen(pin_data) == 0)																/* ������Ϊ�� ,ѡ�������뷽ʽ*/
	{
		strcat(field22, "2");
	}
	else
	{
		strcat(field22, "1");
	}
	sprintf(ps.content[22].data, field22);
	ps.content[22].dataLen = strlen(field22);
	ps.content[22].is_exist = 1;
	/* 23 �� �����к�*/
	if (strlen(csn) != 0)
	{
		sprintf(ps.content[23].data, csn, strlen(csn));
		ps.content[23].dataLen = strlen(csn);
		ps.content[23].is_exist = 1;
	}
	/* 25 ��*/
	sprintf(ps.content[25].data, "06", 2);
	ps.content[25].dataLen = 2;
	ps.content[25].is_exist = 1;
	/* 26�� */
	if (strlen(pan) != 0)
	{
		sprintf(ps.content[26].data, "12", 2);
		ps.content[26].dataLen = 2;
		ps.content[26].is_exist = 1;
	}
	/* 35 ���� */
	if (strlen(trk2) != 0)
	{
		sprintf(ps.content[35].data, trk2, strlen(trk2));
		ps.content[35].dataLen = strlen(trk2);
		ps.content[35].is_exist = 1;
	}
	/* 36 ���� */
	if (strlen(trk3) != 0)
	{
		sprintf(ps.content[35].data, trk3, strlen(trk3));
		ps.content[35].dataLen = strlen(trk3);
		ps.content[35].is_exist = 1;
	}

	/* 52 ������ */
	if (strlen(pin_data) != 0)
	{
		/* TODO: ��Ҫ��������� - ��common �д��� TODO*/
		sprintf(ps.content[52].data, pin_data, strlen(pin_data));
		ps.content[52].dataLen = strlen(pin_data);
		ps.content[52].is_exist = 1;
	}
	/* 53 ��ȫ������Ϣ */
	if (strlen(pin_data) != 0)
	{
		char *filed53 = "2600000000000000";
		sprintf(ps.content[53].data, filed53, strlen(filed53));
		ps.content[53].dataLen = strlen(filed53);
		ps.content[53].is_exist = 1;
	}
	/* 55 �� IC��������*/
	if (strlen(ic_data) != 0)
	{
		/* TODO: ��Ҫ��������� - ��common �д��� TODO*/
		sprintf(ps.content[55].data, ic_data, strlen(ic_data));
		ps.content[55].dataLen = strlen(ic_data) / 2;
		ps.content[55].is_exist = 1;
	}
	/* д��56�� */
	char temp56[600];
	memset(temp56, 0, sizeof(temp56));
	INNER_PACK56(mchTransId, cretiId, orgMchTransId, temp56);
	memcpy(ps.content[56].data, temp56, strlen(temp56));
	ps.content[56].dataLen = strlen(temp56) / 2;
	ps.content[56].is_exist = 1;//TODO:change there 
	/* д������ 60 �� */
	char batch[7];
	memset(batch, 0, sizeof(batch));
	char temp[20];
	memset(temp, 0, sizeof(temp));
	int batchint = 0;
	CFG_GetProfileString(CFG_BATCHNO, temp);
	batchint = atoi(temp);
	char filed60[20];
	memset(filed60, 0, sizeof(filed60));

	sprintf(filed60, "11%06d000501", batchint);												/* �˻� */
	sprintf(ps.content[60].data, filed60, strlen(filed60));
	ps.content[60].dataLen = strlen(filed60);
	ps.content[60].is_exist = 1;
	/* 63��  ��ȫ 00 */
	ps.content[63].dataLen = strlen("000");
	sprintf(ps.content[63].data, "000", ps.content[63].dataLen);
	ps.content[63].is_exist = 1;

	TRANS_RET tr;
	memset(&tr, 0, sizeof(TRANS_RET));
	UNPACKRESULT ur;
	memset(&ur, 0, sizeof(UNPACKRESULT));
	int ret = TRANS_COMMON(&ps, &tr, mKey, &ur);
	/* ��֧������ֵ���д��� */
	//if (strlen(tr.respCode) == 0)
	//{
	//	memcpy(tr.respCode, "98", strlen("98"));
	//}
	ret = INNER_packRetJson(tr.respCode, tr.mchTransETime, tr.transId,
		tr.accInfo, tr.txnAmt, tr.cretiId, tr.cusInfo, tr.mchTransId, out);
	return 0;
}

/* ������Ԥ��Ȩ���*/
int INNER_UNION_PRE_PAY_FINISH(cJSON *cardInfo, char const * mKey, int txnAmt,
	char const *mchTransId, char const *mchTransSTime, char const *cretiId,
	char const *orgMchTransId, char *out)
{
	char entCode[3], csn[5], trk2[39], trk3[106], pin_data[13], ic_data[1024], exp_date[5], pan[20];
	memset(entCode, 0, sizeof(entCode));
	memset(csn, 0, sizeof(csn));
	memset(trk2, 0, sizeof(trk2));
	memset(trk3, 0, sizeof(trk3));
	memset(pin_data, 0, sizeof(pin_data));
	memset(ic_data, 0, sizeof(ic_data));
	memset(exp_date, 0, sizeof(exp_date));
	memset(pan, 0, sizeof(pan));
	INNER_parseCardInfo_CARD(cardInfo, entCode, csn, trk2, trk3, pin_data, ic_data, exp_date, pan);
	/* ��ʼ�齨 �������ױ��� */
	PACKSOURCE ps;
	ISO_FILEDS_INIT(&ps);
	char *msgTypePay = "0700";
	memcpy(ps.content[0].data, msgTypePay, strlen(msgTypePay));
	ps.content[0].dataLen = strlen(msgTypePay);
	ps.content[0].is_exist = 1;

	/* ��� PAN Ϊ�� �ɶ��Ž�ȡ PAN .���PAN��Ϊ��,��ȡPAN.ʹ��ԭPAN */
	if (strlen(pan) == 0)
	{
		/* pan Ϊ�� trk2��Ϊ��*/
		if (strlen(trk2) != 0)
		{
			/* �� ���Ž�ȡ PAN */
			char trk2temp[60];
			memset(trk2temp, 0, sizeof(trk2temp));
			memcpy(trk2temp, trk2, strlen(trk2));
			//��ʼ���ݷָ�����ȡ����. " d" ���� "D" �ָ�
			char *pantemp = NULL;
			pantemp = strtok(trk2temp, "d,D");
			if (pantemp != NULL)
			{
				memcpy(pan, pantemp, strlen(pantemp));
			}
		}
	}
	/* ȡ pan ���*/
	/* ����  ������Ϣ */
	if (strlen(pan) != 0)
	{
		ps.content[2].is_exist = 1;
		memcpy(ps.content[2].data, pan, strlen(pan));
		ps.content[2].dataLen = strlen(pan);
	}
	/* 3 ���״���*/
	char *processCodePay = "000018";
	memcpy(ps.content[3].data, processCodePay, strlen(processCodePay));
	ps.content[3].dataLen = strlen(processCodePay);
	ps.content[3].is_exist = 1;
	/* 4 ��� */
	sprintf(ps.content[4].data, "%012d", txnAmt);
	ps.content[4].dataLen = 12;
	ps.content[4].is_exist = 1;
	/* 14 �� ����Ч��*/
	if (strlen(exp_date) != 0)
	{
		sprintf(ps.content[14].data, exp_date, strlen(exp_date));
		ps.content[14].dataLen = strlen(exp_date);
		ps.content[14].is_exist = 1;
	}
	/* 22 �� ��������뷽ʽ*/
	char field22[4];
	memset(field22, 0, sizeof(field22));
	strcpy(field22, entCode);
	if (strlen(pin_data) == 0)																/* ������Ϊ�� ,ѡ�������뷽ʽ*/
	{
		strcat(field22, "2");
	}
	else
	{
		strcat(field22, "1");
	}
	sprintf(ps.content[22].data, field22);
	ps.content[22].dataLen = strlen(field22);
	ps.content[22].is_exist = 1;
	/* 23 �� �����к�*/
	if (strlen(csn) != 0)
	{
		sprintf(ps.content[23].data, csn, strlen(csn));
		ps.content[23].dataLen = strlen(csn);
		ps.content[23].is_exist = 1;
	}
	/* 25 ��*/
	sprintf(ps.content[25].data, "06", 2);
	ps.content[25].dataLen = 2;
	ps.content[25].is_exist = 1;
	/* 26�� */
	if (strlen(pan) != 0)
	{
		sprintf(ps.content[26].data, "12", 2);
		ps.content[26].dataLen = 2;
		ps.content[26].is_exist = 1;
	}
	/* 35 ���� */
	if (strlen(trk2) != 0)
	{
		sprintf(ps.content[35].data, trk2, strlen(trk2));
		ps.content[35].dataLen = strlen(trk2);
		ps.content[35].is_exist = 1;
	}
	/* 36 ���� */
	if (strlen(trk3) != 0)
	{
		sprintf(ps.content[35].data, trk3, strlen(trk3));
		ps.content[35].dataLen = strlen(trk3);
		ps.content[35].is_exist = 1;
	}

	/* 52 ������ */
	if (strlen(pin_data) != 0)
	{
		/* TODO: ��Ҫ��������� - ��common �д��� TODO*/
		sprintf(ps.content[52].data, pin_data, strlen(pin_data));
		ps.content[52].dataLen = strlen(pin_data);
		ps.content[52].is_exist = 1;
	}
	/* 53 ��ȫ������Ϣ */
	if (strlen(pin_data) != 0)
	{
		char *filed53 = "2600000000000000";
		sprintf(ps.content[53].data, filed53, strlen(filed53));
		ps.content[53].dataLen = strlen(filed53);
		ps.content[53].is_exist = 1;
	}
	/* 55 �� IC��������*/
	if (strlen(ic_data) != 0)
	{
		/* TODO: ��Ҫ��������� - ��common �д��� TODO*/
		sprintf(ps.content[55].data, ic_data, strlen(ic_data));
		ps.content[55].dataLen = strlen(ic_data) / 2;
		ps.content[55].is_exist = 1;
	}
	/* д��56�� */
	char temp56[600];
	memset(temp56, 0, sizeof(temp56));
	INNER_PACK56(mchTransId, cretiId, orgMchTransId, temp56);
	memcpy(ps.content[56].data, temp56, strlen(temp56));
	ps.content[56].dataLen = strlen(temp56) / 2;
	ps.content[56].is_exist = 1;//TODO:change there 
	/* д������ 60 �� */
	char batch[7];
	memset(batch, 0, sizeof(batch));
	char temp[20];
	memset(temp, 0, sizeof(temp));
	int batchint = 0;
	CFG_GetProfileString(CFG_BATCHNO, temp);
	batchint = atoi(temp);
	char filed60[20];
	memset(filed60, 0, sizeof(filed60));

	sprintf(filed60, "20%06d000501", batchint);												/* �˻� */
	sprintf(ps.content[60].data, filed60, strlen(filed60));
	ps.content[60].dataLen = strlen(filed60);
	ps.content[60].is_exist = 1;
	/* 63��  ��ȫ 00 */
	ps.content[63].dataLen = strlen("000");
	sprintf(ps.content[63].data, "000", ps.content[63].dataLen);
	ps.content[63].is_exist = 1;

	TRANS_RET tr;
	memset(&tr, 0, sizeof(TRANS_RET));
	UNPACKRESULT ur;
	memset(&ur, 0, sizeof(UNPACKRESULT));
	int ret = TRANS_COMMON(&ps, &tr, mKey, &ur);
	/* ��֧������ֵ���д��� */
	//if (strlen(tr.respCode) == 0)
	//{
	//	memcpy(tr.respCode, "98", strlen("98"));
	//}
	ret = INNER_packRetJson(tr.respCode, tr.mchTransETime, tr.transId,
		tr.accInfo, tr.txnAmt, tr.cretiId, tr.cusInfo, tr.mchTransId, out);
	return 0;
}

/* ������Ԥ��Ȩ��ɳ���*/
int INNER_UNION_PRE_PAY_FINSHI_REVOKE(cJSON *cardInfo, char const * mKey, int txnAmt,
	char const *mchTransId, char const *mchTransSTime, char const *cretiId,
	char const *orgMchTransId, char *out)
{
	char entCode[3], csn[5], trk2[39], trk3[106], pin_data[13], ic_data[1024], exp_date[5], pan[20];
	memset(entCode, 0, sizeof(entCode));
	memset(csn, 0, sizeof(csn));
	memset(trk2, 0, sizeof(trk2));
	memset(trk3, 0, sizeof(trk3));
	memset(pin_data, 0, sizeof(pin_data));
	memset(ic_data, 0, sizeof(ic_data));
	memset(exp_date, 0, sizeof(exp_date));
	memset(pan, 0, sizeof(pan));
	INNER_parseCardInfo_CARD(cardInfo, entCode, csn, trk2, trk3, pin_data, ic_data, exp_date, pan);
	/* ��ʼ�齨 �������ױ��� */
	PACKSOURCE ps;
	ISO_FILEDS_INIT(&ps);
	char *msgTypePay = "0700";
	memcpy(ps.content[0].data, msgTypePay, strlen(msgTypePay));
	ps.content[0].dataLen = strlen(msgTypePay);
	ps.content[0].is_exist = 1;

	/* ��� PAN Ϊ�� �ɶ��Ž�ȡ PAN .���PAN��Ϊ��,��ȡPAN.ʹ��ԭPAN */
	if (strlen(pan) == 0)
	{
		/* pan Ϊ�� trk2��Ϊ��*/
		if (strlen(trk2) != 0)
		{
			/* �� ���Ž�ȡ PAN */
			char trk2temp[60];
			memset(trk2temp, 0, sizeof(trk2temp));
			memcpy(trk2temp, trk2, strlen(trk2));
			//��ʼ���ݷָ�����ȡ����. " d" ���� "D" �ָ�
			char *pantemp = NULL;
			pantemp = strtok(trk2temp, "d,D");
			if (pantemp != NULL)
			{
				memcpy(pan, pantemp, strlen(pantemp));
			}
		}
	}
	/* ȡ pan ���*/
	/* ����  ������Ϣ */
	if (strlen(pan) != 0)
	{
		ps.content[2].is_exist = 1;
		memcpy(ps.content[2].data, pan, strlen(pan));
		ps.content[2].dataLen = strlen(pan);
	}
	/* 3 ���״���*/
	char *processCodePay = "200021";
	memcpy(ps.content[3].data, processCodePay, strlen(processCodePay));
	ps.content[3].dataLen = strlen(processCodePay);
	ps.content[3].is_exist = 1;
	/* 4 ��� */
	sprintf(ps.content[4].data, "%012d", txnAmt);
	ps.content[4].dataLen = 12;
	ps.content[4].is_exist = 1;
	/* 14 �� ����Ч��*/
	if (strlen(exp_date) != 0)
	{
		sprintf(ps.content[14].data, exp_date, strlen(exp_date));
		ps.content[14].dataLen = strlen(exp_date);
		ps.content[14].is_exist = 1;
	}
	/* 22 �� ��������뷽ʽ*/
	char field22[4];
	memset(field22, 0, sizeof(field22));
	strcpy(field22, entCode);
	if (strlen(pin_data) == 0)																/* ������Ϊ�� ,ѡ�������뷽ʽ*/
	{
		strcat(field22, "2");
	}
	else
	{
		strcat(field22, "1");
	}
	sprintf(ps.content[22].data, field22);
	ps.content[22].dataLen = strlen(field22);
	ps.content[22].is_exist = 1;
	/* 23 �� �����к�*/
	if (strlen(csn) != 0)
	{
		sprintf(ps.content[23].data, csn, strlen(csn));
		ps.content[23].dataLen = strlen(csn);
		ps.content[23].is_exist = 1;
	}
	/* 25 ��*/
	sprintf(ps.content[25].data, "06", 2);
	ps.content[25].dataLen = 2;
	ps.content[25].is_exist = 1;
	/* 26�� */
	if (strlen(pan) != 0)
	{
		sprintf(ps.content[26].data, "12", 2);
		ps.content[26].dataLen = 2;
		ps.content[26].is_exist = 1;
	}
	/* 35 ���� */
	if (strlen(trk2) != 0)
	{
		sprintf(ps.content[35].data, trk2, strlen(trk2));
		ps.content[35].dataLen = strlen(trk2);
		ps.content[35].is_exist = 1;
	}
	/* 36 ���� */
	if (strlen(trk3) != 0)
	{
		sprintf(ps.content[35].data, trk3, strlen(trk3));
		ps.content[35].dataLen = strlen(trk3);
		ps.content[35].is_exist = 1;
	}

	/* 52 ������ */
	if (strlen(pin_data) != 0)
	{
		/* TODO: ��Ҫ��������� - ��common �д��� TODO*/
		sprintf(ps.content[52].data, pin_data, strlen(pin_data));
		ps.content[52].dataLen = strlen(pin_data);
		ps.content[52].is_exist = 1;
	}
	/* 53 ��ȫ������Ϣ */
	if (strlen(pin_data) != 0)
	{
		char *filed53 = "2600000000000000";
		sprintf(ps.content[53].data, filed53, strlen(filed53));
		ps.content[53].dataLen = strlen(filed53);
		ps.content[53].is_exist = 1;
	}
	/* 55 �� IC��������*/
	if (strlen(ic_data) != 0)
	{
		/* TODO: ��Ҫ��������� - ��common �д��� TODO*/
		sprintf(ps.content[55].data, ic_data, strlen(ic_data));
		ps.content[55].dataLen = strlen(ic_data) / 2;
		ps.content[55].is_exist = 1;
	}
	/* д��56�� */
	char temp56[600];
	memset(temp56, 0, sizeof(temp56));
	INNER_PACK56(mchTransId, cretiId, orgMchTransId, temp56);
	memcpy(ps.content[56].data, temp56, strlen(temp56));
	ps.content[56].dataLen = strlen(temp56) / 2;
	ps.content[56].is_exist = 1;//TODO:change there 
	/* д������ 60 �� */
	char batch[7];
	memset(batch, 0, sizeof(batch));
	char temp[20];
	memset(temp, 0, sizeof(temp));
	int batchint = 0;
	CFG_GetProfileString(CFG_BATCHNO, temp);
	batchint = atoi(temp);
	char filed60[20];
	memset(filed60, 0, sizeof(filed60));

	sprintf(filed60, "21%06d000501", batchint);												/* �˻� */
	sprintf(ps.content[60].data, filed60, strlen(filed60));
	ps.content[60].dataLen = strlen(filed60);
	ps.content[60].is_exist = 1;
	/* 63��  ��ȫ 00 */
	ps.content[63].dataLen = strlen("000");
	sprintf(ps.content[63].data, "000", ps.content[63].dataLen);
	ps.content[63].is_exist = 1;

	TRANS_RET tr;
	memset(&tr, 0, sizeof(TRANS_RET));
	UNPACKRESULT ur;
	memset(&ur, 0, sizeof(UNPACKRESULT));
	int ret = TRANS_COMMON(&ps, &tr, mKey, &ur);
	/* ��֧������ֵ���д��� */
	//if (strlen(tr.respCode) == 0)
	//{
	//	memcpy(tr.respCode, "98", strlen("98"));
	//}
	ret = INNER_packRetJson(tr.respCode, tr.mchTransETime, tr.transId,
		tr.accInfo, tr.txnAmt, tr.cretiId, tr.cusInfo, tr.mchTransId, out);
	return 0;
}

/* �⿨����*/
int INNER_OUTSIDE_PAY(cJSON *cardInfo, char const * mKey, int txnAmt,
	char const *mchTransId, char const *mchTransSTime, char const *cretiId,
	char const *orgMchTransId, char *out)
{
	return 0;
}

/* �⿨���ѳ���*/
int INNER_OUTSIDE_FLUSH(cJSON *cardInfo, char const * mKey, int txnAmt,
	char const *mchTransId, char const *mchTransSTime, char const *cretiId,
	char const *orgMchTransId, char *out)
{
	return 0;
}

/* �⿨�����˻�*/
int INNER_OUTSIDE_REFUND(cJSON *cardInfo, char const * mKey, int txnAmt,
	char const *mchTransId, char const *mchTransSTime, char const *cretiId,
	char const *orgMchTransId, char *out)
{
	return 0;
}

/* �⿨���ѳ���*/
int INNER_OUTSIDE_REVOKE(cJSON *cardInfo, char const * mKey, int txnAmt,
	char const *mchTransId, char const *mchTransSTime, char const *cretiId,
	char const *orgMchTransId, char *out)
{
	return 0;
}

/* ������Ƭ��Ϣ�� ��Ƭ*/
int INNER_parseCardInfo_CARD(cJSON *json, char *entCode, char *csn,
	char *trk2, char *trk3, char *pin_data, char *ic_data, char* exp_date,char* pan)
{
	cJSON *jsonfind = NULL;
	/* �ڿ�,�⿨����*/
	jsonfind = cJSON_GetObjectItem(json, "entCode");
	if (jsonfind == NULL)
	{
		return -2;
	}
	memcpy(entCode, jsonfind->valuestring, strlen(jsonfind->valuestring));

	jsonfind = cJSON_GetObjectItem(json, "csn");
	if (jsonfind != NULL)
	{
		memcpy(csn, jsonfind->valuestring, sizeof(jsonfind->valuestring));
	}

	jsonfind = cJSON_GetObjectItem(json, "exp_date");
	if (jsonfind != NULL)
	{
		memcpy(exp_date, jsonfind->valuestring, sizeof(jsonfind->valuestring));
	}

	jsonfind = cJSON_GetObjectItem(json, "trk2");
	if (jsonfind != NULL)
	{
		memcpy(trk2, jsonfind->valuestring, sizeof(jsonfind->valuestring));
	}
	memcpy(trk2, jsonfind->valuestring, strlen(jsonfind->valuestring));

	jsonfind = cJSON_GetObjectItem(json, "trk3");
	if (jsonfind != NULL)
	{
		memcpy(trk3, jsonfind->valuestring, strlen(jsonfind->valuestring));
	}

	jsonfind = cJSON_GetObjectItem(json, "pin_data");
	if (jsonfind != NULL)
	{
		memcpy(pin_data, jsonfind->valuestring, strlen(jsonfind->valuestring));
	}

	jsonfind = cJSON_GetObjectItem(json, "ic_data");
	if (jsonfind != NULL)
	{
		memcpy(ic_data, jsonfind->valuestring, strlen(jsonfind->valuestring));
	}

	jsonfind = cJSON_GetObjectItem(json, "pan");
	if (jsonfind != NULL)
	{
		memcpy(pan, jsonfind->valuestring, strlen(jsonfind->valuestring));
	}

	return 0;
}


/* ������Ƭ��Ϣ�� ��ά��*/
int INNER_parseCardInfo_CODE(cJSON *json, char *code)
{
	cJSON *jsonfind = NULL;
	/* ֧����΢�� */
	jsonfind = cJSON_GetObjectItem(json, "QRCode");
	//char *out = cJSON_Print(json);
	//printf(out);
	//free(out);
	if (jsonfind == NULL)
	{
		return -1;
	}
	memcpy(code, jsonfind->valuestring, strlen(jsonfind->valuestring));
	return 0;
}

/* ���JOSN��� �������ݼ��ĵ� */
int INNER_packRetJson(char *respCode, char *mchTransETime, char *transId,
	char *accInfo, int txnAmt, char *cretild, char *cusInfo, char *mchTransId, char *output)
{
	if (respCode == NULL) { respCode = ""; }
	if (mchTransETime == NULL) { mchTransETime = ""; }
	if (transId == NULL) { transId = ""; }
	if (accInfo == NULL) { accInfo = ""; }
	if (txnAmt == NULL) { txnAmt = 0; }
	if (cretild == NULL) { cretild = ""; }
	if (cusInfo == NULL) { cusInfo = ""; }
	if (mchTransId == NULL) { mchTransId = ""; }
	if (output == NULL) { return -1; }
	cJSON *root;
	root = cJSON_CreateObject();
	cJSON_AddStringToObject(root, "respCode", respCode);
	cJSON_AddStringToObject(root, "mchTransETime", mchTransETime);
	cJSON_AddStringToObject(root, "transId", transId);
	cJSON_AddStringToObject(root, "accInfo", accInfo);
	cJSON_AddNumberToObject(root, "txnAmt", txnAmt);
	cJSON_AddStringToObject(root, "cretiId", cretild);
	cJSON_AddStringToObject(root, "cusInfo", cusInfo);
	cJSON_AddStringToObject(root, "mchTransId", mchTransId);
	char *out;
	out = cJSON_Print(root);
	memcpy(output, out, strlen(out));
	free(out);
	cJSON_Delete(root);
	return 0;
}



/* ��56�� */
int INNER_PACK56(char const *mchTransId, char const *cretiId, char const *orgMchTransId, char *out)
{
	char temp[MAX_LEN] = { 0 };
	int  rlen = 0;
	if (mchTransId[0] != '\0')
	{
		rlen += FormXZZFTlv(temp, "\x9f\x02", 2, mchTransId, strlen(mchTransId));
	}
	if (cretiId[0] != '\0')
	{
		rlen += FormXZZFTlv(temp + rlen, "\x9f\x10", 2, cretiId, strlen(cretiId));
	}
	if (orgMchTransId[0] != '\0')
	{
		rlen += FormXZZFTlv(temp + rlen, "\x9f\x11", 2, orgMchTransId, strlen(orgMchTransId));
	}
	return ISO8583_Str2BcdStr(temp, rlen, out);
}


unsigned short FormXZZFTlv(char *lpOut, char *tag, char taglen, char *value, unsigned short valuelen)
{
	unsigned short outLen = 0;
	//char printbuf[64] = { 0 };
	memcpy(lpOut, tag, taglen);
	outLen += taglen;
	if (valuelen > 0)
	{
		if (valuelen > 255)
		{
			lpOut[outLen++] = 0x82;
			lpOut[outLen++] = (char)(valuelen >> 8);
		}
		else if (valuelen > 127)
		{
			lpOut[outLen++] = 0x81;
		}
		lpOut[outLen++] = valuelen;
		memcpy(&lpOut[outLen], value, valuelen);
		outLen += valuelen;
	}
	else
	{
		outLen = 0;
	}
	return outLen;
}
