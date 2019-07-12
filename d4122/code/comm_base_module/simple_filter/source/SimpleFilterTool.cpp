//////////////////////////////////////////////////////////////////////////
//SimpleFilterTool.cpp

#include "simple_filter/SimpleFilterTool.h"


bool CSimpleFilterTool::filter_packet(CCommonMacTCPIPDecode::TCPIP_DECODE_CONTEXT& context,
									const CTimeStamp32& pkttime,
									const CSimpleFltCondition& flt_condition,
									BYTE cardno,BYTE ifno)
{
	if( flt_condition.IsEmpty() )
		return true;

	if( context.dwError != 0 )
		return false;

	CCommonEtherDecode::MAC_DECODE_INFO mac_info;

	mac_info.pMacAddrHeader = (unsigned char *)context.pMacAddrHeader;

	if( context.bIP )
	{
		mac_info.nProtoIndex = CCommonEtherDecode::INDEX_PID_MAC_IP;

		CCommonIPv4Decode::IPv4_DECODE_INFO IPv4Info;
		CCommonTCPDecode::TCP_DECODE_INFO  TCPInfo;
		CCommonUDPDecode::UDP_DECODE_INFO  UDPInfo;

		IPv4Info.ipDst = ACE_HTONL(context.addrTransportConnect.server.dwIP);
		IPv4Info.ipSrc = ACE_HTONL(context.addrTransportConnect.client.dwIP);

		IPv4Info.nProtoIndex = context.dwIndexProtocl;


		if( CCommonIPv4Decode::INDEX_PID_IPv4_TCP == context.dwIndexProtocl )
		{
			TCPInfo.portDst = context.addrTransportConnect.server.wdPort;
			TCPInfo.portSrc = context.addrTransportConnect.client.wdPort;
		}
		else if( CCommonIPv4Decode::INDEX_PID_IPv4_UDP == context.dwIndexProtocl )
		{
			UDPInfo.portDst = context.addrTransportConnect.server.wdPort;
			UDPInfo.portSrc = context.addrTransportConnect.client.wdPort;
		}

		std::vector<ONE_FILTER>::const_iterator it;

		for(it = flt_condition.m_vt_filter.begin(); it != flt_condition.m_vt_filter.end(); ++it)
		{
			if( filter_packet(*it,pkttime,mac_info,&IPv4Info,&TCPInfo,&UDPInfo,
				flt_condition.IsHaveIPFilter(),flt_condition.IsHavePortFilter(),cardno,ifno) )
				return true;
		}
	}
	else
	{
		std::vector<ONE_FILTER>::const_iterator it;

		for(it = flt_condition.m_vt_filter.begin(); it != flt_condition.m_vt_filter.end(); ++it)
		{
			if( filter_packet(*it,pkttime,mac_info,NULL,NULL,NULL,
				flt_condition.IsHaveIPFilter(),flt_condition.IsHavePortFilter(),cardno,ifno) )
				return true;
		}
	}

	return false;

}

bool CSimpleFilterTool::filter_packet(const char * pPacket,int packetlen,
									const CTimeStamp32& pkttime,
									const CSimpleFltCondition& flt_condition,
									BYTE cardno,BYTE ifno)
{	
	if( flt_condition.IsEmpty() )
		return true;
	
	CCommonEtherDecode::MAC_DECODE_INFO mac_info;
	CCommonIPv4Decode::IPv4_DECODE_INFO IPv4Info;
	CCommonTCPDecode::TCP_DECODE_INFO  TCPInfo;
	CCommonUDPDecode::UDP_DECODE_INFO  UDPInfo;

	filter_decode(pPacket,packetlen,mac_info,IPv4Info,TCPInfo,UDPInfo,
		flt_condition.IsHaveIPFilter(),flt_condition.IsHavePortFilter());

	std::vector<ONE_FILTER>::const_iterator it;

	for(it = flt_condition.m_vt_filter.begin(); it != flt_condition.m_vt_filter.end(); ++it)
	{
		if( filter_packet(*it,pkttime,mac_info,&IPv4Info,&TCPInfo,&UDPInfo,
			flt_condition.IsHaveIPFilter(),flt_condition.IsHavePortFilter(),cardno,ifno) )
		{
			return true;
		}
	}

	return false;

}



