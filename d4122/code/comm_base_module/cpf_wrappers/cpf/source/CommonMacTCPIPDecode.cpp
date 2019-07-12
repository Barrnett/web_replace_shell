//////////////////////////////////////////////////////////////////////////
//CommonMacTCPIPDecode.h

#include "cpf/CommonMacTCPIPDecode.h"
#include "cpf/CommonEtherDecode.h"
#include "cpf/CommonIPv4Decode.h"
#include "cpf/CommonTCPUDPDecode.h"
#include "cpf/CommonPPPOEDecode.h"
#include "cpf/crc_tools.h"

CCommonMacTCPIPDecode::CCommonMacTCPIPDecode(void)
{
}

CCommonMacTCPIPDecode::~CCommonMacTCPIPDecode(void)
{
}

void CCommonMacTCPIPDecode::RecalDataPointer(
	CCommonMacTCPIPDecode::TCPIP_DECODE_CONTEXT& context,
	const BYTE * newdatahead)
{
	const BYTE * oldhead = context.pMACHeader;

	const INT_PTR offset = (INT_PTR)(newdatahead-oldhead);

	context.pMACHeader = newdatahead;

	if( context.pMacAddrHeader )
	{
		context.pMacAddrHeader += offset;
	}

	if( context.pPPPOEHeader )
	{
		context.pPPPOEHeader += offset;
	}

	if( context.pIPHeader )
	{
		context.pIPHeader += offset;
	}

	if( context.pTransHeader )
	{
		context.pTransHeader += offset;
	}

	if( context.pAppHeader )
	{
		context.pAppHeader += offset;
	}

	return;
}

//mac解码到IP层
void CCommonMacTCPIPDecode::MacDecodeToIP(const BYTE * pMacheader,unsigned int nMaclen,
										  TCPIP_DECODE_CONTEXT& context,
										  //bipv4_checksum,是否要进行ipv4的校验和验证
										  BOOL bipv4_checksum)
{
	context.reset();

	context.dwError = 0;

	context.pMACHeader = pMacheader;
	context.dwMaclen = nMaclen;

	CCommonEtherDecode::MAC_DECODE_INFO mac_decode_info;

	CCommonIPv4Decode::IPv4_DECODE_INFO ipv4_decode_info;

	context.dwError = MacDecodeToIP(
		pMacheader,nMaclen,
		mac_decode_info,
		ipv4_decode_info,
		context.pppoe_stage,
		context.pPPPOEHeader,
		context.pppoe_session_id,
		context.pppoe_pkt_len,
		context.dwProtoOverMac,
		context.pIPHeader,
		context.dwIPPacketLen,
		bipv4_checksum);

	if(mac_decode_info.nErrorStyle == 0 )
	{
		context.pMacAddrHeader = mac_decode_info.pMacAddrHeader;

		context.vlanId = mac_decode_info.vlan_id;
	}

	if(context.IsIP())
	{
		if( ipv4_decode_info.nErrorStyle == 0 )
		{
			context.conn_addr.client.dwIP = ACE_NTOHL(ipv4_decode_info.ipSrc);
			context.conn_addr.server.dwIP = ACE_NTOHL(ipv4_decode_info.ipDst);

			context.pTransHeader = ipv4_decode_info.pUpperData;
			context.dwTransPacketLen = ipv4_decode_info.dwUpperProtoLength;

			context.dwIndexProtocl = ipv4_decode_info.nProtoIndex;

			context.dwIPFragment = ipv4_decode_info.dwFragment;
			context.dwIPID = ipv4_decode_info.dwID;
			context.dwTTL = ipv4_decode_info.dwTTL;
		}
	}

	return;
}

