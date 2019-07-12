#include "stdafx.h"
#include "TS_BackNet.h"
#include "cpf/ConstructPacket.h"
#include "PacketIO/WriteDataStream.h"
#include "FluxControlObserver.h"

CTS_BackNet::CTS_BackNet()
{
	m_nIfIndex = 0xFFFF;
	m_nIfStatus = -1;
	memset(m_aMacAddr, 0, 6);
	
	m_piGatewayDct = NULL;
	
	m_nChkPeriod = 0;
	m_nChkQueue = 0;
	m_nChkLostNum = 0;
	m_nChkReplyNum = 0;
	m_nChkEchoSeq = 0;
	
	m_vtPeerDct.clear();
	
	m_piPeerPktFilter = NULL;
	m_vtPeerPktQueue.clear();
}

CTS_BackNet::~CTS_BackNet()
{
	if (m_piGatewayDct)
	{
		delete m_piGatewayDct;
	}

	//
	size_t max = m_vtPeerDct.size();
	for (size_t i=0; i<max; i++)
	{
		delete m_vtPeerDct[i];
	}
	m_vtPeerDct.clear();

	//
	if (m_piPeerPktFilter)
	{
		delete m_piPeerPktFilter;
	}

	//
	max = m_vtPeerPktQueue.size();
	for (size_t i=0; i<max; i++)
	{
		delete m_vtPeerPktQueue[i];
	}
	m_vtPeerPktQueue.clear();
}

void CTS_BackNet::f_SendEchoReq(ACE_UINT32 hlHostIp, ACE_UINT8* dstMac)
{
	BYTE ping_packet[1600];
	BYTE *p_MAC_head = ping_packet;
	ACE_UINT16 *p_type_head = (ACE_UINT16 *)(p_MAC_head+12);
	BYTE *p_IP_head = (BYTE *)(p_type_head + 1);
	BYTE icmp_data[128];

	// MAC
	memcpy(p_MAC_head, dstMac, 6);
	memcpy(p_MAC_head+6, m_aMacAddr, 6);

	// type
	*p_type_head = ACE_HTONS(0x0800); // IP报文

	// add 2016-7-10, ping判断优化
	m_nChkEchoSeq++;
	ACE_UINT16 i_id = (ACE_UINT16)(1270);
	ACE_UINT16 i_seq = (ACE_UINT16)(m_nChkEchoSeq);

	// ICMP 部分的字段
	size_t icmp_data_len = 
	CConstructPacket::ICMP_Construct_EchoPacket
	(
		icmp_data,
		i_id, //0xac87,
		i_seq, //0x7834,
		NULL,
		0
	);

	// IP部分的字段
	unsigned int ip_data_len = 
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
		ip_data_len + 14 + 4,
		(1 << m_nIfIndex),
		(int)1
	);

}
/////////////////////////

//向指定主机发送ARP请求报文
void CTS_BackNet::f_SendArpReq(ACE_UINT32 dst_ip)
{
	BYTE arp_packet[1600];

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

	m_pTsInterfaceCfg->GetWriteDataStream()->WritePacket
	(
		PACKET_OK,
		(const char *)arp_packet,
		arp_len,
		(1 << m_nIfIndex),
		(int)1
	);
}

void CTS_BackNet::f_HandleMyIcmp(PACKET_INFO_EX& packet_info)
{
	// IP头的长度
	ACE_UINT8 ip_head_length =( (* packet_info.tcpipContext.pIPHeader) & 0x0F)*4;

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
		
		if (1270 == i_id)
		{
			m_vtPeerDct[peer_index]->m_iHostTest.OnGetOneRsp(i_seq, packet_info.packet->pHeaderinfo->stamp.sec);
		}
	}
	else if ((8 == icmp_header->i_type) && (0 == icmp_header->i_code))	// 只回应请求ping报文
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

BOOL CTS_BackNet::f_HandleArp(PACKET_INFO_EX& packet_info)
{
	if (packet_info.m_arp_decode_info.destNetOrderIPAddr == TS_HTONL(m_hlIp))
	{
		size_t max = m_vtPeerDct.size();
		for (size_t i=0; i<max; i++)
		{
			if (packet_info.m_arp_decode_info.senderNetOrderIPAddr == TS_HTONL(m_vtPeerDct[i]->m_hlHostIp))
			{
				memcpy(m_vtPeerDct[i]->m_aHostMac, packet_info.m_arp_decode_info.senderMacAddr, 6);

				break;
			}
		}

		return TRUE;
	}
	
	//备网接口收到本厂家的MAC，不向主网扩散
	ACE_UINT8* pSrcMac = (ACE_UINT8*)(packet_info.packet->pPacket) + 6;
	if ((pSrcMac[0] == m_aMacAddr[0]) && (pSrcMac[1] == m_aMacAddr[1]) && (pSrcMac[2] == m_aMacAddr[2]))
	{
		return TRUE;
	}

	return FALSE;
}

BOOL CTS_BackNet::f_HandleIp(PACKET_INFO_EX& packet_info)
{
	if (packet_info.tcpipContext.conn_addr.server.dwIP == m_hlIp)
	{
		if (packet_info.tcpipContext.dwIndexProtocl == CCommonIPv4Decode::INDEX_PID_IPv4_ICMP)
		{
			f_HandleMyIcmp(packet_info);
		}

		return TRUE;
	}
	
	//备网接口收到本厂家的MAC，不向主网扩散
	ACE_UINT8* pSrcMac = (ACE_UINT8*)(packet_info.packet->pPacket) + 6;
	if ((pSrcMac[0] == m_aMacAddr[0]) && (pSrcMac[1] == m_aMacAddr[1]) && (pSrcMac[2] == m_aMacAddr[2]))
	{
		return TRUE;
	}

	return FALSE;
}

