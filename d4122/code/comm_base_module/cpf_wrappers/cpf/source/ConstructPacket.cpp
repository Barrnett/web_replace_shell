#include "cpf/ConstructPacket.h"
#include "cpf/crc_tools.h"
#include "cpf/CommonEtherDecode.h"
#include "cpf/CommonIPv4Decode.h"
#include "cpf/CommonDNSDecode.h"
#include "cpf/CommonTCPUDPDecode.h"
#include "cpf/proto_struct_def.h"


void TurnTCPSeq(TCPPacketHead * pTcpHead,unsigned int iplen)
{
	DWORD dwSeq = ACE_NTOHL(pTcpHead->SeqNo);

	if (0==pTcpHead->AckNo)
		pTcpHead->SeqNo = htonl(1111);
	else
		pTcpHead->SeqNo = pTcpHead->AckNo;

	unsigned int ack = dwSeq+iplen-(20+((pTcpHead->HLen&0xf0)>>4)*4);

	pTcpHead->AckNo = htonl(ack);

	return;
}

void NextTCPSeq(TCPPacketHead * pTcpHead,unsigned int iplen)
{
	unsigned int SeqNo = ntohl(pTcpHead->SeqNo)+iplen-(20+((pTcpHead->HLen&0xf0)>>4)*4);
	pTcpHead->SeqNo = htonl(SeqNo);
}



CConstructPacket::CConstructPacket(void)
{
}

CConstructPacket::~CConstructPacket(void)
{
}

unsigned int CConstructPacket::MAC_Contruct_Packet(BYTE * outbuf,int vlan_id,
										   const BYTE * dstmacaddr,const BYTE * srcmacaddr,WORD protottype,
										   const BYTE * upperdata,size_t upperlen)
{
	unsigned int mac_header_len = MAC_Contruct_Header(outbuf,vlan_id,dstmacaddr,srcmacaddr,protottype);

	if( (int)upperlen > 0 )
		memcpy(outbuf+mac_header_len,upperdata,upperlen);

	return (unsigned int)(mac_header_len+upperlen);
}

unsigned int CConstructPacket::MAC_Contruct_Header(BYTE * outbuf,int vlan_id,
										const BYTE * dstmacaddr,const BYTE * srcmacaddr,
										WORD protottype//网络字节序
										)
{
	BYTE * pos = outbuf;

	memcpy(pos,dstmacaddr,6);
	pos += 6;

	memcpy(pos,srcmacaddr,6);
	pos += 6;

	if( vlan_id != -1 )
	{
		*(ACE_UINT16 *)(pos) = ACE_HTONS(0x08100);
		pos += 2;

		*(ACE_UINT16 *)(pos) = ACE_HTONS(vlan_id);
		pos += 2;
	}

	*(WORD *)(pos) = protottype;
	pos += 2;

	return (int)(pos-outbuf);
}

unsigned int CConstructPacket::MAC_Contruct_Header_For_IPPackte(BYTE * outbuf,
										 const BYTE * dstmacaddr,const BYTE * srcmacaddr,
										 ACE_UINT32 vlan_id,ACE_UINT16 pppoe_session_id,
										 int IPPacketLen//包括IP包的总长度，包括IP头长度
										 )
{
	if( outbuf == NULL )
	{
		unsigned int header_len = 0;

		header_len += 12;//两个mac地址

		if( vlan_id != -1 )
		{
			header_len += 4;
		}

		if( pppoe_session_id != 0xffff )
		{
			header_len += 2;//协议字段，2个字节

			header_len += sizeof(pppoe_header);
			header_len += 2;//协议字段，2个字节
		}
		else
		{
			header_len += 2;//协议字段，2个字节
		}

		return header_len;
	}

	BYTE * pos = outbuf;

	memcpy(pos,dstmacaddr,6);
	pos += 6;

	memcpy(pos,(const void *)srcmacaddr,6);
	pos += 6;

	if( vlan_id != -1 )
	{
		*(ACE_UINT16 *)(pos) = ACE_HTONS(0x08100);
		pos += 2;

		*(ACE_UINT16 *)(pos) = ACE_HTONS(vlan_id);
		pos += 2;
	}

	if( pppoe_session_id != 0xffff )
	{
		*(ACE_UINT16 *)(pos) = ACE_HTONS(0x8864);
		pos += 2;

		pppoe_header * header = (pppoe_header *)pos;
		header->version = 1;
		header->type = 1;

		header->code = 0;
		header->session_id = pppoe_session_id;

		header->payload_len = ACE_HTONS(2+IPPacketLen);

		memcpy(pos,header,sizeof(pppoe_header));
		pos += sizeof(pppoe_header);

		*(ACE_UINT16 *)pos = ACE_HTONS(0x0021);;
		pos += 2;
	}
	else
	{
		*(ACE_UINT16 *)(pos) = ACE_HTONS(0x0800);
		pos += 2;
	}

	return (unsigned int)(pos-outbuf);
}

