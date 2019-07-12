//////////////////////////////////////////////////////////////////////////
//CPF_NetSeg.cpp

#include "cpf/CPF_NetSeg.h"


CPF_NetSeg::CPF_NetSeg()
{
}

CPF_NetSeg::CPF_NetSeg(DWORD dwNetOrderIP,DWORD dwNetOrderMask)
{	
	ACE_ASSERT( dwNetOrderIP != 0 && dwNetOrderIP != -1 );

	SetNetAddress(dwNetOrderIP,dwNetOrderMask);
}

CPF_NetSeg::CPF_NetSeg(DWORD begin_hostip,DWORD end_hostip,DWORD dwNetOrderMask)
{
	ACE_ASSERT(end_hostip>=begin_hostip);

	SetNetAddress(begin_hostip,end_hostip,dwNetOrderMask);
}


CPF_NetSeg::~CPF_NetSeg()
{
}

CPF_NetSeg& CPF_NetSeg::operator = (const CPF_NetSeg& other)
{
	if(this != &other)
	{
		SetNetAddress(other.m_dwHostOrderBeginIP,
			other.m_dwHostOrderEndIP,
			other.GetMask().GetNetOrderIP());

		m_strName = other.m_strName;

		m_vtHostOrderIP = other.m_vtHostOrderIP;
	}

	return *this;
}

void CPF_NetSeg::SetNetAddress(DWORD dwNetOrderIP,DWORD dwNetOrderMask)
{	
	DWORD dwbeginip;
	DWORD dwendip;

	CPF_NetSeg::GetBeinEndIP(dwNetOrderIP,dwNetOrderMask,dwbeginip,dwendip);

	SetNetAddress(dwbeginip,dwendip,dwNetOrderMask);

	return;
}

void CPF_NetSeg::SetNetAddress(DWORD begin_hostip,DWORD end_hostip,DWORD dwNetOrderMask)
{
	m_dwHostOrderBeginIP = begin_hostip;
	m_dwHostOrderEndIP = end_hostip;

	m_mask = dwNetOrderMask;

	m_beginIP = ACE_HTONL(begin_hostip);
	m_endIP = ACE_HTONL(end_hostip);

	return;
}

BOOL CPF_NetSeg::IsIPInSeg(DWORD dwNetOrderIP) const
{
	DWORD dwHostOrderIP = ACE_NTOHL(dwNetOrderIP);

	if( dwHostOrderIP >= m_dwHostOrderBeginIP 
		&& dwHostOrderIP <= m_dwHostOrderEndIP )
	{
		return true;
	}

	return FALSE;
}

BOOL CPF_NetSeg::SearchIP(DWORD dwNetOrderIP) const
{
	ACE_UINT32 hostorder_ip = ACE_NTOHL(dwNetOrderIP);

	std::vector<ACE_UINT32>::const_iterator it =
		std::lower_bound(m_vtHostOrderIP.begin(),m_vtHostOrderIP.end(),hostorder_ip);

	if( it != m_vtHostOrderIP.end() 
		&& *it == hostorder_ip )
	{
		return true;
	}

	return false;
}

BOOL CPF_NetSeg::InsertIP(ACE_UINT32 hostorder_ip)
{
	std::vector<ACE_UINT32>::iterator it =
		std::lower_bound(m_vtHostOrderIP.begin(),m_vtHostOrderIP.end(),hostorder_ip);

	if( it != m_vtHostOrderIP.end() 
		&& *it == hostorder_ip )
	{
		return false;
	}

	m_vtHostOrderIP.insert(it,hostorder_ip);

	return true;
}



//////////////////////////////////////////////////////////////////////////
//CPF_NetSegList
//////////////////////////////////////////////////////////////////////////

#include "ace/Global_Macros.h"
#include "ace/Guard_T.h"

CPF_NetSegList::CPF_NetSegList()
{
}

CPF_NetSegList::~CPF_NetSegList()
{
	fini();

	return;
	
}

bool CPF_NetSegList::init()
{
	return true;
}


void CPF_NetSegList::fini()
{

	ACE_Guard<ACE_Recursive_Thread_Mutex>	lock(m_tm);

	const std::vector<CIPRangeTable<CPF_NetSeg *>::IP_RANGE_TYPE>& seg_table = m_list_netseg.GetIPTable();

	std::vector<CIPRangeTable<CPF_NetSeg *>::IP_RANGE_TYPE>::const_iterator it;

	for(it = seg_table.begin(); it != seg_table.end(); ++it )
	{
		CPF_NetSeg * pNetSeg =(*it).data;

		if( pNetSeg )
		{
			delete pNetSeg;
		}
	}

	m_list_netseg.clear();

	return;
}

