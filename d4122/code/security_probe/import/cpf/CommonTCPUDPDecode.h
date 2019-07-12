//////////////////////////////////////////////////////////////////////////
//CommonTCPUDPDecode.h

#pragma once

#include "cpf/cpf.h"

#define GET_CUR_SEQ(pTCPHeader) (NBUF_TO_UINT(((char *)(pTCPHeader))+4))
#define GET_CUR_ACK(pTCPHeader) (NBUF_TO_UINT(((char *)(pTCPHeader))+8))

enum{
	 MASK_TCP_URG = 0x20,
	 MASK_TCP_ACK = 0x10,
	 MASK_TCP_PSH = 0x08,
	 MASK_TCP_RST = 0x04,
	 MASK_TCP_SYN = 0x02,
	 MASK_TCP_FIN = 0x01
};

class CPF_CLASS CCommonTCPDecode
{
public:
	CCommonTCPDecode();
	~CCommonTCPDecode();

	typedef enum{

		TCP_ERR_NOERROR = 0,
		TCP_ERR_OTHER_ERROR = 1,
		TCP_ERR_HEADERSHORT = 2,
		TCP_ERR_CHECKSUM = 3

	}TCP_ERROR_STYLE;

	typedef struct  
	{
		unsigned int			dwPackLen;//tcp本身的长度

		unsigned int			dwHeaderLength; //被解码协议的协议头部长度(tcp头部长度)

		unsigned char *			pUpperData;
		unsigned int			dwUpperProtoLength;  //被解码协议的上层协议的数据长度

		ACE_UINT16				portSrc;//网络字节序
		ACE_UINT16				portDst;//网络字节序

		unsigned int			dwSeqNum;
		unsigned int			dwAckNum;

		unsigned int			dwMask;
		unsigned int			dwWindow;
		unsigned int			dwUrgPtr;

		TCP_ERROR_STYLE			nErrorStyle;

		void reset()
		{
			dwPackLen = 0;

			dwHeaderLength = 0;

			pUpperData = NULL;
			dwUpperProtoLength = 0;

			portSrc = portDst = 0;

			dwSeqNum = dwAckNum = 0;

			dwMask = 0;
			dwWindow = 0;
			dwUrgPtr = 0;

			nErrorStyle = TCP_ERR_OTHER_ERROR;
		}

	}TCP_DECODE_INFO;

public:
	static void TCPDecode(const BYTE * pPacket,
		unsigned int dwPacketLen,
		TCP_DECODE_INFO& tcpDecodeInfo);

	static void TCPDecode_Checksum(const BYTE * pPacket,
		unsigned int dwPacketLen,
		TCP_DECODE_INFO& tcpDecodeInfo,
		//ip_netorder_src和ip_netorder_dst用于计算校验和
		ACE_UINT32 ip_netorder_src,ACE_UINT32 ip_netorder_dst);

};


class CPF_CLASS CCommonUDPDecode
{
public:
	CCommonUDPDecode();
	~CCommonUDPDecode();

	typedef enum{

		UDP_ERR_NOERROR = 0,
		UDP_ERR_OTHER_ERROR = 1,
		UDP_ERR_HEADERSHORT = 2,
		UDP_ERR_CHECKSUM = 3

	}UDP_ERROR_STYLE;

	enum _UDP_PORT_ID
	{
		PID_UDPPORT_ECHO        = 7,
		PID_UDPPORT_FTP_DATA    = 20,
		PID_UDPPORT_FTP         = 21,
		PID_UDPPORT_DNS         = 53,
		PID_UDPPORT_OSQLNET     = 66,
		PID_UDPPORT_BOOTPSERVER = 67,
		PID_UDPPORT_BOOTPCLIENT = 68,
		PID_UDPPORT_TFTP        = 69,
		PID_UDPPORT_SUNRPC      = 111,
		PID_UDPPORT_NTP         = 123,
		PID_UDPPORT_NETBIOS_NS  = 137,
		PID_UDPPORT_NETBIOS_DGM = 138,
		PID_UDPPORT_SQLNET      = 150,
		PID_UDPPORT_SQLSRV      = 156,
		PID_UDPPORT_SNMP161     = 161,
		PID_UDPPORT_SNMP162     = 162,
		PID_UDPPORT_BGP         = 179,
		PID_UDPPORT_LDAP        = 389,
		PID_UDPPORT_RIP         = 520,
		PID_UDPPORT_WHO         = 565,

		PID_UDPPORT_H225RAS     = 1719,
		PID_UDPPORT_H225DISGK   = 1718,
	};

	enum _UDP_PROTOCOL_INDEX
	{
		INDEX_PID_UDP_OTHER       = 0,
		INDEX_PID_UDP_NETBIOS_NS  = 1,
		INDEX_PID_UDP_NETBIOS_DGM = 2,
		INDEX_PID_UDP_DNS         = 3,
		INDEX_PID_UDP_BOOTPSERVER = 4,
		INDEX_PID_UDP_BOOTPCLIENT = 5,
		INDEX_PID_UDP_SNMP161     = 6,
		INDEX_PID_UDP_SNMP162     = 7,
		INDEX_PID_UDP_TFTP        = 8,
		INDEX_PID_UDP_SUNRPC      = 9,
		INDEX_PID_UDP_RIP         = 10,
		INDEX_PID_UDP_WHO         = 11,
	};


	typedef struct  
	{
		unsigned int			dwPackLen;//udp本身的长度

		unsigned int			dwHeaderLength; //被解码协议的协议头部长度(udp头部长度)

		unsigned char *			pUpperData;
		unsigned int			dwUpperProtoLength;  //被解码协议的上层协议的数据长度

		ACE_UINT16				portSrc;//网络字节序
		ACE_UINT16				portDst;//网络字节序


		UDP_ERROR_STYLE			nErrorStyle;

		void reset()
		{
			dwPackLen = 0;

			dwHeaderLength = 0;

			pUpperData = NULL;
			dwUpperProtoLength = 0;

			portSrc = portDst = 0;

			nErrorStyle = UDP_ERR_OTHER_ERROR;
		}

	}UDP_DECODE_INFO;

public:
	static void UDPDecode(const BYTE * pPacket,
		unsigned int dwPacketLen,
		UDP_DECODE_INFO& udpDecodeInfo);

	static void UDPDecode_Checksum(const BYTE * pPacket,
		unsigned int dwPacketLen,
		UDP_DECODE_INFO& udpDecodeInfo,
		//ip_netorder_src和ip_netorder_dst用于计算校验和
		ACE_UINT32 ip_netorder_src,ACE_UINT32 ip_netorder_dst);

	static int GetIndexPIDOverUDP(unsigned int dwHostOrderSPort,unsigned int dwHostOrderDPort);

};