//从mac层构造中断tcp的数据包
unsigned int CConstructPacket::MAC_Construct_Intrupt_TCP_Packet(BYTE * pMacPacket,unsigned int maclen,BOOL bTurn)
{
	if (NULL==pMacPacket || maclen<=14)
		return 0;

	BYTE * pMacAddrHeader = NULL;
	BYTE * pIPPacket = NULL;
	unsigned int iplen = 0;
	int vlan_id = -1;

	unsigned int upper_proto =  mac_simple_decode_to_ip(pMacPacket,maclen,vlan_id,pMacAddrHeader,pIPPacket,iplen);

	if( (unsigned int)-1 == upper_proto )
		return 0;

	turn_to_mac(pMacAddrHeader);

	unsigned int ipdatalen = IP_Construct_Intrupt_TCP_Packet(pIPPacket,iplen,bTurn);

	if( CCommonEtherDecode::INDEX_PID_MAC_PPPOE == upper_proto )
	{
		if( vlan_id == -1 )
			*(unsigned short *)(pMacAddrHeader+12+2+4) = ACE_HTONS(ipdatalen+2);
		else
			*(unsigned short *)(pMacAddrHeader+12+4+2+4) = ACE_HTONS(ipdatalen+2);
	}

	return ipdatalen+(int)(pIPPacket-pMacPacket);
}

unsigned int CConstructPacket::IP_Construct_Packet(BYTE * outbuf,ACE_UINT32 dwIPSrc,ACE_UINT32 dwIPDst,
										BYTE Proto,
										const BYTE * upperdata,size_t upperlen,int ttl,int tos,short ipid)
{
	
	unsigned int ip_header_len = IP_Construct_Header(outbuf,dwIPSrc,dwIPDst,Proto,upperlen,ttl,tos,ipid);

	if( upperlen > 0 )
	{
		memcpy(outbuf + ip_header_len, upperdata, upperlen);
	}

	return (unsigned int)(ip_header_len+upperlen);

}

unsigned int CConstructPacket::IP_Construct_Header(BYTE * outbuf,
										ACE_UINT32 dwIPSrc,ACE_UINT32 dwIPDst,
										BYTE Proto,size_t upperlen,
										int ttl,int tos,short ipid)
{
	IPPacketHead* pIpHead = (IPPacketHead*)outbuf;


	pIpHead->VerHLen = 0x45;
	pIpHead->Type = tos;
	pIpHead->TtlLen = (ACE_UINT16)(ACE_HTONS(IP_HEAD_LEN+upperlen));
	pIpHead->Id = ipid;
	pIpHead->FlgOff = ACE_HTONS(0x4000);
	pIpHead->TTL = (unsigned char)ttl;
	pIpHead->Proto = Proto;
	pIpHead->ChkSum = 0X00;
	pIpHead->SourIP = dwIPSrc;
	pIpHead->DestIP = dwIPDst;

	pIpHead->ChkSum =ACE_HTONS(CPF::ComputeCheckSum( (BYTE *)outbuf, IP_HEAD_LEN ));

	return (unsigned int)IP_HEAD_LEN;
}


//构造中断tcp的数据包
unsigned int CConstructPacket::IP_Construct_Intrupt_TCP_Packet(BYTE * pIPPacket,unsigned int iplen,BOOL bTurn)
{
	//ip
	IPPacketHead* pIpHead = (IPPacketHead*)pIPPacket;
	BYTE * p = pIPPacket;


	unsigned short ipId = ntohs(pIpHead->Id);
	pIpHead->Id = ACE_HTONS(++ipId);

	if (bTurn) {
		unsigned int iptmp = pIpHead->SourIP ;
		pIpHead->SourIP = pIpHead->DestIP;
		pIpHead->DestIP = iptmp;
	}

	const int ip_header_len = ((pIpHead->VerHLen&0x0f)<<2);

	p += ip_header_len;

	//tcp
	TCPPacketHead* pTcpHead = (TCPPacketHead*)p;

	const int tcp_header_len = ((pTcpHead->HLen>>2)&0xFC);//从TCP头中取出头部长度

	p += tcp_header_len;

	if (bTurn)
	{
		TurnTCPSeq(pTcpHead,ACE_NTOHS(pIpHead->TtlLen));
	} 
	else 
	{
		NextTCPSeq(pTcpHead,ACE_NTOHS(pIpHead->TtlLen));
	}

	pTcpHead->Flag = (MASK_TCP_RST|MASK_TCP_ACK);

	if (bTurn) {
		unsigned short porttmp = pTcpHead->SourPort;
		pTcpHead->SourPort = pTcpHead->DestPort;
		pTcpHead->DestPort = porttmp;
	}
	static unsigned short iii = 0;
	pTcpHead->WndSize = ACE_HTONS(++iii);
	pTcpHead->UrgPtr = 0;

	pIpHead->TtlLen = ACE_HTONS(tcp_header_len+ip_header_len);

	pIpHead->ChkSum = 0x0000;
	pIpHead->ChkSum = ACE_HTONS(CPF::ComputeCheckSum( (BYTE *)pIpHead, ip_header_len ));

	pTcpHead->ChkSum = 0x0000;
	pTcpHead->ChkSum = ACE_HTONS(CPF::CalculateTCPCheckSum(pIpHead->SourIP,pIpHead->DestIP,(const BYTE *)pTcpHead,tcp_header_len));

	return ip_header_len+tcp_header_len;

}

