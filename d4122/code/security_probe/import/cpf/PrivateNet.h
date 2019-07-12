//////////////////////////////////////////////////////////////////////////
//PrivateNet.h

#pragma once

#include "cpf/cpf.h"
#include "cpf/addr_tools.h"
#include "cpf/InetAddress.h"
#include "ace/Null_Mutex.h"
#include "ace/Singleton.h"

//缺省的私网网段配置
//[private_netseg]
//private_netseg_05=172.224.0.0:172.255.255.255:255.255.255.0
//private_netseg_04=172.16.0.0:172.31.255.255:255.255.255.0
//private_netseg_03=169.254.0.0:169.254.255.255:255.255.255.0
//private_netseg_02=10.0.0.0:10.255.255.255:255.255.255.0
//private_netseg_01=192.168.0.0:192.168.255.255:255.255.255.0

class CPF_CLASS CPrivateNetRange
{
public:
	CPrivateNetRange(ACE_UINT32 netorder_begin_ip,ACE_UINT32 netorder_end_ip,ACE_UINT32 netorder_mask)
	{
		m_beginIP = netorder_begin_ip;
		m_endIP = netorder_end_ip;
		m_Mask = netorder_mask;
	}

	CPrivateNetRange(const CPrivateNetRange& other)
	{
		*this = other;
	}

	~CPrivateNetRange(void)
	{

	}

	CPrivateNetRange& operator = (const CPrivateNetRange& other)
	{
		if( this != &other )
		{
			m_beginIP = other.m_beginIP;
			m_endIP = other.m_endIP;
			m_Mask = other.m_Mask;
		}

		return *this;
	}

	int operator == (const CPrivateNetRange& other ) const
	{
		return ((m_beginIP == other.m_beginIP)
			&& (m_endIP == other.m_endIP)
			&& (m_Mask == other.m_Mask) );
	}


	int operator != (const CPrivateNetRange& other ) const
	{
		return !(*this == other);
	}

	BOOL find(ACE_UINT32 netorder_ip) const
	{
		ACE_UINT32 hostorder_ip = ACE_NTOHL(netorder_ip);

		return (hostorder_ip >= m_beginIP.GetHostOrderIP() && hostorder_ip <= m_endIP.GetHostOrderIP());
	}


	ACE_UINT32 GetNetOrderMask() const
	{
		return m_Mask.GetNetOrderIP();
	} 

	ACE_UINT32 GetHostOrderMask() const
	{
		return ACE_NTOHL(m_Mask.GetNetOrderIP());
	} 


	BOOL IsInSameNetSeg(ACE_UINT32 netorder_ip1,ACE_UINT32 netorder_ip2) const
	{
		return ((netorder_ip1&GetNetOrderMask()) == (netorder_ip2&GetNetOrderMask()));
	}


public:
	InetAddress	m_beginIP;
	InetAddress	m_endIP;
	InetAddress	m_Mask;
};


class CPF_CLASS CPrivateNetList
{
public:
	CPrivateNetList()
	{
		AddDefaultPrivateNetRange();

	}
	~CPrivateNetList()
	{
		RemoveAll();
	}

	CPrivateNetList& operator = (const CPrivateNetList& other)
	{
		if(this != &other )
		{
			m_vtPrivateNet = other.m_vtPrivateNet;
		}

		return *this;
	}

	int operator == (const CPrivateNetList& other) const
	{
		if( this->m_vtPrivateNet.size() != other.m_vtPrivateNet.size() )
			return false;
		
		for(size_t i = 0; i < m_vtPrivateNet.size(); ++i)
		{
			if( m_vtPrivateNet[i] != other.m_vtPrivateNet[i] )
				return false;
		}
	
		return true;
	}

	int operator != (const CPrivateNetList& other) const
	{
		return !(*this == other);
	}

	size_t GetCount() const
	{
		return m_vtPrivateNet.size();
	}

	void SetToDefaultPrivateNetRange();
	BOOL AddDefaultPrivateNetRange();
	void RemoveAll();

	BOOL AddPrivateNetRange(ACE_UINT32 netorder_begin_ip,ACE_UINT32 netorder_end_ip,ACE_UINT32 netorder_mask);

	const CPrivateNetRange * find(DWORD dwNetOrderIP) const
	{
		for(size_t i = 0; i < m_vtPrivateNet.size(); ++i)
		{
			if(m_vtPrivateNet[i].find(dwNetOrderIP))
			{
				return &m_vtPrivateNet[i];
			}
		}

		return NULL;
	}

	const CPrivateNetRange * IsInSamePrivetNetRange(DWORD dwNetOrderIP1,DWORD dwNetOrderIP2) const
	{
		const CPrivateNetRange * pNetSeg1 = find(dwNetOrderIP1);
		if( !pNetSeg1 )
			return NULL;

		const CPrivateNetRange * pNetSeg2 = find(dwNetOrderIP2);
		if( !pNetSeg2 )
			return NULL;

		if( pNetSeg2 != pNetSeg1 )
			return NULL;

		return pNetSeg1;
	}

	BOOL IsInSameNetSeg(DWORD dwNetOrderIP1,DWORD dwNetOrderIP2) const
	{
		const CPrivateNetRange *pNetSeg1 = 
			this->IsInSamePrivetNetRange(dwNetOrderIP1,dwNetOrderIP2);

		if( !pNetSeg1 )
		{
			return false;
		}

		return pNetSeg1->IsInSameNetSeg(dwNetOrderIP1,dwNetOrderIP2);
	}

public:
	std::vector<CPrivateNetRange>	m_vtPrivateNet;
};

class CPF_CLASS CPrivateNetList_Singleton: public ACE_Singleton<CPrivateNetList,ACE_Null_Mutex>
{
public:
	static CPrivateNetList * instance()
	{
		return ACE_Singleton<CPrivateNetList,ACE_Null_Mutex>::instance();
	}

};


namespace CPF
{
	//判断一个是文件或者路径
	CPF_EXPORT
		BOOL IsPrivateIPAddr(ACE_UINT32 hostorder_ip);


	//通过IP地址和端口号来判断方向
	//type=0的判断顺序是:
	//如果一个端口大于1024，一个端口小于1024，则小的端口为服务器端口
	//如果两个端口都大于1024或者都小于1024，则根据地址来判断。内网地址为客户端。外网地址为服务器端。
	//如果上面都不能判断，则返回DIR_CS_UNKNOWN

	//type=1的判断顺序是:
	//如果两个地址一个为内网地址，一个为外网地址，内网地址为客户端。外网地址为服务器端。
	//如果两个地址都是内网或者都是外网则一个端口大于1024，一个端口小于1024，则小的端口为服务器端口.。
	//如果上面都不能判断，则返回DIR_CS_UNKNOWN

	CPF_EXPORT
		DIR_CS JudgeDirBy_IPv4_CONNECT_ADDR(const IPv4_CONNECT_ADDR& connect_addr,int type = 0);

	//如果JudgeDirBy_IPv4_CONNECT_ADDR不能判断，则小端口为服务器端口
	CPF_EXPORT
		DIR_CS JudgeDirBy_IPv4_CONNECT_ADDR_Ex(const IPv4_CONNECT_ADDR& connect_addr,int type = 0);

}