bool CSimpleFilterTool::filter_decode(const char * pPacket,int packetlen,
									CCommonEtherDecode::MAC_DECODE_INFO& mac_info,
									CCommonIPv4Decode::IPv4_DECODE_INFO& IPv4Info,
									CCommonTCPDecode::TCP_DECODE_INFO&  TCPInfo,
									CCommonUDPDecode::UDP_DECODE_INFO&  UDPInfo,
									BOOL bIPFilter,BOOL bPortFilter)
{

	CCommonEtherDecode::MACProtocolDecode((const BYTE *)pPacket,(DWORD)packetlen, mac_info);

	if( mac_info.nErrorStyle != 0 )
		return false;

	if( CCommonEtherDecode::INDEX_PID_MAC_IP != mac_info.nProtoIndex )
		return true;

	if( bIPFilter || bPortFilter )
	{
		CCommonIPv4Decode::IPv4ProtocolDecode( mac_info.pUpperData, (DWORD)mac_info.dwUpperProtoLength, IPv4Info,true);

		if( IPv4Info.nErrorStyle != 0 )
			return false;
	}

	if( bPortFilter )
	{
		if( CCommonIPv4Decode::INDEX_PID_IPv4_UDP == IPv4Info.nProtoIndex )
		{
			CCommonUDPDecode::UDPDecode_Checksum(IPv4Info.pUpperData, IPv4Info.dwUpperProtoLength,
				UDPInfo,IPv4Info.ipSrc,IPv4Info.ipDst); //对UDP进行解码分析以及统计

			if( 0 != UDPInfo.nErrorStyle )
				return false;
		}
		else if( CCommonIPv4Decode::INDEX_PID_IPv4_TCP == IPv4Info.nProtoIndex )
		{
			CCommonTCPDecode::TCPDecode_Checksum(IPv4Info.pUpperData,IPv4Info.dwUpperProtoLength,
				TCPInfo,IPv4Info.ipSrc,IPv4Info.ipDst);

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

bool CSimpleFilterTool::filter_packet(const ONE_FILTER& onefilter,
									const CTimeStamp32& pkttime,
						  const CCommonEtherDecode::MAC_DECODE_INFO& mac_info,
						  const CCommonIPv4Decode::IPv4_DECODE_INFO* IPv4Info,
						  const CCommonTCPDecode::TCP_DECODE_INFO * TCPInfo,
						  const CCommonUDPDecode::UDP_DECODE_INFO * UDPInfo,
						  BOOL bIPFilter,BOOL bPortFilter,
						  BYTE cardno,BYTE ifno)
{	
	BOOL bFilter = filter_card(cardno,ifno,onefilter);

	if( bFilter )
	{
		bFilter = filter_packet_noattr(onefilter,
			pkttime,mac_info,
			IPv4Info,TCPInfo,UDPInfo,
			bIPFilter,bPortFilter);
	}

	if( onefilter.bNot )
		return !bFilter;
	else
		return bFilter;
}


bool CSimpleFilterTool::filter_packet_noattr(const ONE_FILTER& onefilter,
										   const CTimeStamp32& pkttime,
									const CCommonEtherDecode::MAC_DECODE_INFO& mac_info,
									const CCommonIPv4Decode::IPv4_DECODE_INFO * pIPv4Info,
									const CCommonTCPDecode::TCP_DECODE_INFO * pTCPInfo,
									const CCommonUDPDecode::UDP_DECODE_INFO * pUDPInfo,
									BOOL bIPFilter,BOOL bPortFilter)
{
	if( !compare<Time_FILTER,CTimeStamp32>(onefilter.vt_time,pkttime) )
		return false;

	if( mac_info.nErrorStyle != 0 )
		return false;

	if( !compare<MAC_FILTER,CCommonEtherDecode::MAC_DECODE_INFO>(onefilter.vt_mac,mac_info) )
		return false;

	if( bIPFilter )
	{
		if( !pIPv4Info )
			return false;

		if( pIPv4Info->nErrorStyle != 0 )
			return false;

		if( !compare<IP_FILTER,CCommonIPv4Decode::IPv4_DECODE_INFO>(onefilter.vt_ip,*pIPv4Info) )
			return false;
	}

	if( bPortFilter )
	{
		if( CCommonIPv4Decode::INDEX_PID_IPv4_TCP == pIPv4Info->nProtoIndex )
		{
			if( !pTCPInfo )
				return false;

			if( pTCPInfo->nErrorStyle != 0 )
				return false;

			if( !compare<PORT_FILTER,CCommonTCPDecode::TCP_DECODE_INFO>(onefilter.vt_port,*pTCPInfo) )
				return false;
		}
		else if( CCommonIPv4Decode::INDEX_PID_IPv4_UDP == pIPv4Info->nProtoIndex )
		{
			if( !pUDPInfo )
				return false;

			if( pUDPInfo->nErrorStyle != 0 )
				return false;

			if( !compare<PORT_FILTER,CCommonUDPDecode::UDP_DECODE_INFO>(onefilter.vt_port,*pUDPInfo) )
				return false;
		}
		else
		{
			return false;
		}
	}


	if( onefilter.vt_data_pat.size() > 0 )
	{
		//最后进行数据模式的匹配
		for(size_t i = 0; i < onefilter.vt_data_pat.size(); ++i)
		{
			if( onefilter.vt_data_pat[i].Compare(mac_info.pMacAddrHeader,mac_info.dwPackLen) )
			{
				return true;
			}
		}	

		return false;
	}

	return true;

}

bool CSimpleFilterTool::compare(const Time_FILTER& time_filter,const CTimeStamp32& pkttime)
{
	return time_filter.Compare(pkttime);
}

bool CSimpleFilterTool::filter_card(BYTE cardno,BYTE ifno,const ONE_FILTER& onefilter)
{
	return onefilter.if_filter.Compare(cardno);
}

bool CSimpleFilterTool::compare(const MAC_FILTER& mac_filter,const CCommonEtherDecode::MAC_DECODE_INFO& mac_info)
{
	return mac_filter.Compare(mac_info.pMacAddrHeader+6,mac_info.pMacAddrHeader,mac_info.nProtoIndex);
}

bool CSimpleFilterTool::compare(const IP_FILTER& ip_filter,const CCommonIPv4Decode::IPv4_DECODE_INFO& IPv4Info)
{
	return ip_filter.Compare(IPv4Info.ipSrc,IPv4Info.ipDst,IPv4Info.nProtoIndex);
	
}

bool CSimpleFilterTool::compare(const PORT_FILTER& port_filter,const CCommonTCPDecode::TCP_DECODE_INFO& TCPInfo)
{
	return port_filter.Compare(ACE_NTOHS(TCPInfo.portSrc),ACE_NTOHS(TCPInfo.portDst));
}
bool CSimpleFilterTool::compare(const PORT_FILTER& port_filter,const CCommonUDPDecode::UDP_DECODE_INFO& UDPInfo)
{
	return port_filter.Compare(ACE_NTOHS(UDPInfo.portSrc),ACE_NTOHS(UDPInfo.portDst));
}

