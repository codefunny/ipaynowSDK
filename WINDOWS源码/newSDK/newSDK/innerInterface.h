/*
 _
(_)  _ __     __ _   _   _   _ __     ___   __      __
| | | '_ \   / _` | | | | | | '_ \   / _ \  \ \ /\ / /
| | | |_) | | (_| | | |_| | | | | | | (_) |  \ V  V /
|_| | .__/   \__,_|  \__, | |_| |_|  \___/    \_/\_/
    |_|              |___/

ipaynow支付SDK内部接口
author : nxd
time	: 2015.8.10
*/
#ifndef INNERINTERFACE_H
#define INNERINTERFACE_H
#include "cJSON.h"
#include "trans.h"
#define MAX_LEN			8192

/* 交易唯一接口 输入JSON格式字符串,输出JSON格式字符串 成功返回0 失败返回负数 */
int INNER_JSON_INTERFACE(char const *input, char *out);

/* 交易 结构体接口 */
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
int INNER_STRUCT_INTERFACE(SDKINTRADE const *in, SDKOUTTRADE *out);

/* 支付宝交易 */
int INNER_ALIPAY_PAY(cJSON *cardInfo, char const * mKey, int txnAmt,
	char const *mchTransId, char const *mchTransSTime, char const *cretiId,
	char const *orgMchTransId, char *out);

/* 支付宝交易查询 */
int INNER_ALIPAY_PAY_QUERY(cJSON *cardInfo, char const * mKey, int txnAmt,
	char const *mchTransId, char const *mchTransSTime, char const *cretiId,
	char const *orgMchTransId, int batchintSrc, int traceintSrc, TRANS_RET *tr);

/* 支付宝冲正 */
int INNER_ALIPAY_FLUSH(cJSON *cardInfo, char const * mKey, int txnAmt,
	char const *mchTransId, char const *mchTransSTime, char const *cretiId,
	char const *orgMchTransId, char *out);

/* 支付宝退货 */
int INNER_ALIPAY_REFUND(cJSON *cardInfo, char const * mKey, int txnAmt,
	char const *mchTransId, char const *mchTransSTime, char const *cretiId,
	char const *orgMchTransId, char *out);

/* 微信交易*/
int INNER_WX_PAY(cJSON *cardInfo, char const * mKey, int txnAmt,
	char const *mchTransId, char const *mchTransSTime, char const *cretiId,
	char const *orgMchTransId, char *out);
/* 微信交易查询*/
int INNER_WX_PAY_QUERY(cJSON *cardInfo, char const * mKey, int txnAmt,
	char const *mchTransId, char const *mchTransSTime, char const *cretiId,
	char const *orgMchTransId, int batchintSrc, int traceintSrc, TRANS_RET *tr);
/* 微信冲正*/
int INNER_WX_FLUSH(cJSON *cardInfo, char const * mKey, int txnAmt,
	char const *mchTransId, char const *mchTransSTime, char const *cretiId,
	char const *orgMchTransId, char *out);
/* 微信退货*/
int INNER_WX_REFUND(cJSON *cardInfo, char const * mKey, int txnAmt,
	char const *mchTransId, char const *mchTransSTime, char const *cretiId,
	char const *orgMchTransId, char *out);

/* 银联交易*/
int INNER_UNION_PAY(cJSON *cardInfo, char const * mKey, int txnAmt,
	char const *mchTransId, char const *mchTransSTime, char const *cretiId,
	char const *orgMchTransId, char *out);
/* 银联消费冲正*/
int INNER_UNION_FLUSH(cJSON *cardInfo, char const * mKey, int txnAmt,
	char const *mchTransId, char const *mchTransSTime, char const *cretiId,
	char const *orgMchTransId, char *out);
/* 银联消费退货*/
int INNER_UNION_REFUND(cJSON *cardInfo, char const * mKey, int txnAmt,
	char const *mchTransId, char const *mchTransSTime, char const *cretiId,
	char const *orgMchTransId, char *out);