//从mac层构造干扰udp的数据包,返回数据包的长度
unsigned int CConstructPacket::MAC_Construct_Disturb_UDP_Packet(IN OUT BYTE * pMacPacket,unsigned int maclen,BOOL bTurn)
{
	if (NULL==pMacPacket || maclen<=14)
		return 0;

	BYTE * pMacAddrHeader = NULL;
	BYTE * pIPPacket = NULL;
	unsigned int iplen = 0;
	int vlan_id = -1;

	unsigned int upper_proto =  mac_simple_decode_to_ip(pMacPacket,maclen,vlan_id,pMacAddrHeader,pIPPacket,iplen);

	if( (unsigned int)-1 == upper_proto )
		return 0;

	turn_to_mac(pMacAddrHeader);

	unsigned int ipdatalen = IP_Construct_Disturb_UDP_Packet(pIPPacket,maclen-(int)(pIPPacket-pMacPacket),bTurn);

	if( CCommonEtherDecode::INDEX_PID_MAC_PPPOE == upper_proto )
	{
		if( vlan_id == -1 )
			*(unsigned short *)(pMacAddrHeader+12+2+4) = ACE_HTONS(ipdatalen+2);
		else
			*(unsigned short *)(pMacAddrHeader+12+4+2+4) = ACE_HTONS(ipdatalen+2);

	}

	return ipdatalen+(int)(pIPPacket-pMacPacket);

}

//从IP层构造干扰udp的数据包,返回数据包的长度
unsigned int CConstructPacket::IP_Construct_Disturb_UDP_Packet(IN OUT BYTE * pIPPacket,unsigned int iplen,BOOL bTurn)
{
	//ip
	IPPacketHead* pIpHead = (IPPacketHead*)pIPPacket;

	if (bTurn) 
	{
		unsigned int iptmp = pIpHead->SourIP ;
		pIpHead->SourIP = pIpHead->DestIP;
		pIpHead->DestIP = iptmp;
	}

	const int ip_header_len = ((pIpHead->VerHLen&0x0f)<<2);

	BYTE * p = pIPPacket + ip_header_len;

	//tcp
	UDPPacketHead* pUDPHead = (UDPPacketHead*)p;

	if (bTurn) 
	{
		unsigned short porttmp = pUDPHead->SourPort;
		pUDPHead->SourPort = pUDPHead->DestPort;
		pUDPHead->DestPort = porttmp;
	}

	//一半的包使用乱码干扰，一半使用原来的数据反射
	static int timers = 0;

	if( timers % 2 == 0 )
	{
		pUDPHead->ChkSum = 0;

		p += sizeof(UDPPacketHead);

		memset(p,0xcd,pUDPHead->Len-sizeof(UDPPacketHead));

		ACE_UINT16 wchecksum = CPF::CalculateUDPCheckSum_NetOrder(pIpHead->SourIP,pIpHead->DestIP,
			(const BYTE *)pUDPHead,ACE_NTOHS(pUDPHead->Len));

		pUDPHead->ChkSum = wchecksum;
	}
	else
	{

	}

	return iplen;
}


unsigned int CConstructPacket::MAC_Construct_Intrupt_UDP_Packet(IN const BYTE * pOrginMACPacket,int orig_pkt_len,
													 BYTE * pOutBufferPacket,int outbufferlen,int code)
{
	if (NULL==pOrginMACPacket || orig_pkt_len<=14)
		return 0;

	BYTE * pMacAddrHeader = NULL;
	BYTE * pIPPacket = NULL;
	unsigned int iplen = 0;
	int vlan_id = -1;

	unsigned int upper_proto =  mac_simple_decode_to_ip((BYTE *)pOrginMACPacket,orig_pkt_len,vlan_id,pMacAddrHeader,pIPPacket,iplen);

	if( (unsigned int)-1 == upper_proto )
		return 0;

	memcpy(pOutBufferPacket,pOrginMACPacket,pIPPacket-pOrginMACPacket);

	pMacAddrHeader = pOutBufferPacket + (pMacAddrHeader-pOrginMACPacket);

	turn_to_mac(pMacAddrHeader);

	unsigned int ipdatalen = IP_Construct_Intrupt_UDP_Packet(pOrginMACPacket+14,
		pOutBufferPacket+(pIPPacket-pOrginMACPacket),outbufferlen-(int)(pIPPacket-pOrginMACPacket),code);

	if( CCommonEtherDecode::INDEX_PID_MAC_PPPOE == upper_proto )
	{
		if( vlan_id == -1 )
			*(unsigned short *)(pMacAddrHeader+12+2+4) = ACE_HTONS(ipdatalen+2);
		else
			*(unsigned short *)(pMacAddrHeader+12+4+2+4) = ACE_HTONS(ipdatalen+2);
	}

	return 14+ipdatalen;
}