BOOL CTS_BackNet::f_HandlePacket(PACKET_INFO_EX& packet_info)
{
	if (CCommonEtherDecode::MAC_DECODE_INFO::IsIP(packet_info.tcpipContext.dwProtoOverMac) == TRUE)
	{
		// 目标IP是自己
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

/*BOOL CTS_BackNet::f_ForwardIp(PACKET_INFO_EX& packet_info, ACE_UINT32 des_index)
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

	if (-1 != packet_info.tcpipContext.vlanId)//去掉VALN字段
	{
		packet_info.packet->pPacket = (unsigned char*)(packet_info.packet->pPacket) + 4;
		packet_info.packet->nPktlen -= 4;
	}
	
	memcpy(packet_info.packet->pPacket, m_vtPeerDct[des_index]->m_aHostMac, 6);  // 目的MAC
	memcpy(packet_info.packet->pPacket+6, m_aMacAddr, 6);   // 源MAC
	
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

BOOL CTS_BackNet::f_ForwardArp(PACKET_INFO_EX& packet_info, ACE_UINT32 des_index)
{
	if (ECHO_OK != m_vtPeerDct[des_index]->m_eHostStat)
	{ 
		return FALSE; 
	}

	if (-1 != packet_info.tcpipContext.vlanId)//去掉VALN字段
	{
		packet_info.packet->pPacket = (unsigned char*)(packet_info.packet->pPacket) + 4;
		packet_info.packet->nPktlen -= 4;
	}
	
	memcpy(packet_info.packet->pPacket, m_vtPeerDct[des_index]->m_aHostMac, 6);  // 目的MAC
	memcpy(packet_info.packet->pPacket+6, m_aMacAddr, 6);   // 源MAC
	
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

// 加入队列
BOOL CTS_BackNet::f_PushPktQueue(PACKET_INFO_EX& packet_info, ACE_UINT32 des_index)
{
	unsigned int delete_count=0;

	// 业务是否允许
	if (TRUE != m_piPeerPktFilter->f_ApplyFilter(packet_info))
	{ 
		return TRUE; 
	}

	void *pBuf;
	pBuf = QueueWriteAllocateBuffer(&(m_vtPeerPktQueue[des_index]->m_tagHandle), packet_info.packet->nPktlen);
	while (NULL == pBuf)
	{
		unsigned char *rd_pt;
		if(QueueReadNextPacket(&(m_vtPeerPktQueue[des_index]->m_tagHandle), &rd_pt) == 0)
		{ 
			return(FALSE); 
		}  // 去掉一个包
		delete_count++;
		if(delete_count > 30)
		{ 
			return(FALSE); 
		}
		pBuf = QueueWriteAllocateBuffer(&(m_vtPeerPktQueue[des_index]->m_tagHandle), packet_info.packet->nPktlen);
	}

	if (-1 != packet_info.tcpipContext.vlanId)//去掉VALN字段
	{
		packet_info.packet->pPacket = (unsigned char*)(packet_info.packet->pPacket) + 4;
		packet_info.packet->nPktlen -= 4;
	}

	memcpy(pBuf, packet_info.packet->pPacket, packet_info.packet->nPktlen);
	QueueWriteDataFinished(&(m_vtPeerPktQueue[des_index]->m_tagHandle), packet_info.packet->nPktlen);

	return(TRUE);
}

//释放缓存队列
BOOL CTS_BackNet::f_FlushPktQueue(ACE_UINT32 des_index)
{
	if (ECHO_OK != m_vtPeerDct[des_index]->m_eHostStat)
	{ 
		return FALSE; 
	}

	unsigned char *cur_send_pt = NULL;
	int cur_send_size = 0;

	do
	{
		cur_send_size = QueueReadNextPacket(&(m_vtPeerPktQueue[des_index]->m_tagHandle), &cur_send_pt);

		if((cur_send_pt != NULL) && (cur_send_size > 0))
		{
			memcpy((void *)(cur_send_pt), m_vtPeerDct[des_index]->m_aHostMac, 6);  // 目的MAC
			memcpy((void *)(cur_send_pt+6), m_aMacAddr, 6);   // 源MAC

			m_pTsInterfaceCfg->GetWriteDataStream()->WritePacket
			(
				PACKET_OK,
				(const char *)cur_send_pt,
				cur_send_size,
				(1<<m_nIfIndex),
				1
			);
		}
		else
		{ 
			break; 
		}
	}while(1);

	return(TRUE);
}*/


// 是否是认识的探测IP
BOOL CTS_BackNet::f_IsPeerTestIP(ACE_UINT32 hlIp, ACE_UINT32 &peer_index)
{
	size_t max = m_vtPeerDct.size();
	for (size_t i=0; i<max; i++)
	{
		// 备网探测IP
		if (hlIp == m_vtPeerDct[i]->m_hlHostIp)
		{
			peer_index = (ACE_UINT32)i;
			return(TRUE);
		}
	}

	return(FALSE);
}

BOOL CTS_BackNet::f_IsSubNetIp(ACE_UINT32 hlIp)
{
	ACE_UINT32 hlDstSubnet;
	
	hlDstSubnet = hlIp & m_hlNetmask;
	if (hlDstSubnet == m_hlSubnet)
	{
		return TRUE;
	}

	return FALSE;
}

CTS_BackNetEoSgl1::CTS_BackNetEoSgl1()
{
}

CTS_BackNetEoSgl1::~CTS_BackNetEoSgl1()
{	
}

BOOL CTS_BackNetEoSgl1::f_ModInit(CTS_InterfaceCfg* pTsInterfaceCfg, CTS_CONFIG* pTsConfig, ACE_Log_File_Msg* pLogFile)
{
	m_pTsInterfaceCfg = pTsInterfaceCfg;
	m_pTsCfg = pTsConfig;
	m_pLogFile = pLogFile;

	// 备用网口使用的物理网口编号
	m_nIfIndex = (ACE_UINT32)EtherIO_GetOneIndexByShowName(pTsConfig->m_iBackNet1Cfg.m_strCardName.c_str());

	m_hlIp = pTsConfig->m_iBackNet1Cfg.m_hlIpAddr; 
	m_hlNetmask = pTsConfig->m_iBackNet1Cfg.m_hlNetmask;
	m_hlSubnet = m_hlIp & m_hlNetmask;
	
	EtherIO_GetDeviceMac(m_nIfIndex, m_aMacAddr);

	m_nChkPeriod = pTsConfig->m_iBackNet1Cfg.m_nDetectPeriod;
	m_nChkQueue = pTsConfig->m_iBackNet1Cfg.m_nDetectQueue;
	m_nChkLostNum = pTsConfig->m_iBackNet1Cfg.m_nSwitchLostThreshold;
	m_nChkReplyNum = pTsConfig->m_iBackNet1Cfg.m_nDetectQueue - pTsConfig->m_iBackNet1Cfg.m_nRecoverLostThreshold;

	m_piGatewayDct = NULL;

	// 远程主机监测表
	f_PeerInit(pTsConfig);
	
	f_FilterInit(pTsConfig);

	f_QueueInit(pTsConfig);

	pTsConfig->f_SetBackNet1Mgmt();
	
	return(TRUE);
}

BOOL CTS_BackNetEoSgl1::f_ModUpdateCfg(CTS_CONFIG* pNewTsCfg)
{
	if (m_pTsCfg->m_iBackNet1Cfg.m_strCardName != pNewTsCfg->m_iBackNet1Cfg.m_strCardName)
	{
		MY_ACE_DEBUG(m_pLogFile, (LM_ERROR, ACE_TEXT("[Info][%D]system reboot: back net interface changed.\n")));
		ACE_OS::system("reboot -p");
		return FALSE;
	}

	if ((m_pTsCfg->m_iBackNet1Cfg.m_hlIpAddr != pNewTsCfg->m_iBackNet1Cfg.m_hlIpAddr)
		|| (m_pTsCfg->m_iBackNet1Cfg.m_hlNetmask != pNewTsCfg->m_iBackNet1Cfg.m_hlNetmask))
	{
		m_hlIp = pNewTsCfg->m_iBackNet1Cfg.m_hlIpAddr;
		m_hlNetmask = pNewTsCfg->m_iBackNet1Cfg.m_hlNetmask;
		m_hlSubnet = m_hlIp & m_hlNetmask;

		pNewTsCfg->f_SetBackNet1Mgmt();
	}

	/*if (m_pTsCfg->m_iBackNet1Cfg.m_hlGateway != pNewTsCfg->m_iBackNet1Cfg.m_hlGateway)
	{
	}*/

	if (m_pTsCfg->m_iBackNet1Cfg.m_nDetectPeriod != pNewTsCfg->m_iBackNet1Cfg.m_nDetectPeriod)
	{
		m_nChkPeriod = pNewTsCfg->m_iBackNet1Cfg.m_nDetectPeriod;
	}

	if (m_pTsCfg->m_iBackNet1Cfg.m_nDetectQueue != pNewTsCfg->m_iBackNet1Cfg.m_nDetectQueue)
	{
		m_nChkQueue = pNewTsCfg->m_iBackNet1Cfg.m_nDetectQueue;
		
		size_t max = m_vtPeerDct.size();
		size_t i;
		for (i=0; i<max; i++)
		{
			m_vtPeerDct[i]->f_ResetTestNum(m_nChkQueue);
		}
	}
	
	if (m_pTsCfg->m_iBackNet1Cfg.m_nSwitchLostThreshold != pNewTsCfg->m_iBackNet1Cfg.m_nSwitchLostThreshold)
	{
		m_nChkLostNum = pNewTsCfg->m_iBackNet1Cfg.m_nSwitchLostThreshold;
	}

	if (m_pTsCfg->m_iBackNet1Cfg.m_nRecoverLostThreshold != pNewTsCfg->m_iBackNet1Cfg.m_nRecoverLostThreshold)
	{
		m_nChkReplyNum = pNewTsCfg->m_iBackNet1Cfg.m_nDetectQueue - pNewTsCfg->m_iBackNet1Cfg.m_nRecoverLostThreshold;
	}

	f_PeerUpdateCfg(pNewTsCfg);

	f_FilterUpdateCfg(pNewTsCfg);

	f_QueueUpdateCfg(pNewTsCfg);

	m_pTsCfg = pNewTsCfg;
	
	return TRUE;
}

BOOL CTS_BackNetEoSgl1::f_PeerInit(CTS_CONFIG* pTsConfig)
{
	size_t max = pTsConfig->m_vtPeerCfg.size();
	for (size_t i=0; i<max; i++)
	{
		CTS_HOST_DETECT* piBackNetChk = new CTS_HOST_DETECT;
		if (NULL == piBackNetChk)
		{
			return FALSE;
		}

		piBackNetChk->f_Init(pTsConfig->m_vtPeerCfg[i].m_nB1TestIp, m_nChkQueue);

		m_vtPeerDct.push_back(piBackNetChk);
	}

	return TRUE;
}

BOOL CTS_BackNetEoSgl1::f_PeerUpdateCfg(CTS_CONFIG* pNewTsCfg)
{
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
				if (m_pTsCfg->m_vtPeerCfg[i].m_nB1TestIp != pNewPeerCfg->m_nB1TestIp)
				{
					CTS_HOST_DETECT* pHostDetect = *iter;
					pHostDetect->f_ResetIp(pNewPeerCfg->m_nB1TestIp);
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
				CTS_HOST_DETECT* piBackNetChk = new CTS_HOST_DETECT;
				if (NULL == piBackNetChk)
				{
					return FALSE;
				}

				piBackNetChk->f_Init(pNewTsCfg->m_vtPeerCfg[i].m_nB1TestIp, m_nChkQueue);

				m_vtPeerDct.push_back(piBackNetChk);
			}
		}
	}

	return TRUE;
}

BOOL CTS_BackNetEoSgl1::f_FilterInit(CTS_CONFIG* pTsConfig)
{
	m_piPeerPktFilter = new CTS_PACKET_FILTER;
	if (NULL == m_piPeerPktFilter)
	{
		return FALSE;
	}
	
	size_t max = pTsConfig->m_iBackNet1Cfg.m_vtFilterId.size();
	for (size_t i=0; i<max; i++)
	{
		CTS_FILTER_CONFIG* pFilter = pTsConfig->f_FindFilter(pTsConfig->m_iBackNet1Cfg.m_vtFilterId[i]);
		if (NULL == pFilter)
		{
			return FALSE;
		}

		m_piPeerPktFilter->m_vtFilter.push_back(pFilter);
	}

	return TRUE;
}

BOOL CTS_BackNetEoSgl1::f_FilterUpdateCfg(CTS_CONFIG* pNewTsCfg)
{
	size_t nCurMax = m_pTsCfg->m_iBackNet1Cfg.m_vtFilterId.size();
	size_t nNewMax = pNewTsCfg->m_iBackNet1Cfg.m_vtFilterId.size();
	size_t i;

	if (nCurMax <= nNewMax)
	{
		for (i=0; i<nCurMax; i++)
		{
			CTS_FILTER_CONFIG* pFilter = pNewTsCfg->f_FindFilter(pNewTsCfg->m_iBackNet1Cfg.m_vtFilterId[i]);

			m_piPeerPktFilter->m_vtFilter[i] = pFilter;
		}

		for (i=nCurMax; i<nNewMax; i++)
		{
			CTS_FILTER_CONFIG* pFilter = pNewTsCfg->f_FindFilter(pNewTsCfg->m_iBackNet1Cfg.m_vtFilterId[i]);

			m_piPeerPktFilter->m_vtFilter.push_back(pFilter);
		}
	}
	else
	{
		for (i=0; i<nNewMax; i++)
		{
			CTS_FILTER_CONFIG* pFilter = pNewTsCfg->f_FindFilter(pNewTsCfg->m_iBackNet1Cfg.m_vtFilterId[i]);

			m_piPeerPktFilter->m_vtFilter[i] = pFilter;
		}

		m_piPeerPktFilter->m_vtFilter.erase(m_piPeerPktFilter->m_vtFilter.begin()+nNewMax, m_piPeerPktFilter->m_vtFilter.end());
	}

	return TRUE;
}

BOOL CTS_BackNetEoSgl1::f_QueueInit(CTS_CONFIG* pTsConfig)
{
	if (64 > pTsConfig->m_iGlobalCfg.m_nQueueSize)
	{
		return TRUE;
	}
	
	//每个对局使用独立队列
	size_t max = pTsConfig->m_vtPeerCfg.size();
	for (size_t i=0; i<max; i++)
	{
		CTS_PACKET_QUEUE* piPacketQueue = new CTS_PACKET_QUEUE;
		if (NULL == piPacketQueue)
		{
			return FALSE;
		}

		piPacketQueue->f_Init(pTsConfig->m_iGlobalCfg.m_nQueueSize);

		m_vtPeerPktQueue.push_back(piPacketQueue);
	}

	return TRUE;
}