/* 银联消费撤销*/
int INNER_UNION_REVOKE(cJSON *cardInfo, char const * mKey, int txnAmt,
	char const *mchTransId, char const *mchTransSTime, char const *cretiId,
	char const *orgMchTransId, char *out);
/* 银联卡余额查询*/
int INNER_UNION_QUERY(cJSON *cardInfo, char const * mKey, int txnAmt,
	char const *mchTransId, char const *mchTransSTime, char const *cretiId,
	char const *orgMchTransId, char *out);

/* 银联卡预授权*/
int INNER_UNION_PRE_PAY(cJSON *cardInfo, char const * mKey, int txnAmt,
	char const *mchTransId, char const *mchTransSTime, char const *cretiId,
	char const *orgMchTransId, char *out);
/* 银联卡预授权冲正*/
int INNER_UNION_PRE_PAY_FLUSH(cJSON *cardInfo, char const * mKey, int txnAmt,
	char const *mchTransId, char const *mchTransSTime, char const *cretiId,
	char const *orgMchTransId, char *out);
/* 银联卡预授权撤销*/
int INNER_UNION_PRE_PAY_REVOKE(cJSON *cardInfo, char const * mKey, int txnAmt,
	char const *mchTransId, char const *mchTransSTime, char const *cretiId,
	char const *orgMchTransId, char *out);
/* 银联卡预授权完成*/
int INNER_UNION_PRE_PAY_FINISH(cJSON *cardInfo, char const * mKey, int txnAmt,
	char const *mchTransId, char const *mchTransSTime, char const *cretiId,
	char const *orgMchTransId, char *out);
/* 银联卡预授权完成撤销*/
int INNER_UNION_PRE_PAY_FINSHI_REVOKE(cJSON *cardInfo, char const * mKey, int txnAmt,
	char const *mchTransId, char const *mchTransSTime, char const *cretiId,
	char const *orgMchTransId, char *out);


/* 外卡交易*/
int INNER_OUTSIDE_PAY(cJSON *cardInfo, char const * mKey, int txnAmt,
	char const *mchTransId, char const *mchTransSTime, char const *cretiId,
	char const *orgMchTransId, char *out);
/* 外卡消费冲正*/
int INNER_OUTSIDE_FLUSH(cJSON *cardInfo, char const * mKey, int txnAmt,
	char const *mchTransId, char const *mchTransSTime, char const *cretiId,
	char const *orgMchTransId, char *out);
/* 外卡消费撤销*/
int INNER_OUTSIDE_REVOKE(cJSON *cardInfo, char const * mKey, int txnAmt,
	char const *mchTransId, char const *mchTransSTime, char const *cretiId,
	char const *orgMchTransId, char *out);
/* 外卡消费退货*/
int INNER_OUTSIDE_REFUND(cJSON *cardInfo, char const * mKey, int txnAmt,
	char const *mchTransId, char const *mchTransSTime, char const *cretiId,
	char const *orgMchTransId, char *out);


/* 组56域 */
int INNER_PACK56(char const *mchTransId, char const *cretiId, char const *orgMchTransId,char *out);


unsigned short FormXZZFTlv(char *lpOut, char *tag, char taglen, char *value, unsigned short valuelen);


/* 解析卡片信息域 卡片*/
int INNER_parseCardInfo_CARD(cJSON *json, char *entCode, char *csn,
	char *trk2, char *trk3, char *pin_data, char *ic_data, char* exp_date,
	char *pan);

/* 解析卡片信息域 二维码*/
int INNER_parseCardInfo_CODE(cJSON *josn, char *code);

/* 打包JOSN结果 解释内容见文档 */
int INNER_packRetJson(char *respCode, char *mchTransETime, char *transId,
	char *accInfo, int txnAmt, char *cretild, char *cusInfo, char *mchTransId, char *output);

/* 得到交易类型映射 */
int INNER_getMsgInt(char *msgtype);


#endif