unsigned int CConstructPacket::IP_Construct_Intrupt_UDP_Packet(IN const BYTE * pOrginIPPacket,
													BYTE * pOutBufferPacket,int outbufferlen,int code)
{
	IPPacketHead* pOrignIpHead = (IPPacketHead*)pOrginIPPacket;

	unsigned int len = IP_Construct_Header(pOutBufferPacket,
		pOrignIpHead->DestIP,pOrignIpHead->SourIP,0x01,8+20+8);

	memcpy(pOutBufferPacket+28,pOrginIPPacket,28);

	ICMPPacketHead * pICMPHeader = (ICMPPacketHead *)(pOutBufferPacket+20);

	pICMPHeader->type = 0x03;
	pICMPHeader->code = code;
	pICMPHeader->checksum = 0;
	pICMPHeader->option = 0;

	pICMPHeader->checksum = CPF::ComputeCheckSum_NetOrder((BYTE *)pICMPHeader,8+20+8);

	return (20+8+20+8);
}



unsigned int CConstructPacket::Http_Redir_Packet(BYTE * outbuf,const char *target_url,const char * param)
{
	static const char REDIRECT_HTTP_HDR[] = "HTTP/1.1 307  Redirect\r\n";
	static const char REDIRECT_CONTENT_TYPE[] = "Content-Type: text/html\r\n";
	static const char REDIRECT_CONTENT_SERVER[] = "Server: Microsoft-IIS/6.0\r\n";
	static const char REDIRECT_BODY[] = "<head>\n<title>307 Document TempoaryRedirect</title>\n</head><body><h1>Document Tempory Redirect</h1>\n</body>\n";
	static const char REDIRECT_DATE[] = "Date: Mon, 24 Oct 2005 16:11:12 GMT\r\n";
	static const char REDIRECT_HEAD_END[] = "\r\n";
	
	int len = 0;

	char * ptr = (char *)outbuf;

	strcpy(ptr , REDIRECT_HTTP_HDR);
	ptr	+=	sizeof(REDIRECT_HTTP_HDR)-1;

	strcpy(ptr , REDIRECT_DATE);
	ptr	+=	sizeof(REDIRECT_DATE)-1;

	strcpy(ptr , REDIRECT_CONTENT_SERVER);
	ptr	+=	sizeof(REDIRECT_CONTENT_SERVER)-1;

	len	=	sprintf(ptr , "Location: %s" , target_url);	//"Http://10.10.136.200/");
	ptr	+=	len;

	len	=	sprintf(ptr, "%s\r\n", param);
	ptr	+=	len;


	len	=	sprintf(ptr , "Content-Length: %d\r\n", strlen(REDIRECT_BODY));
	ptr	+=	len;

	strcpy(ptr , REDIRECT_CONTENT_TYPE);
	ptr	+=	sizeof(REDIRECT_CONTENT_TYPE)-1;

	strcpy(ptr , REDIRECT_HEAD_END);
	ptr	+=	sizeof(REDIRECT_HEAD_END)-1;

	strcpy(ptr , REDIRECT_BODY);
	ptr	+=	sizeof(REDIRECT_BODY)-1;

	*ptr = 0;

	return (unsigned int)(ptr-(char *)outbuf);
}

//在http层构造文本的应答数据包
unsigned int CConstructPacket::Http_Rsp_Text_Packet(OUT BYTE * outbuf,const char *body_data,int body_len)
{
	static const char RSP_HTTP_HDR[] = "HTTP/1.1 200 OK\r\n";
	static const char RSP_CONTENT_TYPE[] = "Content-Type: text/html\r\n";
	static const char RSP_CONTENT_SERVER[] = "Server: Microsoft-IIS/6.0\r\n";
	static const char RSP_DATE[] = "Date: Mon, 24 Oct 2005 16:11:12 GMT\r\n";
	static const char RSP_HEAD_END[] = "\r\n";

	int len = 0;

	char * ptr = (char *)outbuf;

	strcpy(ptr , RSP_HTTP_HDR);
	ptr	+=	sizeof(RSP_HTTP_HDR)-1;

	strcpy(ptr , RSP_DATE);
	ptr	+=	sizeof(RSP_DATE)-1;

	strcpy(ptr , RSP_CONTENT_SERVER);
	ptr	+=	sizeof(RSP_CONTENT_SERVER)-1;

	len	=	sprintf(ptr , "Content-Length: %d\r\n", body_len);
	ptr	+=	len;

	strcpy(ptr , RSP_CONTENT_TYPE);
	ptr	+=	sizeof(RSP_CONTENT_TYPE)-1;

	strcpy(ptr , RSP_HEAD_END);
	ptr	+=	sizeof(RSP_HEAD_END)-1;

	if( body_len > 0 )
	memcpy(ptr,body_data,body_len);
	ptr	+=	body_len;

	*ptr = 0;

	return (unsigned int)(ptr-(char *)outbuf);

}