void CCommonMacTCPIPDecode::MacDecodeToTransport(const BYTE * pMacheader,unsigned int nMaclen,
						   TCPIP_DECODE_CONTEXT& context,
						   BOOL bipv4_checksum,BOOL btrans_checksum)
{
	MacDecodeToIP(pMacheader,nMaclen,context,bipv4_checksum);

	if( context.IsIP() && context.dwError == 0 )
	{
		if( CCommonIPv4Decode::INDEX_PID_IPv4_TCP == context.dwIndexProtocl )
		{
			if( context.IsFirstIPFragment() )
			{
				CCommonTCPDecode::TCP_DECODE_INFO tcp_decode_info;

				if( btrans_checksum )
				{
					CCommonTCPDecode::TCPDecode_Checksum(context.pTransHeader,context.dwTransPacketLen,tcp_decode_info,
						ACE_HTONL(context.conn_addr.client.dwIP),
						ACE_HTONL(context.conn_addr.server.dwIP));
				}
				else
				{
					CCommonTCPDecode::TCPDecode(context.pTransHeader,context.dwTransPacketLen,tcp_decode_info);
				}

				if( (int)tcp_decode_info.nErrorStyle != 0 )
				{
					context.dwError = MAKEWORD(5,(unsigned int)tcp_decode_info.nErrorStyle);
					return;
				}

				context.conn_addr.client.wdPort = ACE_NTOHS(tcp_decode_info.portSrc);
				context.conn_addr.server.wdPort = ACE_NTOHS(tcp_decode_info.portDst);
				context.dwTransHeaderLen = tcp_decode_info.dwHeaderLength;

				context.pAppHeader = context.pTransHeader+context.dwTransHeaderLen;
				context.dwAppPacketLen = context.dwTransPacketLen-context.dwTransHeaderLen;
			}
		}
		else if( CCommonIPv4Decode::INDEX_PID_IPv4_UDP == context.dwIndexProtocl )
		{
			if( context.IsFirstIPFragment() )
			{
				CCommonUDPDecode::UDP_DECODE_INFO udp_decode_info;

				if( btrans_checksum )
				{
					CCommonUDPDecode::UDPDecode_Checksum(context.pTransHeader,context.dwTransPacketLen,udp_decode_info,
						ACE_HTONL(context.conn_addr.client.dwIP),
						ACE_HTONL(context.conn_addr.server.dwIP));
				}
				else
				{
					CCommonUDPDecode::UDPDecode(context.pTransHeader,context.dwTransPacketLen,udp_decode_info);
				}

				if( (int)udp_decode_info.nErrorStyle != 0 )
				{
					context.dwError = MAKEWORD(6,(unsigned int)udp_decode_info.nErrorStyle);
					return;
				}

				context.conn_addr.client.wdPort = ACE_NTOHS(udp_decode_info.portSrc);
				context.conn_addr.server.wdPort = ACE_NTOHS(udp_decode_info.portDst);
				context.dwTransHeaderLen = udp_decode_info.dwHeaderLength;

				context.pAppHeader = context.pTransHeader+context.dwTransHeaderLen;
				context.dwAppPacketLen = context.dwTransPacketLen-context.dwTransHeaderLen;
			}
		}
		else if( CCommonIPv4Decode::INDEX_PID_IPv4_ICMP == context.dwIndexProtocl )
		{
		}
	}

	return;
}

