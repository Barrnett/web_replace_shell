//////////////////////////////////////////////////////////////////////////
//PING.h

#pragma once

#include "cpf/cpf.h"

class CPF_CLASS PING
{
public:
	//超时时间单位为ms
	PING(unsigned int nTryNums=3,unsigned int nSendTimeout=3000, unsigned int nRcvTimeout = 3000,
		const void * pdata = NULL,int data_len = 0);
	~PING(void);

public:
	BOOL myping(DWORD dwNetOrderIP);
	BOOL mySendICMP(DWORD dwNetOrderIP);

	static BOOL ping(DWORD dwNetOrderIP,unsigned int nTryNums=3,
		unsigned int nSendTimeout=3000, unsigned int nRcvTimeout = 3000,
		const void * pdata = NULL,int data_len = 0);

	static BOOL SendIcmpRequest(DWORD dwNetOrderIP,
		int send_nums=3,unsigned int nSendTimeout=3000,
		const void * pdata=NULL,int data_len=0);

private:
	static BOOL inner_ping(int hsocket,DWORD dwNetOrderIP,
		unsigned int nTryNums,unsigned int recvTimeout,
		const void * pdata,int len);

	static BOOL inner_SendIcmpRequest(int hsocket,DWORD dwNetOrderIP,
		int send_nums,const void * pdata,int len);

private:
	enum{MAX_PACKET = 1024};

	static unsigned char		ms_append_data[];

	const void *		m_pAppend_Data;
	int					m_nAppend_DataLen;

	UINT_PTR			m_sockRaw;
	unsigned int		m_nSendTimeout;
	unsigned int		m_nRcvTimeout;
	int					m_nTryNums;	
	char				m_sendBuf[MAX_PACKET];
	char				m_recvBuf[MAX_PACKET];

private:
	static void fill_icmp_data(char * icmp_data,const void * pdata,int len);
	static int decode_resp(char *buf, int bytes,DWORD& srcip,DWORD& dstip,
		const void * pdata,int len);
};


