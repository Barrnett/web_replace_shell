//////////////////////////////////////////////////////////////////////////
//CommonTCPUDPDecode.cpp

#include "cpf/CommonTCPUDPDecode.h"
#include "cpf/crc_tools.h"

CCommonTCPDecode::CCommonTCPDecode()
{

}
CCommonTCPDecode::~CCommonTCPDecode()
{

}


void CCommonTCPDecode::TCPDecode(const BYTE * pPacket,unsigned int dwPackLen,
								 TCP_DECODE_INFO& tcpDecodeInfo )
{
	tcpDecodeInfo.dwPackLen = dwPackLen;

	tcpDecodeInfo.nErrorStyle = TCP_ERR_OTHER_ERROR;

	if(dwPackLen < 20)
	{
		tcpDecodeInfo.nErrorStyle = TCP_ERR_HEADERSHORT;
		return;
	}

	tcpDecodeInfo.dwHeaderLength = (pPacket[12]>>2)&0xFC;//从TCP头中取出头部长度
	if(dwPackLen < tcpDecodeInfo.dwHeaderLength || tcpDecodeInfo.dwHeaderLength < 20)
	{
		tcpDecodeInfo.nErrorStyle = TCP_ERR_HEADERSHORT;
		return;
	}

	tcpDecodeInfo.dwMask    = (unsigned int)((pPacket[13])&0x3F);
 

	// 取标志位、窗口大小、紧急指针
	tcpDecodeInfo.dwWindow = (unsigned int)NBUF_TO_WORD(pPacket+14);
	tcpDecodeInfo.dwUrgPtr = (unsigned int)NBUF_TO_WORD(pPacket+18);

	tcpDecodeInfo.dwUpperProtoLength = dwPackLen - tcpDecodeInfo.dwHeaderLength;
	tcpDecodeInfo.pUpperData = (BYTE *)pPacket + tcpDecodeInfo.dwHeaderLength;	

	tcpDecodeInfo.portSrc = *(ACE_UINT16 *)(pPacket);
	tcpDecodeInfo.portDst = *(ACE_UINT16 *)(pPacket+2);

	// 取得32位序号与32位确认序号
	tcpDecodeInfo.dwSeqNum = NBUF_TO_UINT(pPacket+4);
	tcpDecodeInfo.dwAckNum = NBUF_TO_UINT(pPacket+8);

	tcpDecodeInfo.nErrorStyle = TCP_ERR_NOERROR;

	return;
}

void CCommonTCPDecode::TCPDecode_Checksum(const BYTE * pPacket,
							   unsigned int dwPacketLen,
							   TCP_DECODE_INFO& tcpDecodeInfo,
							   //ip_netorder_src和ip_netorder_dst用于计算校验和
							   ACE_UINT32 ip_netorder_src,ACE_UINT32 ip_netorder_dst)
{
	CCommonTCPDecode::TCPDecode(pPacket,dwPacketLen,tcpDecodeInfo);

	if( tcpDecodeInfo.nErrorStyle == TCP_ERR_NOERROR )
	{
		if( 0 != *(ACE_UINT16 *)(pPacket+16)
			&& 0 != CPF::CalculateTCPCheckSum_NetOrder(ip_netorder_src,ip_netorder_dst,pPacket,dwPacketLen) )
		{
			tcpDecodeInfo.nErrorStyle = TCP_ERR_CHECKSUM;
		}
	}

	return;

}
//////////////////////////////////////////////////////////////////////////
//CCommonUDPDecode
//////////////////////////////////////////////////////////////////////////

CCommonUDPDecode::CCommonUDPDecode()
{

}
CCommonUDPDecode::~CCommonUDPDecode()
{

}


