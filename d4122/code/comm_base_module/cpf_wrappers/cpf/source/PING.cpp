//////////////////////////////////////////////////////////////////////////
//PING.cpp

#include "stdafx.h"
#include "cpf/PING.h"
#include "cpf/crc_tools.h"
#include "cpf/ostypedef.h"

#define ICMP_ECHO 8
#define ICMP_ECHOREPLY 0

#define ICMP_MIN 8 // minimum 8 byte icmp packet (just header)

/* The IP header */
typedef struct _iphdr {
	unsigned char h_len:4;          // length of the header
	unsigned char version:4;        // Version of IP
	unsigned char tos;             // Type of service
	unsigned short total_len;      // total length of the packet
	unsigned short ident;          // unique identifier
	unsigned short frag_and_flags; // flags
	unsigned char  ttl; 
	unsigned char proto;           // protocol (TCP, UDP etc)
	unsigned short checksum;       // IP checksum

	unsigned int sourceIP;
	unsigned int destIP;
}IpHeader;


#pragma pack(push)
#pragma pack(1)
// ICMP header
typedef struct _ihdr {
	BYTE	i_type;
	BYTE	i_code; /* type sub code */
	USHORT	i_cksum;
	USHORT	i_id;
	USHORT	i_seq;

	char	i_data[1];

}IcmpHeader;
#pragma pack(pop)

unsigned char PING::ms_append_data[] = {0,1,2,3,4,5,6,7,8,9,
								'a','B','c','D','e','F','g',
								'H','i','J','k','L','m','N',
								'o','P','q','R','s','T','u',
								'V','w','X','y','Z'};


PING::PING(unsigned int nTryNums, unsigned int nSendTimeout, unsigned int nRcvTimeout,
		   const void * pdata,int data_len)
{
	if( pdata )
	{
		m_pAppend_Data = pdata;
		m_nAppend_DataLen = data_len;
	}
	else
	{
		m_pAppend_Data = (const void *)ms_append_data;
		m_nAppend_DataLen = sizeof(ms_append_data);
	}
	
	m_sockRaw = (int)socket(AF_INET, SOCK_RAW,IPPROTO_ICMP);

	if (m_sockRaw == INVALID_SOCKET ) 
	{
		return ;
	}	

	if (setsockopt (m_sockRaw, SOL_SOCKET, SO_SNDTIMEO,
		(char *)&nSendTimeout, sizeof(nSendTimeout))==SOCKET_ERROR )	
	{
		closesocket(m_sockRaw);
		m_sockRaw = INVALID_SOCKET;
		return ;
	}

	if (setsockopt (m_sockRaw, SOL_SOCKET, SO_RCVTIMEO,
		(char *)&nRcvTimeout, sizeof(nRcvTimeout))==SOCKET_ERROR )	
	{
		closesocket(m_sockRaw);
		m_sockRaw = INVALID_SOCKET;
		return ;
	}

	m_nSendTimeout = nSendTimeout;
	m_nRcvTimeout = nRcvTimeout;
	m_nTryNums = nTryNums;	
}

PING::~PING(void)
{
	if( INVALID_SOCKET != m_sockRaw )
	{
		closesocket(m_sockRaw);
	}
}

BOOL PING::myping(DWORD dwNetOrderIP)
{
	return PING::inner_ping(m_sockRaw,dwNetOrderIP,
		m_nTryNums,m_nRcvTimeout,
		m_pAppend_Data,m_nAppend_DataLen);

}

BOOL PING::mySendICMP(DWORD dwNetOrderIP)
{
	return PING::inner_SendIcmpRequest(m_sockRaw,dwNetOrderIP,
		m_nTryNums,m_pAppend_Data,m_nAppend_DataLen);
}

BOOL PING::ping(DWORD dwNetOrderIP,unsigned int nTryNums,
				unsigned int nSendTimeout, unsigned int nRcvTimeout,
				const void * pdata,int data_len)
{
	int sockRaw = (int)socket(AF_INET, SOCK_RAW,IPPROTO_ICMP);

	if (sockRaw == INVALID_SOCKET ) 
	{
		return false;
	}	

	if (setsockopt (sockRaw, SOL_SOCKET, SO_SNDTIMEO,
		(char *)&nSendTimeout, sizeof(nSendTimeout))==SOCKET_ERROR )	
	{
		closesocket(sockRaw);
		sockRaw = INVALID_SOCKET;
		return false;
	}

	if (setsockopt (sockRaw, SOL_SOCKET, SO_RCVTIMEO,
		(char *)&nRcvTimeout, sizeof(nRcvTimeout))==SOCKET_ERROR )	
	{
		closesocket(sockRaw);
		sockRaw = INVALID_SOCKET;
		return false;
	}

	BOOL bPingOK = false;

	if( pdata )
	{
		bPingOK = PING::inner_ping(sockRaw,dwNetOrderIP,
			nTryNums,nRcvTimeout,
			pdata,data_len);
	}
	else
	{
		bPingOK = PING::inner_ping(sockRaw,dwNetOrderIP,
			nTryNums,nRcvTimeout,
			ms_append_data,sizeof(ms_append_data));
	}

	closesocket(sockRaw);

	return bPingOK;

}

