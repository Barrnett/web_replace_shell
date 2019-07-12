#include "stdafx.h"
#include "TS_MainNet.h"
#include "cpf/ConstructPacket.h"
#include "PacketIO/WriteDataStream.h"
#include "FluxControlObserver.h"

CTS_MainNet::CTS_MainNet()
{
	m_nIfIndex = 0xFFFF;
	m_nIfStatus = -1;
	memset(m_aMacAddr, 0, 6);
	m_piGatewayDct = NULL;
	m_vtPeerDct.clear();

	m_nChkPeriod = 0;
	m_nChkQueue = 0;
	m_nChkLostNum = 0;
	m_nChkReplyNum = 0;
	m_nChkEchoSeq = 0;
}

CTS_MainNet::~CTS_MainNet()
{
	delete m_piGatewayDct;

	size_t max = m_vtPeerDct.size();
	for (size_t i=0; i<max; i++)
	{
		delete m_vtPeerDct[i];
	}
	m_vtPeerDct.clear();
}

// 发送ARP请求报文,监测、查询路由器
void CTS_MainNet::f_SendArpReq(ACE_UINT32 dst_ip)
{
	BYTE arp_packet[64];

	unsigned int arp_len =
	CConstructPacket::MAC_Construct_Arp_Req_Packet
	(
		arp_packet,
		m_aMacAddr,
		TS_HTONL(m_hlIp),
		TS_HTONL(dst_ip),
		NULL,
		- 1
	);

	// 发送
	m_pTsInterfaceCfg->GetWriteDataStream()->WritePacket
	(
		PACKET_OK,
		(const char *)arp_packet,
		arp_len,
		(1 << m_nIfIndex),
		(int)1
	);
}

// 发送ping报文
void CTS_MainNet::f_SendEchoReq(ACE_UINT32 hlHostIp, ACE_UINT8* dstMac)
{
	BYTE ping_packet[1600];
	BYTE *p_MAC_head = ping_packet;
	ACE_UINT16 *p_type_head = (ACE_UINT16 *)(p_MAC_head+12);
	BYTE *p_IP_head = (BYTE *)(p_type_head + 1);
	BYTE icmp_data[128];

	//MAC
	memcpy(p_MAC_head, dstMac, 6);
	memcpy(p_MAC_head+6, m_aMacAddr, 6);

	// type
	*p_type_head = ACE_HTONS(0x0800); // IP报文

	// add 2016-7-10, ping判断优化
	m_nChkEchoSeq++;
	ACE_UINT16 i_id = (ACE_UINT16)(1269);
	ACE_UINT16 i_seq = (ACE_UINT16)(m_nChkEchoSeq);


	// ICMP 部分的字段
	ACE_UINT32 icmp_data_len =
	CConstructPacket::ICMP_Construct_EchoPacket
	(
		icmp_data,
		i_id,
		i_seq,
		NULL,
		12
	);

	// IP部分的字段
	ACE_UINT32 ip_data_len = 
	CConstructPacket::IP_Construct_ICMPPacket
	(
		p_IP_head,
		TS_HTONL(m_hlIp),
		TS_HTONL(hlHostIp),
		icmp_data,
		icmp_data_len
	);

	m_pTsInterfaceCfg->GetWriteDataStream()->WritePacket
	(
		PACKET_OK,
		(const char *)ping_packet,
		ip_data_len + 14+4,
		(1 << m_nIfIndex),
		(int)1
	);
}

int CTS_MainNet::f_ForwardIp(PACKET_INFO_EX& packet_info)
{
	m_pTsInterfaceCfg->GetWriteDataStream()->WritePacket
	(
		PACKET_OK,
		(const char *)(packet_info.packet->pPacket),
		packet_info.packet->nPktlen,
		(1<<m_nIfIndex),
		1
	);

	return TRUE;
}

int CTS_MainNet::f_ForwardArp(PACKET_INFO_EX& packet_info)
{
	m_pTsInterfaceCfg->GetWriteDataStream()->WritePacket
	(
		PACKET_OK,
		(const char *)(packet_info.packet->pPacket),
		packet_info.packet->nPktlen,
		(1<<m_nIfIndex),
		1
	);

	return TRUE;
}

int CTS_MainNet::f_ForwardPacket(PACKET_INFO_EX& packet_info)
{
	m_pTsInterfaceCfg->GetWriteDataStream()->WritePacket
	(
		PACKET_OK,
		(const char *)(packet_info.packet->pPacket),
		packet_info.packet->nPktlen,
		(1<<m_nIfIndex),
		1
	);

	return TRUE;
}

// 是否是认识的探测IP
BOOL CTS_MainNet::f_IsPeerTestIP(ACE_UINT32 hlIp, ACE_UINT32 &peer_index)
{
	size_t max = m_vtPeerDct.size();
	for (size_t i=0; i<max; i++)
	{
		// 主网探测IP
		if (hlIp == m_vtPeerDct[i]->m_hlHostIp)
		{
			peer_index = (ACE_UINT32)i;
			return TRUE;
		}
	}

	return FALSE;
}


