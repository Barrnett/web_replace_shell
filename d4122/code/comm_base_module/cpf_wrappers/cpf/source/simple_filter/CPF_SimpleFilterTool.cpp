//////////////////////////////////////////////////////////////////////////
//CPF_SimpleFilterTool.cpp

#include "cpf/simple_filter/CPF_SimpleFilterTool.h"


bool CPF_SimpleFilterTool::filter_packet(CCommonMacTCPIPDecode::TCPIP_DECODE_CONTEXT& context,
									const CPF_SimpleFltCondition& flt_condition)
{
	if( flt_condition.IsEmpty() )
		return true;

	if( context.dwError != 0 )
		return false;

	CCommonEtherDecode::MAC_DECODE_INFO mac_info;

	mac_info.pMacAddrHeader = (unsigned char *)context.pMacAddrHeader;

	if( context.IsIP() )
	{
		mac_info.nProtoIndex = CCommonEtherDecode::INDEX_PID_MAC_IP;

		CCommonIPv4Decode::IPv4_DECODE_INFO IPv4Info;
		CCommonTCPDecode::TCP_DECODE_INFO  TCPInfo;
		CCommonUDPDecode::UDP_DECODE_INFO  UDPInfo;

		IPv4Info.nProtoIndex = context.dwIndexProtocl;

		IPv4Info.ipDst = context.conn_addr.server.dwIP;
		IPv4Info.ipSrc = context.conn_addr.client.dwIP;

		if( CCommonIPv4Decode::INDEX_PID_IPv4_TCP == context.dwIndexProtocl )
		{
			TCPInfo.portDst = context.conn_addr.server.wdPort;
			TCPInfo.portSrc = context.conn_addr.client.wdPort;
		}
		else if( CCommonIPv4Decode::INDEX_PID_IPv4_UDP == context.dwIndexProtocl )
		{
			UDPInfo.portDst = context.conn_addr.server.wdPort;
			UDPInfo.portSrc = context.conn_addr.client.wdPort;
		}

		for(size_t i = 0; i < flt_condition.m_vt_filter.size(); ++i)
		{
			if( filter_packet(flt_condition.m_vt_filter[i],mac_info,&IPv4Info,&TCPInfo,&UDPInfo,
				flt_condition.IsHaveIPFilter((int)i),flt_condition.IsHavePortFilter((int)i)) )
			{
				return true;
			}
		}
	}
	else
	{
		for(size_t i = 0; i < flt_condition.m_vt_filter.size(); ++i)
		{
			if( filter_packet(flt_condition.m_vt_filter[i],mac_info,NULL,NULL,NULL,
				flt_condition.IsHaveIPFilter((int)i),flt_condition.IsHavePortFilter((int)i)) )
			{
				return true;
			}
		}
	}

	return false;

}

bool CPF_SimpleFilterTool::filter_packet(const char * pPacket,int packetlen,									
									const CPF_SimpleFltCondition& flt_condition)
{	
	if( flt_condition.IsEmpty() )
		return true;
	
	CCommonEtherDecode::MAC_DECODE_INFO mac_info;
	CCommonIPv4Decode::IPv4_DECODE_INFO IPv4Info;
	CCommonTCPDecode::TCP_DECODE_INFO  TCPInfo;
	CCommonUDPDecode::UDP_DECODE_INFO  UDPInfo;

	unsigned int dwProtoOverMac = 0;

	filter_decode(pPacket,packetlen,
		mac_info,IPv4Info,dwProtoOverMac,
		TCPInfo,UDPInfo,
		flt_condition.IsHaveIPFilter(),
		flt_condition.IsHavePortFilter());

	if( CCommonEtherDecode::MAC_DECODE_INFO::IsIP(dwProtoOverMac) )
	{
		IPv4Info.ipDst = ACE_NTOHL(IPv4Info.ipDst);
		IPv4Info.ipSrc = ACE_NTOHL(IPv4Info.ipSrc);

		TCPInfo.portDst = ACE_NTOHS(TCPInfo.portDst);
		TCPInfo.portSrc = ACE_NTOHS(TCPInfo.portSrc);

		UDPInfo.portDst = ACE_NTOHS(UDPInfo.portDst);
		UDPInfo.portSrc = ACE_NTOHS(UDPInfo.portSrc);

		for(size_t i = 0; i < flt_condition.m_vt_filter.size(); ++i)
		{
			if( filter_packet(flt_condition.m_vt_filter[i],mac_info,&IPv4Info,&TCPInfo,&UDPInfo,
				flt_condition.IsHaveIPFilter((int)i),flt_condition.IsHavePortFilter((int)i)) )
			{
				return true;
			}
		}
	}
	else
	{
		for(size_t i = 0; i < flt_condition.m_vt_filter.size(); ++i)
		{
			if( filter_packet(flt_condition.m_vt_filter[i],mac_info,NULL,NULL,NULL,
				flt_condition.IsHaveIPFilter((int)i),flt_condition.IsHavePortFilter((int)i)) )
			{
				return true;
			}
		}
	}


	return false;

}