void CCommonUDPDecode::UDPDecode(const BYTE * pPacket,
					  unsigned int dwPacketLen,
					  UDP_DECODE_INFO& udpDecodeInfo)
{
	udpDecodeInfo.dwPackLen = dwPacketLen;

	udpDecodeInfo.nErrorStyle = UDP_ERR_OTHER_ERROR;

	if(dwPacketLen < 8)
	{
		udpDecodeInfo.nErrorStyle = UDP_ERR_HEADERSHORT;
		return;
	}

	udpDecodeInfo.dwHeaderLength = 8;

	unsigned int dwUDPPacketLen = NBUF_TO_WORD(pPacket+4);//从UDP头中取出整个报文的长度

	if(dwUDPPacketLen < 8)
	{
		udpDecodeInfo.nErrorStyle = UDP_ERR_HEADERSHORT;
		return;
	}

	if (dwPacketLen < dwUDPPacketLen)//UDP头部参数错误，但还可以解码UDP头
	{
		udpDecodeInfo.nErrorStyle = UDP_ERR_HEADERSHORT;
		udpDecodeInfo.dwUpperProtoLength = 0;//UDP上层协议包长度
	}
	else
	{
		udpDecodeInfo.dwUpperProtoLength = dwUDPPacketLen - udpDecodeInfo.dwHeaderLength;
	}

	udpDecodeInfo.pUpperData =(BYTE*)pPacket + udpDecodeInfo.dwHeaderLength;	

	udpDecodeInfo.portSrc = *(ACE_UINT16 *)(pPacket);
	udpDecodeInfo.portDst = *(ACE_UINT16 *)(pPacket+2);

	udpDecodeInfo.nErrorStyle = UDP_ERR_NOERROR;

	return;
}

void CCommonUDPDecode::UDPDecode_Checksum(const BYTE * pPacket,
							   unsigned int dwPacketLen,
							   UDP_DECODE_INFO& udpDecodeInfo,
							   //ip_netorder_src和ip_netorder_dst用于计算校验和
							   ACE_UINT32 ip_netorder_src,ACE_UINT32 ip_netorder_dst)
{
	CCommonUDPDecode::UDPDecode(pPacket,dwPacketLen,udpDecodeInfo);

	if( udpDecodeInfo.nErrorStyle == UDP_ERR_NOERROR )
	{
		if( 0 != *(ACE_UINT16 *)(pPacket+6)
			&& 0 != CPF::CalculateUDPCheckSum_NetOrder(ip_netorder_src,ip_netorder_dst,pPacket,dwPacketLen) )
		{
			udpDecodeInfo.nErrorStyle = UDP_ERR_CHECKSUM;
		}
	}

	return;
}

int CCommonUDPDecode::GetIndexPIDOverUDP(unsigned int dwHostOrderSPort,unsigned int dwHostOrderDPort)
{
	if( dwHostOrderSPort == PID_UDPPORT_BOOTPSERVER 
		&& dwHostOrderDPort == PID_UDPPORT_BOOTPCLIENT )
	{
		return INDEX_PID_UDP_BOOTPSERVER;
	}

	if( dwHostOrderSPort == PID_UDPPORT_BOOTPCLIENT 
		&& dwHostOrderDPort == PID_UDPPORT_BOOTPSERVER )
	{
		return INDEX_PID_UDP_BOOTPCLIENT;
	}

	switch(dwHostOrderDPort)
	{
	case PID_UDPPORT_NETBIOS_NS:	return INDEX_PID_UDP_NETBIOS_NS;
	case PID_UDPPORT_NETBIOS_DGM:	return INDEX_PID_UDP_NETBIOS_DGM;
	case PID_UDPPORT_DNS:			return INDEX_PID_UDP_DNS;
	case PID_UDPPORT_SNMP161:		return INDEX_PID_UDP_SNMP161;
	case PID_UDPPORT_SNMP162:		return INDEX_PID_UDP_SNMP162;
	case PID_UDPPORT_TFTP:			return INDEX_PID_UDP_TFTP;
	case PID_UDPPORT_SUNRPC:		return INDEX_PID_UDP_SUNRPC;
	case PID_UDPPORT_RIP:			return INDEX_PID_UDP_RIP;
	case PID_UDPPORT_WHO:			return INDEX_PID_UDP_WHO;
	default:						break;
	}

	switch(dwHostOrderSPort)
	{
	case PID_UDPPORT_NETBIOS_NS:	return INDEX_PID_UDP_NETBIOS_NS;
	case PID_UDPPORT_NETBIOS_DGM:	return INDEX_PID_UDP_NETBIOS_DGM;
	case PID_UDPPORT_DNS:			return INDEX_PID_UDP_DNS;
	case PID_UDPPORT_SNMP161:		return INDEX_PID_UDP_SNMP161;
	case PID_UDPPORT_SNMP162:		return INDEX_PID_UDP_SNMP162;
	case PID_UDPPORT_TFTP:			return INDEX_PID_UDP_TFTP;
	case PID_UDPPORT_SUNRPC:		return INDEX_PID_UDP_SUNRPC;
	case PID_UDPPORT_RIP:			return INDEX_PID_UDP_RIP;
	case PID_UDPPORT_WHO:			return INDEX_PID_UDP_WHO;
	default:						break;
	}

	return INDEX_PID_UDP_OTHER;
}


