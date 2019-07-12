//////////////////////////////////////////////////////////////////////////
//SimpleFilterTool.h

#pragma once

#include "cpf/CommonEtherDecode.h"
#include "cpf/CommonIPv4Decode.h"
#include "cpf/CommonTCPUDPDecode.h"
#include "cpf/CommonMacTCPIPDecode.h"
#include "simple_filter/SimpleFltCondition.h"


class SIMPLE_FILTER_CLASS CSimpleFilterTool
{
public:
	static bool filter_packet(const char * pPacket,int packetlen,
		const CTimeStamp32& pkttime,
		const CSimpleFltCondition& flt_condition,
		BYTE cardno,BYTE ifno);

	static bool filter_packet(CCommonMacTCPIPDecode::TCPIP_DECODE_CONTEXT& context,
		const CTimeStamp32& pkttime,
		const CSimpleFltCondition& flt_condition,
		BYTE cardno,BYTE ifno);

private:
	static bool filter_decode(const char * pPacket,int packetlen,
		CCommonEtherDecode::MAC_DECODE_INFO& mac_info,
		CCommonIPv4Decode::IPv4_DECODE_INFO& IPv4Info,
		CCommonTCPDecode::TCP_DECODE_INFO&  TCPInfo,
		CCommonUDPDecode::UDP_DECODE_INFO&  UDPInfo,
		BOOL bIPFilter,BOOL bPortFilter);

	static bool filter_packet(const ONE_FILTER& onefilter,
		const CTimeStamp32& pkttime,
		const CCommonEtherDecode::MAC_DECODE_INFO& mac_info,
		const CCommonIPv4Decode::IPv4_DECODE_INFO* IPv4Info,
		const CCommonTCPDecode::TCP_DECODE_INFO * TCPInfo,
		const CCommonUDPDecode::UDP_DECODE_INFO * UDPInfo,
		BOOL bIPFilter,BOOL bPortFilter,
		BYTE cardno,BYTE ifno);

	//不带属性的过滤
	static bool filter_packet_noattr(const ONE_FILTER& onefilter,
		const CTimeStamp32& pkttime,
		const CCommonEtherDecode::MAC_DECODE_INFO& mac_info,
		const CCommonIPv4Decode::IPv4_DECODE_INFO* IPv4Info,
		const CCommonTCPDecode::TCP_DECODE_INFO * TCPInfo,
		const CCommonUDPDecode::UDP_DECODE_INFO * UDPInfo,
		BOOL bIPFilter,BOOL bPortFilter);

	static bool filter_card(BYTE cardno,BYTE ifno,const ONE_FILTER& filter);


	static bool compare(const Time_FILTER& time_filter,const CTimeStamp32& pkttime);
	static bool compare(const MAC_FILTER& mac_filter,const CCommonEtherDecode::MAC_DECODE_INFO& mac_info);
	static bool compare(const IP_FILTER& ip_filter,const CCommonIPv4Decode::IPv4_DECODE_INFO& IPv4Info);
	static bool compare(const PORT_FILTER& port_filter,const CCommonTCPDecode::TCP_DECODE_INFO& TCPInfo);
	static bool compare(const PORT_FILTER& port_filter,const CCommonUDPDecode::UDP_DECODE_INFO& UDPInfo);
	

private:
	template<typename FILTRER_TYPE,typename DECODE_TYPE>
	static bool compare(const std::vector<FILTRER_TYPE>& filter,const DECODE_TYPE& decodeinfo)
	{
		if( filter.size() > 0 )
		{
			std::vector<FILTRER_TYPE>::const_iterator it;

			for(it = filter.begin(); it != filter.end(); ++it)
			{
				if( compare(*it,decodeinfo) )
					break;
			}

			if( it == filter.end() )
			{
				return false;
			}
		}

		return true;
	}

};


