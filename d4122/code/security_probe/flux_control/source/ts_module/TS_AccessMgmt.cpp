#include "stdafx.h"
#include "TS_AccessMgmt.h"
#include "cpf/ConstructPacket.h"
#include "PacketIO/WriteDataStream.h"
#include "FluxControlObserver.h"

CTS_AccessMgmt::CTS_AccessMgmt()
{
	m_nIfIndex = 0xFFFF;
	m_hlIp = 0;
	m_hlNetmask = 0;
	memset(m_aMacAddr, 0, 6);
	m_hlGateway = 0;
	m_nPeerMgmt = FALSE;
}

CTS_AccessMgmt::~CTS_AccessMgmt()
{
}

BOOL CTS_AccessMgmt::f_ModInit(CTS_InterfaceCfg* pInterfaceCfg, CTS_CONFIG* pTsConfig, ACE_Log_File_Msg* pLogFile)
{
	m_pTsInterfaceCfg = pInterfaceCfg;
	m_pTsCfg = pTsConfig;
	m_pLogFile = pLogFile;

	m_nIfIndex = (ACE_UINT32)EtherIO_GetOneIndexByShowName(pTsConfig->m_iLocalMgmtCfg.m_strCardName.c_str());

	m_hlIp = pTsConfig->m_iLocalMgmtCfg.m_hlIpAddr;

	m_hlNetmask = pTsConfig->m_iLocalMgmtCfg.m_hlNetmask;

	EtherIO_GetDeviceMac(m_nIfIndex, m_aMacAddr);

	// �Լ�������IP,  ( ���ص�MAC��ַ,��Ҫ�Լ�ȥ��ѯ,���� ��ѯ)
	//m_hlGateway = ts_config.m_iLocalMgmtCfg.m_hlGateway;

	pTsConfig->f_SetLocalMgmt();

	return TRUE;
}

BOOL CTS_AccessMgmt::f_ModUpdateCfg(CTS_CONFIG* pNewTsCfg)
{
	if (m_pTsCfg->m_iLocalMgmtCfg.m_strCardName != pNewTsCfg->m_iLocalMgmtCfg.m_strCardName)
	{
		MY_ACE_DEBUG(m_pLogFile, (LM_ERROR, ACE_TEXT("[Info][%D]system reboot: management interface changed.\n")));
		ACE_OS::system("reboot -p");
		return FALSE;
	}

	if ((m_pTsCfg->m_iLocalMgmtCfg.m_hlIpAddr != pNewTsCfg->m_iLocalMgmtCfg.m_hlIpAddr)
		|| (m_pTsCfg->m_iLocalMgmtCfg.m_hlNetmask != pNewTsCfg->m_iLocalMgmtCfg.m_hlNetmask))
	{
		m_hlIp = pNewTsCfg->m_iLocalMgmtCfg.m_hlIpAddr;
		m_hlNetmask = pNewTsCfg->m_iLocalMgmtCfg.m_hlNetmask;

		pNewTsCfg->f_SetLocalMgmt();
	}

	/*if (m_pTsCfg->m_iLocalMgmtCfg.m_hlGateway != pNewTsCfg->m_iLocalMgmtCfg.m_hlGateway)
	{
	}*/
	
	m_pTsCfg = pNewTsCfg;
	
	return TRUE;
}

void CTS_AccessMgmt::f_HandleMyIcmp(PACKET_INFO_EX& packet_info)
{
	// IPͷ�ĳ���
	ACE_UINT8 ip_head_length =( (* packet_info.tcpipContext.pIPHeader) & 0x0F)*4;

	// ICMPͷ
	TS_IcmpHeader *icmp_header = (TS_IcmpHeader *)(packet_info.tcpipContext.pIPHeader + ip_head_length);
	ACE_UINT16 i_id = ACE_HTONS(icmp_header->i_id);
	ACE_UINT16 i_seq = ACE_HTONS(icmp_header->i_seq);

	if ((8 == icmp_header->i_type) && (0 == icmp_header->i_code))	// ֻ��Ӧ����ping����
	{
		BYTE icmp_field[256];
		
		// ICMP��������(Ӧ�û������������������?)
		ACE_UINT32 icmp_field_length = 
		CConstructPacket::ICMP_Construct_EchoReplyPacket
		(
			icmp_field,
			i_id,
			i_seq,
			NULL,
			0
		);
		//////////

		// IP��������
		BYTE ping_packet[1600];
		BYTE *p_MAC_head = ping_packet;
		ACE_UINT16 *p_type_head = (ACE_UINT16 *)(p_MAC_head+12);
		BYTE *p_IP_head = (BYTE *)(p_type_head + 1);

		ACE_UINT32 ip_length =
		CConstructPacket::IP_Construct_ICMPPacket
		(
			p_IP_head,
			TS_HTONL(m_hlIp),
			TS_HTONL(packet_info.tcpipContext.conn_addr.client.dwIP),
			icmp_field,
			icmp_field_length,
			0x80,
			0
		);

		// MAC����
		memcpy(p_MAC_head, packet_info.tcpipContext.pMACHeader+6, 6);
		memcpy(p_MAC_head+6, m_aMacAddr, 6);

		// type
		*p_type_head = ACE_HTONS(0x0800); // IP����

		// ����
		m_pTsInterfaceCfg->GetWriteDataStream()->WritePacket
		(
			PACKET_OK,
			(const char *)ping_packet,
			ip_length + 14+4,
			(1 << m_nIfIndex),
			1
		);
	}
}

// IP����
BOOL CTS_AccessMgmt::f_HandleIp(PACKET_INFO_EX& packet_info)
{
	if (packet_info.tcpipContext.conn_addr.server.dwIP == m_hlIp)
	{
		// ֻ����ICMP����
		if(packet_info.tcpipContext.dwIndexProtocl == CCommonIPv4Decode::INDEX_PID_IPv4_ICMP)
		{
			f_HandleMyIcmp(packet_info);
		}
		
		return TRUE;
	}

	return FALSE;
}

// ������Ч�ı���
BOOL CTS_AccessMgmt::f_HandlePacket(PACKET_INFO_EX& packet_info)
{
	if (CCommonEtherDecode::MAC_DECODE_INFO::IsIP(packet_info.tcpipContext.dwProtoOverMac))
	{
		// Ŀ��IP���Լ�
		if (packet_info.tcpipContext.conn_addr.server.dwIP == m_hlIp)
		{
			f_HandleIp(packet_info);
			return TRUE;
		}
	}

	return FALSE;
}

