//////////////////////////////////////////////////////////////////////////
//IPPortHashFinder.h

#pragma once

#include "cpf/ostypedef.h"
#include "cpf/addr_tools.h"
#include "cpf/THashFinderEx.h"


#ifndef DEF_CIPPortHashFinder_DECLEAR
#define DEF_CIPPortHashFinder_DECLEAR		template<typename T> 
#define DEF_CIPPortHashFinder_CLASS			CIPPortHashFinder<T>
#endif // DEF_CIPPortHashFinder_DECLEAR

template<typename T>
class CIPPortHashFinder
{
public:
	CIPPortHashFinder(void)
	{

	}

	~CIPPortHashFinder(void)
	{
		Destroy();
	}

public:
	typedef struct
	{
		IPv4_TRANSPORT_ADDR	ipport;
		T					data;
	}IPPortItem;

private:
	class TransHashAndCmp
	{
	public:		
		int Hash(const IPv4_TRANSPORT_ADDR& addr) const
		{
			return (int)(addr.wdPort^addr.dwIP);
		}

		int Cmp(const IPPortItem& item,const IPv4_TRANSPORT_ADDR& addr) const
		{
			return ( item.ipport.dwIP == addr.dwIP && item.ipport.wdPort == addr.wdPort );
		}
	};
public:
	BOOL Create(size_t nums,size_t hashsize,BOOL bSupportLink = TRUE)
	{
		return m_IPPortPool.Create(nums,hashsize,bSupportLink);
	}
	void Destroy()
	{
		m_IPPortPool.Destroy();
	}

	BOOL InsertIPPort(ACE_UINT32 hipaddr,ACE_UINT16 hport,const T& data)
	{
		IPv4_TRANSPORT_ADDR ipport;
		ipport.dwIP = hipaddr;
		ipport.wdPort = hport;

		return InsertIPPort(ipport,data);

	}

	T * InsertIPPort(const IPv4_TRANSPORT_ADDR& ipport,const T& data);


	BOOL DelIPPort(ACE_UINT32 hipaddr,ACE_UINT16 hport)
	{
		IPv4_TRANSPORT_ADDR ipport;
		ipport.dwIP = hipaddr;
		ipport.wdPort = hport;

		return DelIPPort(ipport);
//		return DelIPPort(ipport,data);
	}

	BOOL DelIPPort(const IPv4_TRANSPORT_ADDR& ipport);

	T * FindIPPort(ACE_UINT32 hipaddr,ACE_UINT16 hport) const
	{
		IPv4_TRANSPORT_ADDR ipport;
		ipport.dwIP = hipaddr;
		ipport.wdPort = hport;

		return FindIPPort(ipport);
	}

	T * FindIPPort(ACE_UINT32 hipaddr,ACE_UINT16 hport,DIR_CS& dir) const
	{
		IPv4_TRANSPORT_ADDR ipport;
		ipport.dwIP = hipaddr;
		ipport.wdPort = hport;

		return FindIPPort(ipport,dir);
	}


	T * FindIPPort(const IPv4_TRANSPORT_ADDR& ipport) const
	{
		IPPortItem * pItem = m_IPPortPool.FindOnly(ipport);

		if(pItem)
		{
			return &pItem->data;
		}

		return NULL;
	}

	T * FindIPPort(const IPv4_CONNECT_ADDR& connectAddr,DIR_CS& dir) const
	{
		T* data = FindIPPort(connectAddr.server);
		if( data )
		{
			dir = DIR_C2S;
			return data;
		}

		data = FindIPPort(connectAddr.client);
		if( data )
		{
			dir = DIR_S2C;
			return data;
		}

		return NULL;
	}


public:
	CTHashFinderEx<IPPortItem,IPv4_TRANSPORT_ADDR,TransHashAndCmp>	m_IPPortPool;
};

DEF_CIPPortHashFinder_DECLEAR
T * DEF_CIPPortHashFinder_CLASS::InsertIPPort(const IPv4_TRANSPORT_ADDR& ipport,
												const T& data)
{
	IPPortItem * pItem = m_IPPortPool.FindOnly(ipport);

	if( pItem )
	{
		pItem->data = data;
		
		return &pItem->data;
	}

	pItem = m_IPPortPool.AllocBlockByKey(ipport);

	if( pItem )
	{
		pItem->data = data;
		pItem->ipport = ipport;

		return &pItem->data;
	}

	return NULL;
}

DEF_CIPPortHashFinder_DECLEAR
BOOL DEF_CIPPortHashFinder_CLASS::DelIPPort(const IPv4_TRANSPORT_ADDR& ipport)
{
	IPPortItem * pItem = m_IPPortPool.FindOnly(ipport);

	if(pItem)
	{
		m_IPPortPool.FreeBlock(pItem);

		return true;
	}

	return false;
}