BOOL CTS_BackNetEoSgl1::f_QueueUpdateCfg(CTS_CONFIG* pNewTsCfg)
{
	if (0 == m_vtPeerPktQueue.size())
	{
		return TRUE;
	}

	if (m_pTsCfg->m_vtPeerCfg != pNewTsCfg->m_vtPeerCfg)
	{
		size_t max = m_pTsCfg->m_vtPeerCfg.size();
		size_t i;

		std::vector<CTS_PACKET_QUEUE*>::iterator iter;

		for (i=0, iter=m_vtPeerPktQueue.begin(); i<max; i++)
		{
			CTS_PEER_CONFIG* pNewPeerCfg = pNewTsCfg->f_FindPeerCfg(m_pTsCfg->m_vtPeerCfg[i].m_nPeerId);
			if (NULL == pNewPeerCfg)
			{
				delete *iter;
				iter = m_vtPeerPktQueue.erase(iter);
			}
			else
			{
				if (m_pTsCfg->m_vtPeerCfg[i].m_vtPeerIpRange != pNewTsCfg->m_vtPeerCfg[i].m_vtPeerIpRange)
				{
					CTS_PACKET_QUEUE* pPacketQueue = *iter;
					pPacketQueue->f_ClearQueue();
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
				CTS_PACKET_QUEUE* piPacketQueue = new CTS_PACKET_QUEUE;
				if (NULL == piPacketQueue)
				{
					return FALSE;
				}

				piPacketQueue->f_Init(pNewTsCfg->m_iGlobalCfg.m_nQueueSize);

				m_vtPeerPktQueue.push_back(piPacketQueue);
			}
		}
	}

	return TRUE;
}

void CTS_BackNetEoSgl1::f_BackNetCheck(ACE_UINT32 now_time)
{
	m_nChkPeriod++;
	if (m_nChkPeriod < m_pTsCfg->m_iBackNet1Cfg.m_nDetectPeriod)
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
				m_vtPeerDct[i]->m_eHostStat = ARP_CHECKING;
				f_SendArpReq(m_vtPeerDct[i]->m_hlHostIp);
				
				MY_ACE_DEBUG(m_pLogFile, (LM_ERROR,
					ACE_TEXT("[Info][%D]back net(%d@%s) connectivity changed. %s to %s\n"),
					i, CPF::hip_to_string(m_vtPeerDct[i]->m_hlHostIp),
					"\"UNKNOWN\"", "\"ARP_CHECKING\""));
				break;

			case ARP_CHECKING:
				if (0 != memcmp(m_vtPeerDct[i]->m_aHostMac, "\0\0\0\0\0\0\0", 6))
				{
					m_vtPeerDct[i]->m_eHostStat = ARP_OK;
					
					MY_ACE_DEBUG(m_pLogFile, (LM_ERROR,
						ACE_TEXT("[Info][%D]back net(%d@%s) connectivity changed. %s to %s\n"),
						i, CPF::hip_to_string(m_vtPeerDct[i]->m_hlHostIp),
						"\"ARP_CHECKING\"", "\"ARP_OK\""));
				}
				else
				{
					f_SendArpReq(m_vtPeerDct[i]->m_hlHostIp);
				}
				break;

			case ARP_OK:
				m_vtPeerDct[i]->m_eHostStat = ECHO_CHECKING;
				m_vtPeerDct[i]->m_nHostRecoverTimer = 0;
				for (ACE_UINT32 j=0; j<m_nChkQueue; j++)
				{
					f_SendEchoReq(m_vtPeerDct[i]->m_hlHostIp, m_vtPeerDct[i]->m_aHostMac);
					m_vtPeerDct[i]->m_iHostTest.OnSendOneRequest(m_nChkEchoSeq, now_time);
				}
				
				MY_ACE_DEBUG(m_pLogFile, (LM_ERROR,
					ACE_TEXT("[Info][%D]back net(%d@%s) connectivity changed. %s to %s\n"),
					i, CPF::hip_to_string(m_vtPeerDct[i]->m_hlHostIp),
					"\"ARP_OK\"", "\"ECHO_CHECKING\""));
				break;

			case ARP_FAILED:
				break;

			case ECHO_CHECKING:
				if (m_nChkReplyNum <= m_vtPeerDct[i]->m_iHostTest.GetRspNum())
				{
					m_vtPeerDct[i]->m_nHostRecoverTimer += m_pTsCfg->m_iBackNet1Cfg.m_nDetectPeriod;
					if (m_vtPeerDct[i]->m_nHostRecoverTimer >= m_pTsCfg->m_iBackNet1Cfg.m_nRecoverTimeout)
					{
						m_vtPeerDct[i]->m_eHostStat = ECHO_OK;
						
						MY_ACE_DEBUG(m_pLogFile, (LM_ERROR,
							ACE_TEXT("[Info][%D]back net(%d@%s) connectivity changed. %s to %s\n"),
							i, CPF::hip_to_string(m_vtPeerDct[i]->m_hlHostIp),
							"\"ECHO_CHECKING\"", "\"ECHO_OK\""));
					}
				}
				else if (m_nChkLostNum <= m_vtPeerDct[i]->m_iHostTest.CheckTimeout(now_time, 0))
				{
					m_vtPeerDct[i]->m_eHostStat = ECHO_FAILED;
					m_vtPeerDct[i]->m_nHostRecoverTimer = 0;
										
					MY_ACE_DEBUG(m_pLogFile, (LM_ERROR,
						ACE_TEXT("[Info][%D]back net(%d@%s) connectivity changed. %s to %s\n"),
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
					m_vtPeerDct[i]->m_nHostRecoverTimer = 0;
					
					MY_ACE_DEBUG(m_pLogFile, (LM_ERROR,
						ACE_TEXT("[Info][%D]back net(%d@%s) connectivity changed. %s to %s\n"),
						i, CPF::hip_to_string(m_vtPeerDct[i]->m_hlHostIp),
						"\"ECHO_OK\"", "\"ECHO_FAILED\""));
				}
				f_SendEchoReq(m_vtPeerDct[i]->m_hlHostIp, m_vtPeerDct[i]->m_aHostMac);
				m_vtPeerDct[i]->m_iHostTest.OnSendOneRequest(m_nChkEchoSeq, now_time);
				break;

			case ECHO_FAILED:
				m_vtPeerDct[i]->m_nHostRecoverTimer += m_pTsCfg->m_iBackNet1Cfg.m_nDetectPeriod;
				if (60 <= m_vtPeerDct[i]->m_nHostRecoverTimer)//ARP老化
				{
					m_vtPeerDct[i]->m_eHostStat = ARP_CHECKING;
					
					memset(m_vtPeerDct[i]->m_aHostMac, 0, 6);
					
					MY_ACE_DEBUG(m_pLogFile, (LM_ERROR,
						ACE_TEXT("[Info][%D]back net(%d@%s) connectivity changed. %s to %s\n"),
						i, CPF::hip_to_string(m_vtPeerDct[i]->m_hlHostIp),
						"\"ECHO_FAILED\"", "\"ARP_CHECKING\""));
				}
				else
				{
					if (m_nChkReplyNum <= m_vtPeerDct[i]->m_iHostTest.GetRspNum())
					{
						m_vtPeerDct[i]->m_eHostStat = ECHO_CHECKING;
						m_vtPeerDct[i]->m_nHostRecoverTimer = 0;
						
						MY_ACE_DEBUG(m_pLogFile, (LM_ERROR,
							ACE_TEXT("[Info][%D]back net(%d@%s) connectivity changed. %s to %s\n"),
							i, CPF::hip_to_string(m_vtPeerDct[i]->m_hlHostIp),
							"\"ECHO_FAILED\"", "\"ECHO_CHECKING\""));
					}				
					f_SendEchoReq(m_vtPeerDct[i]->m_hlHostIp, m_vtPeerDct[i]->m_aHostMac);
					m_vtPeerDct[i]->m_iHostTest.OnSendOneRequest(m_nChkEchoSeq, now_time);
				}
				break;

			default:
				break;
		}
	}
}

CTS_BackNetEoSgl2::CTS_BackNetEoSgl2()
{
}

CTS_BackNetEoSgl2::~CTS_BackNetEoSgl2()
{	
}

BOOL CTS_BackNetEoSgl2::f_ModInit(CTS_InterfaceCfg* pTsInterfaceCfg, CTS_CONFIG* pTsConfig, ACE_Log_File_Msg* pLogFile)
{
	m_pTsInterfaceCfg = pTsInterfaceCfg;
	m_pTsCfg = pTsConfig;
	m_pLogFile = pLogFile;

	// 备用网口使用的物理网口编号
	m_nIfIndex = (ACE_UINT32)EtherIO_GetOneIndexByShowName(pTsConfig->m_iBackNet2Cfg.m_strCardName.c_str());

	m_hlIp = pTsConfig->m_iBackNet2Cfg.m_hlIpAddr; 
	m_hlNetmask = pTsConfig->m_iBackNet2Cfg.m_hlNetmask;
	m_hlSubnet = m_hlIp & m_hlNetmask;
	
	EtherIO_GetDeviceMac(m_nIfIndex, m_aMacAddr);

	m_nChkPeriod = pTsConfig->m_iBackNet2Cfg.m_nDetectPeriod;
	m_nChkQueue = pTsConfig->m_iBackNet2Cfg.m_nDetectQueue;
	m_nChkLostNum = pTsConfig->m_iBackNet2Cfg.m_nSwitchLostThreshold;
	m_nChkReplyNum = pTsConfig->m_iBackNet2Cfg.m_nDetectQueue - pTsConfig->m_iBackNet2Cfg.m_nRecoverLostThreshold;

	m_piGatewayDct = NULL;

	// 远程主机监测表
	f_PeerInit(pTsConfig);
	
	f_FilterInit(pTsConfig);

	f_QueueInit(pTsConfig);

	pTsConfig->f_SetBackNet2Mgmt();
	
	return(TRUE);
}

BOOL CTS_BackNetEoSgl2::f_ModUpdateCfg(CTS_CONFIG* pNewTsCfg)
{
	if (m_pTsCfg->m_iBackNet2Cfg.m_strCardName != pNewTsCfg->m_iBackNet2Cfg.m_strCardName)
	{
		MY_ACE_DEBUG(m_pLogFile, (LM_ERROR, ACE_TEXT("[Info][%D]system reboot: back net interface changed.\n")));
		ACE_OS::system("reboot -p");
		return FALSE;
	}

	if ((m_pTsCfg->m_iBackNet2Cfg.m_hlIpAddr != pNewTsCfg->m_iBackNet2Cfg.m_hlIpAddr)
		|| (m_pTsCfg->m_iBackNet2Cfg.m_hlNetmask != pNewTsCfg->m_iBackNet2Cfg.m_hlNetmask))
	{
		m_hlIp = pNewTsCfg->m_iBackNet2Cfg.m_hlIpAddr;
		m_hlNetmask = pNewTsCfg->m_iBackNet2Cfg.m_hlNetmask;
		m_hlSubnet = m_hlIp & m_hlNetmask;

		pNewTsCfg->f_SetBackNet2Mgmt();
	}

	/*if (m_pTsCfg->m_iBackNet1Cfg.m_hlGateway != pNewTsCfg->m_iBackNet1Cfg.m_hlGateway)
	{
	}*/

	if (m_pTsCfg->m_iBackNet2Cfg.m_nDetectPeriod != pNewTsCfg->m_iBackNet2Cfg.m_nDetectPeriod)
	{
		m_nChkPeriod = pNewTsCfg->m_iBackNet2Cfg.m_nDetectPeriod;
	}

	if (m_pTsCfg->m_iBackNet2Cfg.m_nDetectQueue != pNewTsCfg->m_iBackNet2Cfg.m_nDetectQueue)
	{
		m_nChkQueue = pNewTsCfg->m_iBackNet2Cfg.m_nDetectQueue;
		
		size_t max = m_vtPeerDct.size();
		size_t i;
		for (i=0; i<max; i++)
		{
			m_vtPeerDct[i]->f_ResetTestNum(m_nChkQueue);
		}
	}
	
	if (m_pTsCfg->m_iBackNet2Cfg.m_nSwitchLostThreshold != pNewTsCfg->m_iBackNet2Cfg.m_nSwitchLostThreshold)
	{
		m_nChkLostNum = pNewTsCfg->m_iBackNet2Cfg.m_nSwitchLostThreshold;
	}

	if (m_pTsCfg->m_iBackNet2Cfg.m_nRecoverLostThreshold != pNewTsCfg->m_iBackNet2Cfg.m_nRecoverLostThreshold)
	{
		m_nChkReplyNum = pNewTsCfg->m_iBackNet2Cfg.m_nDetectQueue - pNewTsCfg->m_iBackNet2Cfg.m_nRecoverLostThreshold;
	}

	f_PeerUpdateCfg(pNewTsCfg);

	f_FilterUpdateCfg(pNewTsCfg);

	f_QueueUpdateCfg(pNewTsCfg);

	m_pTsCfg = pNewTsCfg;
	
	return TRUE;
}

BOOL CTS_BackNetEoSgl2::f_PeerInit(CTS_CONFIG* pTsConfig)
{
	size_t max = pTsConfig->m_vtPeerCfg.size();
	for (size_t i=0; i<max; i++)
	{
		CTS_HOST_DETECT* piBackNetChk = new CTS_HOST_DETECT;
		if (NULL == piBackNetChk)
		{
			return FALSE;
		}

		piBackNetChk->f_Init(pTsConfig->m_vtPeerCfg[i].m_nB2TestIp, m_nChkQueue);

		m_vtPeerDct.push_back(piBackNetChk);
	}

	return TRUE;
}

BOOL CTS_BackNetEoSgl2::f_PeerUpdateCfg(CTS_CONFIG* pNewTsCfg)
{
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
				if (m_pTsCfg->m_vtPeerCfg[i].m_nB2TestIp != pNewPeerCfg->m_nB2TestIp)
				{
					CTS_HOST_DETECT* pHostDetect = *iter;
					pHostDetect->f_ResetIp(pNewPeerCfg->m_nB2TestIp);
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
				CTS_HOST_DETECT* piBackNetChk = new CTS_HOST_DETECT;
				if (NULL == piBackNetChk)
				{
					return FALSE;
				}

				piBackNetChk->f_Init(pNewTsCfg->m_vtPeerCfg[i].m_nB2TestIp, m_nChkQueue);

				m_vtPeerDct.push_back(piBackNetChk);
			}
		}
	}

	return TRUE;
}

