#include "iso8583.h"
#include "connect.h"
#include "config.h"
#include "cJSON.h"
#include "trans.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifndef  _WIN32
#include <unistd.h>
#else
#include <windows.h>
#endif


/* 计时程序 单位毫秒*/
int TRANS_WAIT(int time)
{
#ifndef _WIN32
	sleep(time/1000);
#else
	Sleep(time);
#endif
	return 0;
}

/* 自动签到*/
int TRANS_AUTOSIGNUP(char const *key)
{
	/* 判断是否需要签到*/
	char cfgtemp[60];																		/* 临时存放从配置文件取出的信息*/
	memset(cfgtemp, 0, sizeof(cfgtemp));
	int is_signup = 0, is_useful = 0;	
	CFG_GetProfileString(CFG_IS_SIGNUP, cfgtemp);
	is_signup = atoi(cfgtemp);
	memset(cfgtemp, 0, sizeof(cfgtemp));
	CFG_GetProfileString(CFG_IS_USEFUL, cfgtemp);
	is_useful = atoi(cfgtemp);
	int ret = 0;
	if (is_signup == 0 || is_useful == 0)													/* 需要进行签到*/
	{
		/* 签到过程 */
		int is_auto_sign = 0, signup_times = 0;
		memset(cfgtemp, 0, sizeof(cfgtemp));
		CFG_GetProfileString(CFG_IS_AUTO_SIGN, cfgtemp);
		is_auto_sign = atoi(cfgtemp);
		memset(cfgtemp, 0, sizeof(cfgtemp));
		CFG_GetProfileString(CFG_SIGNUP_TIMES, cfgtemp);
		signup_times = atoi(cfgtemp);
		
		for (int i = 0; i < signup_times; i++)
		{
			ret = TRANS_SIGNUP(key);
			if (ret >= 0)	break;	
			/* 如果第一次签到失败,五秒后重签*/
			TRANS_WAIT(5000);
		}
		CFG_GetProfileString(CFG_IS_SIGNUP, cfgtemp);
		is_signup = atoi(cfgtemp);
		memset(cfgtemp, 0, sizeof(cfgtemp));
		CFG_GetProfileString(CFG_IS_USEFUL, cfgtemp);
		is_useful = atoi(cfgtemp);
		if (is_signup == 0 || is_useful == 0)
			return -1;
	}
	return ret;
}

