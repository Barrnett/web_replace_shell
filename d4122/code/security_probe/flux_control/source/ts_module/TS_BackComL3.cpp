#include "stdafx.h"
#include "TS_BackComL3.h"
#include "cpf/ConstructPacket.h"
#include "PacketIO/WriteDataStream.h"

CTS_BackComL3::CTS_BackComL3()
{
}

CTS_BackComL3::~CTS_BackComL3()
{
}

BOOL CTS_BackComL3::f_ForwardIp(PACKET_INFO_EX& packet_info, ACE_UINT32 des_index)
{
	if (ECHO_OK != m_vtPeerDct[des_index]->m_eHostStat)
	{ 
		return FALSE; 
	}

	// 业务是否允许
	if (TRUE != m_piPeerPktFilter->f_ApplyFilter(packet_info))
	{ 
		return TRUE; 
	}

	/*if (-1 != packet_info.tcpipContext.vlanId)//去掉VALN字段
	{
		packet_info.packet->pPacket = (unsigned char*)(packet_info.packet->pPacket) + 4;
		packet_info.packet->nPktlen -= 4;
	}*/
	
	//memcpy(packet_info.packet->pPacket, m_vtPeerDct[des_index]->m_aHostMac, 6);  // 目的MAC
	//memcpy(packet_info.packet->pPacket+6, m_aMacAddr, 6);   // 源MAC
	
	m_pTsSerialInterface->m_piSerialDataSend->f_SendPacket(packet_info.packet->pPacket, packet_info.packet->nPktlen, m_nComIndex);
	
	return TRUE;
}

BOOL CTS_BackComL3::f_MulticastIp(PACKET_INFO_EX& packet_info)
{	
	m_pTsSerialInterface->m_piSerialDataSend->f_SendPacket(packet_info.packet->pPacket, packet_info.packet->nPktlen, m_nComIndex);
	
	return TRUE;
}

BOOL CTS_BackComL3::f_ForwardArp(PACKET_INFO_EX& packet_info, ACE_UINT32 des_index)
{
	return TRUE;
}