DWORD CCommonMacTCPIPDecode::MacDecodeToIP(const BYTE * pMacheader,unsigned int nMaclen,
						  OUT CCommonEtherDecode::MAC_DECODE_INFO& mac_decode_info,
						  OUT CCommonIPv4Decode::IPv4_DECODE_INFO& ipv4_decode_info,
						  OUT CCommonPPPOEDecode::PPPOE_STAGE&	pppoe_stage,
						  OUT const BYTE *& pPPPOEHeader,
						  OUT ACE_UINT16& pppoe_session_id,
						  OUT unsigned int& pppoe_pkt_len,
						  OUT unsigned int& dwProtoOverMac,
						  OUT const BYTE *& pIpv4Header,
						  OUT unsigned int& ipv4_pkt_len,
						  //bipv4_checksum,是否要进行ipv4的校验和验证
						  IN BOOL bipv4_checksum)
{
	dwProtoOverMac = 0;

	pIpv4Header = NULL;
	ipv4_pkt_len = 0;

	{
		CCommonEtherDecode::MACProtocolDecode(pMacheader,nMaclen,mac_decode_info);

		if( (int)mac_decode_info.nErrorStyle != 0 )
		{
			return MAKEWORD(1,(unsigned int)mac_decode_info.nErrorStyle);
		}

		if( CCommonEtherDecode::INDEX_PID_MAC_PPPOE == mac_decode_info.nProtoIndex )
		{
			CCommonPPPOEDecode::PPPOE_DECODE_INFO pppoe_decode_info;

			CCommonPPPOEDecode::PPPOEDecode(mac_decode_info.pUpperData, mac_decode_info.dwUpperProtoLength,pppoe_decode_info);

			pPPPOEHeader = mac_decode_info.pUpperData;
			pppoe_pkt_len =  mac_decode_info.dwUpperProtoLength;
			pppoe_stage = pppoe_decode_info.m_stage;
			pppoe_session_id = pppoe_decode_info.m_sessionId;

			if( (int)pppoe_decode_info.nErrorStyle != 0 )
			{
				return MAKEWORD(2,(unsigned int)pppoe_decode_info.nErrorStyle);
			}

			if(pppoe_decode_info.nProtoIndex != CCommonPPPOEDecode::INDEX_PID_PPP )
			{
				return 0;
			}

			CCommonPPPDecode::PPP_DECODE_INFO ppp_decode_info;

			CCommonPPPDecode::PPPDecode(pppoe_decode_info.pUpperData,pppoe_decode_info.dwUpperProtoLength,ppp_decode_info);

			if( (int)ppp_decode_info.nErrorStyle != 0 )
			{
				return MAKEWORD(3,(unsigned int)ppp_decode_info.nErrorStyle);
			}

			if( ppp_decode_info.nProtoIndex == CCommonPPPDecode::INDEX_PID_IP )
			{
				dwProtoOverMac = CCommonEtherDecode::INDEX_PID_MAC_IP;
			}
			
			pIpv4Header = ppp_decode_info.pUpperData;
			ipv4_pkt_len = ppp_decode_info.dwUpperProtoLength;
		}
		else
		{
			dwProtoOverMac = mac_decode_info.nProtoIndex;

			pIpv4Header = mac_decode_info.pUpperData;
			ipv4_pkt_len = mac_decode_info.dwUpperProtoLength;						
		}
	}

	if( CCommonEtherDecode::MAC_DECODE_INFO::IsIP(dwProtoOverMac) )
	{
		CCommonIPv4Decode::IPv4ProtocolDecode(pIpv4Header,ipv4_pkt_len,
			ipv4_decode_info,bipv4_checksum);

		if( (int)ipv4_decode_info.nErrorStyle != 0 )
		{
			return MAKEWORD(4,(unsigned int)ipv4_decode_info.nErrorStyle);
		}		
	}

	return 0;

}

void CCommonMacTCPIPDecode::ReculateCheckSum(
	IN OUT  CCommonMacTCPIPDecode::TCPIP_DECODE_CONTEXT& tcpipContext)
{

	{//重新计算IP校验和
		*(ACE_UINT16 *)(tcpipContext.pIPHeader + 10) = 0;

		*(ACE_UINT16 *)(tcpipContext.pIPHeader + 10) =
			ACE_HTONS(CPF::ComputeCheckSum( (BYTE *)tcpipContext.pIPHeader, 
			tcpipContext.dwIPPacketLen - tcpipContext.dwTransPacketLen ));
	}

	//重新计算tcp或者udp,icmp的校验和
	if(tcpipContext.dwIndexProtocl== CCommonIPv4Decode::INDEX_PID_IPv4_TCP )
	{
		*(ACE_UINT16 *)(tcpipContext.pTransHeader+16) = 0;

		*(ACE_UINT16 *)(tcpipContext.pTransHeader+16) =
			CPF::CalculateTCPCheckSum_NetOrder(
			ACE_HTONL(tcpipContext.conn_addr.client.dwIP),
			ACE_HTONL(tcpipContext.conn_addr.server.dwIP),
			tcpipContext.pTransHeader,
			tcpipContext.dwTransPacketLen);
	}
	else if(tcpipContext.dwIndexProtocl== CCommonIPv4Decode::INDEX_PID_IPv4_UDP)
	{
		*(ACE_UINT16 *)(tcpipContext.pTransHeader+6) = 0;

		*(ACE_UINT16 *)(tcpipContext.pTransHeader+6) =
			CPF::CalculateUDPCheckSum_NetOrder(
			ACE_HTONL(tcpipContext.conn_addr.client.dwIP),
			ACE_HTONL(tcpipContext.conn_addr.server.dwIP),
			tcpipContext.pTransHeader,
			tcpipContext.dwTransPacketLen);
	}
	else if(tcpipContext.dwIndexProtocl== CCommonIPv4Decode::INDEX_PID_IPv4_ICMP)
	{
		*(ACE_UINT16 *)(tcpipContext.pTransHeader+2) = 0;

		*(ACE_UINT16 *)(tcpipContext.pTransHeader+2) = 
			ACE_HTONS(CPF::ComputeCheckSum(tcpipContext.pTransHeader,tcpipContext.dwTransPacketLen));

	}

	return;
}

