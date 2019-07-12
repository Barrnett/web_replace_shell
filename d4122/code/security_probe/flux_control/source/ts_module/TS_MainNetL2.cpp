#include "stdafx.h"
#include "TS_MainNetL2.h"
#include "cpf/ConstructPacket.h"
#include "PacketIO/WriteDataStream.h"
#include "FluxControlObserver.h"

CTS_MainNetL2::CTS_MainNetL2()
{
}

CTS_MainNetL2::~CTS_MainNetL2()
{
}

BOOL CTS_MainNetL2::f_ModInit(CTS_InterfaceCfg* pTsInterfaceCfg, CTS_CONFIG* pTsConfig, ACE_Log_File_Msg* pLogFile, CArpTableEx* pArpTable)
{
	// 基础类的初始化
	m_pTsInterfaceCfg = pTsInterfaceCfg;
	m_pTsCfg = pTsConfig;
	m_pLogFile = pLogFile;
	m_pArpTable = pArpTable;

	// CK网口使用的物理网口编号
	m_nIfIndex  = (ACE_UINT32)EtherIO_GetOneIndexByShowName(pTsConfig->m_iMainNetCfg.m_strCardName.c_str());

	// 使用的静态IP
	m_hlIp = pTsConfig->m_iMainNetCfg.m_hlIpAddr;

	// 自己的MAC地址
	EtherIO_GetDeviceMac(m_nIfIndex, m_aMacAddr);

	m_nChkPeriod = pTsConfig->m_iMainNetCfg.m_nDetectPeriod;
	m_nChkQueue = pTsConfig->m_iMainNetCfg.m_nDetectQueue;
	m_nChkLostNum = pTsConfig->m_iMainNetCfg.m_nSwitchLostThreshold;
	m_nChkReplyNum = pTsConfig->m_iMainNetCfg.m_nDetectQueue - pTsConfig->m_iMainNetCfg.m_nRecoverLostThreshold;

	// 路由器的监测(二层交换机模式下才用到)
	m_piGatewayDct = new CTS_HOST_DETECT;
	if (NULL == m_piGatewayDct)
	{
		return FALSE;
	}
	m_piGatewayDct->f_Init(pTsConfig->m_iMainNetCfg.m_hlGateway, 1);
	
	// 远程主机监测表
	size_t max = pTsConfig->m_vtPeerCfg.size();
	for (size_t i=0; i<max; i++)
	{
		CTS_HOST_DETECT* piMainNetChk = new CTS_HOST_DETECT;
		if (NULL == piMainNetChk)
		{
			return FALSE;
		}

		piMainNetChk->f_Init(pTsConfig->m_vtPeerCfg[i].m_nMainTestIp, m_nChkQueue);

		m_vtPeerDct.push_back(piMainNetChk);
	}

	pTsConfig->f_SetMainNetMgmt();

	return TRUE;
}