bool CPF_SimpleFilterTool::filter_decode(const char * pPacket,int packetlen,
									OUT CCommonEtherDecode::MAC_DECODE_INFO& mac_info,
									OUT CCommonIPv4Decode::IPv4_DECODE_INFO& IPv4Info,
									OUT unsigned int & dwProtoOverMac,
									OUT CCommonTCPDecode::TCP_DECODE_INFO&  TCPInfo,
									OUT CCommonUDPDecode::UDP_DECODE_INFO&  UDPInfo,
									BOOL bIPFilter,BOOL bPortFilter)
{
	const BYTE * pIPv4Header = NULL;
	unsigned int ipv4_pkt_len = 0;

	CCommonPPPOEDecode::PPPOE_STAGE	pppoe_stage;
	const BYTE *	pPPPOEHeader;
	unsigned int	pppoe_pkt_len;
	ACE_UINT16		pppoe_session_id;

	if( 0 != CCommonMacTCPIPDecode::MacDecodeToIP(
		(const BYTE *)pPacket,(unsigned int)packetlen,
		mac_info,IPv4Info,
		pppoe_stage,pPPPOEHeader,pppoe_session_id,pppoe_pkt_len,
		dwProtoOverMac,pIPv4Header,ipv4_pkt_len) )
	{
		return false;
	}

	if( !CCommonEtherDecode::MAC_DECODE_INFO::IsIP(dwProtoOverMac) )
	{
		return true;
	}

	if( bPortFilter )
	{
		if( CCommonIPv4Decode::INDEX_PID_IPv4_UDP == IPv4Info.nProtoIndex )
		{
			//CCommonUDPDecode::UDPDecode_Checksum(IPv4Info.pUpperData, IPv4Info.dwUpperProtoLength,
			//	UDPInfo,IPv4Info.ipSrc,IPv4Info.ipDst); //对UDP进行解码分析以及统计

			CCommonUDPDecode::UDPDecode(IPv4Info.pUpperData, IPv4Info.dwUpperProtoLength,UDPInfo); //对UDP进行解码分析以及统计

			if( 0 != UDPInfo.nErrorStyle )
				return false;
		}
		else if( CCommonIPv4Decode::INDEX_PID_IPv4_TCP == IPv4Info.nProtoIndex )
		{
			//CCommonTCPDecode::TCPDecode_Checksum(IPv4Info.pUpperData,IPv4Info.dwUpperProtoLength,
			//	TCPInfo,IPv4Info.ipSrc,IPv4Info.ipDst);

			CCommonTCPDecode::TCPDecode(IPv4Info.pUpperData,IPv4Info.dwUpperProtoLength,TCPInfo);

			if( 0 != TCPInfo.nErrorStyle )
				return false;
		}
	}
	else
	{
		if( bPortFilter )
		{
			return false;
		}
	}

	return true;

}

bool CPF_SimpleFilterTool::filter_packet(const CPF_ONE_FILTER& onefilter,
						  const CCommonEtherDecode::MAC_DECODE_INFO& mac_info,
						  const CCommonIPv4Decode::IPv4_DECODE_INFO* IPv4Info,
						  const CCommonTCPDecode::TCP_DECODE_INFO * TCPInfo,
						  const CCommonUDPDecode::UDP_DECODE_INFO * UDPInfo,
						  BOOL bIPFilter,BOOL bPortFilter)
{	

	BOOL bFilter = filter_packet_noattr(onefilter,
		mac_info,
		IPv4Info,TCPInfo,UDPInfo,
		bIPFilter,bPortFilter);

	return (onefilter.bNot?(!bFilter):(bFilter));
}


bool CPF_SimpleFilterTool::filter_packet_noattr(const CPF_ONE_FILTER& onefilter,
									const CCommonEtherDecode::MAC_DECODE_INFO& mac_info,
									const CCommonIPv4Decode::IPv4_DECODE_INFO * pIPv4Info,
									const CCommonTCPDecode::TCP_DECODE_INFO * pTCPInfo,
									const CCommonUDPDecode::UDP_DECODE_INFO * pUDPInfo,
									BOOL bIPFilter,BOOL bPortFilter)
{
	if( mac_info.nErrorStyle != 0 )
		return false;

	if( bIPFilter )
	{
		if( !pIPv4Info )
			return false;

		if( pIPv4Info->nErrorStyle != 0 )
			return false;

		if( !compare(onefilter.ip_flt,*pIPv4Info) )
		{
			return false;
		}
	}

	if( bPortFilter )
	{
		if( !pIPv4Info )
			return false;

		if( CCommonIPv4Decode::INDEX_PID_IPv4_TCP == pIPv4Info->nProtoIndex )
		{
			if( !pTCPInfo )
				return false;

			if( pTCPInfo->nErrorStyle != 0 )
				return false;

			if( !compare(onefilter.port_flt,*pTCPInfo) )
			{
				return false;
			}
		}
		else if( CCommonIPv4Decode::INDEX_PID_IPv4_UDP == pIPv4Info->nProtoIndex )
		{
			if( !pUDPInfo )
				return false;

			if( pUDPInfo->nErrorStyle != 0 )
				return false;

			if( !compare(onefilter.port_flt,*pUDPInfo) )
			{
				return false;
			}
		}
		else
		{
			return false;
		}
	}

	return true;

}



bool CPF_SimpleFilterTool::compare(const SIMPLE_IP_FILTER& ip_filter,const CCommonIPv4Decode::IPv4_DECODE_INFO& IPv4Info)
{
	return ip_filter.Compare(IPv4Info.ipSrc,IPv4Info.ipDst,IPv4Info.nProtoIndex);
	
}

bool CPF_SimpleFilterTool::compare(const SIMPLE_PORT_FILTER& port_filter,const CCommonTCPDecode::TCP_DECODE_INFO& TCPInfo)
{
	return port_filter.Compare(TCPInfo.portSrc,TCPInfo.portDst);
}
bool CPF_SimpleFilterTool::compare(const SIMPLE_PORT_FILTER& port_filter,const CCommonUDPDecode::UDP_DECODE_INFO& UDPInfo)
{
	return port_filter.Compare(UDPInfo.portSrc,UDPInfo.portDst);
}

