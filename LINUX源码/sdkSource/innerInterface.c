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

/* 得到交易类型映射 */
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

/* 交易唯一接口 输入JSON格式字符串,输出JSON格式字符串 成功返回0 失败返回负数
	-1 JSON 解析失败

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
		INNER_packRetJson(NULL, NULL, NULL, NULL, 0, NULL, "json 解析失败", NULL, out);
		return -1;
	}
	/* 解析输入json串 */
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
	/* 获取msgType*/
	jsonfind = cJSON_GetObjectItem(json, "msgType");
	if (jsonfind == NULL)
	{
		memset(msgType, 0, sizeof(msgType));
		LOG_WriteLog("input json string : msgType[must exist] parse failed");
		INNER_packRetJson(NULL, NULL, NULL, NULL, 0, NULL, "msgType[must exist] parse failed", NULL, out);
		return -1;
	}
	memcpy(msgType, jsonfind->valuestring, strlen(jsonfind->valuestring));
	/* 获取card info */
	cJSON *jsoncard = NULL;
	jsoncard = cJSON_GetObjectItem(json, "cardInfo");
	if (jsoncard == NULL)
	{
		LOG_WriteLog("input json string : cardInfo[must exist] parse failed");
		INNER_packRetJson(NULL, NULL, NULL, NULL, 0, NULL, "cardInfo[must exist] parse failed", NULL, out);
		return -1;
	}
	/* 获取 mKey*/
	jsonfind = cJSON_GetObjectItem(json, "mKey");
	if (jsonfind == NULL)
	{
		memset(mKey, 0, sizeof(mKey));
		LOG_WriteLog("input json string : mKey[must exist] parse failed");
		INNER_packRetJson(NULL, NULL, NULL, NULL, 0, NULL, "mKey[must exist] parse failed", NULL, out);
		return -1;
	}
	memcpy(mKey, jsonfind->valuestring, strlen(jsonfind->valuestring));
	/* 获取 txnAmt */
	jsonfind = cJSON_GetObjectItem(json, "txnAmt");
	if (jsonfind == NULL)
	{
		txnAmt = 0;
		LOG_WriteLog("input json string : txnAmt parse failed");
		INNER_packRetJson(NULL, NULL, NULL, NULL, 0, NULL, "txnAmt parse failed", NULL, out);
		return -1;
	}
	txnAmt = jsonfind->valueint;
	/* 获取 mchTransId 非必填 */
	jsonfind = cJSON_GetObjectItem(json, "mchTransId");
	if (jsonfind != NULL)		
	{
		memcpy(mchTransId, jsonfind->valuestring, strlen(jsonfind->valuestring));
	}
	/* 获取 mchTransSTime 必填*/
	jsonfind = cJSON_GetObjectItem(json, "mchTransSTime");
	if (jsonfind == NULL)
	{
		LOG_WriteLog("input json string : mchTransSTime[must exist] parse failed");
		INNER_packRetJson(NULL, NULL, NULL, NULL, 0, NULL, "mchTransSTime[must exist] parse failed", NULL, out);
		return -1;
	}
	memcpy(mchTransSTime, jsonfind->valuestring, strlen(jsonfind->valuestring));
	/* 获取 cretiId 非必填*/
	jsonfind = cJSON_GetObjectItem(json, "cretiId");
	if (jsonfind != NULL)
	{
		memcpy(cretiId, jsonfind->valuestring, strlen(jsonfind->valuestring));
	}
	/* 获取 orgMchTransId 非必填*/
	jsonfind = cJSON_GetObjectItem(json, "orgMchTransId");
	if (jsonfind != NULL)
	{
		memcpy(orgMchTransId, jsonfind->valuestring, strlen(jsonfind->valuestring));
	}
	/* 自动签到 */
	int ret = 0;
	ret = TRANS_AUTOSIGNUP(mKey);
	if (ret != 0)
	{
		LOG_WriteLog("AUTO SIGN UP : error");
		return ret;
	}
	/*获取参数信息结束. 开始解析*/
	int msgTypeInt = INNER_getMsgInt(msgType);
	char log[200];
	memset(log, 0, sizeof(log));
	sprintf(log, "Start process [%d] trans.", msgTypeInt);
	LOG_WriteLog(log);
	switch (msgTypeInt)
	{
	case 12://银联卡消费	
		ret = INNER_UNION_PAY(jsoncard, mKey, txnAmt, mchTransId, mchTransSTime, cretiId, orgMchTransId, out);
		break;
	case 13://银联卡冲正
		ret = INNER_UNION_FLUSH(jsoncard, mKey, txnAmt, mchTransId, mchTransSTime, cretiId, orgMchTransId, out);
		break;
	case 14://银联卡撤销
		ret = INNER_UNION_REVOKE(jsoncard, mKey, txnAmt, mchTransId, mchTransSTime, cretiId, orgMchTransId, out);
		break; 
	case 15://银联卡退货
		ret = INNER_UNION_REFUND(jsoncard, mKey, txnAmt, mchTransId, mchTransSTime, cretiId, orgMchTransId, out);
		break;
	case 16://银联卡预授权
		ret = INNER_UNION_PRE_PAY(jsoncard, mKey, txnAmt, mchTransId, mchTransSTime, cretiId, orgMchTransId, out);
		break;
	case 17://银联卡预授权撤销
		ret = INNER_UNION_PRE_PAY_REVOKE(jsoncard, mKey, txnAmt, mchTransId, mchTransSTime, cretiId, orgMchTransId, out);
		break;
	case 18://银联卡预授权完成
		ret = INNER_UNION_PRE_PAY_FINISH(jsoncard, mKey, txnAmt, mchTransId, mchTransSTime, cretiId, orgMchTransId, out);
		break;
	case 19://银联卡 余额查询
		ret = INNER_UNION_QUERY(jsoncard, mKey, txnAmt, mchTransId, mchTransSTime, cretiId, orgMchTransId, out);
		break;
	case 20://预授权冲正
		ret = INNER_UNION_PRE_PAY_FLUSH(jsoncard, mKey, txnAmt, mchTransId, mchTransSTime, cretiId, orgMchTransId, out);
		break;
	case 21://预授权完成撤销
		ret = INNER_UNION_PRE_PAY_FINSHI_REVOKE(jsoncard, mKey, txnAmt, mchTransId, mchTransSTime, cretiId, orgMchTransId, out);
		break;
	case 22://支付宝支付
		ret = INNER_ALIPAY_PAY(jsoncard, mKey, txnAmt, mchTransId, mchTransSTime, cretiId, orgMchTransId, out);
		break;
	case 23://支付宝冲正
		ret = INNER_ALIPAY_FLUSH(jsoncard, mKey, txnAmt, mchTransId, mchTransSTime, cretiId, orgMchTransId, out);
		break;
	case 25://支付宝退货
		ret = INNER_ALIPAY_REFUND(jsoncard, mKey, txnAmt, mchTransId, mchTransSTime, cretiId, orgMchTransId, out);
		break;
	case 32://微信支付
		ret = INNER_WX_PAY(jsoncard, mKey, txnAmt, mchTransId, mchTransSTime, cretiId, orgMchTransId, out);
		break;
	case 33://微信冲正
		ret = INNER_WX_FLUSH(jsoncard, mKey, txnAmt, mchTransId, mchTransSTime, cretiId, orgMchTransId, out);
		break;
	case 35://微信退货
		ret = INNER_WX_REFUND(jsoncard, mKey, txnAmt, mchTransId, mchTransSTime, cretiId, orgMchTransId, out);
		break;
	case 42://外卡消费
		ret = INNER_OUTSIDE_PAY(jsoncard, mKey, txnAmt, mchTransId, mchTransSTime, cretiId, orgMchTransId, out);
		break;
	case 43://外卡消费冲正
		ret = INNER_OUTSIDE_FLUSH(jsoncard, mKey, txnAmt, mchTransId, mchTransSTime, cretiId, orgMchTransId, out);
		break;
	case 44://外卡撤销
		ret = INNER_OUTSIDE_REVOKE(jsoncard, mKey, txnAmt, mchTransId, mchTransSTime, cretiId, orgMchTransId, out);
		break;
	case 45://外卡退货
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
	/* 开始解析output,并将结果写入结构体*/
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
/* 支付宝交易 */
int INNER_ALIPAY_PAY(cJSON *cardInfo, char const * mKey, int txnAmt,
	char const *mchTransId, char const *mchTransSTime, char const *cretiId,
	char const *orgMchTransId, char *out)
{
	char code[60];
	memset(code, 0, sizeof(code));
	INNER_parseCardInfo_CODE(cardInfo, code);
	int traceint = 0, batchint = 0;																/* 流水 批次 查询的时候需要*/
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
	/* 写入56域 */
	char temp56[600];
	memset(temp56, 0, sizeof(temp56));
	INNER_PACK56(mchTransId, cretiId, orgMchTransId, temp56);
	memcpy(ps.content[56].data, temp56, strlen(temp56));
	ps.content[56].dataLen = strlen(temp56) / 2;
	ps.content[56].is_exist = 1;
	/* 写入批次 60 域 */
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
	/* 写入二维码信息 */
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

	/* 对支付返回值进行处理 */
	if (strcmp(tr.respCode, "IP") != 0)														/* 支付宝支付成功 */
	{
		//if (strcmp(tr.respCode, "94") == 0 || strcmp(tr.respCode, "A0") == 0 || strcmp(tr.respCode, "a0") == 0 )
		//{
		//	goto retry;
		//}
		ret = INNER_packRetJson(tr.respCode,tr.mchTransETime,tr.transId,
			tr.accInfo,tr.txnAmt,tr.cretiId,tr.cusInfo,tr.mchTransId,out);
		return ret;
	}
	
	/* 等待用户输入密码 查询*/
	/* 获得流水*/
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
		/* 做查询 */
		ret = INNER_ALIPAY_PAY_QUERY(cardInfo, mKey, txnAmt, mchTransId,
			mchTransSTime, cretiId, orgMchTransId, batchint,traceint, &tr);
		/* 查询结束*/
		if (ret == 0)
		{
			//查询结束,正常返回.
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
	int traceint = 0, batchint = 0;																/* 流水 批次 查询的时候需要*/
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
	/* 写入批次 60 域 */
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
	/* 写入原批次号流水 到 61域 */
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
	/* 对支付返回值进行处理 */
	if (strcmp(tr->respCode, "00") == 0 || strcmp(tr->respCode,"IF") == 0)														/* 支付宝支付成功 */
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

/* 支付宝冲正 */
int INNER_ALIPAY_FLUSH(cJSON *cardInfo, char const * mKey, int txnAmt,
	char const *mchTransId, char const *mchTransSTime, char const *cretiId,
	char const *orgMchTransId, char *out)
{
	char code[60];
	memset(code, 0, sizeof(code));
	INNER_parseCardInfo_CODE(cardInfo, code);
	int traceint = 0, batchint = 0;																/* 流水 批次 查询的时候需要*/
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

	/* 39 冲正原因 */
	char *flushReason = "06";
	sprintf(ps.content[39].data, flushReason, strlen(flushReason));
	ps.content[39].dataLen = strlen(flushReason);
	ps.content[39].is_exist = 1;
	
	/* 写入56域 */
	char temp56[600];
	memset(temp56, 0, sizeof(temp56));
	INNER_PACK56(mchTransId, cretiId, orgMchTransId, temp56);
	memcpy(ps.content[56].data, temp56, strlen(temp56));
	ps.content[56].dataLen = strlen(temp56) / 2;
	ps.content[56].is_exist = 1; 
	/* 写入批次 60 域 */
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
	/* 对支付返回值进行处理 */
	if (strcmp(tr.respCode, "00") == 0 || strcmp(tr.respCode, "25"))														/* 支付宝支付成功 */
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

/* 支付宝退货 */
int INNER_ALIPAY_REFUND(cJSON *cardInfo, char const * mKey, int txnAmt,
	char const *mchTransId, char const *mchTransSTime, char const *cretiId,
	char const *orgMchTransId, char *out)
{
	char code[60];
	memset(code, 0, sizeof(code));
	INNER_parseCardInfo_CODE(cardInfo, code);
	int traceint = 0, batchint = 0;																/* 流水 批次 查询的时候需要*/
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

	/* 写入56域 */
	char temp56[600];
	memset(temp56, 0, sizeof(temp56));
	INNER_PACK56(mchTransId, cretiId, orgMchTransId, temp56);
	memcpy(ps.content[56].data, temp56, strlen(temp56));
	ps.content[56].dataLen = strlen(temp56) / 2;
	ps.content[56].is_exist = 1;
	/* 写入批次 60 域 */
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
	/* 对支付返回值进行处理 */
	if (strcmp(tr.respCode, "00") == 0 || strcmp(tr.respCode, "25") == 0 || strcmp(tr.respCode,"12") == 0 )														/* 支付宝支付成功 */
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


/* 微信交易*/
int INNER_WX_PAY(cJSON *cardInfo, char const * mKey, int txnAmt,
	char const *mchTransId, char const *mchTransSTime, char const *cretiId,
	char const *orgMchTransId, char *out)
{
	char code[60];
	memset(code, 0, sizeof(code));
	INNER_parseCardInfo_CODE(cardInfo, code);
	int traceint = 0, batchint = 0;																/* 流水 批次 查询的时候需要*/
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
	/* 写入56域 */
	char temp56[600];
	memset(temp56, 0, sizeof(temp56));
	INNER_PACK56(mchTransId, cretiId, orgMchTransId, temp56);
	memcpy(ps.content[56].data, temp56, strlen(temp56));
	ps.content[56].dataLen = strlen(temp56) / 2;
	ps.content[56].is_exist = 1;
	/* 写入批次 60 域 */
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
	/* 写入二维码信息 */
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
	/* 对支付返回值进行处理 */
	if (strcmp(tr.respCode, "IP") != 0 )														/* 微信支付成功 */
	{
		//if (strcmp(tr.respCode, "94") == 0 || strcmp(tr.respCode, "A0") == 0 || strcmp(tr.respCode, "a0") == 0 )
		//{
		//	goto retry;
		//}
		ret = INNER_packRetJson(tr.respCode, tr.mchTransETime, tr.transId,
			tr.accInfo, tr.txnAmt, tr.cretiId, tr.cusInfo, tr.mchTransId, out);
		return ret;
	}
	/* 等待用户输入密码 查询*/
	/* 获得流水*/
	// 临时存放原交易的transID
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
		/* 做查询 */
		ret = INNER_WX_PAY_QUERY(cardInfo, mKey, txnAmt, mchTransId,
			mchTransSTime, cretiId, orgMchTransId, batchint, traceint, &tr);
		/* 查询结束*/
		if (ret == 0)
		{	// 查询结束,支付成功
			memset(tr.transId, 0, sizeof(tr.transId));
			memcpy(tr.transId, transIdTemp, strlen(transIdTemp));
			INNER_packRetJson(tr.respCode, tr.mchTransETime, tr.transId,
				tr.accInfo, tr.txnAmt, tr.cretiId, tr.cusInfo, tr.mchTransId, out);
			return ret;
		}
		if (strcmp(tr.respCode, "I5") != 0 )
		{	// 查询结果非 I5,退出查询
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


/* 微信交易查询*/
int INNER_WX_PAY_QUERY(cJSON *cardInfo, char const * mKey, int txnAmt,
	char const *mchTransId, char const *mchTransSTime, char const *cretiId,
	char const *orgMchTransId, int batchintSrc, int traceintSrc, TRANS_RET *tr)
{
	char code[60];
	memset(code, 0, sizeof(code));
	INNER_parseCardInfo_CODE(cardInfo, code);
	int traceint = 0, batchint = 0;																/* 流水 批次 查询的时候需要*/
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
	/* 写入批次 60 域 */
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
	/* 写入原批次号流水 到 61域 */
	sprintf(ps.content[61].data, "%06d%06d", batchintSrc, traceintSrc);
	ps.content[61].dataLen = strlen(ps.content[61].data);
	ps.content[61].is_exist = 1;

	//TRANS_RET tr;
	memset(tr, 0, sizeof(TRANS_RET));
	UNPACKRESULT ur;
	memset(&ur, 0, sizeof(UNPACKRESULT));
	int ret = TRANS_COMMON(&ps, tr, mKey, &ur);
	/* 对支付返回值进行处理 */
	if (strcmp(tr->respCode, "00") == 0 || strcmp(tr->respCode, "IF") == 0)														/* 支付宝支付成功 */
	{
		//ret = INNER_packRetJson(tr.respCode, tr.mchTransETime, tr.transId,
		//	tr.accInfo, tr.txnAmt, tr.cretiId, tr.cusInfo, tr.mchTransId, out);
		return 0;
	}
	//INNER_packRetJson(tr.respCode, tr.mchTransETime, tr.transId,
	//	tr.accInfo, tr.txnAmt, tr.cretiId, tr.cusInfo, tr.mchTransId, out);
	return -1;
}
/* 微信冲正*/
int INNER_WX_FLUSH(cJSON *cardInfo, char const * mKey, int txnAmt,
	char const *mchTransId, char const *mchTransSTime, char const *cretiId,
	char const *orgMchTransId, char *out)
{
	char code[60];
	memset(code, 0, sizeof(code));
	INNER_parseCardInfo_CODE(cardInfo, code);
	int traceint = 0, batchint = 0;																/* 流水 批次 查询的时候需要*/
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

	/* 39 冲正原因 */
	char *flushReason = "06";
	sprintf(ps.content[39].data, flushReason, strlen(flushReason));
	ps.content[39].dataLen = strlen(flushReason);
	ps.content[39].is_exist = 1;

	/* 写入56域 */
	char temp56[600];
	memset(temp56, 0, sizeof(temp56));
	INNER_PACK56(mchTransId, cretiId, orgMchTransId, temp56);
	memcpy(ps.content[56].data, temp56, strlen(temp56));
	ps.content[56].dataLen = strlen(temp56) / 2;
	ps.content[56].is_exist = 1;
	/* 写入批次 60 域 */
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
	/* 对支付返回值进行处理 */
	if (strcmp(tr.respCode, "00") == 0 || strcmp(tr.respCode, "25"))														/* 支付宝支付成功 */
	{
		ret = INNER_packRetJson(tr.respCode, tr.mchTransETime, tr.transId,
			tr.accInfo, tr.txnAmt, tr.cretiId, tr.cusInfo, tr.mchTransId, out);
		return 0;
	}
	INNER_packRetJson(tr.respCode, tr.mchTransETime, tr.transId,
		tr.accInfo, tr.txnAmt, tr.cretiId, tr.cusInfo, tr.mchTransId, out);
	return 0;
}
/* 微信退货*/
int INNER_WX_REFUND(cJSON *cardInfo, char const * mKey, int txnAmt,
	char const *mchTransId, char const *mchTransSTime, char const *cretiId,
	char const *orgMchTransId, char *out)
{
	char code[60];
	memset(code, 0, sizeof(code));
	INNER_parseCardInfo_CODE(cardInfo, code);
	int traceint = 0, batchint = 0;																/* 流水 批次 查询的时候需要*/
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

	/* 写入56域 */
	char temp56[600];
	memset(temp56, 0, sizeof(temp56));
	INNER_PACK56(mchTransId, cretiId, orgMchTransId, temp56);
	memcpy(ps.content[56].data, temp56, strlen(temp56));
	ps.content[56].dataLen = strlen(temp56) / 2;
	ps.content[56].is_exist = 1;
	/* 写入批次 60 域 */
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
	/* 对支付返回值进行处理 */
	if (strcmp(tr.respCode, "00") == 0 || strcmp(tr.respCode, "25") == 0 || strcmp(tr.respCode, "12") == 0)														/* 支付宝支付成功 */
	{
		ret = INNER_packRetJson(tr.respCode, tr.mchTransETime, tr.transId,
			tr.accInfo, tr.txnAmt, tr.cretiId, tr.cusInfo, tr.mchTransId, out);
		return 0;
	}
	INNER_packRetJson(tr.respCode, tr.mchTransETime, tr.transId,
		tr.accInfo, tr.txnAmt, tr.cretiId, tr.cusInfo, tr.mchTransId, out);
	return 0;
}



/* 银联交易*/
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
	/* 开始组建 银联交易报文 */
	PACKSOURCE ps;
	ISO_FILEDS_INIT(&ps);
	char *msgTypePay = "0200";
	memcpy(ps.content[0].data, msgTypePay, strlen(msgTypePay));
	ps.content[0].dataLen = strlen(msgTypePay);
	ps.content[0].is_exist = 1;

	/* 如果 PAN 为空 由二磁截取 PAN .如果PAN不为空,不取PAN.使用原PAN */
	if (strlen(pan) == 0)
	{
		/* pan 为空 trk2不为空*/
		if (strlen(trk2) != 0)
		{
			/* 由 二磁截取 PAN */
			char trk2temp[60];
			memset(trk2temp, 0, sizeof(trk2temp));
			memcpy(trk2temp, trk2, strlen(trk2));
			//开始根据分隔符截取卡号. " d" 或者 "D" 分隔
			char *pantemp = NULL;
			pantemp = strtok(trk2temp, "d,D");
			if (pantemp != NULL)
			{
				memcpy(pan, pantemp, strlen(pantemp));
			}
		}
	}
	/* 取 pan 完毕*/
	
	/* 二域  卡号信息 */
	if (strlen(pan) != 0)
	{
		ps.content[2].is_exist = 1;
		memcpy(ps.content[2].data, pan, strlen(pan));
		ps.content[2].dataLen = strlen(pan);
	}
	/* 交易处理*/
	char *processCodePay = "000012";
	memcpy(ps.content[3].data, processCodePay, strlen(processCodePay));
	ps.content[3].dataLen = strlen(processCodePay);
	ps.content[3].is_exist = 1;
	/* 金额 */
	sprintf(ps.content[4].data, "%012d", txnAmt);
	ps.content[4].dataLen = 12;
	ps.content[4].is_exist = 1;

	/* 14 域 卡有效期*/
	if (strlen(exp_date) != 0)
	{
		sprintf(ps.content[14].data, exp_date, strlen(exp_date));
		ps.content[14].dataLen = strlen(exp_date);
		ps.content[14].is_exist = 1;
	}
	/* 22 域 服务点输入方式*/
	char field22[4];
	memset(field22, 0, sizeof(field22));
	strcpy(field22, entCode);
	if (strlen(pin_data) == 0)																/* 密码域为空 ,选择无密码方式*/
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

	/* 23 域 卡序列号*/
	if (strlen(csn) != 0)
	{
		sprintf(ps.content[23].data, csn, strlen(csn));
		ps.content[23].dataLen = strlen(csn);
		ps.content[23].is_exist = 1;
	}
	/* 25 域*/
	sprintf(ps.content[25].data, "00", 2);
	ps.content[25].dataLen = 2;
	ps.content[25].is_exist = 1;
	/* 26域 */
	if (strlen(pan) != 0)
	{
		sprintf(ps.content[26].data, "12", 2);
		ps.content[26].dataLen = 2;
		ps.content[26].is_exist = 1;
	}
	/* 35 二磁 */
	if (strlen(trk2) != 0)
	{
		sprintf(ps.content[35].data, trk2, strlen(trk2));
		ps.content[35].dataLen = strlen(trk2);
		ps.content[35].is_exist = 1;
	}
	/* 36 三磁 */
	if (strlen(trk3) != 0)
	{
		sprintf(ps.content[35].data, trk3, strlen(trk3));
		ps.content[35].dataLen = strlen(trk3);
		ps.content[35].is_exist = 1;
	}

	/* 52 密码域 */
	if (strlen(pin_data) != 0)
	{
		/* TODO: 需要对密码加密 - 在common 中处理 TODO*/
		sprintf(ps.content[52].data, pin_data, strlen(pin_data));
		ps.content[52].dataLen = strlen(pin_data);
		ps.content[52].is_exist = 1;
	}
	/* 53 安全控制信息 */
	if (strlen(pin_data) != 0)
	{
		char *filed53 = "2600000000000000";
		sprintf(ps.content[53].data, filed53, strlen(filed53));
		ps.content[53].dataLen = strlen(filed53);
		ps.content[53].is_exist = 1;
	}
	/* 55 域 IC卡数据域*/
	if (strlen(ic_data) != 0)
	{
		/* TODO: 需要对密码加密 - 在common 中处理 TODO*/
		sprintf(ps.content[55].data, ic_data, strlen(ic_data));
		ps.content[55].dataLen = strlen(ic_data)/2;
		ps.content[55].is_exist = 1;
	}
	/* 写入56域 */
	char temp56[600];
	memset(temp56, 0, sizeof(temp56));
	INNER_PACK56(mchTransId, cretiId, orgMchTransId, temp56);
	memcpy(ps.content[56].data, temp56, strlen(temp56));
	ps.content[56].dataLen = strlen(temp56) / 2;
	ps.content[56].is_exist = 1;//TODO:change there 
	/* 写入批次 60 域 */
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
	/* 对支付返回值进行处理 */

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

/* 银联消费冲正*/
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
	/* 开始组建 银联交易报文 */
	PACKSOURCE ps;
	ISO_FILEDS_INIT(&ps);
	char *msgTypePay = "0700";
	memcpy(ps.content[0].data, msgTypePay, strlen(msgTypePay));
	ps.content[0].dataLen = strlen(msgTypePay);
	ps.content[0].is_exist = 1;

	/* 如果 PAN 为空 由二磁截取 PAN .如果PAN不为空,不取PAN.使用原PAN */
	if (strlen(pan) == 0)
	{
		/* pan 为空 trk2不为空*/
		if (strlen(trk2) != 0)
		{
			/* 由 二磁截取 PAN */
			char trk2temp[60];
			memset(trk2temp, 0, sizeof(trk2temp));
			memcpy(trk2temp, trk2, strlen(trk2));
			//开始根据分隔符截取卡号. " d" 或者 "D" 分隔
			char *pantemp = NULL;
			pantemp = strtok(trk2temp, "d,D");
			if (pantemp != NULL)
			{
				memcpy(pan, pantemp, strlen(pantemp));
			}
		}
	}
	/* 取 pan 完毕*/
	/* 二域  卡号信息 */
	if (strlen(pan) != 0)
	{
		ps.content[2].is_exist = 1;
		memcpy(ps.content[2].data, pan, strlen(pan));
		ps.content[2].dataLen = strlen(pan);
	}
	/* 交易处理*/
	char *processCodePay = "000012";
	memcpy(ps.content[3].data, processCodePay, strlen(processCodePay));
	ps.content[3].dataLen = strlen(processCodePay);
	ps.content[3].is_exist = 1;
	/* 金额 */
	sprintf(ps.content[4].data, "%012d", txnAmt);
	ps.content[4].dataLen = 12;
	ps.content[4].is_exist = 1;
	/* 14 域 卡有效期*/
	if (strlen(exp_date) != 0)
	{
		sprintf(ps.content[14].data, exp_date, strlen(exp_date));
		ps.content[14].dataLen = strlen(exp_date);
		ps.content[14].is_exist = 1;
	}
	/* 22 域 服务点输入方式*/
	char field22[4];
	memset(field22, 0, sizeof(field22));
	strcpy(field22, entCode);
	if (strlen(pin_data) == 0)																/* 密码域为空 ,选择无密码方式*/
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
	/* 23 域 卡序列号*/
	if (strlen(csn) != 0)
	{
		sprintf(ps.content[23].data, csn, strlen(csn));
		ps.content[23].dataLen = strlen(csn);
		ps.content[23].is_exist = 1;
	}
	/* 25 域*/
	sprintf(ps.content[25].data, "00", 2);
	ps.content[25].dataLen = 2;
	ps.content[25].is_exist = 1;
	/* 26域 */
	if (strlen(pan) != 0)
	{
		sprintf(ps.content[26].data, "12", 2);
		ps.content[26].dataLen = 2;
		ps.content[26].is_exist = 1;
	}
	/* 35 二磁 */
	if (strlen(trk2) != 0)
	{
		sprintf(ps.content[35].data, trk2, strlen(trk2));
		ps.content[35].dataLen = strlen(trk2);
		ps.content[35].is_exist = 1;
	}
	/* 36 三磁 */
	if (strlen(trk3) != 0)
	{
		sprintf(ps.content[35].data, trk3, strlen(trk3));
		ps.content[35].dataLen = strlen(trk3);
		ps.content[35].is_exist = 1;
	}
	/* 39域 冲正原因 写死06 */
	sprintf(ps.content[39].data, "06", 2);
	ps.content[39].dataLen = 2;
	ps.content[39].is_exist = 1;
	/* 52 密码域 */
	if (strlen(pin_data) != 0)
	{
		/* TODO: 需要对密码加密 - 在common 中处理 TODO*/
		sprintf(ps.content[52].data, pin_data, strlen(pin_data));
		ps.content[52].dataLen = strlen(pin_data);
		ps.content[52].is_exist = 1;
	}
	/* 53 安全控制信息 */
	if (strlen(pin_data) != 0)
	{
		char *filed53 = "2600000000000000";
		sprintf(ps.content[53].data, filed53, strlen(filed53));
		ps.content[53].dataLen = strlen(filed53);
		ps.content[53].is_exist = 1;
	}
	/* 55 域 IC卡数据域*/
	if (strlen(ic_data) != 0)
	{
		/* TODO: 需要对密码加密 - 在common 中处理 TODO*/
		sprintf(ps.content[55].data, ic_data, strlen(ic_data));
		ps.content[55].dataLen = strlen(ic_data) / 2;
		ps.content[55].is_exist = 1;
	}
	/* 写入56域 */
	char temp56[600];
	memset(temp56, 0, sizeof(temp56));
	INNER_PACK56(mchTransId, cretiId, orgMchTransId, temp56);
	memcpy(ps.content[56].data, temp56, strlen(temp56));
	ps.content[56].dataLen = strlen(temp56) / 2;
	ps.content[56].is_exist = 1;//TODO:change there 
	/* 写入批次 60 域 */
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
	/* 对支付返回值进行处理 */
	//if (strlen(tr.respCode) == 0)
	//{
	//	memcpy(tr.respCode, "98", strlen("98"));
	//}
	ret = INNER_packRetJson(tr.respCode, tr.mchTransETime, tr.transId,
		tr.accInfo, tr.txnAmt, tr.cretiId, tr.cusInfo, tr.mchTransId, out);
	return 0;
}

/* 银联消费退货*/
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
	/* 开始组建 银联交易报文 */
	PACKSOURCE ps;
	ISO_FILEDS_INIT(&ps);
	char *msgTypePay = "0720";
	memcpy(ps.content[0].data, msgTypePay, strlen(msgTypePay));
	ps.content[0].dataLen = strlen(msgTypePay);
	ps.content[0].is_exist = 1;

	/* 如果 PAN 为空 由二磁截取 PAN .如果PAN不为空,不取PAN.使用原PAN */
	if (strlen(pan) == 0)
	{
		/* pan 为空 trk2不为空*/
		if (strlen(trk2) != 0)
		{
			/* 由 二磁截取 PAN */
			char trk2temp[60];
			memset(trk2temp, 0, sizeof(trk2temp));
			memcpy(trk2temp, trk2, strlen(trk2));
			//开始根据分隔符截取卡号. " d" 或者 "D" 分隔
			char *pantemp = NULL;
			pantemp = strtok(trk2temp, "d,D");
			if (pantemp != NULL)
			{
				memcpy(pan, pantemp, strlen(pantemp));
			}
		}
	}
	/* 取 pan 完毕*/
	/* 二域  卡号信息 */
	if (strlen(pan) != 0)
	{
		ps.content[2].is_exist = 1;
		memcpy(ps.content[2].data, pan, strlen(pan));
		ps.content[2].dataLen = strlen(pan);
	}
	/* 交易处理*/
	char *processCodePay = "200125";
	memcpy(ps.content[3].data, processCodePay, strlen(processCodePay));
	ps.content[3].dataLen = strlen(processCodePay);
	ps.content[3].is_exist = 1;
	/* 金额 */
	sprintf(ps.content[4].data, "%012d", txnAmt);
	ps.content[4].dataLen = 12;
	ps.content[4].is_exist = 1;
	/* 14 域 卡有效期*/
	if (strlen(exp_date) != 0)
	{
		sprintf(ps.content[14].data, exp_date, strlen(exp_date));
		ps.content[14].dataLen = strlen(exp_date);
		ps.content[14].is_exist = 1;
	}
	/* 22 域 服务点输入方式*/
	char field22[4];
	memset(field22, 0, sizeof(field22));
	strcpy(field22, entCode);
	if (strlen(pin_data) == 0)																/* 密码域为空 ,选择无密码方式*/
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
	/* 23 域 卡序列号*/
	if (strlen(csn) != 0)
	{
		sprintf(ps.content[23].data, csn, strlen(csn));
		ps.content[23].dataLen = strlen(csn);
		ps.content[23].is_exist = 1;
	}
	/* 25 域*/
	sprintf(ps.content[25].data, "00", 2);
	ps.content[25].dataLen = 2;
	ps.content[25].is_exist = 1;
	/* 26域 */
	if (strlen(pan) != 0)
	{
		sprintf(ps.content[26].data, "12", 2);
		ps.content[26].dataLen = 2;
		ps.content[26].is_exist = 1;
	}
	/* 35 二磁 */
	if (strlen(trk2) != 0)
	{
		sprintf(ps.content[35].data, trk2, strlen(trk2));
		ps.content[35].dataLen = strlen(trk2);
		ps.content[35].is_exist = 1;
	}
	/* 36 三磁 */
	if (strlen(trk3) != 0)
	{
		sprintf(ps.content[35].data, trk3, strlen(trk3));
		ps.content[35].dataLen = strlen(trk3);
		ps.content[35].is_exist = 1;
	}

	/* 52 密码域 */
	if (strlen(pin_data) != 0)
	{
		/* TODO: 需要对密码加密 - 在common 中处理 TODO*/
		sprintf(ps.content[52].data, pin_data, strlen(pin_data));
		ps.content[52].dataLen = strlen(pin_data);
		ps.content[52].is_exist = 1;
	}
	/* 53 安全控制信息 */
	if (strlen(pin_data) != 0)
	{
		char *filed53 = "2600000000000000";
		sprintf(ps.content[53].data, filed53, strlen(filed53));
		ps.content[53].dataLen = strlen(filed53);
		ps.content[53].is_exist = 1;
	}
	/* 55 域 IC卡数据域*/
	if (strlen(ic_data) != 0)
	{
		/* TODO: 需要对密码加密 - 在common 中处理 TODO*/
		sprintf(ps.content[55].data, ic_data, strlen(ic_data));
		ps.content[55].dataLen = strlen(ic_data) / 2;
		ps.content[55].is_exist = 1;
	}
	/* 写入56域 */
	char temp56[600];
	memset(temp56, 0, sizeof(temp56));
	INNER_PACK56(mchTransId, cretiId, orgMchTransId, temp56);
	memcpy(ps.content[56].data, temp56, strlen(temp56));
	ps.content[56].dataLen = strlen(temp56) / 2;
	ps.content[56].is_exist = 1;//TODO:change there 
	/* 写入批次 60 域 */
	char batch[7];
	memset(batch, 0, sizeof(batch));
	char temp[20];
	memset(temp, 0, sizeof(temp));
	int batchint = 0;
	CFG_GetProfileString(CFG_BATCHNO, temp);
	batchint = atoi(temp);
	char filed60[20];
	memset(filed60, 0, sizeof(filed60));
	
	sprintf(filed60, "25%06d000501", batchint);												/* 退货 */
	sprintf(ps.content[60].data, filed60, strlen(filed60));
	ps.content[60].dataLen = strlen(filed60);
	ps.content[60].is_exist = 1;
	/* 63域  填全 00 */
	ps.content[63].dataLen = strlen("000");
	sprintf(ps.content[63].data, "000", ps.content[63].dataLen);
	ps.content[63].is_exist = 1;

	TRANS_RET tr;
	memset(&tr, 0, sizeof(TRANS_RET));
	UNPACKRESULT ur;
	memset(&ur, 0, sizeof(UNPACKRESULT));
	int ret = TRANS_COMMON(&ps, &tr, mKey, &ur);
	/* 对支付返回值进行处理 */
	//if (strlen(tr.respCode) == 0)
	//{
	//	memcpy(tr.respCode, "98", strlen("98"));
	//}
	ret = INNER_packRetJson(tr.respCode, tr.mchTransETime, tr.transId,
		tr.accInfo, tr.txnAmt, tr.cretiId, tr.cusInfo, tr.mchTransId, out);
	return 0;
}

/* 银联消费撤销*/
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
	/* 开始组建 银联交易报文 */
	PACKSOURCE ps;
	ISO_FILEDS_INIT(&ps);
	char *msgTypePay = "0700";
	memcpy(ps.content[0].data, msgTypePay, strlen(msgTypePay));
	ps.content[0].dataLen = strlen(msgTypePay);
	ps.content[0].is_exist = 1;

	/* 如果 PAN 为空 由二磁截取 PAN .如果PAN不为空,不取PAN.使用原PAN */
	if (strlen(pan) == 0)
	{
		/* pan 为空 trk2不为空*/
		if (strlen(trk2) != 0)
		{
			/* 由 二磁截取 PAN */
			char trk2temp[60];
			memset(trk2temp, 0, sizeof(trk2temp));
			memcpy(trk2temp, trk2, strlen(trk2));
			//开始根据分隔符截取卡号. " d" 或者 "D" 分隔
			char *pantemp = NULL;
			pantemp = strtok(trk2temp, "d,D");
			if (pantemp != NULL)
			{
				memcpy(pan, pantemp, strlen(pantemp));
			}
		}
	}
	/* 取 pan 完毕*/
	/* 二域  卡号信息 */
	if (strlen(pan) != 0)
	{
		ps.content[2].is_exist = 1;
		memcpy(ps.content[2].data, pan, strlen(pan));
		ps.content[2].dataLen = strlen(pan);
	}
	/* 3 交易处理*/
	char *processCodePay = "200014";
	memcpy(ps.content[3].data, processCodePay, strlen(processCodePay));
	ps.content[3].dataLen = strlen(processCodePay);
	ps.content[3].is_exist = 1;
	/* 4 金额 */
	sprintf(ps.content[4].data, "%012d", txnAmt);
	ps.content[4].dataLen = 12;
	ps.content[4].is_exist = 1;
	/* 14 域 卡有效期*/
	if (strlen(exp_date) != 0)
	{
		sprintf(ps.content[14].data, exp_date, strlen(exp_date));
		ps.content[14].dataLen = strlen(exp_date);
		ps.content[14].is_exist = 1;
	}
	/* 22 域 服务点输入方式*/
	char field22[4];
	memset(field22, 0, sizeof(field22));
	strcpy(field22, entCode);
	if (strlen(pin_data) == 0)																/* 密码域为空 ,选择无密码方式*/
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
	/* 23 域 卡序列号*/
	if (strlen(csn) != 0)
	{
		sprintf(ps.content[23].data, csn, strlen(csn));
		ps.content[23].dataLen = strlen(csn);
		ps.content[23].is_exist = 1;
	}
	/* 25 域*/
	sprintf(ps.content[25].data, "00", 2);
	ps.content[25].dataLen = 2;
	ps.content[25].is_exist = 1;
	/* 26域 */
	if (strlen(pan) != 0)
	{
		sprintf(ps.content[26].data, "12", 2);
		ps.content[26].dataLen = 2;
		ps.content[26].is_exist = 1;
	}
	/* 35 二磁 */
	if (strlen(trk2) != 0)
	{
		sprintf(ps.content[35].data, trk2, strlen(trk2));
		ps.content[35].dataLen = strlen(trk2);
		ps.content[35].is_exist = 1;
	}
	/* 36 三磁 */
	if (strlen(trk3) != 0)
	{
		sprintf(ps.content[35].data, trk3, strlen(trk3));
		ps.content[35].dataLen = strlen(trk3);
		ps.content[35].is_exist = 1;
	}
	
	/* 52 密码域 */
	if (strlen(pin_data) != 0)
	{
		/* TODO: 需要对密码加密 - 在common 中处理 TODO*/
		sprintf(ps.content[52].data, pin_data, strlen(pin_data));
		ps.content[52].dataLen = strlen(pin_data);
		ps.content[52].is_exist = 1;
	}
	/* 53 安全控制信息 */
	if (strlen(pin_data) != 0)
	{
		char *filed53 = "2600000000000000";
		sprintf(ps.content[53].data, filed53, strlen(filed53));
		ps.content[53].dataLen = strlen(filed53);
		ps.content[53].is_exist = 1;
	}
	/* 55 域 IC卡数据域*/
	if (strlen(ic_data) != 0)
	{
		/* TODO: 需要对密码加密 - 在common 中处理 TODO*/
		sprintf(ps.content[55].data, ic_data, strlen(ic_data));
		ps.content[55].dataLen = strlen(ic_data) / 2;
		ps.content[55].is_exist = 1;
	}
	/* 写入56域 */
	char temp56[600];
	memset(temp56, 0, sizeof(temp56));
	INNER_PACK56(mchTransId, cretiId, orgMchTransId, temp56);
	memcpy(ps.content[56].data, temp56, strlen(temp56));
	ps.content[56].dataLen = strlen(temp56) / 2;
	ps.content[56].is_exist = 1;//TODO:change there 
	/* 写入批次 60 域 */
	char batch[7];
	memset(batch, 0, sizeof(batch));
	char temp[20];
	memset(temp, 0, sizeof(temp));
	int batchint = 0;
	CFG_GetProfileString(CFG_BATCHNO, temp);
	batchint = atoi(temp);
	char filed60[20];
	memset(filed60, 0, sizeof(filed60));

	sprintf(filed60, "23%06d000501", batchint);												/* 退货 */
	sprintf(ps.content[60].data, filed60, strlen(filed60));
	ps.content[60].dataLen = strlen(filed60);
	ps.content[60].is_exist = 1;
	/* 63域  填全 00 */
	ps.content[63].dataLen = strlen("000");
	sprintf(ps.content[63].data, "000", ps.content[63].dataLen);
	ps.content[63].is_exist = 1;

	TRANS_RET tr;
	memset(&tr, 0, sizeof(TRANS_RET));
	UNPACKRESULT ur;
	memset(&ur, 0, sizeof(UNPACKRESULT));
	int ret = TRANS_COMMON(&ps, &tr, mKey, &ur);
	/* 对支付返回值进行处理 */
	//if (strlen(tr.respCode) == 0)
	//{
	//	memcpy(tr.respCode, "98", strlen("98"));
	//}
	ret = INNER_packRetJson(tr.respCode, tr.mchTransETime, tr.transId,
		tr.accInfo, tr.txnAmt, tr.cretiId, tr.cusInfo, tr.mchTransId, out);
	return 0;
}

/* 银联卡余额查询*/
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
	/* 开始组建 银联交易报文 */
	PACKSOURCE ps;
	ISO_FILEDS_INIT(&ps);
	char *msgTypePay = "0200";
	memcpy(ps.content[0].data, msgTypePay, strlen(msgTypePay));
	ps.content[0].dataLen = strlen(msgTypePay);
	ps.content[0].is_exist = 1;

	/* 如果 PAN 为空 由二磁截取 PAN .如果PAN不为空,不取PAN.使用原PAN */
	if (strlen(pan) == 0)
	{
		/* pan 为空 trk2不为空*/
		if (strlen(trk2) != 0)
		{
			/* 由 二磁截取 PAN */
			char trk2temp[60];
			memset(trk2temp, 0, sizeof(trk2temp));
			memcpy(trk2temp, trk2, strlen(trk2));
			//开始根据分隔符截取卡号. " d" 或者 "D" 分隔
			char *pantemp = NULL;
			pantemp = strtok(trk2temp, "d,D");
			if (pantemp != NULL)
			{
				memcpy(pan, pantemp, strlen(pantemp));
			}
		}
	}
	/* 取 pan 完毕*/
	/* 二域  卡号信息 */
	if (strlen(pan) != 0)
	{
		ps.content[2].is_exist = 1;
		memcpy(ps.content[2].data, pan, strlen(pan));
		ps.content[2].dataLen = strlen(pan);
	}
	/* 3 交易处理*/
	char *processCodePay = "310000";
	memcpy(ps.content[3].data, processCodePay, strlen(processCodePay));
	ps.content[3].dataLen = strlen(processCodePay);
	ps.content[3].is_exist = 1;
	/* 4 金额 */
	sprintf(ps.content[4].data, "%012d", txnAmt);
	ps.content[4].dataLen = 12;
	ps.content[4].is_exist = 1;
	/* 14 域 卡有效期*/
	if (strlen(exp_date) != 0)
	{
		sprintf(ps.content[14].data, exp_date, strlen(exp_date));
		ps.content[14].dataLen = strlen(exp_date);
		ps.content[14].is_exist = 1;
	}
	/* 22 域 服务点输入方式*/
	char field22[4];
	memset(field22, 0, sizeof(field22));
	strcpy(field22, entCode);
	if (strlen(pin_data) == 0)																/* 密码域为空 ,选择无密码方式*/
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
	/* 23 域 卡序列号*/
	if (strlen(csn) != 0)
	{
		sprintf(ps.content[23].data, csn, strlen(csn));
		ps.content[23].dataLen = strlen(csn);
		ps.content[23].is_exist = 1;
	}
	/* 25 域*/
	sprintf(ps.content[25].data, "00", 2);
	ps.content[25].dataLen = 2;
	ps.content[25].is_exist = 1;
	/* 26域 */
	if (strlen(pan) != 0)
	{
		sprintf(ps.content[26].data, "12", 2);
		ps.content[26].dataLen = 2;
		ps.content[26].is_exist = 1;
	}
	/* 35 二磁 */
	if (strlen(trk2) != 0)
	{
		sprintf(ps.content[35].data, trk2, strlen(trk2));
		ps.content[35].dataLen = strlen(trk2);
		ps.content[35].is_exist = 1;
	}
	/* 36 三磁 */
	if (strlen(trk3) != 0)
	{
		sprintf(ps.content[35].data, trk3, strlen(trk3));
		ps.content[35].dataLen = strlen(trk3);
		ps.content[35].is_exist = 1;
	}

	/* 52 密码域 */
	if (strlen(pin_data) != 0)
	{
		/* TODO: 需要对密码加密 - 在common 中处理 TODO*/
		sprintf(ps.content[52].data, pin_data, strlen(pin_data));
		ps.content[52].dataLen = strlen(pin_data);
		ps.content[52].is_exist = 1;
	}
	/* 53 安全控制信息 */
	if (strlen(pin_data) != 0)
	{
		char *filed53 = "2600000000000000";
		sprintf(ps.content[53].data, filed53, strlen(filed53));
		ps.content[53].dataLen = strlen(filed53);
		ps.content[53].is_exist = 1;
	}
	/* 55 域 IC卡数据域*/
	if (strlen(ic_data) != 0)
	{
		/* TODO: 需要对密码加密 - 在common 中处理 TODO*/
		sprintf(ps.content[55].data, ic_data, strlen(ic_data));
		ps.content[55].dataLen = strlen(ic_data) / 2;
		ps.content[55].is_exist = 1;
	}
	/* 写入56域 */
/*	char temp56[600];
	memset(temp56, 0, sizeof(temp56));
	INNER_PACK56(mchTransId, cretiId, orgMchTransId, temp56);
	memcpy(ps.content[56].data, temp56, strlen(temp56));
	ps.content[56].dataLen = strlen(temp56) / 2;
	ps.content[56].is_exist = 1;//TODO:change there   */
	/* 写入批次 60 域 */
	char batch[7];
	memset(batch, 0, sizeof(batch));
	char temp[20];
	memset(temp, 0, sizeof(temp));
	int batchint = 0;
	CFG_GetProfileString(CFG_BATCHNO, temp);
	batchint = atoi(temp);
	char filed60[20];
	memset(filed60, 0, sizeof(filed60));

	sprintf(filed60, "01%06d000501", batchint);												/* 退货 */
	sprintf(ps.content[60].data, filed60, strlen(filed60));
	ps.content[60].dataLen = strlen(filed60);
	ps.content[60].is_exist = 1;
	/* 63域  填全 00 */
	ps.content[63].dataLen = strlen("000");
	sprintf(ps.content[63].data, "000", ps.content[63].dataLen);
	ps.content[63].is_exist = 1;

	TRANS_RET tr;
	memset(&tr, 0, sizeof(TRANS_RET));
	UNPACKRESULT ur;
	memset(&ur, 0, sizeof(UNPACKRESULT));
	int ret = TRANS_COMMON(&ps, &tr, mKey, &ur);
	/* 对支付返回值进行处理 */
	//if (strlen(tr.respCode) == 0)
	//{
	//	memcpy(tr.respCode, "98", strlen("98"));
	//}
	/* 获取余额 单位分 */
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

/* 银联卡预授权*/
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
	/* 开始组建 银联交易报文 */
	PACKSOURCE ps;
	ISO_FILEDS_INIT(&ps);
	char *msgTypePay = "0100";
	memcpy(ps.content[0].data, msgTypePay, strlen(msgTypePay));
	ps.content[0].dataLen = strlen(msgTypePay);
	ps.content[0].is_exist = 1;

	/* 如果 PAN 为空 由二磁截取 PAN .如果PAN不为空,不取PAN.使用原PAN */
	if (strlen(pan) == 0)
	{
		/* pan 为空 trk2不为空*/
		if (strlen(trk2) != 0)
		{
			/* 由 二磁截取 PAN */
			char trk2temp[60];
			memset(trk2temp, 0, sizeof(trk2temp));
			memcpy(trk2temp, trk2, strlen(trk2));
			//开始根据分隔符截取卡号. " d" 或者 "D" 分隔
			char *pantemp = NULL;
			pantemp = strtok(trk2temp, "d,D");
			if (pantemp != NULL)
			{
				memcpy(pan, pantemp, strlen(pantemp));
			}
		}
	}
	/* 取 pan 完毕*/
	/* 二域  卡号信息 */
	if (strlen(pan) != 0)
	{
		ps.content[2].is_exist = 1;
		memcpy(ps.content[2].data, pan, strlen(pan));
		ps.content[2].dataLen = strlen(pan);
	}
	/* 3 交易处理*/
	char *processCodePay = "030016";
	memcpy(ps.content[3].data, processCodePay, strlen(processCodePay));
	ps.content[3].dataLen = strlen(processCodePay);
	ps.content[3].is_exist = 1;
	/* 4 金额 */
	sprintf(ps.content[4].data, "%012d", txnAmt);
	ps.content[4].dataLen = 12;
	ps.content[4].is_exist = 1;
	/* 14 域 卡有效期*/
	if (strlen(exp_date) != 0)
	{
		sprintf(ps.content[14].data, exp_date, strlen(exp_date));
		ps.content[14].dataLen = strlen(exp_date);
		ps.content[14].is_exist = 1;
	}
	/* 22 域 服务点输入方式*/
	char field22[4];
	memset(field22, 0, sizeof(field22));
	strcpy(field22, entCode);
	if (strlen(pin_data) == 0)																/* 密码域为空 ,选择无密码方式*/
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
	/* 23 域 卡序列号*/
	if (strlen(csn) != 0)
	{
		sprintf(ps.content[23].data, "%04s", csn);
		ps.content[23].dataLen = strlen(csn);
		ps.content[23].is_exist = 1;
	}
	/* 25 域*/
	sprintf(ps.content[25].data, "06", 2);
	ps.content[25].dataLen = 2;
	ps.content[25].is_exist = 1;
	/* 26域 */
	if (strlen(pan) != 0)
	{
		sprintf(ps.content[26].data, "12", 2);
		ps.content[26].dataLen = 2;
		ps.content[26].is_exist = 1;
	}
	/* 35 二磁 */
	if (strlen(trk2) != 0)
	{
		sprintf(ps.content[35].data, trk2, strlen(trk2));
		ps.content[35].dataLen = strlen(trk2);
		ps.content[35].is_exist = 1;
	}
	/* 36 三磁 */
	if (strlen(trk3) != 0)
	{
		sprintf(ps.content[35].data, trk3, strlen(trk3));
		ps.content[35].dataLen = strlen(trk3);
		ps.content[35].is_exist = 1;
	}

	/* 52 密码域 */
	if (strlen(pin_data) != 0)
	{
		/* TODO: 需要对密码加密 - 在common 中处理 TODO*/
		sprintf(ps.content[52].data, pin_data, strlen(pin_data));
		ps.content[52].dataLen = strlen(pin_data);
		ps.content[52].is_exist = 1;
	}
	/* 53 安全控制信息 */
	if (strlen(pin_data) != 0)
	{
		char *filed53 = "2600000000000000";
		sprintf(ps.content[53].data, filed53, strlen(filed53));
		ps.content[53].dataLen = strlen(filed53);
		ps.content[53].is_exist = 1;
	}
	/* 55 域 IC卡数据域*/
	if (strlen(ic_data) != 0)
	{
		/* TODO: 需要对密码加密 - 在common 中处理 TODO*/
		sprintf(ps.content[55].data, ic_data, strlen(ic_data));
		ps.content[55].dataLen = strlen(ic_data) / 2;
		ps.content[55].is_exist = 1;
	}
	/* 写入56域 */
	char temp56[600];
	memset(temp56, 0, sizeof(temp56));
	INNER_PACK56(mchTransId, cretiId, orgMchTransId, temp56);
	memcpy(ps.content[56].data, temp56, strlen(temp56));
	ps.content[56].dataLen = strlen(temp56) / 2;
	ps.content[56].is_exist = 1;//TODO:change there 
	/* 写入批次 60 域 */
	char batch[7];
	memset(batch, 0, sizeof(batch));
	char temp[20];
	memset(temp, 0, sizeof(temp));
	int batchint = 0;
	CFG_GetProfileString(CFG_BATCHNO, temp);
	batchint = atoi(temp);
	char filed60[20];
	memset(filed60, 0, sizeof(filed60));

	sprintf(filed60, "10%06d000501", batchint);												/* 预授权 */
	sprintf(ps.content[60].data, filed60, strlen(filed60));
	ps.content[60].dataLen = strlen(filed60);
	ps.content[60].is_exist = 1;
	/* 63域  填全 00 */
	ps.content[63].dataLen = strlen("000");
	sprintf(ps.content[63].data, "000", ps.content[63].dataLen);
	ps.content[63].is_exist = 1;

	TRANS_RET tr;
	memset(&tr, 0, sizeof(TRANS_RET));
	UNPACKRESULT ur;
	memset(&ur, 0, sizeof(UNPACKRESULT));
	int ret = TRANS_COMMON(&ps, &tr, mKey, &ur);
	/* 对支付返回值进行处理 */
	//if (strlen(tr.respCode) == 0)
	//{
	//	memcpy(tr.respCode, "98", strlen("98"));
	//}
	ret = INNER_packRetJson(tr.respCode, tr.mchTransETime, tr.transId,
		tr.accInfo, tr.txnAmt, tr.cretiId, tr.cusInfo, tr.mchTransId, out);
	return 0;
}

/* 银联卡预授权冲正*/
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
	/* 开始组建 银联交易报文 */
	PACKSOURCE ps;
	ISO_FILEDS_INIT(&ps);
	char *msgTypePay = "0700";
	memcpy(ps.content[0].data, msgTypePay, strlen(msgTypePay));
	ps.content[0].dataLen = strlen(msgTypePay);
	ps.content[0].is_exist = 1;

	/* 如果 PAN 为空 由二磁截取 PAN .如果PAN不为空,不取PAN.使用原PAN */
	if (strlen(pan) == 0)
	{
		/* pan 为空 trk2不为空*/
		if (strlen(trk2) != 0)
		{
			/* 由 二磁截取 PAN */
			char trk2temp[60];
			memset(trk2temp, 0, sizeof(trk2temp));
			memcpy(trk2temp, trk2, strlen(trk2));
			//开始根据分隔符截取卡号. " d" 或者 "D" 分隔
			char *pantemp = NULL;
			pantemp = strtok(trk2temp, "d,D");
			if (pantemp != NULL)
			{
				memcpy(pan, pantemp, strlen(pantemp));
			}
		}
	}
	/* 取 pan 完毕*/
	/* 二域  卡号信息 */
	if (strlen(pan) != 0)
	{
		ps.content[2].is_exist = 1;
		memcpy(ps.content[2].data, pan, strlen(pan));
		ps.content[2].dataLen = strlen(pan);
	}
	/* 交易处理*/
	char *processCodePay = "030016";
	memcpy(ps.content[3].data, processCodePay, strlen(processCodePay));
	ps.content[3].dataLen = strlen(processCodePay);
	ps.content[3].is_exist = 1;
	/* 金额 */
	sprintf(ps.content[4].data, "%012d", txnAmt);
	ps.content[4].dataLen = 12;
	ps.content[4].is_exist = 1;
	/* 14 域 卡有效期*/
	if (strlen(exp_date) != 0)
	{
		sprintf(ps.content[14].data, exp_date, strlen(exp_date));
		ps.content[14].dataLen = strlen(exp_date);
		ps.content[14].is_exist = 1;
	}
	/* 22 域 服务点输入方式*/
	char field22[4];
	memset(field22, 0, sizeof(field22));
	strcpy(field22, entCode);
	if (strlen(pin_data) == 0)																/* 密码域为空 ,选择无密码方式*/
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
	/* 23 域 卡序列号*/
	if (strlen(csn) != 0)
	{
		sprintf(ps.content[23].data, "%04s", csn);
		ps.content[23].dataLen = strlen(csn);
		ps.content[23].is_exist = 1;
	}
	/* 25 域*/
	sprintf(ps.content[25].data, "06", 2);
	ps.content[25].dataLen = 2;
	ps.content[25].is_exist = 1;
	/* 26域 */
	if (strlen(pan) != 0)
	{
		sprintf(ps.content[26].data, "12", 2);
		ps.content[26].dataLen = 2;
		ps.content[26].is_exist = 1;
	}
	/* 35 二磁 */
	if (strlen(trk2) != 0)
	{
		sprintf(ps.content[35].data, trk2, strlen(trk2));
		ps.content[35].dataLen = strlen(trk2);
		ps.content[35].is_exist = 1;
	}
	/* 36 三磁 */
	if (strlen(trk3) != 0)
	{
		sprintf(ps.content[35].data, trk3, strlen(trk3));
		ps.content[35].dataLen = strlen(trk3);
		ps.content[35].is_exist = 1;
	}
	/* 39域 冲正原因 写死06 */
	sprintf(ps.content[39].data, "98", 2);
	ps.content[39].dataLen = 2;
	ps.content[39].is_exist = 1;
	/* 52 密码域 */
	//if (strlen(pin_data) != 0)
	//{
	//	/* TODO: 需要对密码加密 - 在common 中处理 TODO*/
	//	sprintf(ps.content[52].data, pin_data, strlen(pin_data));
	//	ps.content[52].dataLen = strlen(pin_data);
	//	ps.content[52].is_exist = 1;
	//}
	/* 53 安全控制信息 */
	//if (strlen(pin_data) != 0)
	//{
	//	char *filed53 = "2600000000000000";
	//	sprintf(ps.content[53].data, filed53, strlen(filed53));
	//	ps.content[53].dataLen = strlen(filed53);
	//	ps.content[53].is_exist = 1;
	//}
	/* 55 域 IC卡数据域*/
	if (strlen(ic_data) != 0)
	{
		/* TODO: 需要对密码加密 - 在common 中处理 TODO*/
		sprintf(ps.content[55].data, ic_data, strlen(ic_data));
		ps.content[55].dataLen = strlen(ic_data) / 2;
		ps.content[55].is_exist = 1;
	}
	/* 写入56域 */
	char temp56[600];
	memset(temp56, 0, sizeof(temp56));
	INNER_PACK56(mchTransId, cretiId, orgMchTransId, temp56);
	memcpy(ps.content[56].data, temp56, strlen(temp56));
	ps.content[56].dataLen = strlen(temp56) / 2;
	ps.content[56].is_exist = 1;//TODO:change there 
	/* 写入批次 60 域 */
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
	/* 对支付返回值进行处理 */
	//if (strlen(tr.respCode) == 0)
	//{
	//	memcpy(tr.respCode, "98", strlen("98"));
	//}
	ret = INNER_packRetJson(tr.respCode, tr.mchTransETime, tr.transId,
		tr.accInfo, tr.txnAmt, tr.cretiId, tr.cusInfo, tr.mchTransId, out);
	return 0;
}
/* 银联卡预授权撤销*/
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
	/* 开始组建 银联交易报文 */
	PACKSOURCE ps;
	ISO_FILEDS_INIT(&ps);
	char *msgTypePay = "0700";
	memcpy(ps.content[0].data, msgTypePay, strlen(msgTypePay));
	ps.content[0].dataLen = strlen(msgTypePay);
	ps.content[0].is_exist = 1;

	/* 如果 PAN 为空 由二磁截取 PAN .如果PAN不为空,不取PAN.使用原PAN */
	if (strlen(pan) == 0)
	{
		/* pan 为空 trk2不为空*/
		if (strlen(trk2) != 0)
		{
			/* 由 二磁截取 PAN */
			char trk2temp[60];
			memset(trk2temp, 0, sizeof(trk2temp));
			memcpy(trk2temp, trk2, strlen(trk2));
			//开始根据分隔符截取卡号. " d" 或者 "D" 分隔
			char *pantemp = NULL;
			pantemp = strtok(trk2temp, "d,D");
			if (pantemp != NULL)
			{
				memcpy(pan, pantemp, strlen(pantemp));
			}
		}
	}
	/* 取 pan 完毕*/
	/* 二域  卡号信息 */
	if (strlen(pan) != 0)
	{
		ps.content[2].is_exist = 1;
		memcpy(ps.content[2].data, pan, strlen(pan));
		ps.content[2].dataLen = strlen(pan);
	}
	/* 3 交易处理*/
	char *processCodePay = "200017";
	memcpy(ps.content[3].data, processCodePay, strlen(processCodePay));
	ps.content[3].dataLen = strlen(processCodePay);
	ps.content[3].is_exist = 1;
	/* 4 金额 */
	sprintf(ps.content[4].data, "%012d", txnAmt);
	ps.content[4].dataLen = 12;
	ps.content[4].is_exist = 1;
	/* 14 域 卡有效期*/
	if (strlen(exp_date) != 0)
	{
		sprintf(ps.content[14].data, exp_date, strlen(exp_date));
		ps.content[14].dataLen = strlen(exp_date);
		ps.content[14].is_exist = 1;
	}
	/* 22 域 服务点输入方式*/
	char field22[4];
	memset(field22, 0, sizeof(field22));
	strcpy(field22, entCode);
	if (strlen(pin_data) == 0)																/* 密码域为空 ,选择无密码方式*/
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
	/* 23 域 卡序列号*/
	if (strlen(csn) != 0)
	{
		sprintf(ps.content[23].data, csn, strlen(csn));
		ps.content[23].dataLen = strlen(csn);
		ps.content[23].is_exist = 1;
	}
	/* 25 域*/
	sprintf(ps.content[25].data, "06", 2);
	ps.content[25].dataLen = 2;
	ps.content[25].is_exist = 1;
	/* 26域 */
	if (strlen(pan) != 0)
	{
		sprintf(ps.content[26].data, "12", 2);
		ps.content[26].dataLen = 2;
		ps.content[26].is_exist = 1;
	}
	/* 35 二磁 */
	if (strlen(trk2) != 0)
	{
		sprintf(ps.content[35].data, trk2, strlen(trk2));
		ps.content[35].dataLen = strlen(trk2);
		ps.content[35].is_exist = 1;
	}
	/* 36 三磁 */
	if (strlen(trk3) != 0)
	{
		sprintf(ps.content[35].data, trk3, strlen(trk3));
		ps.content[35].dataLen = strlen(trk3);
		ps.content[35].is_exist = 1;
	}

	/* 52 密码域 */
	if (strlen(pin_data) != 0)
	{
		/* TODO: 需要对密码加密 - 在common 中处理 TODO*/
		sprintf(ps.content[52].data, pin_data, strlen(pin_data));
		ps.content[52].dataLen = strlen(pin_data);
		ps.content[52].is_exist = 1;
	}
	/* 53 安全控制信息 */
	if (strlen(pin_data) != 0)
	{
		char *filed53 = "2600000000000000";
		sprintf(ps.content[53].data, filed53, strlen(filed53));
		ps.content[53].dataLen = strlen(filed53);
		ps.content[53].is_exist = 1;
	}
	/* 55 域 IC卡数据域*/
	if (strlen(ic_data) != 0)
	{
		/* TODO: 需要对密码加密 - 在common 中处理 TODO*/
		sprintf(ps.content[55].data, ic_data, strlen(ic_data));
		ps.content[55].dataLen = strlen(ic_data) / 2;
		ps.content[55].is_exist = 1;
	}
	/* 写入56域 */
	char temp56[600];
	memset(temp56, 0, sizeof(temp56));
	INNER_PACK56(mchTransId, cretiId, orgMchTransId, temp56);
	memcpy(ps.content[56].data, temp56, strlen(temp56));
	ps.content[56].dataLen = strlen(temp56) / 2;
	ps.content[56].is_exist = 1;//TODO:change there 
	/* 写入批次 60 域 */
	char batch[7];
	memset(batch, 0, sizeof(batch));
	char temp[20];
	memset(temp, 0, sizeof(temp));
	int batchint = 0;
	CFG_GetProfileString(CFG_BATCHNO, temp);
	batchint = atoi(temp);
	char filed60[20];
	memset(filed60, 0, sizeof(filed60));

	sprintf(filed60, "11%06d000501", batchint);												/* 退货 */
	sprintf(ps.content[60].data, filed60, strlen(filed60));
	ps.content[60].dataLen = strlen(filed60);
	ps.content[60].is_exist = 1;
	/* 63域  填全 00 */
	ps.content[63].dataLen = strlen("000");
	sprintf(ps.content[63].data, "000", ps.content[63].dataLen);
	ps.content[63].is_exist = 1;

	TRANS_RET tr;
	memset(&tr, 0, sizeof(TRANS_RET));
	UNPACKRESULT ur;
	memset(&ur, 0, sizeof(UNPACKRESULT));
	int ret = TRANS_COMMON(&ps, &tr, mKey, &ur);
	/* 对支付返回值进行处理 */
	//if (strlen(tr.respCode) == 0)
	//{
	//	memcpy(tr.respCode, "98", strlen("98"));
	//}
	ret = INNER_packRetJson(tr.respCode, tr.mchTransETime, tr.transId,
		tr.accInfo, tr.txnAmt, tr.cretiId, tr.cusInfo, tr.mchTransId, out);
	return 0;
}

/* 银联卡预授权完成*/
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
	/* 开始组建 银联交易报文 */
	PACKSOURCE ps;
	ISO_FILEDS_INIT(&ps);
	char *msgTypePay = "0700";
	memcpy(ps.content[0].data, msgTypePay, strlen(msgTypePay));
	ps.content[0].dataLen = strlen(msgTypePay);
	ps.content[0].is_exist = 1;

	/* 如果 PAN 为空 由二磁截取 PAN .如果PAN不为空,不取PAN.使用原PAN */
	if (strlen(pan) == 0)
	{
		/* pan 为空 trk2不为空*/
		if (strlen(trk2) != 0)
		{
			/* 由 二磁截取 PAN */
			char trk2temp[60];
			memset(trk2temp, 0, sizeof(trk2temp));
			memcpy(trk2temp, trk2, strlen(trk2));
			//开始根据分隔符截取卡号. " d" 或者 "D" 分隔
			char *pantemp = NULL;
			pantemp = strtok(trk2temp, "d,D");
			if (pantemp != NULL)
			{
				memcpy(pan, pantemp, strlen(pantemp));
			}
		}
	}
	/* 取 pan 完毕*/
	/* 二域  卡号信息 */
	if (strlen(pan) != 0)
	{
		ps.content[2].is_exist = 1;
		memcpy(ps.content[2].data, pan, strlen(pan));
		ps.content[2].dataLen = strlen(pan);
	}
	/* 3 交易处理*/
	char *processCodePay = "000018";
	memcpy(ps.content[3].data, processCodePay, strlen(processCodePay));
	ps.content[3].dataLen = strlen(processCodePay);
	ps.content[3].is_exist = 1;
	/* 4 金额 */
	sprintf(ps.content[4].data, "%012d", txnAmt);
	ps.content[4].dataLen = 12;
	ps.content[4].is_exist = 1;
	/* 14 域 卡有效期*/
	if (strlen(exp_date) != 0)
	{
		sprintf(ps.content[14].data, exp_date, strlen(exp_date));
		ps.content[14].dataLen = strlen(exp_date);
		ps.content[14].is_exist = 1;
	}
	/* 22 域 服务点输入方式*/
	char field22[4];
	memset(field22, 0, sizeof(field22));
	strcpy(field22, entCode);
	if (strlen(pin_data) == 0)																/* 密码域为空 ,选择无密码方式*/
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
	/* 23 域 卡序列号*/
	if (strlen(csn) != 0)
	{
		sprintf(ps.content[23].data, csn, strlen(csn));
		ps.content[23].dataLen = strlen(csn);
		ps.content[23].is_exist = 1;
	}
	/* 25 域*/
	sprintf(ps.content[25].data, "06", 2);
	ps.content[25].dataLen = 2;
	ps.content[25].is_exist = 1;
	/* 26域 */
	if (strlen(pan) != 0)
	{
		sprintf(ps.content[26].data, "12", 2);
		ps.content[26].dataLen = 2;
		ps.content[26].is_exist = 1;
	}
	/* 35 二磁 */
	if (strlen(trk2) != 0)
	{
		sprintf(ps.content[35].data, trk2, strlen(trk2));
		ps.content[35].dataLen = strlen(trk2);
		ps.content[35].is_exist = 1;
	}
	/* 36 三磁 */
	if (strlen(trk3) != 0)
	{
		sprintf(ps.content[35].data, trk3, strlen(trk3));
		ps.content[35].dataLen = strlen(trk3);
		ps.content[35].is_exist = 1;
	}

	/* 52 密码域 */
	if (strlen(pin_data) != 0)
	{
		/* TODO: 需要对密码加密 - 在common 中处理 TODO*/
		sprintf(ps.content[52].data, pin_data, strlen(pin_data));
		ps.content[52].dataLen = strlen(pin_data);
		ps.content[52].is_exist = 1;
	}
	/* 53 安全控制信息 */
	if (strlen(pin_data) != 0)
	{
		char *filed53 = "2600000000000000";
		sprintf(ps.content[53].data, filed53, strlen(filed53));
		ps.content[53].dataLen = strlen(filed53);
		ps.content[53].is_exist = 1;
	}
	/* 55 域 IC卡数据域*/
	if (strlen(ic_data) != 0)
	{
		/* TODO: 需要对密码加密 - 在common 中处理 TODO*/
		sprintf(ps.content[55].data, ic_data, strlen(ic_data));
		ps.content[55].dataLen = strlen(ic_data) / 2;
		ps.content[55].is_exist = 1;
	}
	/* 写入56域 */
	char temp56[600];
	memset(temp56, 0, sizeof(temp56));
	INNER_PACK56(mchTransId, cretiId, orgMchTransId, temp56);
	memcpy(ps.content[56].data, temp56, strlen(temp56));
	ps.content[56].dataLen = strlen(temp56) / 2;
	ps.content[56].is_exist = 1;//TODO:change there 
	/* 写入批次 60 域 */
	char batch[7];
	memset(batch, 0, sizeof(batch));
	char temp[20];
	memset(temp, 0, sizeof(temp));
	int batchint = 0;
	CFG_GetProfileString(CFG_BATCHNO, temp);
	batchint = atoi(temp);
	char filed60[20];
	memset(filed60, 0, sizeof(filed60));

	sprintf(filed60, "20%06d000501", batchint);												/* 退货 */
	sprintf(ps.content[60].data, filed60, strlen(filed60));
	ps.content[60].dataLen = strlen(filed60);
	ps.content[60].is_exist = 1;
	/* 63域  填全 00 */
	ps.content[63].dataLen = strlen("000");
	sprintf(ps.content[63].data, "000", ps.content[63].dataLen);
	ps.content[63].is_exist = 1;

	TRANS_RET tr;
	memset(&tr, 0, sizeof(TRANS_RET));
	UNPACKRESULT ur;
	memset(&ur, 0, sizeof(UNPACKRESULT));
	int ret = TRANS_COMMON(&ps, &tr, mKey, &ur);
	/* 对支付返回值进行处理 */
	//if (strlen(tr.respCode) == 0)
	//{
	//	memcpy(tr.respCode, "98", strlen("98"));
	//}
	ret = INNER_packRetJson(tr.respCode, tr.mchTransETime, tr.transId,
		tr.accInfo, tr.txnAmt, tr.cretiId, tr.cusInfo, tr.mchTransId, out);
	return 0;
}