BOOL CTS_MainNetL2::f_ModUpdateCfg(CTS_CONFIG* pNewTsCfg)
{
	if (m_pTsCfg->m_iMainNetCfg.m_strCardName != pNewTsCfg->m_iMainNetCfg.m_strCardName)
	{
		MY_ACE_DEBUG(m_pLogFile, (LM_ERROR, ACE_TEXT("[Info][%D]system reboot: main net interface changed.\n")));
		ACE_OS::system("reboot -p");
		return FALSE;
	}

	//不管是本端IP还是网关IP修改，都应该复位探测状态机
	if ((m_pTsCfg->m_iMainNetCfg.m_hlIpAddr != pNewTsCfg->m_iMainNetCfg.m_hlIpAddr)
		|| (m_pTsCfg->m_iMainNetCfg.m_hlNetmask != pNewTsCfg->m_iMainNetCfg.m_hlNetmask)
		|| (m_pTsCfg->m_iMainNetCfg.m_hlGateway != pNewTsCfg->m_iMainNetCfg.m_hlGateway))
	{
		m_hlIp = pNewTsCfg->m_iMainNetCfg.m_hlIpAddr;
		m_hlNetmask = pNewTsCfg->m_iMainNetCfg.m_hlNetmask;
		
		m_piGatewayDct->f_ResetIp(pNewTsCfg->m_iMainNetCfg.m_hlGateway);

		pNewTsCfg->f_SetMainNetMgmt();
	}

	if (m_pTsCfg->m_iMainNetCfg.m_nDetectPeriod != pNewTsCfg->m_iMainNetCfg.m_nDetectPeriod)
	{
		m_nChkPeriod = pNewTsCfg->m_iMainNetCfg.m_nDetectPeriod;
	}

	if (m_pTsCfg->m_iMainNetCfg.m_nDetectQueue != pNewTsCfg->m_iMainNetCfg.m_nDetectQueue)
	{
		m_nChkQueue = pNewTsCfg->m_iMainNetCfg.m_nDetectQueue;

		size_t max = m_vtPeerDct.size();
		size_t i;
		for (i=0; i<max; i++)
		{
			m_vtPeerDct[i]->f_ResetTestNum(m_nChkQueue);
		}
	}

	if (m_pTsCfg->m_iMainNetCfg.m_nSwitchLostThreshold != pNewTsCfg->m_iMainNetCfg.m_nSwitchLostThreshold)
	{
		m_nChkLostNum = pNewTsCfg->m_iMainNetCfg.m_nSwitchLostThreshold;
	}

	if (m_pTsCfg->m_iMainNetCfg.m_nRecoverLostThreshold != pNewTsCfg->m_iMainNetCfg.m_nRecoverLostThreshold)
	{
		m_nChkReplyNum = pNewTsCfg->m_iMainNetCfg.m_nDetectQueue - pNewTsCfg->m_iMainNetCfg.m_nRecoverLostThreshold;
	}

	/*if (m_pTsCfg->m_iMainNetCfg.m_nRecoverTimeout != pNewTsCfg->m_iMainNetCfg.m_nRecoverTimeout)
	{
		
	}*/

	if (m_pTsCfg->m_vtPeerCfg != pNewTsCfg->m_vtPeerCfg)
	{		
		size_t max = m_pTsCfg->m_vtPeerCfg.size();
		size_t i;

		std::vector<CTS_HOST_DETECT*>::iterator iter;

		for (i=0, iter=m_vtPeerDct.begin(); i<max; i++)
		{
			CTS_PEER_CONFIG* pNewPeerCfg = pNewTsCfg->f_FindPeerCfg(m_pTsCfg->m_vtPeerCfg[i].m_nPeerId);
			if (NULL == pNewPeerCfg)
			{
				delete *iter;
				iter = m_vtPeerDct.erase(iter);
			}
			else
			{
				if (m_pTsCfg->m_vtPeerCfg[i].m_nMainTestIp != pNewPeerCfg->m_nMainTestIp)
				{
					CTS_HOST_DETECT* pHostDetect = *iter;
					pHostDetect->f_ResetIp(pNewPeerCfg->m_nMainTestIp);
				}

				iter++;
			}
		}

		max = pNewTsCfg->m_vtPeerCfg.size();
		for (i=0; i<max; i++)
		{
			CTS_PEER_CONFIG* pOldPeerCfg = m_pTsCfg->f_FindPeerCfg(pNewTsCfg->m_vtPeerCfg[i].m_nPeerId);
			if (NULL == pOldPeerCfg)
			{
				CTS_HOST_DETECT* piMainNetChk = new CTS_HOST_DETECT;
				if (NULL == piMainNetChk)
				{
					return FALSE;
				}

				piMainNetChk->f_Init(pNewTsCfg->m_vtPeerCfg[i].m_nMainTestIp, m_nChkQueue);

				m_vtPeerDct.push_back(piMainNetChk);
			}
		}
	}

	m_pTsCfg = pNewTsCfg;
	
	return TRUE;
}

