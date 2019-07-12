//////////////////////////////////////////////////////////////////////////
//CommonPPPOEDecode.cpp

#include "cpf/CommonPPPOEDecode.h"

CCommonPPPOEDecode::CCommonPPPOEDecode(void)
{
}

CCommonPPPOEDecode::~CCommonPPPOEDecode(void)
{
}


void CCommonPPPOEDecode::PPPOEDecode(const BYTE * pPacket,unsigned int dwPacketLen,PPPOE_DECODE_INFO& pppoeInfo)
{
	//typedef struct st_pppoe_header
	//{
	//	unsigned char		version:4;
	//	unsigned char		type:4;
	//	unsigned char		code;
	//	unsigned short		session_id;
	//	unsigned short		payload_len;	// 净荷长度，不包括头
	//}pppoe_header;
	//6个字节是pppoe的头部长度

	pppoeInfo.dwPackLen = dwPacketLen;

	pppoeInfo.nErrorStyle = PPPOE_ERROR_STYLE_NOERROR;
	
	if( dwPacketLen < 6 )
	{
		pppoeInfo.nErrorStyle = PPPOE_ERROR_STYLE_TOO_SHORT;
		return;		
	}

	pppoeInfo.m_verheader.ver = ((pPacket[0]&0xf0)>>4);
	pppoeInfo.m_verheader.type = pPacket[0]&0x0f;

	if( pppoeInfo.m_verheader.ver != 0x01 || pppoeInfo.m_verheader.type != 0x01 )
	{
		pppoeInfo.nErrorStyle = PPPOE_ERROR_STYLE_INVALID_HEADER;
		return;		
	}

	pppoeInfo.dwHeaderLength = 6;

	pppoeInfo.m_code = pPacket[1];
	pppoeInfo.m_sessionId = *(ACE_UINT16 *)(pPacket+2);//保持网络字节序
	pppoeInfo.m_wLength = NBUF_TO_WORD(pPacket+4);

	if( dwPacketLen < (unsigned int)pppoeInfo.m_wLength + 6 )
	{
		pppoeInfo.nErrorStyle = PPPOE_ERROR_STYLE_TOO_SHORT;
		return;		
	}

	switch(pppoeInfo.m_code)
	{
	case CCommonPPPOEDecode::DATA:
		pppoeInfo.m_stage = CCommonPPPOEDecode::SESSION;
		pppoeInfo.pUpperData = (BYTE *)pPacket + 6;
		pppoeInfo.dwUpperProtoLength = (WORD)pppoeInfo.m_wLength;
		pppoeInfo.nProtoIndex = INDEX_PID_PPP;
		break;

	case CCommonPPPOEDecode::PADI:
	case CCommonPPPOEDecode::PADO:
	case CCommonPPPOEDecode::PADR:
	case CCommonPPPOEDecode::PADS:
	case CCommonPPPOEDecode::PADT:
		pppoeInfo.m_stage = CCommonPPPOEDecode::DISCORY;
		pppoeInfo.pUpperData = NULL;
		pppoeInfo.dwUpperProtoLength = 0;
		pppoeInfo.nProtoIndex = INDEX_PID_OTHER;
		break;
		
	default:
		pppoeInfo.m_stage = CCommonPPPOEDecode::INIT;
		pppoeInfo.pUpperData = NULL;
		pppoeInfo.dwUpperProtoLength = 0;
		pppoeInfo.nProtoIndex = INDEX_PID_OTHER;
		pppoeInfo.nErrorStyle = PPPOE_ERROR_STYLE_OTHER_ERROR;
		break;
	}



	return;

}

const BYTE * CCommonPPPOEDecode::GetPPPOEHeaderPos(const BYTE * mac_addr_header)
{
	const BYTE * pppoe_header_data = NULL;

	ACE_UINT16 nProtoId = NBUF_TO_WORD(mac_addr_header+12);

	if( nProtoId == 0x8864 )
	{//PPPOE
		pppoe_header_data = mac_addr_header+14;
	}
	else if( nProtoId == 0x0081 )
	{//vlan
		nProtoId = NBUF_TO_WORD(mac_addr_header+12+4);

		if( nProtoId == 0x8864 )
		{//PPPOE
			pppoe_header_data = mac_addr_header+14+4;
		}
	}

	return pppoe_header_data;
}

//////////////////////////////////////////////////////////////////////////
//CCommonPPPDecode
//////////////////////////////////////////////////////////////////////////

CCommonPPPDecode::CCommonPPPDecode()
{

}
CCommonPPPDecode::~CCommonPPPDecode()
{

}


void CCommonPPPDecode::PPPDecode(const BYTE * pPacket,unsigned int dwPacketLen,PPP_DECODE_INFO& pppInfo)
{
	pppInfo.dwPackLen = dwPacketLen;

	pppInfo.nErrorStyle = PPP_ERROR_STYLE_NOERROR;

	if( dwPacketLen  < 2 )
	{
		pppInfo.nErrorStyle = PPP_ERROR_STYLE_TOO_SHORT;
		return;
	}

	pppInfo.dwHeaderLength = 2;

	pppInfo.pUpperData = (BYTE *)pPacket + 2;
	pppInfo.dwUpperProtoLength = dwPacketLen - 2;

	WORD dwType = NBUF_TO_WORD( pPacket );
	if (dwType == PID_PPPYPE_IP)
	{
		pppInfo.nProtoIndex = INDEX_PID_IP;
	}
	else
	{
		pppInfo.nProtoIndex = INDEX_PID_OTHER;
	}

	return;
}