/* 银联卡预授权完成撤销*/
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
	/* 开始组建 银联交易报文 */
	PACKSOURCE ps;
	ISO_FILEDS_INIT(&ps);
	char *msgTypePay = "0700";
	memcpy(ps.content[0].data, msgTypePay, strlen(msgTypePay));
	ps.content[0].dataLen = strlen(msgTypePay);
	ps.content[0].is_exist = 1;

	/* 如果 PAN 为空 由二磁截取 PAN .如果PAN不为空,不取PAN.使用原PAN */
	if (strlen(pan) == 0)
	{
		/* pan 为空 trk2不为空*/
		if (strlen(trk2) != 0)
		{
			/* 由 二磁截取 PAN */
			char trk2temp[60];
			memset(trk2temp, 0, sizeof(trk2temp));
			memcpy(trk2temp, trk2, strlen(trk2));
			//开始根据分隔符截取卡号. " d" 或者 "D" 分隔
			char *pantemp = NULL;
			pantemp = strtok(trk2temp, "d,D");
			if (pantemp != NULL)
			{
				memcpy(pan, pantemp, strlen(pantemp));
			}
		}
	}
	/* 取 pan 完毕*/
	/* 二域  卡号信息 */
	if (strlen(pan) != 0)
	{
		ps.content[2].is_exist = 1;
		memcpy(ps.content[2].data, pan, strlen(pan));
		ps.content[2].dataLen = strlen(pan);
	}
	/* 3 交易处理*/
	char *processCodePay = "200021";
	memcpy(ps.content[3].data, processCodePay, strlen(processCodePay));
	ps.content[3].dataLen = strlen(processCodePay);
	ps.content[3].is_exist = 1;
	/* 4 金额 */
	sprintf(ps.content[4].data, "%012d", txnAmt);
	ps.content[4].dataLen = 12;
	ps.content[4].is_exist = 1;
	/* 14 域 卡有效期*/
	if (strlen(exp_date) != 0)
	{
		sprintf(ps.content[14].data, exp_date, strlen(exp_date));
		ps.content[14].dataLen = strlen(exp_date);
		ps.content[14].is_exist = 1;
	}
	/* 22 域 服务点输入方式*/
	char field22[4];
	memset(field22, 0, sizeof(field22));
	strcpy(field22, entCode);
	if (strlen(pin_data) == 0)																/* 密码域为空 ,选择无密码方式*/
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
	/* 23 域 卡序列号*/
	if (strlen(csn) != 0)
	{
		sprintf(ps.content[23].data, csn, strlen(csn));
		ps.content[23].dataLen = strlen(csn);
		ps.content[23].is_exist = 1;
	}
	/* 25 域*/
	sprintf(ps.content[25].data, "06", 2);
	ps.content[25].dataLen = 2;
	ps.content[25].is_exist = 1;
	/* 26域 */
	if (strlen(pan) != 0)
	{
		sprintf(ps.content[26].data, "12", 2);
		ps.content[26].dataLen = 2;
		ps.content[26].is_exist = 1;
	}
	/* 35 二磁 */
	if (strlen(trk2) != 0)
	{
		sprintf(ps.content[35].data, trk2, strlen(trk2));
		ps.content[35].dataLen = strlen(trk2);
		ps.content[35].is_exist = 1;
	}
	/* 36 三磁 */
	if (strlen(trk3) != 0)
	{
		sprintf(ps.content[35].data, trk3, strlen(trk3));
		ps.content[35].dataLen = strlen(trk3);
		ps.content[35].is_exist = 1;
	}

	/* 52 密码域 */
	if (strlen(pin_data) != 0)
	{
		/* TODO: 需要对密码加密 - 在common 中处理 TODO*/
		sprintf(ps.content[52].data, pin_data, strlen(pin_data));
		ps.content[52].dataLen = strlen(pin_data);
		ps.content[52].is_exist = 1;
	}
	/* 53 安全控制信息 */
	if (strlen(pin_data) != 0)
	{
		char *filed53 = "2600000000000000";
		sprintf(ps.content[53].data, filed53, strlen(filed53));
		ps.content[53].dataLen = strlen(filed53);
		ps.content[53].is_exist = 1;
	}
	/* 55 域 IC卡数据域*/
	if (strlen(ic_data) != 0)
	{
		/* TODO: 需要对密码加密 - 在common 中处理 TODO*/
		sprintf(ps.content[55].data, ic_data, strlen(ic_data));
		ps.content[55].dataLen = strlen(ic_data) / 2;
		ps.content[55].is_exist = 1;
	}
	/* 写入56域 */
	char temp56[600];
	memset(temp56, 0, sizeof(temp56));
	INNER_PACK56(mchTransId, cretiId, orgMchTransId, temp56);
	memcpy(ps.content[56].data, temp56, strlen(temp56));
	ps.content[56].dataLen = strlen(temp56) / 2;
	ps.content[56].is_exist = 1;//TODO:change there 
	/* 写入批次 60 域 */
	char batch[7];
	memset(batch, 0, sizeof(batch));
	char temp[20];
	memset(temp, 0, sizeof(temp));
	int batchint = 0;
	CFG_GetProfileString(CFG_BATCHNO, temp);
	batchint = atoi(temp);
	char filed60[20];
	memset(filed60, 0, sizeof(filed60));

	sprintf(filed60, "21%06d000501", batchint);												/* 退货 */
	sprintf(ps.content[60].data, filed60, strlen(filed60));
	ps.content[60].dataLen = strlen(filed60);
	ps.content[60].is_exist = 1;
	/* 63域  填全 00 */
	ps.content[63].dataLen = strlen("000");
	sprintf(ps.content[63].data, "000", ps.content[63].dataLen);
	ps.content[63].is_exist = 1;

	TRANS_RET tr;
	memset(&tr, 0, sizeof(TRANS_RET));
	UNPACKRESULT ur;
	memset(&ur, 0, sizeof(UNPACKRESULT));
	int ret = TRANS_COMMON(&ps, &tr, mKey, &ur);
	/* 对支付返回值进行处理 */
	//if (strlen(tr.respCode) == 0)
	//{
	//	memcpy(tr.respCode, "98", strlen("98"));
	//}
	ret = INNER_packRetJson(tr.respCode, tr.mchTransETime, tr.transId,
		tr.accInfo, tr.txnAmt, tr.cretiId, tr.cusInfo, tr.mchTransId, out);
	return 0;
}