BOOL PING::SendIcmpRequest(DWORD dwNetOrderIP,
						   int send_nums,unsigned int nSendTimeout,
						   const void * pdata,int data_len)
{
	int sockRaw = (int)socket(AF_INET, SOCK_RAW,IPPROTO_ICMP);

	if (sockRaw == INVALID_SOCKET ) 
	{
		return false;
	}	

	if (setsockopt (sockRaw, SOL_SOCKET, SO_SNDTIMEO,
		(char *)&nSendTimeout, sizeof(nSendTimeout))==SOCKET_ERROR )	
	{
		closesocket(sockRaw);
		sockRaw = INVALID_SOCKET;
		return false;
	}


	BOOL bSendOK = false;

	if( pdata )
	{
		bSendOK = PING::inner_SendIcmpRequest(sockRaw,dwNetOrderIP,
			send_nums,pdata,data_len);
	}
	else
	{
		bSendOK = PING::inner_SendIcmpRequest(sockRaw,dwNetOrderIP,
			send_nums,ms_append_data,sizeof(ms_append_data));
	}

	closesocket(sockRaw);

	return bSendOK;

}


BOOL PING::inner_ping(int hsocket,DWORD dwNetOrderIP,
					  unsigned int nTryNums,unsigned int recvTimeout,
					  const void * pdata,int len)
{
	char icmp_data[MAX_PACKET];

	fill_icmp_data(icmp_data,pdata,len);

	((IcmpHeader*)icmp_data)->i_cksum = 
		ACE_HTONS(CPF::ComputeCheckSum((BYTE *)icmp_data,sizeof(IcmpHeader)-1+len));

	sockaddr_in dest;

	memset( &dest, 0x00, sizeof(dest) );
	dest.sin_family = AF_INET;
	dest.sin_addr.s_addr = dwNetOrderIP;

	BOOL bPingOK = FALSE;

	char icmp_data_recv[MAX_PACKET];

	for( unsigned int i = 0; i < nTryNums && bPingOK == FALSE ; i++ )
	{	
		int bwrote = sendto(hsocket,icmp_data,sizeof(IcmpHeader)-1+len,0,
			(struct sockaddr*)&dest,sizeof(dest));

		if( bwrote < (int)sizeof(IcmpHeader)-1+len )
			continue;

		ACE_Time_Value lastTime = ACE_OS::gettimeofday();

		do{	
			sockaddr_in from;

			int fromlen = (int)(sizeof(from));

			unsigned int bread = recvfrom(hsocket,icmp_data_recv,
				sizeof(icmp_data_recv),0,
				(struct sockaddr*)&from,&fromlen);

			if( SOCKET_ERROR == bread )
			{
				break;
			}

			DWORD dwsrcip,dwdstip;

			if ( decode_resp(icmp_data_recv,bread,dwsrcip,dwdstip,pdata,len)==1 )
			{
				if( dwNetOrderIP == dwsrcip )
				{
					bPingOK = TRUE;
					break;
				}					
			}		

		}while( (ACE_OS::gettimeofday() - lastTime).get_msec() < recvTimeout );
	}

	return bPingOK;
}

BOOL PING::inner_SendIcmpRequest(int hsocket,DWORD dwNetOrderIP,int send_nums,
								 const void * pdata,int len)
{
	char icmp_data[MAX_PACKET];

	fill_icmp_data(icmp_data,pdata,len);

	((IcmpHeader*)icmp_data)->i_cksum = 
		ACE_HTONS(CPF::ComputeCheckSum((BYTE *)icmp_data,sizeof(IcmpHeader)-1+len));

	sockaddr_in dest;

	memset( &dest, 0x00, sizeof(dest) );
	dest.sin_family = AF_INET;
	dest.sin_addr.s_addr = dwNetOrderIP;

	for(int i = 0; i < send_nums; ++i)
	{
		sendto(hsocket,icmp_data,sizeof(IcmpHeader)-1+len,0,
			(struct sockaddr*)&dest,sizeof(dest));

	}
	
	return true;
}


void PING::fill_icmp_data(char * icmp_data,const void * pdata,int len)
{
	static USHORT seq = 0;	

	++seq;

	IcmpHeader *icmp_hdr = (IcmpHeader*)icmp_data;

	icmp_hdr->i_type = ICMP_ECHO;
	icmp_hdr->i_code = 0;
	icmp_hdr->i_id = (WORD)GetCurrentProcessId();
	icmp_hdr->i_seq = ACE_NTOHS(seq);
	icmp_hdr->i_cksum = 0;

	memcpy(icmp_hdr->i_data,pdata,len);

	return;
	
}

int PING::decode_resp(char *buf, int bytes,DWORD& srcip,DWORD& dstip,
					  const void * pdata,int len)
{
	IpHeader *iphdr;
	IcmpHeader *icmphdr;
	unsigned short iphdrlen;

	iphdr = (IpHeader *)buf;

	iphdrlen = iphdr->h_len * 4 ; // number of 32-bit words *4 = bytes
	u_short total_len = htons(iphdr->total_len);

	if (bytes  < iphdrlen + ICMP_MIN) 
	{
		return FALSE;
	}

	icmphdr = (IcmpHeader*)(buf + iphdrlen);

	if (icmphdr->i_type != ICMP_ECHOREPLY) 
	{	
		return FALSE;
	}

	if (icmphdr->i_id != (WORD)GetCurrentProcessId()) 
	{	
		return FALSE;
	}

	if( 0 != memcmp(icmphdr->i_data,pdata,len) )
	{
		return false;
	}

	srcip = iphdr->sourceIP;
	dstip = iphdr->destIP;

	return TRUE;
}

