/*
 _
(_)  _ __     __ _   _   _   _ __     ___   __      __
| | | '_ \   / _` | | | | | | '_ \   / _ \  \ \ /\ / /
| | | |_) | | (_| | | |_| | | | | | | (_) |  \ V  V /
|_| | .__/   \__,_|  \__, | |_| |_|  \___/    \_/\_/
    |_|              |___/

	处理网络连接相关操作
	author : szh 
	time	: 2015.8.5
*/


/* 通信过程*/
int CONN_COMMUNICATION(char const *ip, int port, char const *sendbuf, int sendLen, char *recvbuf, int connectTime, int sendTime, int recvTime);