/* 签到过程 获取PIK MAK 记入配置文件*/
int TRANS_SIGNUP(char const *key)
{
	int ret = 0;
	PACKSOURCE ps;
	ISO_FILEDS_INIT(&ps);
	char batch[7];
	memset(batch, 0, sizeof(batch));
	char trace[7];
	memset(trace, 0, sizeof(trace));
	char temp[20];
	/* 获取当前批次 但是并没有什么用*/
	int batchint = 0;
	memset(temp, 0, sizeof(temp));
	CFG_GetProfileString(CFG_BATCHNO, temp);
	batchint = atoi(temp);
	sprintf(batch, "%06d", batchint);
	/* 获取当前流水 并且+1*/
	int traceint = 0;
	memset(temp, 0, sizeof(temp));
	CFG_GetProfileString(CFG_TRANSNO, temp);
	traceint = atoi(temp);
	traceint++;
	sprintf(trace, "%06d", traceint);
	/* 将+1后的流水重新记入配置文件*/
	memset(temp, 0, sizeof(temp));
	sprintf(temp, "transno=%06d", traceint);
	CFG_SetProfileString(CFG_TRANSNO, temp);
	/* 获取商户号*/
	char mid[20];
	memset(mid, 0, sizeof(mid));
	CFG_GetProfileString(CFG_MID, mid);
	/* 获取终端号*/
	char tid[20];
	memset(tid, 0, sizeof(tid));
	CFG_GetProfileString(CFG_TID, tid);

	/* 组建签到包 */
	char *msgtype = "0800";
	sprintf(ps.content[0].data, msgtype, strlen(msgtype));
	ps.content[0].dataLen = strlen(msgtype);
	ps.content[0].is_exist = 1;
	ps.content[1].is_exist = 1;
	sprintf(ps.content[11].data, trace, sizeof(trace));
	ps.content[11].dataLen = strlen(trace);
	ps.content[11].is_exist = 1;

	sprintf(ps.content[41].data, tid, sizeof(tid));
	ps.content[41].dataLen = strlen(tid);
	ps.content[41].is_exist = 1;

	sprintf(ps.content[42].data, mid, sizeof(mid));
	ps.content[42].dataLen = strlen(mid);
	ps.content[42].is_exist = 1;

	char filed60[20];
	memset(filed60, 0, sizeof(filed60));
	sprintf(filed60, "00%s0030", batch);
	sprintf(ps.content[60].data, filed60, strlen(filed60));
	ps.content[60].dataLen = strlen(filed60);
	ps.content[60].is_exist = 1;

	char filed63[164];
	memset(filed63, 0, sizeof(filed63));
	sprintf(filed63, "01\x20");
	sprintf(ps.content[63].data, filed63, strlen(filed63));
	ps.content[63].dataLen = strlen(filed63);
	ps.content[63].is_exist = 1;
	/* 获取 IP port*/
	char ip[20];
	memset(ip, 0, sizeof(ip));
	memset(temp, 0, sizeof(temp));
	CFG_GetProfileString(CFG_IP, ip);
	CFG_GetProfileString(CFG_PORT, temp);
	int port = atoi(temp);
	/* 要发送的信息*/
	char sendBuf[MAX_LEN];
	memset(sendBuf, 0, sizeof(sendBuf));
	int sendLen = 0;
	ISO8583_PackToStream(&ps, sendBuf, &sendLen,NULL);
	//  网络通信
	char recvBuf[MAX_LEN];
	memset(recvBuf, 0, sizeof(recvBuf));
	int recvLen = 0;
	memset(temp, 0, sizeof(temp));
	CFG_GetProfileString(CFG_CONNECT_OVERTIME, temp);
	int connectTime = atoi(temp);
	memset(temp, 0, sizeof(temp));
	CFG_GetProfileString(CFG_SEND_OVERTIME, temp);
	int sendTime = atoi(temp);
	memset(temp, 0, sizeof(temp));
	CFG_GetProfileString(CFG_RECEIVE_OVERTIME, temp);
	int recvTime = atoi(temp);
#ifdef _DEBUG
	printf("签到发送的信息为: \n");
	for (int i = 0; i < sendLen; i++)
	{
		printf("%02x ", (unsigned char)sendBuf[i]);
	}
#endif
	recvLen = CONN_COMMUNICATION(ip, port, sendBuf, sendLen, recvBuf, connectTime, sendTime, recvTime);
	if (recvLen < 0)
	{
		return -1;
	}
	/* 处理收到的信息*/
	UNPACKRESULT ur;
	ISO8583_StreamToPack(recvBuf, recvLen, &ur);
	// DONE :  处理密钥
	if (strcmp(ur.content[39].data, "00"))													/* 如果签到返回值不是00*/
	{
		CFG_SetProfileString(CFG_IS_USEFUL, "is_useful=0");
		CFG_SetProfileString(CFG_IS_SIGNUP, "is_signup=0");
		return ret = atoi(ur.content[39].data);
	}
	char PIKStr[33], PIKSource[33], PIKCheckValue[9];										/* PIK密文 明文 以及 校验值*/
	char MAKStr[17], MAKSource[33], MAKCheckValue[9];										/* MAK密文 明文 以及 校验值*/
	memset(PIKStr, 0, sizeof(PIKStr));
	memset(PIKSource, 0, sizeof(PIKSource));
	memset(PIKCheckValue, 0, sizeof(PIKCheckValue));

	memset(MAKStr, 0, sizeof(MAKStr));
	memset(MAKSource, 0, sizeof(MAKSource));			
	memset(MAKCheckValue, 0, sizeof(MAKCheckValue));

	memcpy(PIKStr, ur.content[62].data, 32);
	memcpy(PIKCheckValue, ur.content[62].data + 32, 8);
	memcpy(MAKStr, ur.content[62].data + 40, 16);
	memcpy(MAKCheckValue, ur.content[62].data + 72, 8);

	/* 开始验证密钥 */
	char main_key[33];
	memset(main_key, 0, sizeof(main_key));
	CFG_GetProfileString(CFG_KEY, main_key);													/* 得到主密钥*/
	char real_key[33];
	memset(real_key, 0, sizeof(real_key));													/* 真正的主密钥,用用户传入的密钥解密结果*/
	ISO8583_DES16(DECRYPT, main_key, real_key, key);										/* 得到真正的主密钥*/
	ISO8583_DES16(DECRYPT, PIKStr, PIKSource, real_key);									/* 得到PIK明文*/
	ISO8583_DES16(DECRYPT, MAKStr, MAKSource, real_key);									/* 得到MAK明文*/
	/* 开始对16字节0进行加密*/
	char *zero16 = "00000000000000000000000000000000";										/* 16字节待加密0*/
	char PIKCheckResult[33];
	memset(PIKCheckResult, 0, sizeof(PIKCheckResult));
	ISO8583_DES16(ENCRYPT, zero16, PIKCheckResult, PIKSource);
	if (strncmp(PIKCheckResult,PIKCheckValue,8))
	{
		CFG_SetProfileString(CFG_IS_USEFUL, "is_useful=0");
		CFG_SetProfileString(CFG_IS_SIGNUP, "is_signup=0");
		return -1;																			/* PIK 校验失败*/
	}
	char *zero8 = "0000000000000000";														/* 16 字节待加密0*/
	char MAKCheckResult[17];
	memset(MAKCheckResult, 0, sizeof(MAKCheckResult));										
	ISO8583_DES8_str(ENCRYPT, zero8, MAKCheckResult, MAKSource);							
	if (strncmp(MAKCheckResult,MAKCheckValue,8))
	{
		CFG_SetProfileString(CFG_IS_USEFUL, "is_useful=0");
		CFG_SetProfileString(CFG_IS_SIGNUP, "is_signup=0");
		return -1;																			/* MAK 校验失败*/
	}
	/* 密钥验证正确,将密钥密文写入配置文件 */
	char PIKwrite[60];
	memset(PIKwrite, 0, sizeof(PIKwrite));
	sprintf(PIKwrite, "pik=%s", PIKStr);
	CFG_SetProfileString(CFG_PIK, PIKwrite);
	char MAKwrite[60];
	memset(MAKwrite, 0, sizeof(MAKwrite));
	sprintf(MAKwrite, "mak=%s", MAKStr);
	CFG_SetProfileString(CFG_MAK, MAKwrite);
	CFG_SetProfileString(CFG_IS_USEFUL, "is_useful=1");										/* 密钥可用标志设为 1 */
	CFG_SetProfileString(CFG_IS_SIGNUP, "is_signup=1");										/* 签到状态更改为已签到*/
	/* 得到批次号 ,将得到的批次写入配置文件 */
	char newBatchNo[20];
	memset(newBatchNo, 0, sizeof(newBatchNo));
	memcpy(newBatchNo, ur.content[60].data + 2, 6);
	char batchnoin[20];
	memset(batchnoin, 0, sizeof(batchnoin));
	sprintf(batchnoin, "batchno=%06s", newBatchNo);
	CFG_SetProfileString(CFG_BATCHNO, batchnoin);
	return 0;
}

