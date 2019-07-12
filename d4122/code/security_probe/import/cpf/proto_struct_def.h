//////////////////////////////////////////////////////////////////////////
// proto_struct_def.h


#pragma once


#define UDP_HEAD_LEN	8	
#define TCP_HEAD_LEN	20	
#define IP_HEAD_LEN		20	
#define PSEUDO_HEAD_LEN 12 
#define PID_IP_TCP		6 

#pragma pack(1)

struct IPPacketHead {
	unsigned char VerHLen;
	unsigned char Type;
	unsigned short TtlLen;
	unsigned short Id;
	unsigned short FlgOff;
	unsigned char TTL;
	unsigned char Proto;
	unsigned short ChkSum;
	unsigned int SourIP;
	unsigned int DestIP;
};

typedef IPPacketHead	IPv4PacketHead;

struct ICMPPacketHead{
	BYTE type;
	BYTE code;
	ACE_UINT16	checksum;
	ACE_UINT32	option;
};

struct ICMPPacketHead_PING{
	BYTE type;
	BYTE code;
	ACE_UINT16	checksum;
	ACE_UINT16	Identifier;
	ACE_UINT16	Sequence;
};

struct TCPPacketHead {
	unsigned short SourPort;
	unsigned short DestPort;
	unsigned int SeqNo;
	unsigned int AckNo;
	unsigned char HLen;
	unsigned char Flag;
	unsigned short WndSize;
	unsigned short ChkSum;
	unsigned short UrgPtr;
};

struct UDPPacketHead {
	unsigned short SourPort;
	unsigned short DestPort;
	unsigned short Len;
	unsigned short ChkSum;
};

//TCP,UDP 伪首部
struct PseudoHead {
	unsigned int SourIP;
	unsigned int DestIP;
	unsigned char  Pad;
	unsigned char  Proto;
	unsigned short  Len;
};

// ICMP header
typedef struct _ihdr {
	BYTE	i_type;
	BYTE	i_code; /* type sub code */
	USHORT	i_cksum;
	USHORT	i_id;
	USHORT	i_seq;

	char	i_data[1];

}IcmpHeader;


typedef struct _arphdr
{
	unsigned short	arp_hrd;	/*硬件类型*/
	unsigned short	arp_pro;	/*协议类型*/
	unsigned char	arp_hln;	/*硬件地址长度*/
	unsigned char	arp_pln;	/*协议地址长度*/
	unsigned short	arp_op;		/*ARP操作类型*/
	unsigned char	arp_sha[6];	/*发送者的硬件地址*/
	unsigned int	arp_spa;	/*发送者的协议地址*/
	unsigned char	arp_tha[6];	/*目标的硬件地址*/
	unsigned int	arp_tpa;	/*目标的协议地址*/
}ARPPacketHead,*PARPPacketHead;


typedef struct st_pppoe_header
{
	unsigned char		version:4;
	unsigned char		type:4;
	unsigned char		code;
	unsigned short		session_id;
	unsigned short		payload_len;	// 净荷长度，不包括头
}pppoe_header;

// lcp/pap/ipcp的头部
typedef struct st_ppp_upheader
{
	//	ACE_UINT16			protocol;	//	0xc021, 0x8021, 0x0021
	ACE_UINT8			code;
	ACE_UINT8			identifier;
	ACE_UINT16			length;		// 总长度，包括净荷，头部，不包括protocol字段
}ppp_upheader;


//肯定没有pppoe
#define NOT_PPPOE(pppoe_session_id)	((ACE_UINT16)(pppoe_session_id)==(ACE_UINT16)0xffff)

//不知道是不是pppoe
#define IS_UNKNOWN_PPPOE(pppoe_session_id)	((ACE_UINT16)(pppoe_session_id)==(ACE_UINT16)0)

#define IS_PPPOE(pppoe_session_id)		(!NOT_PPPOE(pppoe_session_id)&&!IS_UNKNOWN_PPPOE(pppoe_session_id))

#pragma pack()

