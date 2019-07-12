//////////////////////////////////////////////////////////////////////////
//ConstructPacket.h

//用于各类组装数据包
#include "cpf/cpf.h"

#pragma once

class CPF_CLASS CConstructPacket
{
public:
	CConstructPacket(void);
	~CConstructPacket(void);

public:
	//从mac层构造中断tcp的数据包,返回数据包的长度
	static unsigned int MAC_Construct_Intrupt_TCP_Packet(IN OUT BYTE * pMacPacket,unsigned int maclen,BOOL bTurn=true);

	//从IP层构造中断tcp的数据包,返回数据包的长度
	static unsigned int IP_Construct_Intrupt_TCP_Packet(IN OUT BYTE * pIPPacket,unsigned int iplen,BOOL bTurn=true);

	//从mac层构造干扰udp的数据包,返回数据包的长度
	static unsigned int MAC_Construct_Disturb_UDP_Packet(IN OUT BYTE * pMacPacket,unsigned int maclen,BOOL bTurn=true);

	//从IP层构造干扰udp的数据包,返回数据包的长度
	static unsigned int IP_Construct_Disturb_UDP_Packet(IN OUT BYTE * pIPPacket,unsigned int iplen,BOOL bTurn=true);

	//从MAC层构造假的DNS应答包，ipaddr_netorder希望重定向的ip地址
	static unsigned int MAC_Construct_DNS_Fake_RspPacket(IN const BYTE * pMacPacket,unsigned int maclen,
		OUT BYTE * pout_buffer,size_t outbuf_len,ACE_UINT32 ipaddr_netorder);

	//从IP层构造假的DNS应答包，ipaddr_netorder希望重定向的ip地址
	static unsigned int IP_Construct_DNS_Fake_RspPacket(IN const BYTE * pIPPacket,unsigned int iplen,
		OUT BYTE * pout_buffer,size_t outbuf_len,ACE_UINT32 ipaddr_netorder);

	//从MAC层构造假的DNS应答包(表示没有该主机)，err_code错误码,
	static unsigned int MAC_Construct_DNS_Error_RspPacket(IN const BYTE * pMacPacket,unsigned int maclen,
		OUT BYTE * pout_buffer,size_t outbuf_len,ACE_UINT32 err_code=3);

	//从IP层构造假的DNS应答包(表示没有该主机)，err_code错误码,
	static unsigned int IP_Construct_DNS_Error_RspPacket(IN const BYTE * pIPPacket,unsigned int iplen,
		OUT BYTE * pout_buffer,size_t outbuf_len,ACE_UINT32 err_code=3);

	//从MAC层构造中断udp的数据包,返回数据包的长度
	//outbufferlen应该为14+20+8+20+8(14mac头,20IP数据,8icmp的头长度,20以前包的IP头长度,8以前包的IP数据的8个字节)
	//type类型，

	//0 = net unreachable;
	//1 = host unreachable;
	//2 = protocol unreachable;
	//3 = port unreachable;
	//4 = fragmentation needed and DF set;
	//5 = source route failed.

	static unsigned int MAC_Construct_Intrupt_UDP_Packet(IN const BYTE * pOrginMACPacket,int orig_pkt_len,
		BYTE * pOutBufferPacket,int outbufferlen,int code=1);

	//从IP层构造中断udp的数据包,返回数据包的长度
	//outbufferlen应该为20+8+20+8(20IP数据,8icmp的头长度,20以前包的IP头长度,8以前包的IP数据的8个字节)
	static unsigned int IP_Construct_Intrupt_UDP_Packet(IN const BYTE * pOrginIPPacket,
		BYTE * pOutBufferPacket,int outbufferlen,int code=1);

	static unsigned int MAC_Contruct_Packet(BYTE * outbuf,int vlan_id,
		const BYTE * dstmacaddr,const BYTE * srcmacaddr,
		WORD protottype,//网络字节序
		const BYTE * upperdata,size_t upperlen);

	static unsigned int MAC_Contruct_IPPacket(BYTE * outbuf,int vlan_id,
		const BYTE * dstmacaddr,const BYTE * srcmacaddr,
		const BYTE * upperdata,size_t upperlen)
	{
		return MAC_Contruct_Packet(outbuf,vlan_id,dstmacaddr,srcmacaddr,ACE_HTONS(0x0800),upperdata,upperlen);
	}