BOOL CTS_BackNetEoSgl2::f_FilterInit(CTS_CONFIG* pTsConfig)
{
	m_piPeerPktFilter = new CTS_PACKET_FILTER;
	if (NULL == m_piPeerPktFilter)
	{
		return FALSE;
	}
	
	size_t max = pTsConfig->m_iBackNet2Cfg.m_vtFilterId.size();
	for (size_t i=0; i<max; i++)
	{
		CTS_FILTER_CONFIG* pFilter = pTsConfig->f_FindFilter(pTsConfig->m_iBackNet2Cfg.m_vtFilterId[i]);
		if (NULL == pFilter)
		{
			return FALSE;
		}

		m_piPeerPktFilter->m_vtFilter.push_back(pFilter);
	}

	return TRUE;
}

BOOL CTS_BackNetEoSgl2::f_FilterUpdateCfg(CTS_CONFIG* pNewTsCfg)
{
	size_t nCurMax = m_pTsCfg->m_iBackNet2Cfg.m_vtFilterId.size();
	size_t nNewMax = pNewTsCfg->m_iBackNet2Cfg.m_vtFilterId.size();
	size_t i;

	if (nCurMax <= nNewMax)
	{
		for (i=0; i<nCurMax; i++)
		{
			CTS_FILTER_CONFIG* pFilter = pNewTsCfg->f_FindFilter(pNewTsCfg->m_iBackNet2Cfg.m_vtFilterId[i]);

			m_piPeerPktFilter->m_vtFilter[i] = pFilter;
		}

		for (i=nCurMax; i<nNewMax; i++)
		{
			CTS_FILTER_CONFIG* pFilter = pNewTsCfg->f_FindFilter(pNewTsCfg->m_iBackNet2Cfg.m_vtFilterId[i]);

			m_piPeerPktFilter->m_vtFilter.push_back(pFilter);
		}
	}
	else
	{
		for (i=0; i<nNewMax; i++)
		{
			CTS_FILTER_CONFIG* pFilter = pNewTsCfg->f_FindFilter(pNewTsCfg->m_iBackNet2Cfg.m_vtFilterId[i]);

			m_piPeerPktFilter->m_vtFilter[i] = pFilter;
		}

		m_piPeerPktFilter->m_vtFilter.erase(m_piPeerPktFilter->m_vtFilter.begin()+nNewMax, m_piPeerPktFilter->m_vtFilter.end());
	}

	return TRUE;
}

BOOL CTS_BackNetEoSgl2::f_QueueInit(CTS_CONFIG* pTsConfig)
{
	if (64 > pTsConfig->m_iGlobalCfg.m_nQueueSize)
	{
		return TRUE;
	}
	
	//每个对局使用独立队列
	size_t max = pTsConfig->m_vtPeerCfg.size();
	for (size_t i=0; i<max; i++)
	{
		CTS_PACKET_QUEUE* piPacketQueue = new CTS_PACKET_QUEUE;
		if (NULL == piPacketQueue)
		{
			return FALSE;
		}

		piPacketQueue->f_Init(pTsConfig->m_iGlobalCfg.m_nQueueSize);

		m_vtPeerPktQueue.push_back(piPacketQueue);
	}

	return TRUE;
}

