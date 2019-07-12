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


//ntpʱ����꿪ʼ������ʱ����꿪ʼ����������֮��Ĳ�ֵ

#define  JAN_1970   0x83aa7e80      //3600s*24h*(365days*70years+17days)

//x*10^(-6)*2^32 ΢����ת NtpTime �ṹ�� fraction ����

#define  NTPFRAC(x) (4294 * (x) + ((1981 * (x)) >> 11))  

//NTPFRAC��������

#define  USEC(x) (((x) >> 12) - 759 * ((((x) >> 10) + 32768) >> 16))


//#define  DEF_NTP_SERVER  "210.72.145.44"    //������ʱ���� ip,���ʱ�䲻������

#define DEF_NTP_SERVER "202.120.2.101"//ntp.sjtu.edu.cn 202.120.2.101 (�Ϻ���ͨ��ѧ��������NTP��������ַ��

//#define DEF_NTP_SERVER "stdtime.gov.hk" //��۱�׼ʱ��

//#define DEF_NTP_SERVER "pool.ntp.org"     //ntp�ٷ�ʱ��

#define  DEF_NTP_PORT   123

#define  DEF_PSEC       10

#define  DEF_PMIN       0

#define  DEF_PHOUR      0

#define  DEF_TIMEOUT    10

#define  DEF_LOGEN      1 



//ntpʱ����ṹ
typedef   struct   
{
	unsigned   int  integer;
	unsigned   int  fraction;
} NtpTime;

//У׼��Ϣ�ṹ
typedef   struct
{
	struct  timeval dlytime;
	struct  timeval offtime;
	struct  timeval newtime;

} NtpServResp;

//ntp�ͻ������ýṹ����Ӧ ntpclient.conf �и���
typedef   struct
{
	UINT			hip;//��������IP�������ֽ���
	unsigned   int  port;//�����ֽ���
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