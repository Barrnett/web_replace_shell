
#include "stdafx.h"
#include "TS_MirrorL2.h"
#include "cpf/ConstructPacket.h"
#include "PacketIO/WriteDataStream.h"
#include "FluxControlObserver.h"

CTS_MirrorL2::CTS_MirrorL2()
{
}

CTS_MirrorL2::~CTS_MirrorL2()
{
}

BOOL CTS_MirrorL2::f_HandleArp(PACKET_INFO_EX& packet_info)
{
	return TRUE;
}

BOOL CTS_MirrorL2::f_HandleIp(PACKET_INFO_EX& packet_info)
{
	//组播不转发
	if (*(ACE_UINT8*)(packet_info.packet->pPacket) & 0x01)
	{
		return TRUE;
	}
	
	if (-1 != packet_info.tcpipContext.vlanId)//去掉VALN字段
	{
		unsigned char aMac[12] = {0};
		memcpy(aMac, packet_info.packet->pPacket, 12);
		packet_info.packet->pPacket = (unsigned char*)(packet_info.packet->pPacket) + 4;
		packet_info.packet->nPktlen -= 4;
		memcpy(packet_info.packet->pPacket, aMac, 12);
	}
	
	return FALSE;
}
