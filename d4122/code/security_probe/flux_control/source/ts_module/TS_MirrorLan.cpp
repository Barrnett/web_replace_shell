
#include "stdafx.h"
#include "TS_MirrorLan.h"
#include "cpf/ConstructPacket.h"
#include "PacketIO/WriteDataStream.h"
#include "FluxControlObserver.h"

CTS_MirrorLan::CTS_MirrorLan()
{
}

CTS_MirrorLan::~CTS_MirrorLan()
{
}

BOOL CTS_MirrorLan::f_HandleArp(PACKET_INFO_EX& packet_info)
{
	//组播MAC，由主网接口处理
	if (*(ACE_UINT8*)(packet_info.packet->pPacket) & 0x01)
	{
		return TRUE;
	}
	
	//镜像接口收到本厂家的MAC，很可能是环回包
	ACE_UINT8* pSrcMac = (ACE_UINT8*)(packet_info.packet->pPacket) + 6;
	if ((pSrcMac[0] == m_aMacAddr[0]) && (pSrcMac[1] == m_aMacAddr[1]) && (pSrcMac[2] == m_aMacAddr[2]))
	{
		return TRUE;
	}
	
	return FALSE;
}

BOOL CTS_MirrorLan::f_HandleIp(PACKET_INFO_EX& packet_info)
{
	//组播MAC，由主网接口处理
	if (*(ACE_UINT8*)(packet_info.packet->pPacket) & 0x01)
	{
		return TRUE;
	}
	
	//镜像接口收到本厂家的MAC，很可能是环回包
	ACE_UINT8* pSrcMac = (ACE_UINT8*)(packet_info.packet->pPacket) + 6;
	if ((pSrcMac[0] == m_aMacAddr[0]) && (pSrcMac[1] == m_aMacAddr[1]) && (pSrcMac[2] == m_aMacAddr[2]))
	{
		return TRUE;
	}
	
	return FALSE;
}
