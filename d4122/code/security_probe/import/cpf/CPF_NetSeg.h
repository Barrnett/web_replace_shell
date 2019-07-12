//////////////////////////////////////////////////////////////////////////
//CPF_NetSeg.h

#pragma once

#include "cpf/cpf.h"
#include "cpf/InetAddress.h"
#include "cpf/IPRangeTable.h"
#include "string"

class CPF_CLASS CPF_NetSeg
{
public:
	static void GetBeinEndIP(IN DWORD dwNetOrderIP,IN DWORD dwNetOrderMask,
		OUT DWORD& begin_host_ip,OUT DWORD& end_host_ip)
	{
		begin_host_ip = ACE_NTOHL((dwNetOrderIP & dwNetOrderMask));
		end_host_ip = ACE_NTOHL((dwNetOrderIP | ~dwNetOrderMask));

		return;
	}

public:
	CPF_NetSeg();

	//dwNetOrderMask=0x00ffffff="255.255.255.0"
	CPF_NetSeg(DWORD dwNetOrderIP,DWORD dwNetOrderMask);
	CPF_NetSeg(DWORD begin_hostip,DWORD end_begin_hostip,DWORD dwNetOrderMask);

	virtual ~CPF_NetSeg();	

	CPF_NetSeg& operator = (const CPF_NetSeg& other);

public:
	void SetName(const char * name)
	{
		m_strName = name;
	}

	const char * GetName() const
	{
		return m_strName.c_str();
	}

	std::string GetShowName() const
	{
		if( m_strName.empty() )
		{
			ACE_UINT32 netorder_beginip = ACE_HTONL(m_dwHostOrderBeginIP);

			return inet_ntoa(*(in_addr *)&netorder_beginip);
		}
		else
		{
			return m_strName;
		}
	}

public:
	//dwNetOrderMask=0x00ffffff="255.255.255.0"
	void SetNetAddress(DWORD dwNetOrderIP,DWORD dwNetOrderMask);
	void SetNetAddress(DWORD begin_hostip,DWORD end_hostip,DWORD dwNetOrderMask);

	//判断IP地址是否是这个网段范围的
	BOOL IsIPInSeg(DWORD dwNetOrderIP) const;

	BOOL AddIP(DWORD dwNetOrderIP)
	{
		if( !IsIPInSeg(dwNetOrderIP) )
		{//不是这个IP网段的

			return false;
		}

		if( SearchIP(dwNetOrderIP) )
		{//已经存在了
			return false;
		}

		InsertIP(ACE_NTOHL(dwNetOrderIP));

		return true;
	}

	BOOL SearchIP(DWORD dwNetOrderIP) const;

public:
	const InetAddress& GetMask() const { return m_mask;	}

	ACE_UINT32 GetHostOrderBeginIP() const	{return m_dwHostOrderBeginIP;}
	ACE_UINT32 GetHostOrderEndIP() const	{return m_dwHostOrderEndIP;}

	//该网段有多少个ip地址空间
	int GetIPRangeNums() const
	{
		return m_dwHostOrderEndIP - m_dwHostOrderBeginIP + 1;
	}

	//当前有多少个主机
	int GetRealIPNums() const
	{
		return (int)m_vtHostOrderIP.size();
	}

	//获取所有主机的列表
	const std::vector<ACE_UINT32>& GetIPs() const
	{
		return m_vtHostOrderIP;
	}

	std::vector<ACE_UINT32>& GetIPs()
	{
		return m_vtHostOrderIP;
	}

	
private:
	BOOL InsertIP(ACE_UINT32 hostorder_ip);

private:
	InetAddress			m_beginIP;
	InetAddress			m_endIP;
	InetAddress			m_mask;

	ACE_UINT32			m_dwHostOrderBeginIP;
	ACE_UINT32			m_dwHostOrderEndIP;

	//当前网段中存在的主机
	std::vector<ACE_UINT32>	m_vtHostOrderIP;

	std::string				m_strName;

};

#include <vector>
#include "ace/Recursive_Thread_Mutex.h"

class CPF_CLASS CPF_NetSegList
{
public:
	CPF_NetSegList();
	virtual ~CPF_NetSegList();

	bool init();

	void fini();

public:
	void Lock()
	{	m_tm.acquire();	}

	void UnLock()
	{	m_tm.release();	}

public:
	CPF_NetSeg * load_netseg(const CPF_NetSeg& netseg);

	CPF_NetSeg * add_netseg(DWORD dwNetOrderIP,DWORD dwNetOrderMask);
	CPF_NetSeg * add_netseg(const char * strIP,const char * strMask);

	CPF_NetSeg * add_netseg(DWORD begin_hostip,DWORD end_hostip,DWORD dwNetOrderMask);
	CPF_NetSeg * add_netseg(const char * strBeginIP,const char * strEndIP,const char * strMask);

	//增加肯定不会重复的网段
	CPF_NetSeg * add_diff_netseg(DWORD dwNetOrderIP,DWORD dwNetOrderMask);
	CPF_NetSeg * add_diff_netseg(DWORD begin_hostip,DWORD end_hostip,DWORD dwNetOrderMask);

	CPF_NetSeg * find_netorder_ip(DWORD dwNetOrderIP) const;
	CPF_NetSeg * find_hostorder_ip(DWORD dwHostOrderIP) const;

	CPF_NetSeg * IsSameNetseg(DWORD dwOrderIP1,DWORD dwOrderIP2) const
	{
		CPF_NetSeg * pNetSeg1 = find_netorder_ip(dwOrderIP1);
		if( !pNetSeg1 )
			return NULL;

		CPF_NetSeg * pNetSeg2 = find_netorder_ip(dwOrderIP2);

		if( !pNetSeg2 )
			return NULL;

		if( pNetSeg2 != pNetSeg1 )
			return NULL;

		return pNetSeg1;
	}

	//看这个IP地址是否已经在某个网段中，如果在返回该网段的mask
	DWORD get_netmask(DWORD dwNetOrderIP) const;

	const std::vector<CIPRangeTable<CPF_NetSeg *>::IP_RANGE_TYPE>& GetNetSegs() const;	

private:
	CIPRangeTable<CPF_NetSeg *>	m_list_netseg;

	ACE_Recursive_Thread_Mutex	m_tm;

};

class CNetSegList_Locker
{
public:
	CNetSegList_Locker(CPF_NetSegList& NetSegList)
		:m_NetSegList(NetSegList)
	{
		m_NetSegList.Lock();
	}

	~CNetSegList_Locker()
	{
		m_NetSegList.UnLock();
	}

private:
	CPF_NetSegList& m_NetSegList;
};
