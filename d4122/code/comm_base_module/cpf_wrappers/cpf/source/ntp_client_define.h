//////////////////////////////////////////////////////////////////////////
//ntp_client_define.h

#pragma once

#ifdef OS_LINUX
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>
#include <stdio.h>
typedef	int	 SOCKET;
#define SOCKET_ERROR -1
#define INVALID_SOCKET -1
#else
#include <winsock2.h>
#include <time.h>
#pragma comment(lib, "ws2_32.lib")
#endif


//ntp时间从年开始，本地时间从年开始，这是两者之间的差值

#define  JAN_1970   0x83aa7e80      //3600s*24h*(365days*70years+17days)

//x*10^(-6)*2^32 微妙数转 NtpTime 结构的 fraction 部分

#define  NTPFRAC(x) (4294 * (x) + ((1981 * (x)) >> 11))  

//NTPFRAC的逆运算

#define  USEC(x) (((x) >> 12) - 759 * ((((x) >> 10) + 32768) >> 16))


//#define  DEF_NTP_SERVER  "210.72.145.44"    //国家授时中心 ip,这个时间不能用了

#define DEF_NTP_SERVER "202.120.2.101"//ntp.sjtu.edu.cn 202.120.2.101 (上海交通大学网络中心NTP服务器地址）

//#define DEF_NTP_SERVER "stdtime.gov.hk" //香港标准时间

//#define DEF_NTP_SERVER "pool.ntp.org"     //ntp官方时间

#define  DEF_NTP_PORT   123

#define  DEF_PSEC       10

#define  DEF_PMIN       0

#define  DEF_PHOUR      0

#define  DEF_TIMEOUT    10

#define  DEF_LOGEN      1 



//ntp时间戳结构
typedef   struct   
{
	unsigned   int  integer;
	unsigned   int  fraction;
} NtpTime;

//校准信息结构
typedef   struct
{
	struct  timeval dlytime;
	struct  timeval offtime;
	struct  timeval newtime;

} NtpServResp;

//ntp客户端配置结构，对应 ntpclient.conf 中各项
typedef   struct
{
	UINT			hip;//服务器的IP，主机字节序
	unsigned   int  port;//主机字节序
	int  psec;
	int  pmin;
	int  phour;
	int  timeout;
	bool  logen;
	char  logpath[256];
} NtpConfig;


void  load_default_cfg(NtpConfig& NtpCfg);
bool  get_server_time( SOCKET  sock, NtpServResp *resp);
SOCKET  ntp_conn_server( UINT server_ip,int  port);
int  send_packet( SOCKET  fd);