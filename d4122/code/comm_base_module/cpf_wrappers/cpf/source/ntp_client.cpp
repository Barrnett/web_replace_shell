//////////////////////////////////////////////////////////////////////////
//ntp_client.cpp

#include "cpf/ntp_client.h"
#include "ntp_client_define.h"
#include "cpf/addr_tools.h"
#include "cpf/other_tools.h"

int CPF_CLASS GetNTPTime(ACE_UINT32 ntp_server,ACE_UINT16 port,struct timeval * now_time)
{
	NtpConfig ntp_cfg;

	load_default_cfg(ntp_cfg);
	
	if( ntp_server != 0 )
		ntp_cfg.hip = ntp_server;

	if( port != 0 )
		ntp_cfg.port = port;

	SOCKET hsock = ntp_conn_server(ntp_cfg.hip,ntp_cfg.port);

	if( hsock == INVALID_SOCKET || hsock == 0 )
		return -1;

	if( send_packet(hsock) <= 0 )
	{
		closesocket(hsock);
		return -1;
	}

	NtpServResp resp;

	memset(&resp,0x00,sizeof(resp));

	if( !get_server_time(hsock,&resp) )
	{
		closesocket(hsock);
		return -1;
	}

	*now_time = resp.newtime;

	closesocket(hsock);

	return 0;	
}


int  send_packet( SOCKET  fd)
{
	unsigned   int  data[12];

	int  ret;

	struct  timeval now;

#define  LI 0 //协议头中的元素

#define  VN 3             //版本

#define  MODE 3           //模式 : 客户端请求

#define  STRATUM 0

#define  POLL 4           //连续信息间的最大间隔

#define  PREC -6          //本地时钟精度

	memset(( char *)data, 0,  sizeof (data));

	data[0] = htonl((LI << 30) | (VN << 27) | (MODE << 24) 

		| (STRATUM << 16) | (POLL << 8) | (PREC & 0xff));

	data[1] = htonl(1 << 16);

	data[2] = htonl(1 << 16);

	//获得本地时间

	ACE_Time_Value ace_time = ACE_OS::gettimeofday();

	now.tv_sec = (long)ace_time.sec();
	now.tv_usec = (long)ace_time.usec();


	data[10] = htonl(now.tv_sec + JAN_1970);

	data[11] = htonl(NTPFRAC(now.tv_usec));

	ret = send(fd, (const char *)data, 48, 0);

	return ret;
} 

bool  get_server_time( SOCKET  sock, NtpServResp *resp)
{
	int  ret;

	unsigned   int  data[12];    

	NtpTime oritime, rectime, tratime, destime;

	struct  timeval offtime, dlytime;

	struct  timeval now;

	memset(&data,0x00,sizeof(data));

	ret = recvfrom (sock, (char *)data,  sizeof (data), 0, NULL, 0);

	if  (ret == -1)
	{
		return false;
	}
	else   if  (ret == 0)
	{
		return false;
	}

	ACE_Time_Value ace_time = ACE_OS::gettimeofday();

	now.tv_sec = (long)ace_time.sec();
	now.tv_usec = (long)ace_time.usec();

	destime.integer  = now.tv_sec + JAN_1970;

	destime.fraction = NTPFRAC (now.tv_usec);

#define  DATA(i) ntohl((( unsigned   int  *)data)[i])

	oritime.integer  = DATA(6);

	oritime.fraction = DATA(7);

	rectime.integer  = DATA(8);

	rectime.fraction = DATA(9);

	tratime.integer  = DATA(10);

	tratime.fraction = DATA(11);

#undef  DATA

	//与 send_packet 中发送的 tratime.faction 一致


	//Originate Timestamp       T1        客户端发送请求的时间

	//Receive Timestamp        T2        服务器接收请求的时间

	//Transmit Timestamp       T3        服务器答复时间

	//Destination Timestamp     T4        客户端接收答复的时间

	//网络延时 d 和服务器与客户端的时差 t

	//d = (T2 - T1) + (T4 - T3); t = [(T2 - T1) + (T3 - T4)] / 2;

#define  MKSEC(ntpt)   ((ntpt).integer - JAN_1970)

#define  MKUSEC(ntpt)  (USEC((ntpt).fraction))

#define  TTLUSEC(sec,usec)   (( long   long )(sec)*1000000 + (usec))

#define  GETSEC(us)    ((us)/1000000) 

#define  GETUSEC(us)   ((us)%1000000) 



	long   long  orius, recus, traus, desus, offus, dlyus;



	orius = TTLUSEC(MKSEC(oritime), MKUSEC(oritime));

	recus = TTLUSEC(MKSEC(rectime), MKUSEC(rectime));

	traus = TTLUSEC(MKSEC(tratime), MKUSEC(tratime));

	desus = TTLUSEC(now.tv_sec, now.tv_usec);



	offus = ((recus - orius) + (traus - desus))/2;

	dlyus = (recus - orius) + (desus - traus);

	offtime.tv_sec  = (long)GETSEC(offus);

	offtime.tv_usec = (long)GETUSEC(offus);

	dlytime.tv_sec  = (long)GETSEC(dlyus);

	dlytime.tv_usec = (long)GETUSEC(dlyus);


	struct  timeval  new_time ;


	//粗略校时

	//new.tv_sec = tratime.integer - JAN_1970;

	//new.tv_usec = USEC(tratime.fraction);

	//精确校时

	new_time.tv_sec = destime.integer - JAN_1970 + offtime.tv_sec;

	new_time.tv_usec = USEC(destime.fraction) + offtime.tv_usec;

	resp->newtime =  new_time;

	resp->dlytime = dlytime;

	resp->offtime = offtime;

#ifdef _DEBUG

	const char * str_now_time = CPF::FormatTime(6,(time_t)new_time.tv_sec);

#endif

	return   true ;

}