BOOL CTS_BackNetEoSgl2::f_QueueUpdateCfg(CTS_CONFIG* pNewTsCfg)
{
	if (0 == m_vtPeerPktQueue.size())
	{
		return TRUE;
	}

	if (m_pTsCfg->m_vtPeerCfg != pNewTsCfg->m_vtPeerCfg)
	{
		size_t max = m_pTsCfg->m_vtPeerCfg.size();
		size_t i;

		std::vector<CTS_PACKET_QUEUE*>::iterator iter;

		for (i=0, iter=m_vtPeerPktQueue.begin(); i<max; i++)
		{
			CTS_PEER_CONFIG* pNewPeerCfg = pNewTsCfg->f_FindPeerCfg(m_pTsCfg->m_vtPeerCfg[i].m_nPeerId);
			if (NULL == pNewPeerCfg)
			{
				delete *iter;
				iter = m_vtPeerPktQueue.erase(iter);
			}
			else
			{
				if (m_pTsCfg->m_vtPeerCfg[i].m_vtPeerIpRange != pNewTsCfg->m_vtPeerCfg[i].m_vtPeerIpRange)
				{
					CTS_PACKET_QUEUE* pPacketQueue = *iter;
					pPacketQueue->f_ClearQueue();
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
				CTS_PACKET_QUEUE* piPacketQueue = new CTS_PACKET_QUEUE;
				if (NULL == piPacketQueue)
				{
					return FALSE;
				}

				piPacketQueue->f_Init(pNewTsCfg->m_iGlobalCfg.m_nQueueSize);

				m_vtPeerPktQueue.push_back(piPacketQueue);
			}
		}
	}

	return TRUE;
}

void CTS_BackNetEoSgl2::f_BackNetCheck(ACE_UINT32 now_time)
{
	m_nChkPeriod++;
	if (m_nChkPeriod < m_pTsCfg->m_iBackNet2Cfg.m_nDetectPeriod)
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
				m_vtPeerDct[i]->m_eHostStat = ARP_CHECKING;
				f_SendArpReq(m_vtPeerDct[i]->m_hlHostIp);
				
				MY_ACE_DEBUG(m_pLogFile, (LM_ERROR,
					ACE_TEXT("[Info][%D]back net(%d@%s) connectivity changed. %s to %s\n"),
					i, CPF::hip_to_string(m_vtPeerDct[i]->m_hlHostIp),
					"\"UNKNOWN\"", "\"ARP_CHECKING\""));
				break;

			case ARP_CHECKING:
				if (0 != memcmp(m_vtPeerDct[i]->m_aHostMac, "\0\0\0\0\0\0\0", 6))
				{
					m_vtPeerDct[i]->m_eHostStat = ARP_OK;
					
					MY_ACE_DEBUG(m_pLogFile, (LM_ERROR,
						ACE_TEXT("[Info][%D]back net(%d@%s) connectivity changed. %s to %s\n"),
						i, CPF::hip_to_string(m_vtPeerDct[i]->m_hlHostIp),
						"\"ARP_CHECKING\"", "\"ARP_OK\""));
				}
				else
				{
					f_SendArpReq(m_vtPeerDct[i]->m_hlHostIp);
				}
				break;

			case ARP_OK:
				m_vtPeerDct[i]->m_eHostStat = ECHO_CHECKING;
				m_vtPeerDct[i]->m_nHostRecoverTimer = 0;
				for (ACE_UINT32 j=0; j<m_nChkQueue; j++)
				{
					f_SendEchoReq(m_vtPeerDct[i]->m_hlHostIp, m_vtPeerDct[i]->m_aHostMac);
					m_vtPeerDct[i]->m_iHostTest.OnSendOneRequest(m_nChkEchoSeq, now_time);
				}
				
				MY_ACE_DEBUG(m_pLogFile, (LM_ERROR,
					ACE_TEXT("[Info][%D]back net(%d@%s) connectivity changed. %s to %s\n"),
					i, CPF::hip_to_string(m_vtPeerDct[i]->m_hlHostIp),
					"\"ARP_OK\"", "\"ECHO_CHECKING\""));
				break;

			case ARP_FAILED:
				break;

			case ECHO_CHECKING:
				if (m_nChkReplyNum <= m_vtPeerDct[i]->m_iHostTest.GetRspNum())
				{
					m_vtPeerDct[i]->m_nHostRecoverTimer += m_pTsCfg->m_iBackNet2Cfg.m_nDetectPeriod;
					if (m_vtPeerDct[i]->m_nHostRecoverTimer >= m_pTsCfg->m_iBackNet2Cfg.m_nRecoverTimeout)
					{
						m_vtPeerDct[i]->m_eHostStat = ECHO_OK;
						
						MY_ACE_DEBUG(m_pLogFile, (LM_ERROR,
							ACE_TEXT("[Info][%D]back net(%d@%s) connectivity changed. %s to %s\n"),
							i, CPF::hip_to_string(m_vtPeerDct[i]->m_hlHostIp),
							"\"ECHO_CHECKING\"", "\"ECHO_OK\""));
					}
				}
				else if (m_nChkLostNum <= m_vtPeerDct[i]->m_iHostTest.CheckTimeout(now_time, 0))
				{
					m_vtPeerDct[i]->m_eHostStat = ECHO_FAILED;
					m_vtPeerDct[i]->m_nHostRecoverTimer = 0;
										
					MY_ACE_DEBUG(m_pLogFile, (LM_ERROR,
						ACE_TEXT("[Info][%D]back net(%d@%s) connectivity changed. %s to %s\n"),
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
					m_vtPeerDct[i]->m_nHostRecoverTimer = 0;
					
					MY_ACE_DEBUG(m_pLogFile, (LM_ERROR,
						ACE_TEXT("[Info][%D]back net(%d@%s) connectivity changed. %s to %s\n"),
						i, CPF::hip_to_string(m_vtPeerDct[i]->m_hlHostIp),
						"\"ECHO_OK\"", "\"ECHO_FAILED\""));
				}
				f_SendEchoReq(m_vtPeerDct[i]->m_hlHostIp, m_vtPeerDct[i]->m_aHostMac);
				m_vtPeerDct[i]->m_iHostTest.OnSendOneRequest(m_nChkEchoSeq, now_time);
				break;

			case ECHO_FAILED:
				m_vtPeerDct[i]->m_nHostRecoverTimer += m_pTsCfg->m_iBackNet2Cfg.m_nDetectPeriod;
				if (60 <= m_vtPeerDct[i]->m_nHostRecoverTimer)//ARP老化
				{
					m_vtPeerDct[i]->m_eHostStat = ARP_CHECKING;
					
					memset(m_vtPeerDct[i]->m_aHostMac, 0, 6);
					
					MY_ACE_DEBUG(m_pLogFile, (LM_ERROR,
						ACE_TEXT("[Info][%D]back net(%d@%s) connectivity changed. %s to %s\n"),
						i, CPF::hip_to_string(m_vtPeerDct[i]->m_hlHostIp),
						"\"ECHO_FAILED\"", "\"ARP_CHECKING\""));
				}
				else
				{
					if (m_nChkReplyNum <= m_vtPeerDct[i]->m_iHostTest.GetRspNum())
					{
						m_vtPeerDct[i]->m_eHostStat = ECHO_CHECKING;
						m_vtPeerDct[i]->m_nHostRecoverTimer = 0;
						
						MY_ACE_DEBUG(m_pLogFile, (LM_ERROR,
							ACE_TEXT("[Info][%D]back net(%d@%s) connectivity changed. %s to %s\n"),
							i, CPF::hip_to_string(m_vtPeerDct[i]->m_hlHostIp),
							"\"ECHO_FAILED\"", "\"ECHO_CHECKING\""));
					}				
					f_SendEchoReq(m_vtPeerDct[i]->m_hlHostIp, m_vtPeerDct[i]->m_aHostMac);
					m_vtPeerDct[i]->m_iHostTest.OnSendOneRequest(m_nChkEchoSeq, now_time);
				}
				break;

			default:
				break;
		}
	}
}

CTS_BackNetEoL2::CTS_BackNetEoL2()
{
}

CTS_BackNetEoL2::~CTS_BackNetEoL2()
{
}

BOOL CTS_BackNetEoL2::f_ModInit(CTS_InterfaceCfg * pTsInterfaceCfg, CTS_CONFIG * pTsConfig, ACE_Log_File_Msg * pLogFile)
{
	m_pTsInterfaceCfg = pTsInterfaceCfg;
	m_pTsCfg = pTsConfig;
	m_pLogFile = pLogFile;

	// 备用网口使用的物理网口编号
	m_nIfIndex = (ACE_UINT32)EtherIO_GetOneIndexByShowName(pTsConfig->m_iBackNet2Cfg.m_strCardName.c_str());

	m_hlIp = pTsConfig->m_iBackNet2Cfg.m_hlIpAddr; 
	m_hlNetmask = pTsConfig->m_iBackNet2Cfg.m_hlNetmask;
	m_hlSubnet = m_hlIp & m_hlNetmask;
	
	EtherIO_GetDeviceMac(m_nIfIndex, m_aMacAddr);

	m_nChkPeriod = pTsConfig->m_iBackNet2Cfg.m_nDetectPeriod;
	m_nChkQueue = pTsConfig->m_iBackNet2Cfg.m_nDetectQueue;
	m_nChkLostNum = pTsConfig->m_iBackNet2Cfg.m_nSwitchLostThreshold;
	m_nChkReplyNum = pTsConfig->m_iBackNet2Cfg.m_nDetectQueue - pTsConfig->m_iBackNet2Cfg.m_nRecoverLostThreshold;

	m_piGatewayDct = NULL;

	// 远程主机监测表
	f_PeerInit(pTsConfig);

	f_FilterInit(pTsConfig);

	f_QueueInit(pTsConfig);

	pTsConfig->f_SetBackNet2Mgmt();

	return TRUE;
}

BOOL CTS_BackNetEoL2::f_ModUpdateCfg(CTS_CONFIG* pNewTsCfg)
{
	if (m_pTsCfg->m_iBackNet2Cfg.m_strCardName != pNewTsCfg->m_iBackNet2Cfg.m_strCardName)
	{
		MY_ACE_DEBUG(m_pLogFile, (LM_ERROR, ACE_TEXT("[Info][%D]system reboot: back net interface changed.\n")));
		ACE_OS::system("reboot -p");
		return FALSE;
	}

	if ((m_pTsCfg->m_iBackNet2Cfg.m_hlIpAddr != pNewTsCfg->m_iBackNet2Cfg.m_hlIpAddr)
		|| (m_pTsCfg->m_iBackNet2Cfg.m_hlNetmask != pNewTsCfg->m_iBackNet2Cfg.m_hlNetmask))
	{
		m_hlIp = pNewTsCfg->m_iBackNet2Cfg.m_hlIpAddr;
		m_hlNetmask = pNewTsCfg->m_iBackNet2Cfg.m_hlNetmask;
		m_hlSubnet = m_hlIp & m_hlNetmask;

		pNewTsCfg->f_SetBackNet2Mgmt();
	}
	
	if (m_pTsCfg->m_iBackNet2Cfg.m_hlGateway != pNewTsCfg->m_iBackNet2Cfg.m_hlGateway)
	{
		ACE_OS::system("reboot -p");
	}

	if (m_pTsCfg->m_iBackNet2Cfg.m_nDetectPeriod != pNewTsCfg->m_iBackNet2Cfg.m_nDetectPeriod)
	{
		m_nChkPeriod = pNewTsCfg->m_iBackNet2Cfg.m_nDetectPeriod;
	}

	if (m_pTsCfg->m_iBackNet2Cfg.m_nDetectQueue != pNewTsCfg->m_iBackNet2Cfg.m_nDetectQueue)
	{
		m_nChkQueue = pNewTsCfg->m_iBackNet2Cfg.m_nDetectQueue;
		
		size_t max = m_vtPeerDct.size();
		size_t i;
		for (i=0; i<max; i++)
		{
			m_vtPeerDct[i]->f_ResetTestNum(m_nChkQueue);
		}
	}
	
	if (m_pTsCfg->m_iBackNet2Cfg.m_nSwitchLostThreshold != pNewTsCfg->m_iBackNet2Cfg.m_nSwitchLostThreshold)
	{
		m_nChkLostNum = pNewTsCfg->m_iBackNet2Cfg.m_nSwitchLostThreshold;
	}

	if (m_pTsCfg->m_iBackNet2Cfg.m_nRecoverLostThreshold != pNewTsCfg->m_iBackNet2Cfg.m_nRecoverLostThreshold)
	{
		m_nChkReplyNum = pNewTsCfg->m_iBackNet2Cfg.m_nDetectQueue - pNewTsCfg->m_iBackNet2Cfg.m_nRecoverLostThreshold;
	}

	f_PeerUpdateCfg(pNewTsCfg);

	f_FilterUpdateCfg(pNewTsCfg);

	f_QueueUpdateCfg(pNewTsCfg);

	m_pTsCfg = pNewTsCfg;
	
	return TRUE;
}

BOOL CTS_BackNetEoL2::f_PeerInit(CTS_CONFIG* pTsConfig)
{
	size_t max = pTsConfig->m_vtPeerCfg.size();
	for (size_t i=0; i<max; i++)
	{
		CTS_HOST_DETECT* piBackNetChk = new CTS_HOST_DETECT;
		if (NULL == piBackNetChk)
		{
			return FALSE;
		}

		piBackNetChk->f_Init(pTsConfig->m_vtPeerCfg[i].m_nB2TestIp, m_nChkQueue);

		m_vtPeerDct.push_back(piBackNetChk);
	}

	return TRUE;
}

BOOL CTS_BackNetEoL2::f_PeerUpdateCfg(CTS_CONFIG* pNewTsCfg)
{
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
				if (m_pTsCfg->m_vtPeerCfg[i].m_nB2TestIp != pNewPeerCfg->m_nB2TestIp)
				{
					CTS_HOST_DETECT* pHostDetect = *iter;
					pHostDetect->f_ResetIp(pNewPeerCfg->m_nB2TestIp);
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
				CTS_HOST_DETECT* piBackNetChk = new CTS_HOST_DETECT;
				if (NULL == piBackNetChk)
				{
					return FALSE;
				}

				piBackNetChk->f_Init(pNewTsCfg->m_vtPeerCfg[i].m_nB2TestIp, m_nChkQueue);

				m_vtPeerDct.push_back(piBackNetChk);
			}
		}
	}

	return TRUE;
}

BOOL CTS_BackNetEoL2::f_FilterInit(CTS_CONFIG* pTsConfig)
{
	m_piPeerPktFilter = new CTS_PACKET_FILTER;
	if (NULL == m_piPeerPktFilter)
	{
		return FALSE;
	}
	
	size_t max = pTsConfig->m_iBackNet2Cfg.m_vtFilterId.size();
	for (size_t i=0; i<max; i++)
	{
		CTS_FILTER_CONFIG* pFilter = pTsConfig->f_FindFilter(pTsConfig->m_iBackNet2Cfg.m_vtFilterId[i]);
		if (NULL == pFilter)
		{
			return FALSE;
		}

		m_piPeerPktFilter->m_vtFilter.push_back(pFilter);
	}

	return TRUE;
}

