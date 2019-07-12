
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
	//�鲥MAC���������ӿڴ���
	if (*(ACE_UINT8*)(packet_info.packet->pPacket) & 0x01)
	{
		return TRUE;
	}
	
	//����ӿ��յ������ҵ�MAC���ܿ����ǻ��ذ�
	ACE_UINT8* pSrcMac = (ACE_UINT8*)(packet_info.packet->pPacket) + 6;
	if ((pSrcMac[0] == m_aMacAddr[0]) && (pSrcMac[1] == m_aMacAddr[1]) && (pSrcMac[2] == m_aMacAddr[2]))
	{
		return TRUE;
	}
	
	return FALSE;
}

BOOL CTS_MirrorLan::f_HandleIp(PACKET_INFO_EX& packet_info)
{
	//�鲥MAC���������ӿڴ���
	if (*(ACE_UINT8*)(packet_info.packet->pPacket) & 0x01)
	{
		return TRUE;
	}
	
	//����ӿ��յ������ҵ�MAC���ܿ����ǻ��ذ�
	ACE_UINT8* pSrcMac = (ACE_UINT8*)(packet_info.packet->pPacket) + 6;
	if ((pSrcMac[0] == m_aMacAddr[0]) && (pSrcMac[1] == m_aMacAddr[1]) && (pSrcMac[2] == m_aMacAddr[2]))
	{
		return TRUE;
	}
	
	return FALSE;
}