SOCKET  ntp_conn_server( UINT server_ip, int  port)
{
	SOCKET hsock = INVALID_SOCKET;

	int  addr_len =  sizeof ( struct  sockaddr_in);

	struct  sockaddr_in addr_src; //本地 socket  <netinet/in.h>

	struct  sockaddr_in addr_dst; //服务器 socket

	//UDP数据报套接字

	hsock = socket(PF_INET, SOCK_DGRAM, 0);

	if  (hsock == INVALID_SOCKET)
	{
		return -1;
	}

#ifdef OS_LINUX
	struct timeval timeout={2,0};//2s

	setsockopt(hsock,SOL_SOCKET,SO_SNDTIMEO,(const char*)&timeout,sizeof(timeout));
	setsockopt(hsock,SOL_SOCKET,SO_RCVTIMEO,(const char*)&timeout,sizeof(timeout));
#else
	int timeout = 2000; //2s

	setsockopt(hsock,SOL_SOCKET,SO_SNDTIMEO,(const char *)&timeout,sizeof(timeout));
	setsockopt(hsock,SOL_SOCKET,SO_RCVTIMEO,(const char *)&timeout,sizeof(timeout));
#endif

	memset(&addr_src, 0, addr_len);

	addr_src.sin_family = AF_INET;

	addr_src.sin_port = htons(0);

	addr_src.sin_addr.s_addr = htonl(INADDR_ANY); //<arpa/inet.h>

	//绑定本地地址

	if  (-1 == bind(hsock, ( struct  sockaddr *) &addr_src, addr_len))
	{
		closesocket(hsock);

		return -1;
	}

	memset(&addr_dst, 0, addr_len);

	addr_dst.sin_family = AF_INET;

	addr_dst.sin_port = htons(port);

	addr_dst.sin_addr.s_addr = htonl(server_ip);

	if  (-1 == connect(hsock, ( struct  sockaddr *) &addr_dst, addr_len))
	{
		closesocket(hsock);
		return -1;
	}

	return  hsock;

}

void  load_default_cfg(NtpConfig& NtpCfg )
{
	NtpCfg.hip = CPF::string_to_hip(DEF_NTP_SERVER);

	NtpCfg.port = DEF_NTP_PORT;

	NtpCfg.psec = DEF_PSEC;

	NtpCfg.pmin = DEF_PMIN;

	NtpCfg.phour = DEF_PHOUR;

	NtpCfg.timeout = DEF_TIMEOUT;

	NtpCfg.logen = DEF_LOGEN;

	return;

} 