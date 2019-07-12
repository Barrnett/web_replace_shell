//////////////////////////////////////////////////////////////////////////
//ConstructPacket2.cpp

#include "cpf/ConstructPacket2.h"
#include "cpf/ConstructPacket.h"
#include "cpf/proto_struct_def.h"

CConstructPacket2::CConstructPacket2(void)
{
}

CConstructPacket2::~CConstructPacket2(void)
{
}


unsigned int CConstructPacket2::PackageHttpRedir(const CCommonMacTCPIPDecode::TCPIP_DECODE_CONTEXT& context,
											const char * dsturl,const char *param,
											BYTE * outbuf,int outbuflen)
{
	if( context.dwIndexProtocl != CCommonIPv4Decode::INDEX_PID_IPv4_TCP )
	{
		return 0;
	}

	if( context.pAppHeader == NULL )
	{
		return 0;
	}
	
	//拷贝MAC_head+IP_head+TCP_head
	memcpy(outbuf,context.pMACHeader,mymin(context.pAppHeader - context.pMACHeader,outbuflen));

	CConstructPacket::turn_to_mac(outbuf+(context.pMacAddrHeader-context.pMACHeader));

	unsigned int tempsize = CConstructPacket::IP_Redir_Packet(
		(BYTE *)outbuf + (context.pIPHeader - context.pMACHeader),
		context.dwMaclen - (int)(context.pIPHeader - context.pMACHeader) - 4,
		dsturl,param );

	unsigned int len = (unsigned int)(context.pIPHeader - context.pMACHeader + tempsize+ 4);

	const BYTE * pppoe_header_data = CCommonPPPOEDecode::GetPPPOEHeaderPos(outbuf+(context.pMacAddrHeader-context.pMACHeader));

	if( pppoe_header_data )
	{//PPPOE
		pppoe_header * poe_header = (pppoe_header *)pppoe_header_data;

		poe_header->payload_len = ACE_HTONS(tempsize+2);
	}

	return len;

}

unsigned int CConstructPacket2::PackageHttpRspTextPacket(const CCommonMacTCPIPDecode::TCPIP_DECODE_CONTEXT& context,
												 const char *body_data,int body_len,
												 BYTE * outbuf,int outbuflen)
{
	if( context.dwIndexProtocl != CCommonIPv4Decode::INDEX_PID_IPv4_TCP )
	{
		return 0;
	}

	if( context.pAppHeader == NULL )
	{
		return 0;
	}

	//拷贝MAC_head+IP_head+TCP_head
	memcpy(outbuf,context.pMACHeader,mymin(context.pAppHeader - context.pMACHeader,outbuflen));

	CConstructPacket::turn_to_mac(outbuf+(context.pMacAddrHeader-context.pMACHeader));

	unsigned int tempsize = CConstructPacket::IP_Http_Rsp_Text_Packet(
		(BYTE *)outbuf + (context.pIPHeader - context.pMACHeader),
		context.dwMaclen - (int)(context.pIPHeader - context.pMACHeader) - 4,
		body_data,body_len );

	unsigned int len = (unsigned int)(context.pIPHeader - context.pMACHeader + tempsize+ 4);

	const BYTE * pppoe_header_data = CCommonPPPOEDecode::GetPPPOEHeaderPos(outbuf+(context.pMacAddrHeader-context.pMACHeader));

	if( pppoe_header_data )
	{//PPPOE
		pppoe_header * poe_header = (pppoe_header *)pppoe_header_data;

		poe_header->payload_len = ACE_HTONS(tempsize+2);
	}

	return len;

}

unsigned int CConstructPacket2::PackageTCPInterrupt(const CCommonMacTCPIPDecode::TCPIP_DECODE_CONTEXT& context,
											   int interrupt_dir,BYTE * outbuf,int outbuflen)
{
	if( context.dwIndexProtocl != CCommonIPv4Decode::INDEX_PID_IPv4_TCP )
	{
		return 0;
	}

	{
		int copylen = context.dwTransHeaderLen + (int)(context.pTransHeader-context.pMACHeader);

		copylen = mymin(copylen,outbuflen);

		memcpy(outbuf,context.pMACHeader,copylen);
	}

	unsigned int tempsize = 0;

	if( interrupt_dir == 0x01 )
	{
		//同方向的RST包；
		tempsize = CConstructPacket::IP_Construct_Intrupt_TCP_Packet(
			(BYTE *)outbuf + (int)(context.pIPHeader - context.pMACHeader),
			context.dwMaclen - (int)(context.pIPHeader - context.pMACHeader) - 4,
			false);
	}
	else if( interrupt_dir == 0x02 )
	{	
		CConstructPacket::turn_to_mac(outbuf+(context.pMacAddrHeader-context.pMACHeader));

		tempsize = CConstructPacket::IP_Construct_Intrupt_TCP_Packet(
			(BYTE *)outbuf + (int)(context.pIPHeader - context.pMACHeader),
			context.dwMaclen - (int)(context.pIPHeader - context.pMACHeader) - 4,
			true);
	}

	unsigned int len = (unsigned int)(context.pIPHeader - context.pMACHeader + tempsize+ 4);

	const BYTE * pppoe_header_data = CCommonPPPOEDecode::GetPPPOEHeaderPos(outbuf+(context.pMacAddrHeader-context.pMACHeader));

	if( pppoe_header_data )
	{//PPPOE
		pppoe_header * poe_header = (pppoe_header *)pppoe_header_data;

		poe_header->payload_len = ACE_HTONS(tempsize+2);
	}

	return len;

}