/* 签退过程 */
int TRANS_SIGNOFF()
{
	int ret = 0;
	PACKSOURCE ps;
	ISO_FILEDS_INIT(&ps);
	char batch[7];
	memset(batch, 0, sizeof(batch));
	char trace[7];
	memset(trace, 0, sizeof(trace));
	char temp[20];
	/* 获取当前批次 但是并没有什么用*/
	int batchint = 0;
	memset(temp, 0, sizeof(temp));
	CFG_GetProfileString(CFG_BATCHNO, temp);
	batchint = atoi(temp);
	sprintf(batch, "%06d", batchint);
	/* 获取当前流水 并且+1*/
	int traceint = 0;
	memset(temp, 0, sizeof(temp));
	CFG_GetProfileString(CFG_TRANSNO, temp);
	traceint = atoi(temp);
	traceint++;
	sprintf(trace, "%06d", traceint);
	/* 将+1后的流水重新记入配置文件*/
	memset(temp, 0, sizeof(temp));
	sprintf(temp, "transno=%06d", traceint);
	CFG_SetProfileString(CFG_TRANSNO, temp);
	/* 获取商户号*/
	char mid[20];
	memset(mid, 0, sizeof(mid));
	CFG_GetProfileString(CFG_MID, mid);
	/* 获取终端号*/
	char tid[20];
	memset(tid, 0, sizeof(tid));
	CFG_GetProfileString(CFG_TID, tid);

	/* 组建签到包 */
	char *msgtype = "0820";
	sprintf(ps.content[0].data, msgtype, strlen(msgtype));
	ps.content[0].dataLen = strlen(msgtype);
	ps.content[0].is_exist = 1;
	ps.content[1].is_exist = 1;
	sprintf(ps.content[11].data, trace, sizeof(trace));
	ps.content[11].dataLen = strlen(trace);
	ps.content[11].is_exist = 1;

	sprintf(ps.content[41].data, tid, sizeof(tid));
	ps.content[41].dataLen = strlen(tid);
	ps.content[41].is_exist = 1;

	sprintf(ps.content[42].data, mid, sizeof(mid));
	ps.content[42].dataLen = strlen(mid);
	ps.content[42].is_exist = 1;

	char filed60[20];
	memset(filed60, 0, sizeof(filed60));
	sprintf(filed60, "00%s0020", batch);
	sprintf(ps.content[60].data, filed60, strlen(filed60));
	ps.content[60].dataLen = strlen(filed60);
	ps.content[60].is_exist = 1;

	
	/* 获取 IP port*/
	char ip[20];
	memset(ip, 0, sizeof(ip));
	memset(temp, 0, sizeof(temp));
	CFG_GetProfileString(CFG_IP, ip);
	CFG_GetProfileString(CFG_PORT, temp);
	int port = atoi(temp);
	/* 要发送的信息*/
	char sendBuf[MAX_LEN];
	memset(sendBuf, 0, sizeof(sendBuf));
	int sendLen = 0;
	ISO8583_PackToStream(&ps, sendBuf, &sendLen, NULL);
	//  网络通信
	char recvBuf[MAX_LEN];
	memset(recvBuf, 0, sizeof(recvBuf));
	int recvLen = 0;
	memset(temp, 0, sizeof(temp));
	CFG_GetProfileString(CFG_CONNECT_OVERTIME, temp);
	int connectTime = atoi(temp);
	memset(temp, 0, sizeof(temp));
	CFG_GetProfileString(CFG_SEND_OVERTIME, temp);
	int sendTime = atoi(temp);
	memset(temp, 0, sizeof(temp));
	CFG_GetProfileString(CFG_RECEIVE_OVERTIME, temp);
	int recvTime = atoi(temp);
#ifdef _DEBUG
	printf("签退发送的信息为: \n");
	for (int i = 0; i < sendLen; i++)
	{
		printf("%02x ", (unsigned char)sendBuf[i]);
	}
#endif
	recvLen = CONN_COMMUNICATION(ip, port, sendBuf, sendLen, recvBuf, connectTime, sendTime, recvTime);
	if (recvLen < 0)
	{
		return -1;
	}
	/* 处理收到的信息*/
	UNPACKRESULT ur;
	ISO8583_StreamToPack(recvBuf, recvLen, &ur);
	// DONE :  签退
	if (strcmp(ur.content[39].data, "00") == 0)													/* 如果签到返回值不是00*/
	{
		CFG_SetProfileString(CFG_IS_USEFUL, "is_useful=0");
		CFG_SetProfileString(CFG_IS_SIGNUP, "is_signup=0");
		char temp[20];																			/* 配置文件中转 */
		memset(temp, 0, sizeof(temp));
		int traceint = 10;
		sprintf(temp, "transno=%06d", traceint);
		CFG_SetProfileString(CFG_TRANSNO, temp);											/* 将新流水重新记入配置文件*/

		return ret = atoi(ur.content[39].data);
	}
	else 
	{
		return -1;
	}
	return 0;
}