unsigned int CConstructPacket::IP_Redir_Packet(IN OUT BYTE * pIPPacket,unsigned int iplen
									   ,const char *target_url,const char * param,BOOL bTurn)
{
	//ip
	IPPacketHead* pIpHead = (IPPacketHead*)pIPPacket;
	BYTE * p = pIPPacket;

	unsigned short ipId = ntohs(pIpHead->Id);
	pIpHead->Id = ACE_HTONS(++ipId);

	if( bTurn )
	{
		unsigned int iptmp = pIpHead->SourIP ;
		pIpHead->SourIP = pIpHead->DestIP;
		pIpHead->DestIP = iptmp;
	}
	
	const int ip_header_len = ((pIpHead->VerHLen&0x0f)<<2);

	p += ip_header_len;

	//tcp
	TCPPacketHead* pTcpHead = (TCPPacketHead*)p;
	
	const int tcp_header_len = ((pTcpHead->HLen>>2)&0xFC);//从TCP头中取出头部长度

	p += tcp_header_len;

	if( bTurn )
	{
		TurnTCPSeq(pTcpHead,ACE_NTOHS(pIpHead->TtlLen));
	}
	else
	{
		NextTCPSeq(pTcpHead,ACE_NTOHS(pIpHead->TtlLen));
	}

	pTcpHead->Flag = (MASK_TCP_FIN|MASK_TCP_ACK);

	if( bTurn )
	{
		unsigned short porttmp = pTcpHead->SourPort;
		pTcpHead->SourPort = pTcpHead->DestPort;
		pTcpHead->DestPort = porttmp;
	}
	
	static unsigned short iii = 0;
	pTcpHead->WndSize = 100;
	pTcpHead->UrgPtr = 0;

	unsigned int applen = Http_Redir_Packet(p,target_url,param);

	pIpHead->TtlLen = ACE_HTONS(tcp_header_len+ip_header_len+applen);

	pIpHead->ChkSum = 0x0000;
	pIpHead->ChkSum = ACE_HTONS(CPF::ComputeCheckSum( (BYTE *)pIpHead, ip_header_len ));

	pTcpHead->ChkSum = 0x0000;
	pTcpHead->ChkSum = ACE_HTONS(CPF::CalculateTCPCheckSum(pIpHead->SourIP,pIpHead->DestIP,(const BYTE *)pTcpHead,applen+tcp_header_len));

	return ip_header_len+tcp_header_len+applen;

}


unsigned int CConstructPacket::IP_Http_Rsp_Text_Packet(IN OUT BYTE * pIPPacket,unsigned int iplen,
											   const char *body_data,int body_len,BOOL bTurn)
{
	//ip
	IPPacketHead* pIpHead = (IPPacketHead*)pIPPacket;
	BYTE * p = pIPPacket;

	unsigned short ipId = ntohs(pIpHead->Id);
	pIpHead->Id = ACE_HTONS(++ipId);

	if( bTurn )
	{
		unsigned int iptmp = pIpHead->SourIP ;
		pIpHead->SourIP = pIpHead->DestIP;
		pIpHead->DestIP = iptmp;
	}

	const int ip_header_len = ((pIpHead->VerHLen&0x0f)<<2);

	p += ip_header_len;

	//tcp
	TCPPacketHead* pTcpHead = (TCPPacketHead*)p;

	const int tcp_header_len = ((pTcpHead->HLen>>2)&0xFC);//从TCP头中取出头部长度

	p += tcp_header_len;

	if( bTurn )
	{
		TurnTCPSeq(pTcpHead,ACE_NTOHS(pIpHead->TtlLen));
	}
	else
	{
		NextTCPSeq(pTcpHead,ACE_NTOHS(pIpHead->TtlLen));
	}

	pTcpHead->Flag = MASK_TCP_ACK;

	if( bTurn )
	{
		unsigned short porttmp = pTcpHead->SourPort;
		pTcpHead->SourPort = pTcpHead->DestPort;
		pTcpHead->DestPort = porttmp;
	}

	static unsigned short iii = 0;
	pTcpHead->WndSize = 100;
	pTcpHead->UrgPtr = 0;

	unsigned int applen = Http_Rsp_Text_Packet(p,body_data,body_len);

	pIpHead->TtlLen = ACE_HTONS(tcp_header_len+ip_header_len+applen);

	pIpHead->ChkSum = 0x0000;
	pIpHead->ChkSum = ACE_HTONS(CPF::ComputeCheckSum( (BYTE *)pIpHead, ip_header_len ));

	pTcpHead->ChkSum = 0x0000;
	pTcpHead->ChkSum = ACE_HTONS(CPF::CalculateTCPCheckSum(pIpHead->SourIP,pIpHead->DestIP,(const BYTE *)pTcpHead,applen+tcp_header_len));

	return ip_header_len+tcp_header_len+applen;

}

