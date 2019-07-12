#include "stdafx.h"
#include "TS_BackNetL3.h"
#include "cpf/ConstructPacket.h"
#include "PacketIO/WriteDataStream.h"
#include "FluxControlObserver.h"

CTS_BackNetL3::CTS_BackNetL3()
{
}

CTS_BackNetL3::~CTS_BackNetL3()
{	
}

BOOL CTS_BackNetL3::f_ForwardIp(PACKET_INFO_EX& packet_info, ACE_UINT32 des_index)
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
	
	memcpy(packet_info.packet->pPacket, m_vtPeerDct[des_index]->m_aHostMac, 6);  // 目的MAC
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

BOOL CTS_BackNetL3::f_MulticastIp(PACKET_INFO_EX& packet_info)
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

BOOL CTS_BackNetL3::f_ForwardArp(PACKET_INFO_EX& packet_info, ACE_UINT32 des_index)
{
	/*if (ECHO_OK != m_vtPeerDct[des_index]->m_eHostStat)
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
	);*/

	return TRUE;
}

// 加入队列
BOOL CTS_BackNetL3::f_PushPktQueue(PACKET_INFO_EX& packet_info, ACE_UINT32 des_index)
{
	if (0 == m_vtPeerPktQueue.size())
	{
		return FALSE;
	}
	
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
BOOL CTS_BackNetL3::f_FlushPktQueue(ACE_UINT32 des_index)
{
	if (0 == m_vtPeerPktQueue.size())
	{
		return FALSE;
	}
	
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

CTS_BackNetL3EoSgl1::CTS_BackNetL3EoSgl1()
{
}

CTS_BackNetL3EoSgl1::~CTS_BackNetL3EoSgl1()
{	
}


CTS_BackNetL3EoSgl2::CTS_BackNetL3EoSgl2()
{
}

CTS_BackNetL3EoSgl2::~CTS_BackNetL3EoSgl2()
{	
}

CTS_BackNetL3EoL2::CTS_BackNetL3EoL2()
{
}

CTS_BackNetL3EoL2::~CTS_BackNetL3EoL2()
{
}

CTS_BackNetL3EoL3::CTS_BackNetL3EoL3()
{
}

CTS_BackNetL3EoL3::~CTS_BackNetL3EoL3()
{
}

