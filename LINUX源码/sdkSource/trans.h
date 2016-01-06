/*
 _
(_)  _ __     __ _   _   _   _ __     ___   __      __
| | | '_ \   / _` | | | | | | '_ \   / _ \  \ \ /\ / /
| | | |_) | | (_| | | |_| | | | | | | (_) |  \ V  V /
|_| | .__/   \__,_|  \__, | |_| |_|  \___/    \_/\_/
    |_|              |___/

����ISO8583��������ز���
author : nxd
time	: 2015.8.6
*/

#ifndef TRANS_H
#define TRANS_H
#include "iso8583.h"


#define MAX_LEN 8192

typedef struct trans_result
{
	char respCode[3];
	char mchTransETime[16];
	char transId[38];
	char accInfo[60];
	int txnAmt;
	char cretiId[48];
	char cusInfo[74];
	char mchTransId[60];
}TRANS_RET;

/* ��ʱ���� ��λ����*/
int TRANS_WAIT(int time);

/* �Զ�ǩ�� */
int TRANS_AUTOSIGNUP(char const *key);
/* ǩ������ ��ȡPIK MAK ���������ļ�*/
int TRANS_SIGNUP(char const *key);

/* ǩ�˹��� */
int TRANS_SIGNOFF();

/* ������ */
int TRAN_SETTLE();


/* ͨ�ý���ģ��,������Ϊ�ṹ������,������ˮ���β����û�����,�Զ�����
	����ΪTRANS_RET
*/
int TRANS_COMMON(PACKSOURCE *ps, TRANS_RET *tr, char const* key, UNPACKRESULT *ur);



/* ��56�� */
int INNER_UNPACK56(char const *in, char  *transId, char *cretiId, char *cusInfo, char *mchTransId);







#endif