void CTS_MainNetL2::f_HandleMyIcmp(PACKET_INFO_EX& packet_info)
{
	// IP头的长度
	ACE_UINT8 ip_head_length = ((* packet_info.tcpipContext.pIPHeader) & 0x0F)*4;

	// ICMP头
	TS_IcmpHeader *icmp_header = (TS_IcmpHeader *)(packet_info.tcpipContext.pIPHeader + ip_head_length);
	ACE_UINT16 i_id = ACE_HTONS(icmp_header->i_id);
	ACE_UINT16 i_seq = ACE_HTONS(icmp_header->i_seq);

	if ((0 == icmp_header->i_type) && (0 == icmp_header->i_code))// 收到ping应答，刷新监测表
	{
		ACE_UINT32 peer_index;
		if (FALSE == f_IsPeerTestIP(packet_info.tcpipContext.conn_addr.client.dwIP, peer_index))
		{
			return;
		}

		if (1269 == i_id)
		{		
			//ACE_UINT32 ping_id = ((ACE_UINT32)i_id << 16) + (ACE_UINT32)i_seq;
			//m_ping_status_table[peer_index]->OnGetOneRsp(ping_id, packet_info.packet->pHeaderinfo->stamp.sec);
			m_vtPeerDct[peer_index]->m_iHostTest.OnGetOneRsp(i_seq, packet_info.packet->pHeaderinfo->stamp.sec);
		}
	}
	else if ((8 == icmp_header->i_type) && (0 == icmp_header->i_code))// 只回应请求ping报文
	{
		BYTE icmp_field[256];
		
		// ICMP部分数据
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

		// IP部分数据
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

		// MAC部分
		//memcpy(p_MAC_head, m_my_router_MAC, 6);
		memcpy(p_MAC_head, packet_info.tcpipContext.pMacAddrHeader+6, 6);
		memcpy(p_MAC_head+6, m_aMacAddr, 6);

		// type
		*p_type_head = ACE_HTONS(0x0800); // IP报文

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

BOOL CTS_MainNetL2::f_HandleArp(PACKET_INFO_EX& packet_info)
{
	if (packet_info.m_arp_decode_info.senderNetOrderIPAddr == TS_HTONL(m_piGatewayDct->m_hlHostIp))
	{
		if (0 != memcmp(packet_info.m_arp_decode_info.senderMacAddr, m_aMacAddr, 6))// 排除FR的欺骗报文
		{
			m_piGatewayDct->m_iHostTest.OnGetOneRsp(1, packet_info.packet->pHeaderinfo->stamp.sec);
		}
		
		// 更新网关的MAC
		if (0 != memcmp(packet_info.m_arp_decode_info.senderMacAddr, m_piGatewayDct->m_aHostMac, 6))
		{
			unsigned char* pOldMac = m_piGatewayDct->m_aHostMac;
			unsigned char* pNewMac = packet_info.m_arp_decode_info.senderMacAddr;
			MY_ACE_DEBUG(m_pLogFile,
				(LM_ERROR,
				ACE_TEXT("[Info][%D]main net gateway changed. %02x-%02x-%02x-%02x-%02x-%02x to %02x-%02x-%02x-%02x-%02x-%02x\n"),
				pOldMac[0], pOldMac[1], pOldMac[2], pOldMac[3], pOldMac[4], pOldMac[5],
				pNewMac[0], pNewMac[1], pNewMac[2], pNewMac[3], pNewMac[4], pNewMac[5]
				));
			
			memcpy(m_piGatewayDct->m_aHostMac, packet_info.m_arp_decode_info.senderMacAddr, 6);
		}
	}
	else if (TRUE == m_pTsCfg->f_IsLocalHostIp(ACE_NTOHL(packet_info.m_arp_decode_info.senderNetOrderIPAddr))) // 排除非内网的IP
	{
		// 嗅探内网ARP报文		
		m_pArpTable->InsertArpItem // 加入表中
		(
			packet_info.m_arp_decode_info.senderNetOrderIPAddr,  // 发送者的IP
			packet_info.m_arp_decode_info.senderMacAddr,   // 发送者的MAC
			-1,
			-1,
			packet_info.packet->pHeaderinfo->stamp.sec,
			0,
			NULL
		);

		// 回答假的ARP报文
		if ((ARP_FAILED == m_piGatewayDct->m_eHostStat)
		&& (packet_info.m_arp_decode_info.operation == 1)// 请求报文
		&& (packet_info.m_arp_decode_info.destNetOrderIPAddr == TS_HTONL(m_piGatewayDct->m_hlHostIp)) // 目标是主网网关
		)
		{
			unsigned int arp_len = CConstructPacket::MAC_Construct_Arp_Rsp_Packet
								(
									(BYTE *)(packet_info.packet->pPacket), 
									m_aMacAddr,
									TS_HTONL(m_piGatewayDct->m_hlHostIp), 
									packet_info.m_arp_decode_info.senderNetOrderIPAddr,
									packet_info.m_arp_decode_info.senderMacAddr,
									-1
								);

			// 发送
			packet_info.packet->nPktlen = arp_len;
			m_pTsInterfaceCfg->GetWriteDataStream()->WritePacket
													(
														PACKET_OK,
														(const char *)(packet_info.packet->pPacket),
														packet_info.packet->nPktlen,
														(1 << m_nIfIndex),
														1
													);		
		}
	}

	return TRUE;
}

BOOL CTS_MainNetL2::f_HandleIp(PACKET_INFO_EX& packet_info)
{
	if (packet_info.tcpipContext.conn_addr.server.dwIP == m_hlIp)
	{
		if (packet_info.tcpipContext.dwIndexProtocl == CCommonIPv4Decode::INDEX_PID_IPv4_ICMP)
		{
			f_HandleMyIcmp(packet_info);
		}
		
		return TRUE;
	}
	
	//组播不转发
	if (*(ACE_UINT8*)(packet_info.packet->pPacket) & 0x01)
	{
		return TRUE;
	}

	return FALSE;
}

BOOL CTS_MainNetL2::f_HandlePacket(PACKET_INFO_EX& packet_info)
{
	if (CCommonEtherDecode::MAC_DECODE_INFO::IsIP(packet_info.tcpipContext.dwProtoOverMac) == TRUE)
	{
		if (packet_info.tcpipContext.conn_addr.server.dwIP == m_hlIp)
		{
			if (packet_info.tcpipContext.dwIndexProtocl == CCommonIPv4Decode::INDEX_PID_IPv4_ICMP)
			{
				f_HandleMyIcmp(packet_info);
			}
			
			return TRUE;
		}
	}
	else if (CCommonEtherDecode::MAC_DECODE_INFO::IsARP(packet_info.tcpipContext.dwProtoOverMac) == TRUE)
	{
		f_HandleArp(packet_info);

		return TRUE;
	}

	return FALSE;
}

void CTS_MainNetL2::f_PeerDetect(ACE_UINT32 now_time)
{
	m_nChkPeriod++;
	if (m_nChkPeriod < m_pTsCfg->m_iMainNetCfg.m_nDetectPeriod)
	{
		return;
	}
	else
	{
		m_nChkPeriod = 0;
	}
	
	size_t max = m_vtPeerDct.size();
	for (size_t i=0; i<max; i++)
	{		
		switch (m_vtPeerDct[i]->m_eHostStat)
		{
			case UNKNOWN:
				if (ARP_OK == m_piGatewayDct->m_eHostStat)
				{
					m_vtPeerDct[i]->m_eHostStat = ECHO_CHECKING;
					m_vtPeerDct[i]->m_nHostRecoverTimer = 0;
					memcpy(m_vtPeerDct[i]->m_aHostMac, m_piGatewayDct->m_aHostMac, 6);

					for (ACE_UINT32 j=0; j<m_nChkQueue; j++)
					{
						f_SendEchoReq(m_vtPeerDct[i]->m_hlHostIp, m_vtPeerDct[i]->m_aHostMac);
						m_vtPeerDct[i]->m_iHostTest.OnSendOneRequest(m_nChkEchoSeq, now_time);			
					}
					
					MY_ACE_DEBUG(m_pLogFile, (LM_ERROR,
						ACE_TEXT("[Info][%D]main net(%d@%s) connectivity changed. %s to %s\n"),
						i, CPF::hip_to_string(m_vtPeerDct[i]->m_hlHostIp),
						"\"UNKNOWN\"", "\"ECHO_CHECKING\""));
				}
				break;

			case ARP_CHECKING:
				break;

			case ARP_OK:
				break;

			case ARP_FAILED:
				break;

			case ECHO_CHECKING:
				if (m_nChkReplyNum <= m_vtPeerDct[i]->m_iHostTest.GetRspNum())
				{
					m_vtPeerDct[i]->m_nHostRecoverTimer += m_pTsCfg->m_iMainNetCfg.m_nDetectPeriod;//恢复需要稳定期
					if (m_vtPeerDct[i]->m_nHostRecoverTimer >= m_pTsCfg->m_iMainNetCfg.m_nRecoverTimeout)
					{
						m_vtPeerDct[i]->m_eHostStat = ECHO_OK;
						
						MY_ACE_DEBUG(m_pLogFile, (LM_ERROR,
							ACE_TEXT("[Info][%D]main net(%d@%s) connectivity changed. %s to %s\n"),
							i, CPF::hip_to_string(m_vtPeerDct[i]->m_hlHostIp),
							"\"ECHO_CHECKING\"", "\"ECHO_OK\""));
					}
				}
				else if (m_nChkLostNum <= m_vtPeerDct[i]->m_iHostTest.CheckTimeout(now_time, 0))
				{
					m_vtPeerDct[i]->m_eHostStat = ECHO_FAILED;
					
					MY_ACE_DEBUG(m_pLogFile, (LM_ERROR,
						ACE_TEXT("[Info][%D]main net(%d@%s) connectivity changed. %s to %s\n"),
						i, CPF::hip_to_string(m_vtPeerDct[i]->m_hlHostIp),
						"\"ECHO_CHECKING\"", "\"ECHO_FAILED\""));
				}
				f_SendEchoReq(m_vtPeerDct[i]->m_hlHostIp, m_vtPeerDct[i]->m_aHostMac);
				m_vtPeerDct[i]->m_iHostTest.OnSendOneRequest(m_nChkEchoSeq, now_time);			
				break;

			case ECHO_OK:
				if (m_nChkLostNum <= m_vtPeerDct[i]->m_iHostTest.CheckTimeout(now_time, 0))
				{
					m_vtPeerDct[i]->m_eHostStat = ECHO_FAILED;
					
					MY_ACE_DEBUG(m_pLogFile, (LM_ERROR,
						ACE_TEXT("[Info][%D]main net(%d@%s) connectivity changed. %s to %s\n"),
						i, CPF::hip_to_string(m_vtPeerDct[i]->m_hlHostIp),
						"\"ECHO_OK\"", "\"ECHO_FAILED\""));
				}
				f_SendEchoReq(m_vtPeerDct[i]->m_hlHostIp, m_vtPeerDct[i]->m_aHostMac);
				m_vtPeerDct[i]->m_iHostTest.OnSendOneRequest(m_nChkEchoSeq, now_time);			
				break;

			case ECHO_FAILED:
				if (m_nChkReplyNum <= m_vtPeerDct[i]->m_iHostTest.GetRspNum())
				{
					m_vtPeerDct[i]->m_eHostStat = ECHO_CHECKING;
					m_vtPeerDct[i]->m_nHostRecoverTimer = 0;
					
					MY_ACE_DEBUG(m_pLogFile, (LM_ERROR,
						ACE_TEXT("[Info][%D]main net(%d@%s) connectivity changed. %s to %s\n"),
						i, CPF::hip_to_string(m_vtPeerDct[i]->m_hlHostIp),
						"\"ECHO_FAILED\"", "\"ECHO_CHECKING\""));
				}
				f_SendEchoReq(m_vtPeerDct[i]->m_hlHostIp, m_vtPeerDct[i]->m_aHostMac);
				m_vtPeerDct[i]->m_iHostTest.OnSendOneRequest(m_nChkEchoSeq, now_time);			
				break;

			default:
				break;
		}
	}
}

void CTS_MainNetL2::f_ArpSpoof(void)
{
	if(m_pArpTable->GetTotalCount() > 0)
	{
		// 发送ARP欺骗报文				
		BYTE arp_packet[64];
		int index = m_pArpTable->m_arp_table.GetHeadBlock();

		while(index != -1)
		{
			CPF_ArpTable::ArpTableItem * pTableItem =  m_pArpTable->m_arp_table.GetNextBlockFromHead(index);

			unsigned int arp_len = 
			CConstructPacket::MAC_Construct_Arp_Rsp_Packet
			(
				arp_packet, 
				m_aMacAddr,
				TS_HTONL(m_piGatewayDct->m_hlHostIp), 
				pTableItem->hip,
				(BYTE *)(&(pTableItem->mac)),
				-1
			);

			m_pTsInterfaceCfg->GetWriteDataStream()->WritePacket
			(
				PACKET_OK,
				(const char *)arp_packet,
				arp_len,
				(1 << m_nIfIndex),
				1
			);

			// 同时发送ARP查询报文,防止ARP表老化
			//f_SendArpReq(pTableItem->hip);
		}
	}
}

void CTS_MainNetL2::f_ArpRestore(void)
{
	if(m_pArpTable->GetTotalCount() > 0)
	{
		BYTE arp_packet[64];
		int index = m_pArpTable->m_arp_table.GetHeadBlock();

		while(index != -1)
		{
			CPF_ArpTable::ArpTableItem * pTableItem =  m_pArpTable->m_arp_table.GetNextBlockFromHead(index);

			// 发送ARP纠正报文
			unsigned int arp_len = 
			CConstructPacket::MAC_Construct_Arp_Rsp_Packet
			(
				arp_packet, 
				m_piGatewayDct->m_aHostMac,
				TS_HTONL(m_piGatewayDct->m_hlHostIp), 
				pTableItem->hip,
				(BYTE *)(&(pTableItem->mac)),
				-1
			);

			memcpy(arp_packet+6, m_aMacAddr, 6);

			m_pTsInterfaceCfg->GetWriteDataStream()->WritePacket
			(
				PACKET_OK,
				(const char *)arp_packet,
				arp_len,
				(1 << m_nIfIndex),
				1
			);
		}// while()
	}
}

void CTS_MainNetL2::f_MainNetCheck(ACE_UINT32 now_time)
{
	switch(m_piGatewayDct->m_eHostStat)
	{
		case UNKNOWN:
			for (size_t i=0; i<m_vtPeerDct.size(); i++)
			{
				m_vtPeerDct[i]->m_eHostStat = UNKNOWN;
			}
			
			m_piGatewayDct->m_eHostStat = ARP_CHECKING;
			f_SendArpReq(m_piGatewayDct->m_hlHostIp);
			m_piGatewayDct->m_iHostTest.OnSendOneRequest(1, now_time);
			
			MY_ACE_DEBUG(m_pLogFile, (LM_ERROR, ACE_TEXT("[Info][%D]main net gateway changed. %s to %s\n"), "\"UNKNOWN\"", "\"ARP_CHECKING\"") );
			break;

		case ARP_CHECKING:
			if (1 <= m_piGatewayDct->m_iHostTest.GetRspNum())
			{
				m_piGatewayDct->m_eHostStat = ARP_OK;
				MY_ACE_DEBUG(m_pLogFile, (LM_ERROR, ACE_TEXT("[Info][%D]main net gateway changed. %s to %s\n"), "\"ARP_CHECKING\"", "\"ARP_OK\"") );
			}
			else if (1 <= m_piGatewayDct->m_iHostTest.CheckTimeout(now_time, 0))
			{
				m_piGatewayDct->m_eHostStat = ARP_FAILED;

				size_t max = m_vtPeerDct.size();
				for (size_t i=0; i<max; i++)
				{
					m_vtPeerDct[i]->m_eHostStat = UNKNOWN;
					//memset(m_vtPeerDct[i]->m_aHostMac, 0, 6);
				}
				MY_ACE_DEBUG(m_pLogFile, (LM_ERROR, ACE_TEXT("[Info][%D]main net gateway changed. %s to %s\n"), "\"ARP_CHECKING\"", "\"ARP_FAILED\"") );
			}
			
			f_SendArpReq(m_piGatewayDct->m_hlHostIp);
			m_piGatewayDct->m_iHostTest.OnSendOneRequest(1, now_time);
			break;

		case ARP_OK:
			if (1 <= m_piGatewayDct->m_iHostTest.CheckTimeout(now_time, 0))
			{
				m_piGatewayDct->m_eHostStat = ARP_FAILED;
				
				size_t max = m_vtPeerDct.size();
				for (size_t i=0; i<max; i++)
				{
					m_vtPeerDct[i]->m_eHostStat = UNKNOWN;
					//memset(m_vtPeerDct[i]->m_aHostMac, 0, 6);
				}
				
				MY_ACE_DEBUG(m_pLogFile, (LM_ERROR, ACE_TEXT("[Info][%D]main net gateway changed. %s to %s\n"), "\"ARP_OK\"", "\"ARP_FAILED\"") );
			}
			
			f_SendArpReq(m_piGatewayDct->m_hlHostIp);
			m_piGatewayDct->m_iHostTest.OnSendOneRequest(1, now_time);
			break;

		case ARP_FAILED:
			if (1 <= m_piGatewayDct->m_iHostTest.GetRspNum())
			{
				m_piGatewayDct->m_eHostStat = ARP_CHECKING;

				f_ArpRestore();
				MY_ACE_DEBUG(m_pLogFile, (LM_ERROR, ACE_TEXT("[Info][%D]main net gateway changed. %s to %s\n"), "\"ARP_FAILED\"", "\"ARP_CHECKING\"") );
			}
			
			f_SendArpReq(m_piGatewayDct->m_hlHostIp);
			m_piGatewayDct->m_iHostTest.OnSendOneRequest(1, now_time);
			break;

		default:
			break;
	}

	if (ARP_OK == m_piGatewayDct->m_eHostStat)
	{
		f_PeerDetect(now_time);

		if (0 == (now_time % 5))//ARP表老化
		{
			m_pArpTable->CheckTimeout_DataTime(now_time);
		}
	}
	else if (ARP_FAILED == m_piGatewayDct->m_eHostStat)
	{
		//网关丢失时不进行ARP老化，以保证ARP欺骗稳定
		f_ArpSpoof();
	}
}

int CTS_MainNetL2::f_ForwardIp(PACKET_INFO_EX& packet_info)
{
	if (-1 != packet_info.tcpipContext.vlanId)//untag
	{
		packet_info.packet->pPacket = (unsigned char*)(packet_info.packet->pPacket) + 4;
		packet_info.packet->nPktlen -= 4;
	}
	
	// 查询ARP表,获得目的IP的MAC
	ACE_UINT32 vlan_id = -1;
	ACE_UINT16 pppoe_sesion_id = -1;
	BOOL result = m_pArpTable->QuaryMac
								(
									TS_HTONL(packet_info.tcpipContext.conn_addr.server.dwIP),
									packet_info.packet->pPacket, // 目的MAC
									vlan_id,
									pppoe_sesion_id,
									0
								);

	if (result == FALSE) // 查找失败
	{
		f_SendArpReq(packet_info.tcpipContext.conn_addr.server.dwIP);

		memcpy(packet_info.packet->pPacket, m_piGatewayDct->m_aHostMac, 6);// 目的MAC
	}

	memcpy(packet_info.packet->pPacket+6, m_aMacAddr, 6);// 源MAC

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

int CTS_MainNetL2::f_ForwardArp(PACKET_INFO_EX& packet_info)
{
	return TRUE;
}