BOOL CTS_BackNetEoL2::f_FilterUpdateCfg(CTS_CONFIG* pNewTsCfg)
{
	size_t nCurMax = m_pTsCfg->m_iBackNet2Cfg.m_vtFilterId.size();
	size_t nNewMax = pNewTsCfg->m_iBackNet2Cfg.m_vtFilterId.size();
	size_t i;

	if (nCurMax <= nNewMax)
	{
		for (i=0; i<nCurMax; i++)
		{
			CTS_FILTER_CONFIG* pFilter = pNewTsCfg->f_FindFilter(pNewTsCfg->m_iBackNet2Cfg.m_vtFilterId[i]);

			m_piPeerPktFilter->m_vtFilter[i] = pFilter;
		}

		for (i=nCurMax; i<nNewMax; i++)
		{
			CTS_FILTER_CONFIG* pFilter = pNewTsCfg->f_FindFilter(pNewTsCfg->m_iBackNet2Cfg.m_vtFilterId[i]);

			m_piPeerPktFilter->m_vtFilter.push_back(pFilter);
		}
	}
	else
	{
		for (i=0; i<nNewMax; i++)
		{
			CTS_FILTER_CONFIG* pFilter = pNewTsCfg->f_FindFilter(pNewTsCfg->m_iBackNet2Cfg.m_vtFilterId[i]);

			m_piPeerPktFilter->m_vtFilter[i] = pFilter;
		}

		m_piPeerPktFilter->m_vtFilter.erase(m_piPeerPktFilter->m_vtFilter.begin()+nNewMax, m_piPeerPktFilter->m_vtFilter.end());
	}

	return TRUE;
}

BOOL CTS_BackNetEoL2::f_QueueInit(CTS_CONFIG* pTsConfig)
{
	if (64 > pTsConfig->m_iGlobalCfg.m_nQueueSize)
	{
		return TRUE;
	}
	
	//每个对局使用独立队列
	size_t max = pTsConfig->m_vtPeerCfg.size();
	for (size_t i=0; i<max; i++)
	{
		CTS_PACKET_QUEUE* piPacketQueue = new CTS_PACKET_QUEUE;
		if (NULL == piPacketQueue)
		{
			return FALSE;
		}

		piPacketQueue->f_Init(pTsConfig->m_iGlobalCfg.m_nQueueSize);

		m_vtPeerPktQueue.push_back(piPacketQueue);
	}

	return TRUE;
}

BOOL CTS_BackNetEoL2::f_QueueUpdateCfg(CTS_CONFIG* pNewTsCfg)
{
	if (0 == m_vtPeerPktQueue.size())
	{
		return TRUE;
	}

	if (m_pTsCfg->m_vtPeerCfg != pNewTsCfg->m_vtPeerCfg)
	{
		size_t max = m_pTsCfg->m_vtPeerCfg.size();
		size_t i;

		std::vector<CTS_PACKET_QUEUE*>::iterator iter;

		for (i=0, iter=m_vtPeerPktQueue.begin(); i<max; i++)
		{
			CTS_PEER_CONFIG* pNewPeerCfg = pNewTsCfg->f_FindPeerCfg(m_pTsCfg->m_vtPeerCfg[i].m_nPeerId);
			if (NULL == pNewPeerCfg)
			{
				delete *iter;
				iter = m_vtPeerPktQueue.erase(iter);
			}
			else
			{
				if (m_pTsCfg->m_vtPeerCfg[i].m_vtPeerIpRange != pNewTsCfg->m_vtPeerCfg[i].m_vtPeerIpRange)
				{
					CTS_PACKET_QUEUE* pPacketQueue = *iter;
					pPacketQueue->f_ClearQueue();
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
				CTS_PACKET_QUEUE* piPacketQueue = new CTS_PACKET_QUEUE;
				if (NULL == piPacketQueue)
				{
					return FALSE;
				}

				piPacketQueue->f_Init(pNewTsCfg->m_iGlobalCfg.m_nQueueSize);

				m_vtPeerPktQueue.push_back(piPacketQueue);
			}
		}
	}

	return TRUE;
}

void CTS_BackNetEoL2::f_BackNetCheck(ACE_UINT32 now_time)
{
	m_nChkPeriod++;
	if (m_nChkPeriod < m_pTsCfg->m_iBackNet2Cfg.m_nDetectPeriod)
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
				m_vtPeerDct[i]->m_eHostStat = ARP_CHECKING;
				f_SendArpReq(m_vtPeerDct[i]->m_hlHostIp);
				
				MY_ACE_DEBUG(m_pLogFile, (LM_ERROR,
					ACE_TEXT("[Info][%D]back net(%d@%s) connectivity changed. %s to %s\n"),
					i, CPF::hip_to_string(m_vtPeerDct[i]->m_hlHostIp),
					"\"UNKNOWN\"", "\"ARP_CHECKING\""));
				break;

			case ARP_CHECKING:
				if (0 != memcmp(m_vtPeerDct[i]->m_aHostMac, "\0\0\0\0\0\0\0", 6))
				{
					m_vtPeerDct[i]->m_eHostStat = ARP_OK;
					
					MY_ACE_DEBUG(m_pLogFile, (LM_ERROR,
						ACE_TEXT("[Info][%D]back net(%d@%s) connectivity changed. %s to %s\n"),
						i, CPF::hip_to_string(m_vtPeerDct[i]->m_hlHostIp),
						"\"ARP_CHECKING\"", "\"ARP_OK\""));
				}
				else
				{
					f_SendArpReq(m_vtPeerDct[i]->m_hlHostIp);
				}
				break;

			case ARP_OK:
				m_vtPeerDct[i]->m_eHostStat = ECHO_CHECKING;
				m_vtPeerDct[i]->m_nHostRecoverTimer = 0;
				for (ACE_UINT32 j=0; j<m_nChkQueue; j++)
				{
					f_SendEchoReq(m_vtPeerDct[i]->m_hlHostIp, m_vtPeerDct[i]->m_aHostMac);
					m_vtPeerDct[i]->m_iHostTest.OnSendOneRequest(m_nChkEchoSeq, now_time);
				}
				
				MY_ACE_DEBUG(m_pLogFile, (LM_ERROR,
					ACE_TEXT("[Info][%D]back net(%d@%s) connectivity changed. %s to %s\n"),
					i, CPF::hip_to_string(m_vtPeerDct[i]->m_hlHostIp),
					"\"ARP_OK\"", "\"ECHO_CHECKING\""));
				break;

			case ARP_FAILED:
				break;

			case ECHO_CHECKING:
				if (m_nChkReplyNum <= m_vtPeerDct[i]->m_iHostTest.GetRspNum())
				{
					m_vtPeerDct[i]->m_nHostRecoverTimer += m_pTsCfg->m_iBackNet2Cfg.m_nDetectPeriod;
					if (m_vtPeerDct[i]->m_nHostRecoverTimer >= m_pTsCfg->m_iBackNet2Cfg.m_nRecoverTimeout)
					{
						m_vtPeerDct[i]->m_eHostStat = ECHO_OK;
						
						MY_ACE_DEBUG(m_pLogFile, (LM_ERROR,
							ACE_TEXT("[Info][%D]back net(%d@%s) connectivity changed. %s to %s\n"),
							i, CPF::hip_to_string(m_vtPeerDct[i]->m_hlHostIp),
							"\"ECHO_CHECKING\"", "\"ECHO_OK\""));
					}
				}
				else if (m_nChkLostNum <= m_vtPeerDct[i]->m_iHostTest.CheckTimeout(now_time, 0))
				{
					m_vtPeerDct[i]->m_eHostStat = ECHO_FAILED;
					m_vtPeerDct[i]->m_nHostRecoverTimer = 0;
					
					MY_ACE_DEBUG(m_pLogFile, (LM_ERROR,
						ACE_TEXT("[Info][%D]back net(%d@%s) connectivity changed. %s to %s\n"),
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
					m_vtPeerDct[i]->m_nHostRecoverTimer = 0;
					
					MY_ACE_DEBUG(m_pLogFile, (LM_ERROR,
						ACE_TEXT("[Info][%D]back net(%d@%s) connectivity changed. %s to %s\n"),
						i, CPF::hip_to_string(m_vtPeerDct[i]->m_hlHostIp),
						"\"ECHO_OK\"", "\"ECHO_FAILED\""));
				}
				f_SendEchoReq(m_vtPeerDct[i]->m_hlHostIp, m_vtPeerDct[i]->m_aHostMac);
				m_vtPeerDct[i]->m_iHostTest.OnSendOneRequest(m_nChkEchoSeq, now_time);
				break;

			case ECHO_FAILED:
				m_vtPeerDct[i]->m_nHostRecoverTimer += m_pTsCfg->m_iBackNet2Cfg.m_nDetectPeriod;
				if (60 <= m_vtPeerDct[i]->m_nHostRecoverTimer)//ARP老化
				{
					m_vtPeerDct[i]->m_eHostStat = ARP_CHECKING;
					
					memset(m_vtPeerDct[i]->m_aHostMac, 0, 6);
					
					MY_ACE_DEBUG(m_pLogFile, (LM_ERROR,
						ACE_TEXT("[Info][%D]back net(%d@%s) connectivity changed. %s to %s\n"),
						i, CPF::hip_to_string(m_vtPeerDct[i]->m_hlHostIp),
						"\"ECHO_FAILED\"", "\"ARP_CHECKING\""));
				}
				else
				{
					if (m_nChkReplyNum <= m_vtPeerDct[i]->m_iHostTest.GetRspNum())
					{
						m_vtPeerDct[i]->m_eHostStat = ECHO_CHECKING;
						m_vtPeerDct[i]->m_nHostRecoverTimer = 0;
						
						MY_ACE_DEBUG(m_pLogFile, (LM_ERROR,
							ACE_TEXT("[Info][%D]back net(%d@%s) connectivity changed. %s to %s\n"),
							i, CPF::hip_to_string(m_vtPeerDct[i]->m_hlHostIp),
							"\"ECHO_FAILED\"", "\"ECHO_CHECKING\""));
					}
					f_SendEchoReq(m_vtPeerDct[i]->m_hlHostIp, m_vtPeerDct[i]->m_aHostMac);
					m_vtPeerDct[i]->m_iHostTest.OnSendOneRequest(m_nChkEchoSeq, now_time);
				}
				break;

			default:
				break;
		}
	}
}

CTS_BackNetEoL3::CTS_BackNetEoL3()
{
}

CTS_BackNetEoL3::~CTS_BackNetEoL3()
{
}

BOOL CTS_BackNetEoL3::f_ModInit(CTS_InterfaceCfg * pTsInterfaceCfg, CTS_CONFIG * pTsConfig, ACE_Log_File_Msg * pLogFile)
{
	m_pTsInterfaceCfg = pTsInterfaceCfg;
	m_pTsCfg = pTsConfig;
	m_pLogFile = pLogFile;

	// 备用网口使用的物理网口编号
	m_nIfIndex = (ACE_UINT32)EtherIO_GetOneIndexByShowName(pTsConfig->m_iBackNet2Cfg.m_strCardName.c_str());

	m_hlIp = pTsConfig->m_iBackNet2Cfg.m_hlIpAddr; 
	m_hlNetmask = pTsConfig->m_iBackNet2Cfg.m_hlNetmask;
	m_hlSubnet = m_hlIp & m_hlNetmask;
	
	EtherIO_GetDeviceMac(m_nIfIndex, m_aMacAddr);

	m_nChkPeriod = pTsConfig->m_iBackNet2Cfg.m_nDetectPeriod;
	m_nChkQueue = pTsConfig->m_iBackNet2Cfg.m_nDetectQueue;
	m_nChkLostNum = pTsConfig->m_iBackNet2Cfg.m_nSwitchLostThreshold;
	m_nChkReplyNum = pTsConfig->m_iBackNet2Cfg.m_nDetectQueue - pTsConfig->m_iBackNet2Cfg.m_nRecoverLostThreshold;

	m_piGatewayDct = new CTS_HOST_DETECT;
	if (NULL == m_piGatewayDct)
	{
		return FALSE;
	}
	m_piGatewayDct->f_Init(pTsConfig->m_iBackNet2Cfg.m_hlGateway, 1);

	// 远程主机监测表
	f_PeerInit(pTsConfig);

	f_FilterInit(pTsConfig);

	f_QueueInit(pTsConfig);

	pTsConfig->f_SetBackNet2Mgmt();

	return TRUE;
}

BOOL CTS_BackNetEoL3::f_ModUpdateCfg(CTS_CONFIG* pNewTsCfg)
{
	if (m_pTsCfg->m_iBackNet2Cfg.m_strCardName != pNewTsCfg->m_iBackNet2Cfg.m_strCardName)
	{
		MY_ACE_DEBUG(m_pLogFile, (LM_ERROR, ACE_TEXT("[Info][%D]system reboot: back net interface changed.\n")));
		ACE_OS::system("reboot -p");
		return FALSE;
	}

	if ((m_pTsCfg->m_iBackNet2Cfg.m_hlIpAddr != pNewTsCfg->m_iBackNet2Cfg.m_hlIpAddr)
		|| (m_pTsCfg->m_iBackNet2Cfg.m_hlNetmask != pNewTsCfg->m_iBackNet2Cfg.m_hlNetmask))
	{
		m_hlIp = pNewTsCfg->m_iBackNet2Cfg.m_hlIpAddr;
		m_hlNetmask = pNewTsCfg->m_iBackNet2Cfg.m_hlNetmask;
		m_hlSubnet = m_hlIp & m_hlNetmask;

		pNewTsCfg->f_SetBackNet2Mgmt();
	}
	
	if (m_pTsCfg->m_iBackNet2Cfg.m_hlGateway != pNewTsCfg->m_iBackNet2Cfg.m_hlGateway)
	{
		if (0 == pNewTsCfg->m_iBackNet2Cfg.m_hlGateway)
		{
			ACE_OS::system("reboot -p");
		}
		
		m_piGatewayDct->f_ResetIp(pNewTsCfg->m_iBackNet2Cfg.m_hlGateway);
	}

	if (m_pTsCfg->m_iBackNet2Cfg.m_nDetectPeriod != pNewTsCfg->m_iBackNet2Cfg.m_nDetectPeriod)
	{
		m_nChkPeriod = pNewTsCfg->m_iBackNet2Cfg.m_nDetectPeriod;
	}

	if (m_pTsCfg->m_iBackNet2Cfg.m_nDetectQueue != pNewTsCfg->m_iBackNet2Cfg.m_nDetectQueue)
	{
		m_nChkQueue = pNewTsCfg->m_iBackNet2Cfg.m_nDetectQueue;
		
		size_t max = m_vtPeerDct.size();
		size_t i;
		for (i=0; i<max; i++)
		{
			m_vtPeerDct[i]->f_ResetTestNum(m_nChkQueue);
		}
	}
	
	if (m_pTsCfg->m_iBackNet2Cfg.m_nSwitchLostThreshold != pNewTsCfg->m_iBackNet2Cfg.m_nSwitchLostThreshold)
	{
		m_nChkLostNum = pNewTsCfg->m_iBackNet2Cfg.m_nSwitchLostThreshold;
	}

	if (m_pTsCfg->m_iBackNet2Cfg.m_nRecoverLostThreshold != pNewTsCfg->m_iBackNet2Cfg.m_nRecoverLostThreshold)
	{
		m_nChkReplyNum = pNewTsCfg->m_iBackNet2Cfg.m_nDetectQueue - pNewTsCfg->m_iBackNet2Cfg.m_nRecoverLostThreshold;
	}

	f_PeerUpdateCfg(pNewTsCfg);

	f_FilterUpdateCfg(pNewTsCfg);

	f_QueueUpdateCfg(pNewTsCfg);

	m_pTsCfg = pNewTsCfg;
	
	return TRUE;
}

BOOL CTS_BackNetEoL3::f_PeerInit(CTS_CONFIG* pTsConfig)
{
	size_t max = pTsConfig->m_vtPeerCfg.size();
	for (size_t i=0; i<max; i++)
	{
		CTS_HOST_DETECT* piBackNetChk = new CTS_HOST_DETECT;
		if (NULL == piBackNetChk)
		{
			return FALSE;
		}

		piBackNetChk->f_Init(pTsConfig->m_vtPeerCfg[i].m_nB2TestIp, m_nChkQueue);

		m_vtPeerDct.push_back(piBackNetChk);
	}

	return TRUE;
}

BOOL CTS_BackNetEoL3::f_PeerUpdateCfg(CTS_CONFIG* pNewTsCfg)
{
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
				if (m_pTsCfg->m_vtPeerCfg[i].m_nB2TestIp != pNewPeerCfg->m_nB2TestIp)
				{
					CTS_HOST_DETECT* pHostDetect = *iter;
					pHostDetect->f_ResetIp(pNewPeerCfg->m_nB2TestIp);
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
				CTS_HOST_DETECT* piBackNetChk = new CTS_HOST_DETECT;
				if (NULL == piBackNetChk)
				{
					return FALSE;
				}

				piBackNetChk->f_Init(pNewTsCfg->m_vtPeerCfg[i].m_nB2TestIp, m_nChkQueue);

				m_vtPeerDct.push_back(piBackNetChk);
			}
		}
	}

	return TRUE;
}