	static unsigned int MAC_Contruct_Header(BYTE * outbuf,int vlan_id,
		const BYTE * dstmacaddr,const BYTE * srcmacaddr,
		WORD protottype//网络字节序
		);

	//上层是IP包,vlan_id=-1表示没有vlan，pppoe_session_id=-1表示没有pppoe
	static unsigned int MAC_Contruct_Header_For_IPPackte(BYTE * outbuf,
		const BYTE * dstmacaddr,const BYTE * srcmacaddr,
		ACE_UINT32 vlan_id,ACE_UINT16 pppoe_session_id,
		int IPPacketLen//包括IP包的总长度，包括IP头长度
		);

	static unsigned int MAC_Contruct_IPType_Header(BYTE * outbuf,int vlan_id,
		const BYTE * dstmacaddr,const BYTE * srcmacaddr
		)
	{
		return MAC_Contruct_Header(outbuf,vlan_id,dstmacaddr,srcmacaddr,ACE_HTONS(0x0800));
	}

	//ipid是网络字节序
	static unsigned int IP_Construct_Packet(BYTE * outbuf,
		ACE_UINT32 dwIPSrc,ACE_UINT32 dwIPDst,
		BYTE Proto,
		const BYTE * upperdata,size_t upperlen,int ttl=0x80,int tos=0,short ipid=0);

	//返回IP头部长度,ipid是网络字节序
	static unsigned int IP_Construct_Header(BYTE * outbuf,
		ACE_UINT32 dwIPSrc,ACE_UINT32 dwIPDst,
		BYTE Proto,size_t upperlen,int ttl=0x80,int tos=0,short ipid=0);

	static unsigned int IP_Construct_TCPPacket(BYTE * outbuf,
		ACE_UINT32 dwIPSrc,ACE_UINT32 dwIPDst,
		const BYTE * upperdata,size_t upperlen,int ttl=0x80,int tos=0)
	{
		return IP_Construct_Packet(outbuf,dwIPSrc,dwIPDst,6,upperdata,upperlen,ttl,tos);
	}
	static unsigned int IP_Construct_UDPPPacket(BYTE * outbuf,
		ACE_UINT32 dwIPSrc,ACE_UINT32 dwIPDst,
		const BYTE * upperdata,size_t upperlen,int ttl=0x80,int tos=0)
	{
		return IP_Construct_Packet(outbuf,dwIPSrc,dwIPDst,17,upperdata,upperlen,ttl,tos);
	}

	static unsigned int IP_Construct_ICMPPacket(BYTE * outbuf,
		ACE_UINT32 dwIPSrc,ACE_UINT32 dwIPDst,
		const BYTE * upperdata,size_t upperlen,int ttl=0x80,int tos=0)
	{
		return IP_Construct_Packet(outbuf,dwIPSrc,dwIPDst,0x01,upperdata,upperlen,ttl,tos);
	}

	//如果upperdata==NULL，表示使用任意数据
	static unsigned int ICMP_Construct_Packet(BYTE * outbuf,
		BYTE i_type,BYTE i_code,USHORT	i_id,USHORT	i_seq,
		const BYTE * upperdata,size_t upperlen);

	//如果upperdata==NULL，表示使用任意数据
	static unsigned int ICMP_Construct_EchoPacket(BYTE * outbuf,
		USHORT	i_id,USHORT	i_seq,
		const BYTE * upperdata,size_t upperlen)
	{
		return ICMP_Construct_Packet(outbuf,8,0,i_id,i_seq,upperdata,upperlen);
	}

	//如果upperdata==NULL，表示使用任意数据
	static unsigned int ICMP_Construct_EchoReplyPacket(BYTE * outbuf,
		USHORT	i_id,USHORT	i_seq,
		const BYTE * upperdata,size_t upperlen)
	{
		return ICMP_Construct_Packet(outbuf,0,0,i_id,i_seq,upperdata,upperlen);
	}

	static unsigned int Construct_Arp_Packet(
		OUT BYTE * outbuf,
		int type,//0x0001=request,0x0002=response
		const BYTE * sender_mac,
		ACE_UINT32 netorder_senderIP,ACE_UINT32 netorder_dstip,
		const BYTE *dstmac=NULL/*00-00-00-00-00-00*/);