/* 批结算 */
int TRAN_SETTLE()
{
	int ret = 0;
	PACKSOURCE ps;
	ISO_FILEDS_INIT(&ps);
	char batch[7];
	memset(batch, 0, sizeof(batch));
	char trace[7];
	memset(trace, 0, sizeof(trace));
	char temp[20];
	/* 获取当前批次 但是并没有什么用*/
	int batchint = 0;
	memset(temp, 0, sizeof(temp));
	CFG_GetProfileString(CFG_BATCHNO, temp);
	batchint = atoi(temp);
	sprintf(batch, "%06d", batchint);
	/* 获取当前流水 并且+1*/
	int traceint = 0;
	memset(temp, 0, sizeof(temp));
	CFG_GetProfileString(CFG_TRANSNO, temp);
	traceint = atoi(temp);
	traceint++;
	sprintf(trace, "%06d", traceint);
	/* 将+1后的流水重新记入配置文件*/
	memset(temp, 0, sizeof(temp));
	sprintf(temp, "transno=%06d", traceint);
	CFG_SetProfileString(CFG_TRANSNO, temp);
	/* 获取商户号*/
	char mid[20];
	memset(mid, 0, sizeof(mid));
	CFG_GetProfileString(CFG_MID, mid);
	/* 获取终端号*/
	char tid[20];
	memset(tid, 0, sizeof(tid));
	CFG_GetProfileString(CFG_TID, tid);

	/* 组建签到包 */
	char *msgtype = "0500";
	sprintf(ps.content[0].data, msgtype, strlen(msgtype));
	ps.content[0].dataLen = strlen(msgtype);
	ps.content[0].is_exist = 1;
	ps.content[1].is_exist = 1;
	sprintf(ps.content[11].data, trace, sizeof(trace));
	ps.content[11].dataLen = strlen(trace);
	ps.content[11].is_exist = 1;

	sprintf(ps.content[41].data, tid, sizeof(tid));
	ps.content[41].dataLen = strlen(tid);
	ps.content[41].is_exist = 1;

	sprintf(ps.content[42].data, mid, sizeof(mid));
	ps.content[42].dataLen = strlen(mid);
	ps.content[42].is_exist = 1;

	char filed60[20];
	memset(filed60, 0, sizeof(filed60));
	sprintf(filed60, "00%s2010", batch);
	sprintf(ps.content[60].data, filed60, strlen(filed60));
	ps.content[60].dataLen = strlen(filed60);
	ps.content[60].is_exist = 1;


	/* 获取 IP port*/
	char ip[20];
	memset(ip, 0, sizeof(ip));
	memset(temp, 0, sizeof(temp));
	CFG_GetProfileString(CFG_IP, ip);
	CFG_GetProfileString(CFG_PORT, temp);
	int port = atoi(temp);
	/* 要发送的信息*/
	char sendBuf[MAX_LEN];
	memset(sendBuf, 0, sizeof(sendBuf));
	int sendLen = 0;
	ISO8583_PackToStream(&ps, sendBuf, &sendLen, NULL);
	//  网络通信
	char recvBuf[MAX_LEN];
	memset(recvBuf, 0, sizeof(recvBuf));
	int recvLen = 0;
	memset(temp, 0, sizeof(temp));
	CFG_GetProfileString(CFG_CONNECT_OVERTIME, temp);
	int connectTime = atoi(temp);
	memset(temp, 0, sizeof(temp));
	CFG_GetProfileString(CFG_SEND_OVERTIME, temp);
	int sendTime = atoi(temp);
	memset(temp, 0, sizeof(temp));
	CFG_GetProfileString(CFG_RECEIVE_OVERTIME, temp);
	int recvTime = atoi(temp);
#ifdef _DEBUG
	printf("批结算发送的信息为: \n");
	for (int i = 0; i < sendLen; i++)
	{
		printf("%02x ", (unsigned char)sendBuf[i]);
	}
#endif
	recvLen = CONN_COMMUNICATION(ip, port, sendBuf, sendLen, recvBuf, connectTime, sendTime, recvTime);
	if (recvLen < 0)
	{
		return -1;
	}
	/* 处理收到的信息*/
	UNPACKRESULT ur;
	ISO8583_StreamToPack(recvBuf, recvLen, &ur);
	// DONE :  批结算
	if (strcmp(ur.content[39].data, "00") == 0)													/* 如果签到返回值不是00*/
	{
		return ret = atoi(ur.content[39].data);
	}
	else
	{
		return -1;
	}
	return 0;
}

