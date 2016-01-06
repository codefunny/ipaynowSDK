/*
 _
(_)  _ __     __ _   _   _   _ __     ___   __      __
| | | '_ \   / _` | | | | | | '_ \   / _ \  \ \ /\ / /
| | | |_) | | (_| | | |_| | | | | | | (_) |  \ V  V /
|_| | .__/   \__,_|  \__, | |_| |_|  \___/    \_/\_/
    |_|              |___/

ipaynow֧��SDK�ڲ��ӿ�
author : nxd
time	: 2015.8.10
*/
#ifndef INNERINTERFACE_H
#define INNERINTERFACE_H
#include "cJSON.h"
#include "trans.h"
#define MAX_LEN			8192

/* ����Ψһ�ӿ� ����JSON��ʽ�ַ���,���JSON��ʽ�ַ��� �ɹ�����0 ʧ�ܷ��ظ��� */
int INNER_JSON_INTERFACE(char const *input, char *out);

/* ���� �ṹ��ӿ� */
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

/* ֧�������� */
int INNER_ALIPAY_PAY(cJSON *cardInfo, char const * mKey, int txnAmt,
	char const *mchTransId, char const *mchTransSTime, char const *cretiId,
	char const *orgMchTransId, char *out);

/* ֧�������ײ�ѯ */
int INNER_ALIPAY_PAY_QUERY(cJSON *cardInfo, char const * mKey, int txnAmt,
	char const *mchTransId, char const *mchTransSTime, char const *cretiId,
	char const *orgMchTransId, int batchintSrc, int traceintSrc, TRANS_RET *tr);

/* ֧�������� */
int INNER_ALIPAY_FLUSH(cJSON *cardInfo, char const * mKey, int txnAmt,
	char const *mchTransId, char const *mchTransSTime, char const *cretiId,
	char const *orgMchTransId, char *out);

/* ֧�����˻� */
int INNER_ALIPAY_REFUND(cJSON *cardInfo, char const * mKey, int txnAmt,
	char const *mchTransId, char const *mchTransSTime, char const *cretiId,
	char const *orgMchTransId, char *out);

/* ΢�Ž���*/
int INNER_WX_PAY(cJSON *cardInfo, char const * mKey, int txnAmt,
	char const *mchTransId, char const *mchTransSTime, char const *cretiId,
	char const *orgMchTransId, char *out);
/* ΢�Ž��ײ�ѯ*/
int INNER_WX_PAY_QUERY(cJSON *cardInfo, char const * mKey, int txnAmt,
	char const *mchTransId, char const *mchTransSTime, char const *cretiId,
	char const *orgMchTransId, int batchintSrc, int traceintSrc, TRANS_RET *tr);
/* ΢�ų���*/
int INNER_WX_FLUSH(cJSON *cardInfo, char const * mKey, int txnAmt,
	char const *mchTransId, char const *mchTransSTime, char const *cretiId,
	char const *orgMchTransId, char *out);
/* ΢���˻�*/
int INNER_WX_REFUND(cJSON *cardInfo, char const * mKey, int txnAmt,
	char const *mchTransId, char const *mchTransSTime, char const *cretiId,
	char const *orgMchTransId, char *out);

/* ��������*/
int INNER_UNION_PAY(cJSON *cardInfo, char const * mKey, int txnAmt,
	char const *mchTransId, char const *mchTransSTime, char const *cretiId,
	char const *orgMchTransId, char *out);
/* �������ѳ���*/
int INNER_UNION_FLUSH(cJSON *cardInfo, char const * mKey, int txnAmt,
	char const *mchTransId, char const *mchTransSTime, char const *cretiId,
	char const *orgMchTransId, char *out);
/* ���������˻�*/
int INNER_UNION_REFUND(cJSON *cardInfo, char const * mKey, int txnAmt,
	char const *mchTransId, char const *mchTransSTime, char const *cretiId,
	char const *orgMchTransId, char *out);