unsigned int CConstructPacket::Construct_Arp_Packet(
	OUT BYTE * outbuf,
	int type,//0x0001=request,0x0002=response
	const BYTE * srcmac,
	ACE_UINT32 senderIP,ACE_UINT32 recvip,
	const BYTE *dstmac)
{
#pragma pack(1)
	typedef struct
	{
		WORD	hardType;
		WORD	protoType;
		BYTE	hardAddrLen;
		BYTE	protoAddrLen;
		WORD	operation;

		BYTE	senderMacAddr[6];
		DWORD	senderNetOrderIPAddr;

		BYTE	destMacAddr[6];
		DWORD	destNetOrderIPAddr;
	}ARP_INFO;

#pragma pack()

	ARP_INFO * pArpHead = (ARP_INFO *)(outbuf );

	pArpHead->hardType = ACE_HTONS(0x0001);
	pArpHead->protoType = ACE_HTONS(0x0800);
	pArpHead->hardAddrLen = 6;
	pArpHead->protoAddrLen = 4;
	pArpHead->operation = ACE_HTONS(type);

	memcpy(pArpHead->senderMacAddr, srcmac, 6);
	pArpHead->senderNetOrderIPAddr = senderIP;

	if( dstmac )
		memcpy(pArpHead->destMacAddr,dstmac,6);
	else 
		memset(pArpHead->destMacAddr,0x00,6);

	pArpHead->destNetOrderIPAddr = recvip;

	return sizeof(ARP_INFO);
}


unsigned int CConstructPacket::MAC_Construct_Arp_Packet(OUT BYTE * outbuf,
														int type,
														const BYTE * srcmac,
														ACE_UINT32 senderIP,ACE_UINT32 recvip,
														const BYTE *dstmac,ACE_UINT32 vlan_id)
{
	BYTE * pos = outbuf;

	if( dstmac )
		memcpy(pos,dstmac,6);
	else 
		memset(pos,0xff,6);

	pos += 6;

	memcpy(outbuf+6,srcmac,6);
	pos += 6;

	if( vlan_id != -1 )
	{
		*(ACE_UINT16 *)(pos) = ACE_HTONS(0x08100);
		pos += 2;


		*(WORD *)(pos) = ACE_HTONS(vlan_id);
		pos += 2;
	}


	*(WORD *)(pos) = ACE_HTONS(0X0806);
	pos += 2;

	unsigned int arp_len = CConstructPacket::Construct_Arp_Packet(pos,type,srcmac,senderIP,recvip,dstmac);
	pos += arp_len;

	return (int)(pos-outbuf);
}


//从MAC层构造假的DNS应答包，ipaddr_netorder希望重定向的ip地址
unsigned int CConstructPacket::MAC_Construct_DNS_Fake_RspPacket(IN const BYTE * pMacPacket,unsigned int maclen,
																OUT BYTE * pout_buffer,size_t outbuf_len,ACE_UINT32 ipaddr_netorder)
{
	if (NULL==pMacPacket || maclen<=14)
		return 0;

	BYTE * pMacAddrHeader = NULL;
	BYTE * pIPPacket = NULL;
	unsigned int iplen = 0;
	int vlan_id = -1;

	unsigned int upper_proto =  mac_simple_decode_to_ip((BYTE *)pMacPacket,maclen,vlan_id,pMacAddrHeader,pIPPacket,iplen);

	if( (unsigned int)-1 == upper_proto )
		return 0;

	memcpy(pout_buffer,pMacPacket,pIPPacket-pMacPacket);

	pMacAddrHeader = pout_buffer + (pMacAddrHeader-pMacPacket);

	turn_to_mac(pMacAddrHeader);

	unsigned int ipdatalen = IP_Construct_DNS_Fake_RspPacket(pIPPacket,iplen,
		pout_buffer+(pIPPacket-pMacPacket),outbuf_len-(pIPPacket-pMacPacket),
		ipaddr_netorder);

	if( CCommonEtherDecode::INDEX_PID_MAC_PPPOE == upper_proto )
	{
		if( vlan_id == -1 )
			*(unsigned short *)(pMacAddrHeader+12+2+4) = ACE_HTONS(ipdatalen+2);
		else
			*(unsigned short *)(pMacAddrHeader+12+4+2+4) = ACE_HTONS(ipdatalen+2);
	}

	return ipdatalen+(int)(pIPPacket-pMacPacket);
}