/* 通用交易模板,输入仍为结构体数组,但是流水批次不用用户输入,自动控制
返回为TRANS_RET
*/
int TRANS_COMMON(PACKSOURCE *ps, TRANS_RET *tr, char const *key, UNPACKRESULT *ur)
{
signoff:;
	char temp[20];																			/* 配置文件中转 */
	memset(temp, 0, sizeof(temp));
	/* 得到MAK 明文, PIK明文 */
	char main_key[33];
	char PIKStr[33], PIKSource[33];															/* PIK密文 明文 */
	char MAKStr[17], MAKSource[33];															/* MAK密文 明文 */
	memset(PIKStr, 0, sizeof(PIKStr));		
	memset(PIKSource, 0, sizeof(PIKSource));
	memset(MAKStr, 0, sizeof(MAKStr));
	memset(MAKSource, 0, sizeof(MAKSource));
	memset(main_key, 0, sizeof(main_key));
	CFG_GetProfileString(CFG_PIK, PIKStr);
	CFG_GetProfileString(CFG_MAK, MAKStr);
	CFG_GetProfileString(CFG_KEY, main_key);												/* 得到主密钥*/
	char real_key[33];
	memset(real_key, 0, sizeof(real_key));													/* 真正的主密钥,用用户传入的密钥解密结果*/
	ISO8583_DES16(DECRYPT, main_key, real_key, key);										/* 得到真正的主密钥*/
	ISO8583_DES16(DECRYPT, PIKStr, PIKSource, real_key);									/* 得到PIK明文*/
	ISO8583_DES16(DECRYPT, MAKStr, MAKSource, real_key);									/* 得到MAK明文*/
	/* 开始初始化(写死以及内置)发送信息域 */
	ps->content[1].is_exist = 1;																/* 位图域存在 */
	/* 写入流水 */
	char trace[7];
	memset(trace, 0, sizeof(trace));
	int traceint = 0;
	memset(temp, 0, sizeof(temp));
	CFG_GetProfileString(CFG_TRANSNO, temp);
	traceint = atoi(temp);
	if (traceint >= 900000)
	{
		TRAN_SETTLE();
		TRANS_SIGNOFF();
		goto signoff;
	}
	traceint++;
	sprintf(trace, "%06d", traceint);
	memset(temp, 0, sizeof(temp));
	sprintf(temp, "transno=%06d", traceint);
	CFG_SetProfileString(CFG_TRANSNO, temp);												/* 将+1后的流水重新记入配置文件*/
	sprintf(ps->content[11].data, trace, sizeof(trace));
	ps->content[11].dataLen = strlen(trace);
	ps->content[11].is_exist = 1;
	
	/* 写入商户号 */
	char mid[20];
	memset(mid, 0, sizeof(mid));
	CFG_GetProfileString(CFG_MID, mid);
	sprintf(ps->content[42].data, mid, sizeof(mid));
	ps->content[42].dataLen = strlen(mid);
	ps->content[42].is_exist = 1;
	/* 写入终端号*/
	char tid[20];
	memset(tid, 0, sizeof(tid));
	CFG_GetProfileString(CFG_TID, tid);
	sprintf(ps->content[41].data, tid, sizeof(tid));
	ps->content[41].dataLen = strlen(tid);
	ps->content[41].is_exist = 1;
	/* 写入49域 交易货币代码 */
	sprintf(ps->content[49].data, "156", 3);
	ps->content[49].dataLen = 3;
	ps->content[49].is_exist = 1;

	/* 用户密码加密过程 有密码域开始加密*/
	if (ps->content[52].is_exist == 1)
	{
		/* 有用户密码的情况一定有二磁,有二磁一定有pan域.
			由2域取卡号
			 加密pan的PIK为   PIKSource*/
		char passwordTemp[20];
		memset(passwordTemp, 0, sizeof(passwordTemp));
		ISO8583_PINEncrypt(ps->content[52].data, ps->content[2].data, passwordTemp, PIKSource);
		memcpy(ps->content[52].data, passwordTemp, strlen(passwordTemp));
		ps->content[52].dataLen = strlen(passwordTemp)/2;
	}

	/* 写入63域 */
	if (ps->content[63].is_exist ==  0)
	{
		char filed63[164];
		memset(filed63, 0, sizeof(filed63));
		sprintf(filed63, "01\x20");
		sprintf(ps->content[63].data, filed63, strlen(filed63));
		ps->content[63].dataLen = strlen(filed63);
		ps->content[63].is_exist = 1;
	}
	/* MAC 校验*/
	ps->content[64].is_exist = 1;

	/* 开始通信过程 */
	/* 获取 IP port*/
	char ip[20];
	memset(ip, 0, sizeof(ip));
	memset(temp, 0, sizeof(temp));
	CFG_GetProfileString(CFG_IP, ip);
	CFG_GetProfileString(CFG_PORT, temp);
	int port = atoi(temp);
	/* 要发送的信息*/
	char sendBuf[MAX_LEN];
	memset(sendBuf, 0, sizeof(sendBuf));
	int sendLen = 0;
	/*得到mak明文*/
	ISO8583_PackToStream(ps, sendBuf, &sendLen, MAKSource);
	//  网络通信
	char recvBuf[MAX_LEN];
	memset(recvBuf, 0, sizeof(recvBuf));
	int recvLen = 0;
	memset(temp, 0, sizeof(temp));
	CFG_GetProfileString(CFG_CONNECT_OVERTIME, temp);
	int connectTime = atoi(temp);
	memset(temp, 0, sizeof(temp));
	CFG_GetProfileString(CFG_SEND_OVERTIME, temp);
	int sendTime = atoi(temp);
	memset(temp, 0, sizeof(temp));
	CFG_GetProfileString(CFG_RECEIVE_OVERTIME, temp);
	int recvTime = atoi(temp);
#ifdef _DEBUG
	printf("发送的信息为: \n");
	for (int i = 0; i < sendLen; i++)
	{
		printf("%02x ", (unsigned char)sendBuf[i]);
	}
	printf("\n");
#endif

	recvLen = CONN_COMMUNICATION(ip, port, sendBuf, sendLen, recvBuf, connectTime, sendTime, recvTime);
	if (recvLen < 0)
	{
		// 超时
		memcpy(tr->respCode, "98", strlen("98"));
		return -1;
	}
	/* 处理收到的信息*/
#ifdef _DEBUG
	printf("收到的信息为: \n");
	for (int i = 0; i < recvLen; i++)
	{
		printf("%02x ", (unsigned char)recvBuf[i]);
	}
	printf("\n");
#endif
	int ret = 0;
	//UNPACKRESULT ur;
	ISO8583_StreamToPack(recvBuf, recvLen, ur);
	char respCode[3];
	char mchTransETime[17];
	char transId[38];
	char accInfo[60];
	int txnAmt = 0;
	char cretiId[48];
	char cusInfo[74];
	char mchTransId[60];
	memset(respCode, 0, sizeof(respCode));
	memset(mchTransETime, 0, sizeof(mchTransETime));
	memset(transId, 0, sizeof(transId));
	memset(accInfo, 0, sizeof(accInfo));
	memset(cretiId, 0, sizeof(cretiId));
	memset(cusInfo, 0, sizeof(cusInfo));
	memset(mchTransId, 0, sizeof(mchTransId));
	
	memcpy(respCode, ur->content[39].data, ur->content[39].dataLen);
	ret = INNER_UNPACK56(ur->content[56].data, transId, cretiId, cusInfo, mchTransId);
	txnAmt = atoi(ur->content[4].data);
	memcpy(accInfo, ur->content[45].data, ur->content[45].dataLen);
	/*开始组建返回时间*/
	char time_year[5];
	memset(time_year, 0, sizeof(time_year));
	time_t timep;
	struct tm *p;
	time(&timep);
	p = localtime(&timep);
	sprintf(time_year, "%d", 1900 + p->tm_year);
	strcpy(mchTransETime, time_year);
	strcat(mchTransETime, ur->content[13].data);
	strcat(mchTransETime, ur->content[12].data);

	memcpy(tr->respCode, respCode, sizeof(tr->respCode));
	memcpy(tr->accInfo, accInfo, strlen(accInfo));
	memcpy(tr->cretiId, cretiId, strlen(cretiId));
	memcpy(tr->cusInfo, cusInfo, strlen(cusInfo));
	memcpy(tr->mchTransETime, mchTransETime, strlen(mchTransETime));
	memcpy(tr->mchTransId, mchTransId, strlen(mchTransId));
	memcpy(tr->transId, transId, strlen(transId));
	tr->txnAmt = txnAmt;
	if (strcmp(respCode,"94") == 0 || 
		strcmp(respCode,"A0") == 0 || 
		strcmp(respCode,"a0") == 0 )
	{
		CFG_SetProfileString(CFG_IS_SIGNUP, "is_signup=0");
		ret = TRANS_AUTOSIGNUP(key);
		if (ret != 0)
		{
			return ret;
		}
	}

	return ret;
}