/* �������ѳ���*/
int INNER_UNION_REVOKE(cJSON *cardInfo, char const * mKey, int txnAmt,
	char const *mchTransId, char const *mchTransSTime, char const *cretiId,
	char const *orgMchTransId, char *out);
/* ����������ѯ*/
int INNER_UNION_QUERY(cJSON *cardInfo, char const * mKey, int txnAmt,
	char const *mchTransId, char const *mchTransSTime, char const *cretiId,
	char const *orgMchTransId, char *out);

/* ������Ԥ��Ȩ*/
int INNER_UNION_PRE_PAY(cJSON *cardInfo, char const * mKey, int txnAmt,
	char const *mchTransId, char const *mchTransSTime, char const *cretiId,
	char const *orgMchTransId, char *out);
/* ������Ԥ��Ȩ����*/
int INNER_UNION_PRE_PAY_FLUSH(cJSON *cardInfo, char const * mKey, int txnAmt,
	char const *mchTransId, char const *mchTransSTime, char const *cretiId,
	char const *orgMchTransId, char *out);
/* ������Ԥ��Ȩ����*/
int INNER_UNION_PRE_PAY_REVOKE(cJSON *cardInfo, char const * mKey, int txnAmt,
	char const *mchTransId, char const *mchTransSTime, char const *cretiId,
	char const *orgMchTransId, char *out);
/* ������Ԥ��Ȩ���*/
int INNER_UNION_PRE_PAY_FINISH(cJSON *cardInfo, char const * mKey, int txnAmt,
	char const *mchTransId, char const *mchTransSTime, char const *cretiId,
	char const *orgMchTransId, char *out);
/* ������Ԥ��Ȩ��ɳ���*/
int INNER_UNION_PRE_PAY_FINSHI_REVOKE(cJSON *cardInfo, char const * mKey, int txnAmt,
	char const *mchTransId, char const *mchTransSTime, char const *cretiId,
	char const *orgMchTransId, char *out);


/* �⿨����*/
int INNER_OUTSIDE_PAY(cJSON *cardInfo, char const * mKey, int txnAmt,
	char const *mchTransId, char const *mchTransSTime, char const *cretiId,
	char const *orgMchTransId, char *out);
/* �⿨���ѳ���*/
int INNER_OUTSIDE_FLUSH(cJSON *cardInfo, char const * mKey, int txnAmt,
	char const *mchTransId, char const *mchTransSTime, char const *cretiId,
	char const *orgMchTransId, char *out);
/* �⿨���ѳ���*/
int INNER_OUTSIDE_REVOKE(cJSON *cardInfo, char const * mKey, int txnAmt,
	char const *mchTransId, char const *mchTransSTime, char const *cretiId,
	char const *orgMchTransId, char *out);
/* �⿨�����˻�*/
int INNER_OUTSIDE_REFUND(cJSON *cardInfo, char const * mKey, int txnAmt,
	char const *mchTransId, char const *mchTransSTime, char const *cretiId,
	char const *orgMchTransId, char *out);


/* ��56�� */
int INNER_PACK56(char const *mchTransId, char const *cretiId, char const *orgMchTransId,char *out);


unsigned short FormXZZFTlv(char *lpOut, char *tag, char taglen, char *value, unsigned short valuelen);


/* ������Ƭ��Ϣ�� ��Ƭ*/
int INNER_parseCardInfo_CARD(cJSON *json, char *entCode, char *csn,
	char *trk2, char *trk3, char *pin_data, char *ic_data, char* exp_date,
	char *pan);

/* ������Ƭ��Ϣ�� ��ά��*/
int INNER_parseCardInfo_CODE(cJSON *josn, char *code);

/* ���JOSN��� �������ݼ��ĵ� */
int INNER_packRetJson(char *respCode, char *mchTransETime, char *transId,
	char *accInfo, int txnAmt, char *cretild, char *cusInfo, char *mchTransId, char *output);

/* �õ���������ӳ�� */
int INNER_getMsgInt(char *msgtype);


#endif