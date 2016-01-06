#include "connect.h"
#include "log.h"
#ifndef _WIN32																			
// Linux 平台编译此套程序
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <unistd.h>
#include <asm/ioctls.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#define MAX_LEN 8192

int CONN_COMMUNICATION(char const *ip, int port, char const *sendbuf, int sendLen, char *recvbuf, int connectTime, int sendTime, int recvTime)
{
	int sockfd,numbytes;
	char buf[MAX_LEN];
	struct hostent *he;
	struct sockaddr_in their_addr;
//	printf("sercer IP :%s \n server Port : %d \n",ip,port);
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		LOG_WriteLog("COMMUNICATION .Create socket error");
		return -1;
	}

	unsigned long ul = 1;
	int rm = ioctl(sockfd, FIONBIO, &ul);
	if (rm == -1)
	{
		LOG_WriteLog("COMMUNICATION .set ioctl failed");
		close(sockfd);
		return 0;
	}

	their_addr.sin_family = AF_INET;
	their_addr.sin_port = htons(port);
	//their_addr.sin_addr = *((struct in_addr *)he->h_addr);
	their_addr.sin_addr.s_addr = inet_addr(ip);
	//bzero(&(their_addr.sin_zero), 8);
	memset(&(their_addr.sin_zero),0,8);
//	printf("start connect ! \n");
	if (connect(sockfd, (struct sockaddr *)&their_addr, sizeof(struct sockaddr)) == 0)
	{
//		printf("connected/n");
	}
	if (errno != EINPROGRESS)
	{
		LOG_WriteLog("COMMUNICATION .connect failed");
		return -2;
	}
//	printf("start select process !");
	struct timeval timeout;
	fd_set r;
	FD_ZERO(&r);
	FD_SET(sockfd, &r);
	timeout.tv_sec = connectTime / 1000;
	timeout.tv_usec = 0;
	int retval = select(sockfd + 1, NULL, &r, NULL, &timeout);
	if (retval == -1)
	{
		LOG_WriteLog("COMMUNICATION .select error");
		return -3;
	}
	else if (retval == 0)
	{
		LOG_WriteLog("COMMUNICATION .select timeout error");
		return -4;
	}
//	printf("connected success /n");

	unsigned long ul1 = 0;
	rm = ioctl(sockfd, FIONBIO, (unsigned long*)&ul1);
	if (rm == -1)
	{
		close(sockfd);
		return 0;
	}

	//设置发送超时
	struct timeval sendTimeout = { sendTime/1000,0};
	if (setsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO, (char *)&sendTimeout, sizeof(sendTimeout)) == -1)
	{
		LOG_WriteLog("COMMUNICATION .set send timeout error");

	//	printf("set send time out failed ! ");
		close(sockfd);
		return -6;
	}
	//设置接收超时
	struct timeval recvTimeout = { recvTime/1000,0};
	if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (const char *)&recvTimeout, sizeof(recvTimeout)) == -1)
	{
		close(sockfd);
		LOG_WriteLog("COMMUNICATION .set recv timeout error");
		return -6;
	}


	int ret;
	ret = send(sockfd, sendbuf, sendLen, 0);
	if (ret == -1)
	{
		LOG_WriteLog("COMMUNICATION - send timeout error");
		close(sockfd);
		return -5;
	}

	ret = recv(sockfd, recvbuf, MAX_LEN, 0);
	if (ret == -1)
	{
		LOG_WriteLog("COMMUNICATION - recv timeout error");
		close(sockfd);
		return -7;
	}
	close(sockfd);
	return ret;
}




#else
// WINDOWS  平台编译此套程序
#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")
/*
	返回值错误描述:
	-1 : WSAStartup() failed!
	-2 : Invalid WinSock version!
	-3 : socket() failed!
	-4 : connect() failed!
	-5 : send() failed!
	-6 : set failed
	-7 : recv() failed!

*/
#define MAX_LEN 8192

int CONN_COMMUNICATION(char const *ip, int port, char const *sendbuf, int sendLen, char *recvbuf, int connectTime, int sendTime, int recvTime)
{
	WORD wVersionRequested;
	WSADATA wsaData;
	int ret;
	SOCKET sClient;																			//连接套接字
	struct sockaddr_in saServer;															//地址信息
	//WinSock初始化
	wVersionRequested = MAKEWORD(2, 2);														//希望使用的WinSock DLL的版本
	ret = WSAStartup(wVersionRequested, &wsaData);
	if (ret != 0)
	{	return -1; }
	//确认WinSock DLL支持版本2.2
	if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2)
	{
		LOG_WriteLog("COMMUNICATION .can't suport WinSock 2.2 ");
		WSACleanup();
		return -2;
	}
	//创建Socket,使用TCP协议
	sClient = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sClient == INVALID_SOCKET)
	{
		LOG_WriteLog("COMMUNICATION .Create socket error");
		WSACleanup();
		return -3;
	}
	//设置发送超时
	if (setsockopt(sClient, SOL_SOCKET, SO_SNDTIMEO, (char *)&sendTime, sizeof(sendTime)) == SOCKET_ERROR){
		LOG_WriteLog("COMMUNICATION .set send timeout error");
		return -6;
	}
	//设置接收超时
	if (setsockopt(sClient, SOL_SOCKET, SO_RCVTIMEO, (char *)&recvTime, sizeof(recvTime)) == SOCKET_ERROR){
		LOG_WriteLog("COMMUNICATION .set recv timeout error");
		return -6;
	}
	//设置非阻塞方式连接 (为了设置连接超时)
	unsigned long ul = 1;
	ret = ioctlsocket(sClient, FIONBIO, (unsigned long*)&ul);
	if (ret == SOCKET_ERROR)return -6;
	//构建服务器地址信息
	saServer.sin_family = AF_INET; 
	saServer.sin_port = htons(port); 
	saServer.sin_addr.S_un.S_addr = inet_addr(ip);
	//连接服务器
	ret = connect(sClient, (struct sockaddr *)&saServer, sizeof(saServer));
	/*if (ret == SOCKET_ERROR)
	{
		LOG_WriteLog("COMMUNICATION .connect failed");
		closesocket(sClient);
		WSACleanup();
		return -4;
	}*/
	struct timeval timeout;
	fd_set r;
	FD_ZERO(&r);
	FD_SET(sClient, &r);
	timeout.tv_sec = connectTime / 1000;
	timeout.tv_usec = 0;
	ret = select(0, 0, &r, 0, &timeout);
	if (ret <= 0)
	{
		LOG_WriteLog("COMMUNICATION .select error");
		closesocket(sClient);
		WSACleanup();
		return 0;
	}
	//一般非锁定模式套接比较难控制，可以根据实际情况考虑 再设回阻塞模式
	unsigned long ul1 = 0;
	ret = ioctlsocket(sClient, FIONBIO, (unsigned long*)&ul1);
	if (ret == SOCKET_ERROR)
	{
		closesocket(sClient);
		WSACleanup();
		return -3;
	}
	ret = send(sClient, sendbuf, sendLen, 0);
	if (ret == SOCKET_ERROR)
	{
		LOG_WriteLog("COMMUNICATION - send timeout error");
		closesocket(sClient);
		WSACleanup(); 
		return -5;
	}

	ret = recv(sClient, recvbuf, MAX_LEN, 0);
	if (ret == SOCKET_ERROR)
	{
		LOG_WriteLog("COMMUNICATION - recv timeout error");
		closesocket(sClient);
		WSACleanup();
		return -7;
	}
	closesocket(sClient); 
	WSACleanup();
	return ret;
}

#endif