//从IP层构造假的DNS应答包，ipaddr_netorder希望重定向的ip地址
unsigned int CConstructPacket::IP_Construct_DNS_Fake_RspPacket(IN const BYTE * pIPPacket,unsigned int iplen,
															   OUT BYTE * pout_buffer,size_t outbuf_len,ACE_UINT32 ipaddr_netorder)
{
	memcpy(pout_buffer,pIPPacket,20+8);

	//ip
	IPPacketHead* pIpHead = (IPPacketHead*)pout_buffer;

	{
		unsigned int iptmp = pIpHead->SourIP ;
		pIpHead->SourIP = pIpHead->DestIP;
		pIpHead->DestIP = iptmp;
	}

	const int ip_header_len = ((pIpHead->VerHLen&0x0f)<<2);

	BYTE * p = pout_buffer + ip_header_len;

	//udp
	UDPPacketHead* pUDPHead = (UDPPacketHead*)p;

	{
		unsigned short porttmp = pUDPHead->SourPort;
		pUDPHead->SourPort = pUDPHead->DestPort;
		pUDPHead->DestPort = porttmp;
	}


	p += sizeof(UDPPacketHead);

	size_t nlen = CCommonDNSDecode::construct_fake_rsp_packet((const char *)pIPPacket+28,iplen-28,
		(char *)p,outbuf_len-28,ipaddr_netorder);

	if( (int)nlen <= 0 )
		return -1;

	pUDPHead->ChkSum = 0;
	pUDPHead->Len = ACE_HTONS(8+nlen);

	ACE_UINT16 wchecksum = CPF::CalculateUDPCheckSum_NetOrder(pIpHead->SourIP,pIpHead->DestIP,
		(const BYTE *)pUDPHead,ACE_NTOHS(pUDPHead->Len));

	pUDPHead->ChkSum = wchecksum;

	pIpHead->ChkSum = 0;
	pIpHead->TtlLen = ACE_HTONS(nlen+28);

	pIpHead->ChkSum = CPF::ComputeCheckSum_NetOrder( (BYTE *)pIpHead, ip_header_len );

	return (unsigned int)nlen + 28;
}
//从MAC层构造假的DNS应答包(表示没有该主机)，err_code错误码,
unsigned int CConstructPacket::MAC_Construct_DNS_Error_RspPacket(IN const BYTE * pMacPacket,unsigned int maclen,
																 OUT BYTE * pout_buffer,size_t outbuf_len,
																 ACE_UINT32 err_code)
{
	if (NULL==pMacPacket || maclen<=14)
		return 0;

	BYTE * pMacAddrHeader = NULL;
	BYTE * pIPPacket = NULL;
	unsigned int iplen = 0;
	int vlan_id = -1;

	unsigned int upper_proto =  mac_simple_decode_to_ip((BYTE *)pMacPacket,maclen,vlan_id,pMacAddrHeader,pIPPacket,iplen);

	if( (unsigned int)-1 == upper_proto )
		return 0;

	memcpy(pout_buffer,pMacPacket,pIPPacket-pMacPacket);

	pMacAddrHeader = pout_buffer + (pMacAddrHeader-pMacPacket);

	turn_to_mac(pMacAddrHeader);

	unsigned int ipdatalen = IP_Construct_DNS_Error_RspPacket(pIPPacket,iplen,
		pout_buffer+(pIPPacket-pMacPacket),outbuf_len-(pIPPacket-pMacPacket),err_code);

	if( CCommonEtherDecode::INDEX_PID_MAC_PPPOE == upper_proto )
	{
		if( vlan_id == -1 )
			*(unsigned short *)(pMacAddrHeader+12+2+4) = ACE_HTONS(ipdatalen+2);
		else
			*(unsigned short *)(pMacAddrHeader+12+4+2+4) = ACE_HTONS(ipdatalen+2);
	}

	return ipdatalen+(int)(pIPPacket-pMacPacket);
}