BOOL CTS_BackNetEoL3::f_FilterInit(CTS_CONFIG* pTsConfig)
{
	m_piPeerPktFilter = new CTS_PACKET_FILTER;
	if (NULL == m_piPeerPktFilter)
	{
		return FALSE;
	}
	
	size_t max = pTsConfig->m_iBackNet2Cfg.m_vtFilterId.size();
	for (size_t i=0; i<max; i++)
	{
		CTS_FILTER_CONFIG* pFilter = pTsConfig->f_FindFilter(pTsConfig->m_iBackNet2Cfg.m_vtFilterId[i]);
		if (NULL == pFilter)
		{
			return FALSE;
		}

		m_piPeerPktFilter->m_vtFilter.push_back(pFilter);
	}

	return TRUE;
}

BOOL CTS_BackNetEoL3::f_FilterUpdateCfg(CTS_CONFIG* pNewTsCfg)
{
	size_t nCurMax = m_pTsCfg->m_iBackNet2Cfg.m_vtFilterId.size();
	size_t nNewMax = pNewTsCfg->m_iBackNet2Cfg.m_vtFilterId.size();
	size_t i;

	if (nCurMax <= nNewMax)
	{
		for (i=0; i<nCurMax; i++)
		{
			CTS_FILTER_CONFIG* pFilter = pNewTsCfg->f_FindFilter(pNewTsCfg->m_iBackNet2Cfg.m_vtFilterId[i]);

			m_piPeerPktFilter->m_vtFilter[i] = pFilter;
		}

		for (i=nCurMax; i<nNewMax; i++)
		{
			CTS_FILTER_CONFIG* pFilter = pNewTsCfg->f_FindFilter(pNewTsCfg->m_iBackNet2Cfg.m_vtFilterId[i]);

			m_piPeerPktFilter->m_vtFilter.push_back(pFilter);
		}
	}
	else
	{
		for (i=0; i<nNewMax; i++)
		{
			CTS_FILTER_CONFIG* pFilter = pNewTsCfg->f_FindFilter(pNewTsCfg->m_iBackNet2Cfg.m_vtFilterId[i]);

			m_piPeerPktFilter->m_vtFilter[i] = pFilter;
		}

		m_piPeerPktFilter->m_vtFilter.erase(m_piPeerPktFilter->m_vtFilter.begin()+nNewMax, m_piPeerPktFilter->m_vtFilter.end());
	}

	return TRUE;
}

BOOL CTS_BackNetEoL3::f_QueueInit(CTS_CONFIG* pTsConfig)
{
	if (64 > pTsConfig->m_iGlobalCfg.m_nQueueSize)
	{
		return TRUE;
	}
	
	//每个对局使用独立队列
	size_t max = pTsConfig->m_vtPeerCfg.size();
	for (size_t i=0; i<max; i++)
	{
		CTS_PACKET_QUEUE* piPacketQueue = new CTS_PACKET_QUEUE;
		if (NULL == piPacketQueue)
		{
			return FALSE;
		}

		piPacketQueue->f_Init(pTsConfig->m_iGlobalCfg.m_nQueueSize);

		m_vtPeerPktQueue.push_back(piPacketQueue);
	}

	return TRUE;
}

