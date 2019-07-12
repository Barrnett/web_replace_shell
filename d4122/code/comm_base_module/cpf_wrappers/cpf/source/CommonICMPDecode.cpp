//////////////////////////////////////////////////////////////////////////
//CommonICMPDecode.cpp

#include "cpf/CommonICMPDecode.h"
#include "cpf/crc_tools.h"

CCommonICMPDecode::CCommonICMPDecode(void)
{
}

CCommonICMPDecode::~CCommonICMPDecode(void)
{
}

void CCommonICMPDecode::ICMPProtocolDecode_Checksum(const BYTE * pPacket, DWORD dwPacketLen, ICMP_DECODE_INFO& icmp_info)
{
	ICMPProtocolDecode(pPacket,dwPacketLen,icmp_info);

	if( icmp_info.nError != 0 )
		return;

	//icmp的校验和不不需要伪头部
	ACE_UINT16 i_cksum = ACE_HTONS(CPF::ComputeCheckSum((BYTE *)pPacket,dwPacketLen));

	if( i_cksum != 0 )
	{
		icmp_info.nError = 2;
	}

	return;	
}

void CCommonICMPDecode::ICMPProtocolDecode(const BYTE * pPacket, DWORD dwPacketLen, ICMP_DECODE_INFO& icmp_info)
{
	icmp_info.nError = 0;

	if( dwPacketLen <= sizeof(IcmpHeader) )
	{
		icmp_info.nError = 1;
		return;
	}

	memcpy(&icmp_info.icmp_header,pPacket,sizeof(IcmpHeader));

	icmp_info.i_data = (char *)(pPacket+sizeof(IcmpHeader));
	icmp_info.data_len = dwPacketLen - sizeof(IcmpHeader);

	return;	
}

