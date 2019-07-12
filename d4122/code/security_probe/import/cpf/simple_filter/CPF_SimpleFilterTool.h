//////////////////////////////////////////////////////////////////////////
//CPF_SimpleFilterTool.h

#pragma once

#include "cpf/CommonEtherDecode.h"
#include "cpf/CommonIPv4Decode.h"
#include "cpf/CommonTCPUDPDecode.h"
#include "cpf/CommonMacTCPIPDecode.h"
#include "cpf/simple_filter/CPF_SimpleFltCondition.h"


class CPF_CLASS CPF_SimpleFilterTool
{
public:
	static bool filter_packet(const char * pPacket,int packetlen,
		const CPF_SimpleFltCondition& flt_condition);

	static bool filter_packet(CCommonMacTCPIPDecode::TCPIP_DECODE_CONTEXT& context,
		const CPF_SimpleFltCondition& flt_condition);

private:
	//���bIpv4Decode����0,��ʾû�н��뵽ipv4����ipv4_decode_info,TCPInfo,UDPInfo��Ч��
	static bool filter_decode(const char * pPacket,int packetlen,
		OUT CCommonEtherDecode::MAC_DECODE_INFO& mac_info,
		OUT CCommonIPv4Decode::IPv4_DECODE_INFO& IPv4Info,
		OUT unsigned int & dwProtoOverMac,
		OUT CCommonTCPDecode::TCP_DECODE_INFO&  TCPInfo,
		OUT CCommonUDPDecode::UDP_DECODE_INFO&  UDPInfo,
		BOOL bIPFilter,BOOL bPortFilter);

	//����֮ǰ�����IP��ַ�Ͷ˿ںű�������ֽ���
	static bool filter_packet(const CPF_ONE_FILTER& onefilter,
		const CCommonEtherDecode::MAC_DECODE_INFO& mac_info,
		const CCommonIPv4Decode::IPv4_DECODE_INFO* IPv4Info,
		const CCommonTCPDecode::TCP_DECODE_INFO * TCPInfo,
		const CCommonUDPDecode::UDP_DECODE_INFO * UDPInfo,
		BOOL bIPFilter,BOOL bPortFilter);

	//�������ԵĹ���
	//����֮ǰ�����IP��ַ�Ͷ˿ںű�������ֽ���
	static bool filter_packet_noattr(const CPF_ONE_FILTER& onefilter,
		const CCommonEtherDecode::MAC_DECODE_INFO& mac_info,
		const CCommonIPv4Decode::IPv4_DECODE_INFO* IPv4Info,
		const CCommonTCPDecode::TCP_DECODE_INFO * TCPInfo,
		const CCommonUDPDecode::UDP_DECODE_INFO * UDPInfo,
		BOOL bIPFilter,BOOL bPortFilter);

	static bool compare(const SIMPLE_IP_FILTER& ip_filter,const CCommonIPv4Decode::IPv4_DECODE_INFO& IPv4Info);
	static bool compare(const SIMPLE_PORT_FILTER& port_filter,const CCommonTCPDecode::TCP_DECODE_INFO& TCPInfo);
	static bool compare(const SIMPLE_PORT_FILTER& port_filter,const CCommonUDPDecode::UDP_DECODE_INFO& UDPInfo);
	
};