BOOL CTS_BackNetEoL3::f_QueueUpdateCfg(CTS_CONFIG* pNewTsCfg)
{
	if (0 == m_vtPeerPktQueue.size())
	{
		return TRUE;
	}

	if (m_pTsCfg->m_vtPeerCfg != pNewTsCfg->m_vtPeerCfg)
	{
		size_t max = m_pTsCfg->m_vtPeerCfg.size();
		size_t i;

		std::vector<CTS_PACKET_QUEUE*>::iterator iter;

		for (i=0, iter=m_vtPeerPktQueue.begin(); i<max; i++)
		{
			CTS_PEER_CONFIG* pNewPeerCfg = pNewTsCfg->f_FindPeerCfg(m_pTsCfg->m_vtPeerCfg[i].m_nPeerId);
			if (NULL == pNewPeerCfg)
			{
				delete *iter;
				iter = m_vtPeerPktQueue.erase(iter);
			}
			else
			{
				if (m_pTsCfg->m_vtPeerCfg[i].m_vtPeerIpRange != pNewTsCfg->m_vtPeerCfg[i].m_vtPeerIpRange)
				{
					CTS_PACKET_QUEUE* pPacketQueue = *iter;
					pPacketQueue->f_ClearQueue();
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
				CTS_PACKET_QUEUE* piPacketQueue = new CTS_PACKET_QUEUE;
				if (NULL == piPacketQueue)
				{
					return FALSE;
				}

				piPacketQueue->f_Init(pNewTsCfg->m_iGlobalCfg.m_nQueueSize);

				m_vtPeerPktQueue.push_back(piPacketQueue);
			}
		}
	}

	return TRUE;
}

BOOL CTS_BackNetEoL3::f_HandleArp(PACKET_INFO_EX& packet_info)
{
	if (packet_info.m_arp_decode_info.senderNetOrderIPAddr == TS_HTONL(m_piGatewayDct->m_hlHostIp))
	{
		// 更新网关的MAC
		if (0 != memcmp(packet_info.m_arp_decode_info.senderMacAddr, m_piGatewayDct->m_aHostMac, 6))
		{
			unsigned char* pOldMac = m_piGatewayDct->m_aHostMac;
			unsigned char* pNewMac = packet_info.m_arp_decode_info.senderMacAddr;
			MY_ACE_DEBUG(m_pLogFile,
				(LM_ERROR,
				ACE_TEXT("[Info][%D]back net gateway changed. %02x-%02x-%02x-%02x-%02x-%02x to %02x-%02x-%02x-%02x-%02x-%02x\n"),
				pOldMac[0], pOldMac[1], pOldMac[2], pOldMac[3], pOldMac[4], pOldMac[5],
				pNewMac[0], pNewMac[1], pNewMac[2], pNewMac[3], pNewMac[4], pNewMac[5]
				));
			
			memcpy(m_piGatewayDct->m_aHostMac, packet_info.m_arp_decode_info.senderMacAddr, 6);
		}
		m_piGatewayDct->m_nHostRecoverTimer = 0;//老化时间归零
	}

	//应答ARP请求(代理ARP)
	if (1 == packet_info.m_arp_decode_info.operation)
	{
		packet_info.packet->nPktlen = 
		CConstructPacket::MAC_Construct_Arp_Rsp_Packet
		(
			(BYTE *)(packet_info.packet->pPacket), 
			m_aMacAddr,
			packet_info.m_arp_decode_info.destNetOrderIPAddr, 
			packet_info.m_arp_decode_info.senderNetOrderIPAddr,
			packet_info.m_arp_decode_info.senderMacAddr,
			-1
		);

		m_pTsInterfaceCfg->GetWriteDataStream()->WritePacket
		(
			PACKET_OK,
			(const char *)(packet_info.packet->pPacket),
			packet_info.packet->nPktlen,
			(1 << m_nIfIndex),
			1
		);
	}

	return TRUE;
}

BOOL CTS_BackNetEoL3::f_HandleIp(PACKET_INFO_EX& packet_info)
{
	if (packet_info.tcpipContext.conn_addr.server.dwIP == m_hlIp)
	{
		if (packet_info.tcpipContext.dwIndexProtocl == CCommonIPv4Decode::INDEX_PID_IPv4_ICMP)
		{
			f_HandleMyIcmp(packet_info);
		}

		return TRUE;
	}
	
	//备网接口收到本厂家的MAC，不向主网扩散
	ACE_UINT8* pSrcMac = (ACE_UINT8*)(packet_info.packet->pPacket) + 6;
	if ((pSrcMac[0] == m_aMacAddr[0]) && (pSrcMac[1] == m_aMacAddr[1]) && (pSrcMac[2] == m_aMacAddr[2]))
	{
		return TRUE;
	}

	/*if (TRUE == f_IsSubNetIp(packet_info.tcpipContext.conn_addr.server.dwIP))
	{
		return TRUE;
	}*/

	return FALSE;
}

void CTS_BackNetEoL3::f_PeerDetect(ACE_UINT32 now_time)
{
	m_nChkPeriod++;
	if (m_nChkPeriod < m_pTsCfg->m_iBackNet2Cfg.m_nDetectPeriod)
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
						f_SendEchoReq(m_vtPeerDct[i]->m_hlHostIp, m_piGatewayDct->m_aHostMac);
						m_vtPeerDct[i]->m_iHostTest.OnSendOneRequest(m_nChkEchoSeq, now_time);			
					}
					
					MY_ACE_DEBUG(m_pLogFile, (LM_ERROR,
						ACE_TEXT("[Info][%D]back net(%d@%s) connectivity changed. %s to %s\n"),
						i, CPF::hip_to_string(m_vtPeerDct[i]->m_hlHostIp),
						"\"UNKNOWN\"", "\"ECHO_CHECKING\""));
				}
				break;

			case ECHO_CHECKING:
				if (m_nChkReplyNum <= m_vtPeerDct[i]->m_iHostTest.GetRspNum())
				{
					m_vtPeerDct[i]->m_nHostRecoverTimer += m_pTsCfg->m_iBackNet2Cfg.m_nDetectPeriod;
					if (m_vtPeerDct[i]->m_nHostRecoverTimer >= m_pTsCfg->m_iBackNet2Cfg.m_nRecoverTimeout)
					{
						m_vtPeerDct[i]->m_eHostStat = ECHO_OK;
						
						MY_ACE_DEBUG(m_pLogFile, (LM_ERROR,
							ACE_TEXT("[Info][%D]back net(%d@%s) connectivity changed. %s to %s\n"),
							i, CPF::hip_to_string(m_vtPeerDct[i]->m_hlHostIp),
							"\"ECHO_CHECKING\"", "\"ECHO_OK\""));
					}
				}
				else if (m_nChkLostNum <= m_vtPeerDct[i]->m_iHostTest.CheckTimeout(now_time, 0))
				{
					m_vtPeerDct[i]->m_eHostStat = ECHO_FAILED;
					//m_vtPeerDct[i]->m_nHostRecoverTimer = 0;
					
					MY_ACE_DEBUG(m_pLogFile, (LM_ERROR,
						ACE_TEXT("[Info][%D]back net(%d@%s) connectivity changed. %s to %s\n"),
						i, CPF::hip_to_string(m_vtPeerDct[i]->m_hlHostIp),
						"\"ECHO_CHECKING\"", "\"ECHO_FAILED\""));
				}
				f_SendEchoReq(m_vtPeerDct[i]->m_hlHostIp, m_piGatewayDct->m_aHostMac);
				m_vtPeerDct[i]->m_iHostTest.OnSendOneRequest(m_nChkEchoSeq, now_time);
				break;

			case ECHO_OK:
				if (m_nChkLostNum <= m_vtPeerDct[i]->m_iHostTest.CheckTimeout(now_time, 0))
				{
					m_vtPeerDct[i]->m_eHostStat = ECHO_FAILED;
					//m_vtPeerDct[i]->m_nHostRecoverTimer = 0;
					
					MY_ACE_DEBUG(m_pLogFile, (LM_ERROR,
						ACE_TEXT("[Info][%D]back net(%d@%s) connectivity changed. %s to %s\n"),
						i, CPF::hip_to_string(m_vtPeerDct[i]->m_hlHostIp),
						"\"ECHO_OK\"", "\"ECHO_FAILED\""));
				}
				f_SendEchoReq(m_vtPeerDct[i]->m_hlHostIp, m_piGatewayDct->m_aHostMac);
				m_vtPeerDct[i]->m_iHostTest.OnSendOneRequest(m_nChkEchoSeq, now_time);
				break;

			case ECHO_FAILED:
				if (m_nChkReplyNum <= m_vtPeerDct[i]->m_iHostTest.GetRspNum())
				{
					m_vtPeerDct[i]->m_eHostStat = ECHO_CHECKING;
					m_vtPeerDct[i]->m_nHostRecoverTimer = 0;
					
					MY_ACE_DEBUG(m_pLogFile, (LM_ERROR,
						ACE_TEXT("[Info][%D]back net(%d@%s) connectivity changed. %s to %s\n"),
						i, CPF::hip_to_string(m_vtPeerDct[i]->m_hlHostIp),
						"\"ECHO_FAILED\"", "\"ECHO_CHECKING\""));
				}
				f_SendEchoReq(m_vtPeerDct[i]->m_hlHostIp, m_piGatewayDct->m_aHostMac);
				m_vtPeerDct[i]->m_iHostTest.OnSendOneRequest(m_nChkEchoSeq, now_time);
				break;

			default:
				break;
		}
	}
}

void CTS_BackNetEoL3::f_BackNetCheck(ACE_UINT32 now_time)
{
	switch(m_piGatewayDct->m_eHostStat)
	{
		case UNKNOWN:
			char str[100];
			for (size_t i=0; i<m_vtPeerDct.size(); i++)
			{
				m_vtPeerDct[i]->m_eHostStat = UNKNOWN;
				ACE_OS::sprintf(str, "route add -host %d.%d.%d.%d gw %d.%d.%d.%d dev %s ", 
					(m_vtPeerDct[i]->m_hlHostIp>>24)&0xFF, 
					(m_vtPeerDct[i]->m_hlHostIp>>16)&0xFF, 
					(m_vtPeerDct[i]->m_hlHostIp>>8)&0xFF, 
					(m_vtPeerDct[i]->m_hlHostIp)&0xFF, 
					(m_piGatewayDct->m_hlHostIp>>24)&0xFF, 
					(m_piGatewayDct->m_hlHostIp>>16)&0xFF, 
					(m_piGatewayDct->m_hlHostIp>>8)&0xFF, 
					(m_piGatewayDct->m_hlHostIp)&0xFF, 
					m_pTsCfg->m_iBackNet2Cfg.m_strCardName.c_str());
				ACE_OS::system(str);
			}
			
			m_piGatewayDct->m_eHostStat = ARP_CHECKING;
			f_SendArpReq(m_piGatewayDct->m_hlHostIp);
				
			MY_ACE_DEBUG(m_pLogFile, (LM_ERROR, ACE_TEXT("[Info][%D]back net gateway changed. %s to %s\n"), "\"UNKNOWN\"", "\"ARP_CHECKING\"") );
			break;

		case ARP_CHECKING:			
			if (0 != memcmp(m_piGatewayDct->m_aHostMac, "\0\0\0\0\0\0\0", 6))
			{
				m_piGatewayDct->m_eHostStat = ARP_OK;
				m_piGatewayDct->m_nHostRecoverTimer = 0;
				MY_ACE_DEBUG(m_pLogFile, (LM_ERROR, ACE_TEXT("[Info][%D]back net gateway changed. %s to %s\n"), "\"ARP_CHECKING\"", "\"ARP_OK\"") );
			}
			else
			{
				f_SendArpReq(m_piGatewayDct->m_hlHostIp);
			}
			break;

		case ARP_OK:
			m_piGatewayDct->m_nHostRecoverTimer++;
			if (300 <= m_piGatewayDct->m_nHostRecoverTimer)//老化
			{
				m_piGatewayDct->m_eHostStat = UNKNOWN;
				memset(m_piGatewayDct->m_aHostMac, 0, 6);
				MY_ACE_DEBUG(m_pLogFile, (LM_ERROR, ACE_TEXT("[Info][%D]back net gateway changed. %s to %s\n"), "\"ARP_OK\"", "\"UNKNOWN\"") );
			}
			else if (295 <= m_piGatewayDct->m_nHostRecoverTimer)
			{
				f_SendArpReq(m_piGatewayDct->m_hlHostIp);
			}
			break;

		default:
			break;
	}

	if (ARP_OK == m_piGatewayDct->m_eHostStat)
	{
		f_PeerDetect(now_time);
	}
}

