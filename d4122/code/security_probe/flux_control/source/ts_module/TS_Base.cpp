///////////////////////////////////////////////////////////////
//TS_Base.cpp

#include "stdafx.h"
#include "TS_Base.h"

BOOL f_cmp_srcip_with_filter(ACE_UINT32 ip, const CTS_FILTER_CONFIG  *p_filter)
{
	size_t ii_max = p_filter->src_ip_rang.size();
	if (0 == ii_max)
	{
		return TRUE;
	}
	else
	{
		for (size_t ii=0; ii<ii_max; ii++)
		{
			if ((ip >= p_filter->src_ip_rang[ii].first) && (ip <= p_filter->src_ip_rang[ii].second))
			{
				return TRUE;
			}
		}
	}

	return FALSE;
}

BOOL f_cmp_dstip_with_filter(ACE_UINT32 ip, const CTS_FILTER_CONFIG  *p_filter)
{
	size_t ii_max = p_filter->dst_ip_rang.size();
	if (0 == ii_max)
	{
		return TRUE;
	}
	else
	{
		for (size_t ii=0; ii<ii_max; ii++)
		{
			if ((ip >= p_filter->dst_ip_rang[ii].first) && (ip <= p_filter->dst_ip_rang[ii].second))
			{
				return TRUE;
			}
		}
	}

	return FALSE;
}

BOOL f_cmp_port_with_filter(ACE_UINT16 port, const CTS_FILTER_CONFIG  *p_filter)
{
	size_t ii_max = p_filter->port.size();
	if (0 == ii_max)
	{
		return TRUE;
	}
	else
	{
		for (size_t ii=0; ii<ii_max; ii++)
		{
			if ((port >= p_filter->port[ii].first) && (port <= p_filter->port[ii].second))
			{
				return TRUE;
			}
		}
	}

	return FALSE;
}

//匹配报文与过滤器
BOOL f_cmp_pkt_with_filter(PACKET_INFO_EX& packet_info, const CTS_FILTER_CONFIG  *p_filter)
{
	//源IP
	if ((FALSE == f_cmp_srcip_with_filter(packet_info.tcpipContext.conn_addr.client.dwIP, p_filter))
		&& (FALSE == f_cmp_srcip_with_filter(packet_info.tcpipContext.conn_addr.server.dwIP, p_filter)))
	{
		return FALSE;
	}

	//目的IP
	if ((FALSE == f_cmp_dstip_with_filter(packet_info.tcpipContext.conn_addr.client.dwIP, p_filter))
		&& (FALSE == f_cmp_dstip_with_filter(packet_info.tcpipContext.conn_addr.server.dwIP, p_filter)))
	{
		return FALSE;
	}

	//传输层协议
	if (0xFF != p_filter->protocol_id)
	{
		if (packet_info.tcpipContext.dwIndexProtocl != p_filter->protocol_id)
		{
			return FALSE;
		}
	}

	//端口号(针对TCP和UDP)
	if ((CCommonIPv4Decode::INDEX_PID_IPv4_TCP == packet_info.tcpipContext.dwIndexProtocl)
		|| (CCommonIPv4Decode::INDEX_PID_IPv4_UDP == packet_info.tcpipContext.dwIndexProtocl))
	{
		if ((FALSE == f_cmp_port_with_filter(packet_info.tcpipContext.conn_addr.client.wdPort, p_filter))
			&&(FALSE == f_cmp_port_with_filter(packet_info.tcpipContext.conn_addr.server.wdPort, p_filter)))
		{
			return FALSE;
		}
	}

	return TRUE;
}

CTS_HOST_DETECT::CTS_HOST_DETECT()
{
}

CTS_HOST_DETECT::~CTS_HOST_DETECT()
{
	m_iHostTest.close();
}

BOOL CTS_HOST_DETECT::f_Init(ACE_UINT32 nHostIp, ACE_UINT32 nTestNum)
{
	m_hlHostIp = nHostIp;
	m_eHostStat = UNKNOWN;
	memset(m_aHostMac, 0, 8);
	m_nHostRecoverTimer = 0;
	m_iHostTest.init(1, nTestNum);

	return TRUE;
}

void CTS_HOST_DETECT::f_ResetIp(ACE_UINT32 nHostIp)
{
	m_hlHostIp = nHostIp;
	m_eHostStat = UNKNOWN;
	memset(m_aHostMac, 0, 8);
	m_nHostRecoverTimer = 0;
}

void CTS_HOST_DETECT::f_ResetTestNum(ACE_UINT32 nTestNum)
{
	m_iHostTest.close();
	m_iHostTest.init(1, nTestNum);
}

CTS_PACKET_FILTER::CTS_PACKET_FILTER()
{
	m_vtFilter.clear();
}

CTS_PACKET_FILTER::~CTS_PACKET_FILTER()
{
	m_vtFilter.clear();
}

BOOL CTS_PACKET_FILTER::f_ApplyFilter(PACKET_INFO_EX& packet_info)
{
	size_t max = m_vtFilter.size();
	for (size_t i=0; i<max; i++)
	{
		if (TRUE == f_cmp_pkt_with_filter(packet_info, m_vtFilter[i]))
		{
			return TRUE;
		}
	}

	return FALSE;
}

CTS_PACKET_QUEUE::CTS_PACKET_QUEUE()
{
	m_pBuf = NULL;
	memset(&m_tagHandle,0x00,sizeof(TAG_COMMOM_QUEUE_MANAGER));
}

CTS_PACKET_QUEUE::~CTS_PACKET_QUEUE()
{
	if (m_pBuf != NULL)
	{ 
		QueueUnInitialize(&m_tagHandle);
		free(m_pBuf); 
	}
	m_pBuf = NULL;
}

BOOL CTS_PACKET_QUEUE::f_Init(unsigned int buffer_size)
{
	m_pBuf = (unsigned char *)malloc(buffer_size);
	if (m_pBuf == NULL)
	{ 
		return FALSE; 
	}

	memset(m_pBuf, 0, buffer_size);

	// 初始化队列
	QueueInitialize_new(&m_tagHandle, m_pBuf, buffer_size, 0);
	
	return TRUE;
}

BOOL CTS_PACKET_QUEUE::f_ClearQueue(void)
{
	QueueClearQueue(&m_tagHandle);
	
	return TRUE;
}