/* 解56域 */
int INNER_UNPACK56(char const *in, char  *transId, char *cretiId, char *cusInfo, char *mchTransId)
{
	char tmp[4], buf[256];
	char intemp[MAX_LEN];
	memset(intemp, 0, sizeof(intemp));
	ISO8583_BcdStr2Str(in, strlen(in), intemp);
	char cus[400];
	memset(cus, 0, sizeof(cus));
	char ad_desp[200];
	memset(ad_desp, 0, sizeof(ad_desp));
	char RAI[200];
	memset(RAI, 0, sizeof(RAI));
	int shift = 0;
	char *p = intemp;
	int rlen;
	while (*p != '\0')
	{
		memcpy(tmp, p, 3);
		tmp[3] = '\0';
		ISO8583_Str2BcdStr(tmp, 3, buf);
		rlen = ISO8583_Hex2Int(buf[4]) * 16 + ISO8583_Hex2Int(buf[5]);
		if (rlen <= 0 || rlen > 255)
		{
			break;
		}
		/*if (memcmp(buf, "9f01", 4) == 0 || memcmp(buf, "9F01", 4) == 0)
		{
		strncpy(pMain->ad_desc, p + 3, rlen);
		pMain->ad_desc[rlen] = '\0';
		}*/
		if (memcmp(buf, "9f02", 4) == 0 || memcmp(buf, "9F02", 4) == 0)
		{
			strncpy(mchTransId, p + 3, rlen);
			mchTransId[rlen] = '\0';
			//util_err_log(LERROR, "debug 20150323:[%s]", pMain->custom_id);
		}
		/*
		else if (memcmp(buf, "9f03", 4) == 0 || memcmp(buf, "9F03", 4) == 0)
		{
		strncpy(pMain->custom_desc, p + 3, rlen);
		pMain->custom_desc[rlen] = '\0';
		}*/
		else if (memcmp(buf, "9f04", 4) == 0 || memcmp(buf, "9F04", 4) == 0)
		{
			strncpy(ad_desp, p + 3, rlen);
			ad_desp[rlen] = '\n';
			ad_desp[rlen + 1] = '\0';
		}
		else if (memcmp(buf, "9f08", 4) == 0 || memcmp(buf, "9F08", 4) == 0)
		{
			strncpy(RAI, p + 3, rlen);
			RAI[rlen] = '\0';
		}
		/*
		else if (memcmp(buf, "9f05", 4) == 0 || memcmp(buf, "9F05", 4) == 0)
		{
		strncpy(pMain->custom_iqt, p + 3, rlen);
		pMain->custom_iqt[rlen] = '\0';
		}
		else if (memcmp(buf, "9f06", 4) == 0 || memcmp(buf, "9F06", 4) == 0)
		{
		strncpy(pMain->custom_resp_msg, p + 3, rlen);
		pMain->custom_resp_msg[rlen] = '\0';
		}*/
		else if (memcmp(buf, "9f09", 4) == 0 || memcmp(buf, "9F09", 4) == 0)
		{
			strncpy(transId, p + 3, rlen);
			transId[rlen] = '\0';
		}
		else if (memcmp(buf, "9f10", 4) == 0 || memcmp(buf, "9F10", 4) == 0)
		{
			strncpy(cretiId, p + 3, rlen);
			cretiId[rlen] = '\0';
		}
		else
		{
			//strncpy( pMain->ad_desc, p+3,  rlen);
		}
		p += 3 + rlen;
	}
	strcpy(cus, ad_desp);
	strcat(cus, RAI);
	memcpy(cusInfo, cus, strlen(cus));
	return 0;
}
