//////////////////////////////////////////////////////////////////////////
//CardIP_FilterTool.cpp


#include "simple_filter/CardIP_FilterTool.h"
#include "cpf/CommonEtherDecode.h"
#include "cpf/CommonIPv4Decode.h"


bool CCardIP_FilterTool::filter_packet(CCommonMacTCPIPDecode::TCPIP_DECODE_CONTEXT& context,
									const CCardIP_FltCondition& flt_condition,
									BYTE cardno,BYTE ifno)
{
	if( cardno < 0 || cardno > 31 )
		return true;

	if( context.dwError != 0 )
		return false;

	if( context.bIP )
	{
		return (flt_condition.m_pIPFilter[cardno].flt_ip(context.addrTransportConnect.server.dwIP)
			&& flt_condition.m_pIPFilter[cardno].flt_ip(context.addrTransportConnect.client.dwIP));
	}
	else
	{
		return flt_condition.m_pIPFilter[cardno].cap_notip;
	}

	return false;

}


bool CCardIP_FilterTool::filter_packet(const char * pPacket,int packetlen,
									const CCardIP_FltCondition& flt_condition,
									BYTE cardno,BYTE ifno)
{
	if( cardno < 0 || cardno > 31 )
		return true;

	CCommonEtherDecode::MAC_DECODE_INFO mac_info;

	CCommonEtherDecode::MACProtocolDecode((const BYTE *)pPacket,(DWORD)packetlen, mac_info);

	if( mac_info.nErrorStyle != 0 )
		return false;

	if( CCommonEtherDecode::INDEX_PID_MAC_IP != mac_info.nProtoIndex )
	{
		return flt_condition.m_pIPFilter[cardno].cap_notip;
	}
	
	CCommonIPv4Decode::IPv4_DECODE_INFO IPv4Info;

	CCommonIPv4Decode::IPv4ProtocolDecode( mac_info.pUpperData,
		(DWORD)mac_info.dwUpperProtoLength, IPv4Info,true);

	if( IPv4Info.nErrorStyle != 0 )
		return false;

	ACE_UINT32 dst = ACE_NTOHL(IPv4Info.ipDst);
	ACE_UINT32 src = ACE_NTOHL(IPv4Info.ipSrc);

	return (flt_condition.m_pIPFilter[cardno].flt_ip(dst)
		&& flt_condition.m_pIPFilter[cardno].flt_ip(src));

}
