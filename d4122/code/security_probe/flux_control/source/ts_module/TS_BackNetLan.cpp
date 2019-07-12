#include "stdafx.h"
#include "TS_BackNetLan.h"
#include "cpf/ConstructPacket.h"
#include "PacketIO/WriteDataStream.h"
#include "FluxControlObserver.h"

CTS_BackNetLan::CTS_BackNetLan()
{
	m_iBroadcastArpQueue.f_Init(16000);
}

CTS_BackNetLan::~CTS_BackNetLan()
{	
}

BOOL CTS_BackNetLan::f_ForwardIp(PACKET_INFO_EX& packet_info, ACE_UINT32 des_index)
{
	/*if (ECHO_OK != m_vtPeerDct[des_index]->m_eHostStat)
	{ 
		return FALSE; 
	}*/

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

BOOL CTS_BackNetLan::f_MulticastIp(PACKET_INFO_EX& packet_info)
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

BOOL CTS_BackNetLan::f_ForwardArp(PACKET_INFO_EX& packet_info, ACE_UINT32 des_index)
{
	/*if (ECHO_OK != m_vtPeerDct[des_index]->m_eHostStat)
	{ 
		return FALSE; 
	}*/

	/*if (-1 != packet_info.tcpipContext.vlanId)//去掉VALN字段
	{
		packet_info.packet->pPacket = (unsigned char*)(packet_info.packet->pPacket) + 4;
		packet_info.packet->nPktlen -= 4;
	}*/
	
	//memcpy(packet_info.packet->pPacket, m_vtPeerDct[des_index]->m_aHostMac, 6);  // 目的MAC
	//memcpy(packet_info.packet->pPacket+6, m_aMacAddr, 6);   // 源MAC
	
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
BOOL CTS_BackNetLan::f_PushPktQueue(PACKET_INFO_EX& packet_info, ACE_UINT32 des_index)
{
	void *pBuf;
	unsigned char *rd_pt;
	unsigned int delete_count=0;

	if (CCommonEtherDecode::MAC_DECODE_INFO::IsIP(packet_info.tcpipContext.dwProtoOverMac) == TRUE)
	{
		// 业务是否允许
		if (0 == m_vtPeerPktQueue.size())
		{
			return FALSE;
		}
		
		if (TRUE != m_piPeerPktFilter->f_ApplyFilter(packet_info))
		{ 
			return TRUE; 
		}
		
		pBuf = QueueWriteAllocateBuffer(&(m_vtPeerPktQueue[des_index]->m_tagHandle), packet_info.packet->nPktlen);
		while (NULL == pBuf)
		{
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

		/*if (-1 != packet_info.tcpipContext.vlanId)//去掉VALN字段
		{
			packet_info.packet->pPacket = (unsigned char*)(packet_info.packet->pPacket) + 4;
			packet_info.packet->nPktlen -= 4;
		}*/

		memcpy(pBuf, packet_info.packet->pPacket, packet_info.packet->nPktlen);
		QueueWriteDataFinished(&(m_vtPeerPktQueue[des_index]->m_tagHandle), packet_info.packet->nPktlen);
	}
	else if (CCommonEtherDecode::MAC_DECODE_INFO::IsARP(packet_info.tcpipContext.dwProtoOverMac) == TRUE)
	{
		/*if (*(ACE_UINT8*)(packet_info.packet->pPacket) != 0xFF)
		{
			return TRUE;
		}*/
		
		pBuf = QueueWriteAllocateBuffer(&(m_iBroadcastArpQueue.m_tagHandle), packet_info.packet->nPktlen);
		while (NULL == pBuf)
		{
			if(QueueReadNextPacket(&(m_iBroadcastArpQueue.m_tagHandle), &rd_pt) == 0)
			{ 
				return(FALSE); 
			}  // 去掉一个包
			delete_count++;
			if(delete_count > 2)
			{ 
				return(FALSE); 
			}
			pBuf = QueueWriteAllocateBuffer(&(m_iBroadcastArpQueue.m_tagHandle), packet_info.packet->nPktlen);
		}
		
		memcpy(pBuf, packet_info.packet->pPacket, packet_info.packet->nPktlen);
		QueueWriteDataFinished(&(m_iBroadcastArpQueue.m_tagHandle), packet_info.packet->nPktlen);
	}

	return TRUE;
}

//释放缓存队列
BOOL CTS_BackNetLan::f_FlushPktQueue(ACE_UINT32 des_index)
{
	unsigned char *cur_send_pt = NULL;
	int cur_send_size = 0;

	//释放广播ARP报文
	do
	{
		cur_send_size = QueueReadNextPacket(&(m_iBroadcastArpQueue.m_tagHandle), &cur_send_pt);
		if((cur_send_pt != NULL) && (cur_send_size > 0))
		{
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

	//释放IP报文
	if (0 == m_vtPeerPktQueue.size())
	{
		return FALSE;
	}

	if (ECHO_OK != m_vtPeerDct[des_index]->m_eHostStat)
	{ 
		return FALSE; 
	}

	do
	{
		cur_send_size = QueueReadNextPacket(&(m_vtPeerPktQueue[des_index]->m_tagHandle), &cur_send_pt);
		if((cur_send_pt != NULL) && (cur_send_size > 0))
		{
			//memcpy((void *)(cur_send_pt), m_vtPeerDct[des_index]->m_aHostMac, 6);  // 目的MAC
			//memcpy((void *)(cur_send_pt+6), m_aMacAddr, 6);   // 源MAC

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
}

CTS_BackNetLanEoSgl1::CTS_BackNetLanEoSgl1()
{
}

CTS_BackNetLanEoSgl1::~CTS_BackNetLanEoSgl1()
{	
}


CTS_BackNetLanEoSgl2::CTS_BackNetLanEoSgl2()
{
}

CTS_BackNetLanEoSgl2::~CTS_BackNetLanEoSgl2()
{	
}

CTS_BackNetLanEoL2::CTS_BackNetLanEoL2()
{
}

CTS_BackNetLanEoL2::~CTS_BackNetLanEoL2()
{
}

