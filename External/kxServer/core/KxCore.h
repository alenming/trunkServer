/*
*   Core 服务器核心文件
*   跨平台预处理
*
*   2013-04-08 create By 宝爷
*   2015-04-23 Refactor By 宝爷
*/
#ifndef __KXCORE_H__
#define __KXCORE_H__

#ifndef FD_SETSIZE
#define FD_SETSIZE 1024  //修改服务器同时连接的最大连接数
#endif

#include "KxPlatform.h"
#include "KxObject.h"

#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#define MIN(a, b) (((a) > (b)) ? (b) : (a))

#include <assert.h>
#include <algorithm>
#define KXASSERT assert

//消除平台相关的时间，Socket差异
#if(KX_TARGET_PLATFORM == KX_PLATFORM_WIN32)

#include <WinSock2.h>
#include <WinSock.h>
#include <Windows.h>
#include <time.h>

// 通讯ID
typedef SOCKET KXCOMMID;
typedef int kxSockLen;

// 读写错误，是否可重试
#define SOCKERR_RW_RETRIABLE(e)			\
	((e) == WSAEWOULDBLOCK ||			\
	    (e) == WSAEINTR)

// connect错误，是否可重试
#define SOCKERR_CONNECT_RETRIABLE(e)	\
	((e) == WSAEWOULDBLOCK ||			\
	    (e) == WSAEINTR ||				\
	    (e) == WSAEINPROGRESS ||		\
	    (e) == WSAEINVAL)

// accept错误，是否可重试
#define SOCKERR_ACCEPT_RETRIABLE(e)		\
	SOCKERR_RW_RETRIABLE(e)

// connect错误，是否被拒绝
#define SOCKERR_CONNECT_REFUSED(e)		\
	((e) == WSAECONNREFUSED)

#define snprintf  sprintf_s
#define stlocaltime(pTime,ptm) localtime_s(ptm,pTime)
#define strncpys strncpy_s
#define sstrdup _strdup
#define KXINVALID_COMMID (INVALID_SOCKET)

struct timezone
{
	int tz_minuteswest;
	int tz_dsttime;
};

int gettimeofday(struct timeval * val, struct timezone * zone);

#pragma comment(lib, "ws2_32.lib")

#else

#define KXINVALID_COMMID (-1)

#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/time.h>
#include<errno.h>
#include<fcntl.h>
#include<string.h>
#include<unistd.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<netinet/tcp.h>
#include <signal.h>
#include <stdarg.h>


// 读写错误，是否可重试
#define SOCKERR_RW_RETRIABLE(e)			\
	((e) == EINTR || (e) == EAGAIN ||	\
	(e) == EWOULDBLOCK)

// connect错误，是否可重试
#define SOCKERR_CONNECT_RETRIABLE(e)	\
	((e) == EINTR || (e) == EINPROGRESS)

// accept错误，是否可重试
#define SOCKERR_ACCEPT_RETRIABLE(e)		\
	((e) == EINTR || (e) == EAGAIN ||	\
	(e) == ECONNABORTED)

// connect错误，是否被拒绝
#define SOCKERR_CONNECT_REFUSED(e)		\
	((e) == ECONNREFUSED)

typedef int KXCOMMID;
typedef socklen_t kxSockLen;
#define strncpys strncpy
#define sstrdup strdup
#define vsnprintf_s vsnprintf
#define stlocaltime(pTime,ptm) localtime_r(pTime,ptm)
#endif

#endif