CPF_NetSeg * CPF_NetSegList::load_netseg(const CPF_NetSeg& netseg)
{
	CPF_NetSeg * pNetSeg = add_netseg(netseg.GetHostOrderBeginIP(),
		netseg.GetHostOrderEndIP(),
		netseg.GetMask().GetNetOrderIP());

	return pNetSeg;
}

CPF_NetSeg * CPF_NetSegList::add_netseg(const char * strIP,const char * strMask)
{
	DWORD dwIP = inet_addr(strIP);
	DWORD dwMask = inet_addr(strMask);

	return add_netseg(dwIP,dwMask);
}

CPF_NetSeg * CPF_NetSegList::add_netseg(DWORD dwNetOrderIP,DWORD dwNetOrderMask)
{
	DWORD dwbeginip,dwendip;

	CPF_NetSeg::GetBeinEndIP(dwNetOrderIP,dwNetOrderMask,
		dwbeginip,dwendip);

	return add_netseg(dwbeginip,dwendip,dwNetOrderMask);
}

CPF_NetSeg * CPF_NetSegList::add_netseg(DWORD begin_hostip,DWORD end_hostip,DWORD dwNetOrderMask)
{
	ACE_Guard<ACE_Recursive_Thread_Mutex>	lock(m_tm);

	CPF_NetSeg * pNetSeg = find_hostorder_ip(begin_hostip);

	if( pNetSeg )
		return pNetSeg;

	pNetSeg = find_hostorder_ip(end_hostip);

	if( pNetSeg )
		return pNetSeg;

	return add_diff_netseg(begin_hostip,end_hostip,dwNetOrderMask);
}

CPF_NetSeg * CPF_NetSegList::add_netseg(const char * strBeginIP,const char * strEndIP,const char * strMask)
{
	DWORD dwbeginIP = ACE_NTOHL(inet_addr(strBeginIP));
	DWORD dwendIP = ACE_NTOHL(inet_addr(strEndIP));

	DWORD dwMask = inet_addr(strMask);

	return add_netseg(dwbeginIP,dwendIP,dwMask);
}


//增加肯定不会重复的网段
CPF_NetSeg * CPF_NetSegList::add_diff_netseg(DWORD dwNetOrderIP,DWORD dwNetOrderMask)
{
	DWORD dwbeginip,dwendip;

	CPF_NetSeg::GetBeinEndIP(dwNetOrderIP,dwNetOrderMask,
		dwbeginip,dwendip);

	return add_diff_netseg(dwbeginip,dwendip,dwNetOrderMask);
}

CPF_NetSeg * CPF_NetSegList::add_diff_netseg(DWORD begin_hostip,DWORD end_hostip,DWORD dwNetOrderMask)
{
	CPF_NetSeg * pnew_net_seg = new CPF_NetSeg(begin_hostip,end_hostip,dwNetOrderMask);

	if( m_list_netseg.Insert(pnew_net_seg->GetHostOrderBeginIP(),pnew_net_seg->GetHostOrderEndIP(),pnew_net_seg) )
	{
		return pnew_net_seg;
	}
	else
	{
		delete pnew_net_seg;
		return NULL;
	}
}


CPF_NetSeg * CPF_NetSegList::find_netorder_ip(DWORD dwNetOrderIP) const
{
	return find_hostorder_ip(ACE_NTOHL(dwNetOrderIP));

}

CPF_NetSeg * CPF_NetSegList::find_hostorder_ip(DWORD dwHostOrderIP) const
{
	ACE_Guard<ACE_Recursive_Thread_Mutex>	lock(const_cast<CPF_NetSegList *>(this)->m_tm);

	CPF_NetSeg * const * ppnetseg = m_list_netseg.Find_Binary(dwHostOrderIP);

	if( ppnetseg )
	{
		return (CPF_NetSeg *)(*ppnetseg);
	}

	return NULL;
}


DWORD CPF_NetSegList::get_netmask(DWORD dwNetOrderIP) const
{
	CPF_NetSeg * pNetSeg = find_netorder_ip(dwNetOrderIP);

	if( pNetSeg )
	{
		return pNetSeg->GetMask().GetNetOrderIP();
	}

	return 0;
}


const std::vector<CIPRangeTable<CPF_NetSeg *>::IP_RANGE_TYPE>& CPF_NetSegList::GetNetSegs() const
{
	return m_list_netseg.GetIPTable();
}
