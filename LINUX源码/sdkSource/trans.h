/*
 _
(_)  _ __     __ _   _   _   _ __     ___   __      __
| | | '_ \   / _` | | | | | | '_ \   / _ \  \ \ /\ / /
| | | |_) | | (_| | | |_| | | | | | | (_) |  \ V  V /
|_| | .__/   \__,_|  \__, | |_| |_|  \___/    \_/\_/
    |_|              |___/

处理ISO8583组包解包相关操作
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

/* 计时程序 单位毫秒*/
int TRANS_WAIT(int time);

/* 自动签到 */
int TRANS_AUTOSIGNUP(char const *key);
/* 签到过程 获取PIK MAK 记入配置文件*/
int TRANS_SIGNUP(char const *key);

/* 签退过程 */
int TRANS_SIGNOFF();

/* 批结算 */
int TRAN_SETTLE();


/* 通用交易模板,输入仍为结构体数组,但是流水批次不用用户输入,自动控制
	返回为TRANS_RET
*/
int TRANS_COMMON(PACKSOURCE *ps, TRANS_RET *tr, char const* key, UNPACKRESULT *ur);



/* 解56域 */
int INNER_UNPACK56(char const *in, char  *transId, char *cretiId, char *cusInfo, char *mchTransId);







#endif