	static unsigned int Construct_Arp_Req_Packet(
		OUT BYTE * outbuf,
		const BYTE * sender_mac,
		ACE_UINT32 netorder_senderIP,ACE_UINT32 netorder_dstip,
		const BYTE *dstmac=NULL/*00-00-00-00-00-00*/)
	{
		return Construct_Arp_Packet(outbuf,0x0001,sender_mac,netorder_senderIP,netorder_dstip,dstmac);

	}

	static unsigned int Construct_Arp_Rsp_Packet(
		OUT BYTE * outbuf,
		const BYTE * sender_mac,
		ACE_UINT32 netorder_senderIP,ACE_UINT32 netorder_dstip,
		const BYTE *dstmac=NULL/*00-00-00-00-00-00*/)
	{
		return Construct_Arp_Packet(outbuf,0x0002,sender_mac,netorder_senderIP,netorder_dstip,dstmac);
	}

	static unsigned int MAC_Construct_Arp_Packet(
		OUT BYTE * outbuf,
		int type,//0x0001=request,0x0002=response
		const BYTE * sender_mac,
		ACE_UINT32 netorder_senderIP,ACE_UINT32 netorder_dstip,
		const BYTE *dstmac=NULL,ACE_UINT32 vlan_id=-1);

	static unsigned int MAC_Construct_Arp_Req_Packet
	(
		OUT BYTE * outbuf,
		const BYTE * sender_mac,
		ACE_UINT32 netorder_senderIP,
		ACE_UINT32 netorder_dstip,
		const BYTE *dstmac=NULL,
		ACE_UINT32 vlan_id=-1
	)
	{
		return MAC_Construct_Arp_Packet(outbuf,0x0001,sender_mac,netorder_senderIP,netorder_dstip,dstmac,vlan_id);
	}

	static unsigned int MAC_Construct_Arp_Rsp_Packet
	(
		OUT BYTE * outbuf,
		const BYTE * sender_mac,
		ACE_UINT32 netorder_senderIP,
		ACE_UINT32 netorder_dstip,
		const BYTE *dstmac,
		ACE_UINT32 vlan_id=-1
	)
	{
		return MAC_Construct_Arp_Packet(outbuf,0x0002,sender_mac,netorder_senderIP,netorder_dstip,dstmac,vlan_id);
	}

	//构造免费ARP包
	static unsigned int MAC_Construct_Gratuitous_Arp_Packet(
		OUT BYTE * outbuf,const BYTE * sender_mac,ACE_UINT32 netorder_senderIP,ACE_UINT32 vlan_id=-1)
	{
		return CConstructPacket::MAC_Construct_Arp_Req_Packet(outbuf,
			sender_mac,netorder_senderIP,netorder_senderIP,NULL,vlan_id);
	}

	//在http层构造重定向数据包
	//返回数据包的长度
	static unsigned int Http_Redir_Packet(OUT BYTE * outbuf,const char *target_url,const char * param);

	//在IP层构造重定向数据包
	static unsigned int IP_Redir_Packet(IN OUT BYTE * pIPPacket,unsigned int iplen,const char *target_url,const char * param,BOOL bTurn=true);

	//在http层构造http文本的应答数据包
	static unsigned int Http_Rsp_Text_Packet(OUT BYTE * outbuf,const char *body_data,int body_len);

	//在IP层构造http文本的应答数据包
	static unsigned int IP_Http_Rsp_Text_Packet(IN OUT BYTE * pIPPacket,unsigned int iplen,const char *body_data,int body_len,BOOL bTurn=true);

public:
	//返回-1表示解码错误
	//返回0mac的直接上层的协议号
	//如果是能够得到IP包，则pIPPacket不为NULL并且ipPacketlen不为0
	static unsigned int mac_simple_decode_to_ip(IN BYTE * pMacPacket,unsigned int maclen,
		OUT int& vlan_id,OUT BYTE *& pMacAddrHeader,OUT BYTE *& pIPPacket,OUT unsigned int& ipPacketlen);

	static void turn_to_mac(BYTE * pMacAddrHeader);

};