//从IP层构造假的DNS应答包(表示没有该主机)，err_code错误码,
unsigned int CConstructPacket::IP_Construct_DNS_Error_RspPacket(IN const BYTE * pIPPacket,unsigned int iplen,
																OUT BYTE * pout_buffer,size_t outbuf_len,
																ACE_UINT32 err_code)
{
	memcpy(pout_buffer,pIPPacket,20+8);

	//ip
	IPPacketHead* pIpHead = (IPPacketHead*)pout_buffer;

	{
		unsigned int iptmp = pIpHead->SourIP ;
		pIpHead->SourIP = pIpHead->DestIP;
		pIpHead->DestIP = iptmp;
	}

	const int ip_header_len = ((pIpHead->VerHLen&0x0f)<<2);

	BYTE * p = pout_buffer + ip_header_len;

	//udp
	UDPPacketHead* pUDPHead = (UDPPacketHead*)p;

	{
		unsigned short porttmp = pUDPHead->SourPort;
		pUDPHead->SourPort = pUDPHead->DestPort;
		pUDPHead->DestPort = porttmp;
	}


	p += sizeof(UDPPacketHead);

	size_t nlen = CCommonDNSDecode::construct_error_rsp_packet((const char *)pIPPacket+28,iplen-28,
		(char *)p,outbuf_len-28,err_code);

	if( (int)nlen <= 0 )
		return -1;

	pUDPHead->ChkSum = 0;
	pUDPHead->Len = ACE_HTONS(8+nlen);

	ACE_UINT16 wchecksum = CPF::CalculateUDPCheckSum_NetOrder(pIpHead->SourIP,pIpHead->DestIP,
		(const BYTE *)pUDPHead,ACE_NTOHS(pUDPHead->Len));

	pUDPHead->ChkSum = wchecksum;

	pIpHead->ChkSum = 0;
	pIpHead->TtlLen = ACE_HTONS(nlen+28);

	pIpHead->ChkSum = CPF::ComputeCheckSum_NetOrder( (BYTE *)pIpHead, ip_header_len );

	return (unsigned int)nlen + 28;
}

unsigned int CConstructPacket::ICMP_Construct_Packet(BYTE * outbuf,
										  BYTE i_type,BYTE i_code,
										  USHORT i_id,USHORT i_seq,
										  const BYTE * upperdata,size_t upperlen)
{
	IcmpHeader * pIcmpHeader = (IcmpHeader *)outbuf;

	int icmp_header_size = sizeof(IcmpHeader)-sizeof(pIcmpHeader->i_data);

	pIcmpHeader->i_type = i_type;
	pIcmpHeader->i_code = i_code;

	pIcmpHeader->i_cksum = 0;

	pIcmpHeader->i_id = ACE_HTONS(i_id);
	pIcmpHeader->i_seq = ACE_HTONS(i_seq);

	if( upperdata )
	{
		memcpy(outbuf+icmp_header_size,upperdata,upperlen);
	}
	else
	{
		memset(outbuf+icmp_header_size,0x00,upperlen);
	}

	((IcmpHeader*)pIcmpHeader)->i_cksum = 
		ACE_HTONS(CPF::ComputeCheckSum((BYTE *)outbuf,(int)upperlen+icmp_header_size));

	return (unsigned int)(upperlen+icmp_header_size);

}


//返回-1表示解码错误
//返回0表示不是IP包
//返回1表示是IP包
unsigned int CConstructPacket::mac_simple_decode_to_ip(IN BYTE * pMacPacket,unsigned int maclen,
													   OUT int& vlan_id,BYTE *& pMacAddrHeader,
													   BYTE *& pIPPacket,unsigned int& ipPacketlen)
{
	CCommonEtherDecode::MAC_DECODE_INFO mac_decode;

	CCommonEtherDecode::MACProtocolDecode(pMacPacket,maclen,mac_decode);

	if( 0 != mac_decode.nErrorStyle )
		return -1;

	vlan_id = (int)mac_decode.vlan_id;

	pMacAddrHeader = mac_decode.pMacAddrHeader;

	if( CCommonEtherDecode::INDEX_PID_MAC_PPPOE == mac_decode.nProtoIndex )
	{
		pIPPacket = mac_decode.pUpperData + 6;
		ipPacketlen -= mac_decode.dwUpperProtoLength - 6;

		if( ACE_NTOHS(*(ACE_UINT16 *)(pIPPacket)) != 0x0021 )
		{
			pIPPacket = NULL;
			ipPacketlen = 0;
			return 0;
		}

		pIPPacket = mac_decode.pUpperData + 2;
		ipPacketlen -= mac_decode.dwUpperProtoLength - 2;
	}
	else
	{
		if( mac_decode.nProtoIndex != CCommonEtherDecode::INDEX_PID_MAC_IP )
		{
			pIPPacket = NULL;
			ipPacketlen = 0;

			return 0;
		}

		ipPacketlen = mac_decode.dwUpperProtoLength;
		pIPPacket = mac_decode.pUpperData;
	}

	return mac_decode.nProtoIndex;
}


void CConstructPacket::turn_to_mac(BYTE * pMacAddrHeader)
{
	BYTE mac_temp[6];

	memcpy(mac_temp,pMacAddrHeader,6);
	memcpy(pMacAddrHeader,pMacAddrHeader+6,6);
	memcpy(pMacAddrHeader+6,mac_temp,6);

	return;
}