/* 外卡交易*/
int INNER_OUTSIDE_PAY(cJSON *cardInfo, char const * mKey, int txnAmt,
	char const *mchTransId, char const *mchTransSTime, char const *cretiId,
	char const *orgMchTransId, char *out)
{
	return 0;
}

/* 外卡消费冲正*/
int INNER_OUTSIDE_FLUSH(cJSON *cardInfo, char const * mKey, int txnAmt,
	char const *mchTransId, char const *mchTransSTime, char const *cretiId,
	char const *orgMchTransId, char *out)
{
	return 0;
}

/* 外卡消费退货*/
int INNER_OUTSIDE_REFUND(cJSON *cardInfo, char const * mKey, int txnAmt,
	char const *mchTransId, char const *mchTransSTime, char const *cretiId,
	char const *orgMchTransId, char *out)
{
	return 0;
}

/* 外卡消费撤销*/
int INNER_OUTSIDE_REVOKE(cJSON *cardInfo, char const * mKey, int txnAmt,
	char const *mchTransId, char const *mchTransSTime, char const *cretiId,
	char const *orgMchTransId, char *out)
{
	return 0;
}

/* 解析卡片信息域 卡片*/
int INNER_parseCardInfo_CARD(cJSON *json, char *entCode, char *csn,
	char *trk2, char *trk3, char *pin_data, char *ic_data, char* exp_date,char* pan)
{
	cJSON *jsonfind = NULL;
	/* 内卡,外卡交易*/
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


/* 解析卡片信息域 二维码*/
int INNER_parseCardInfo_CODE(cJSON *json, char *code)
{
	cJSON *jsonfind = NULL;
	/* 支付宝微信 */
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

/* 打包JOSN结果 解释内容见文档 */
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



/* 组56域 */
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
