//////////////////////////////////////////////////////////////////////////
//ConnectAddrHashFinder.h

#pragma once


#include "cpf/THashFinderEx.h"


#ifndef DEF_CConnectAddrHashFinder_DECLEAR
#define DEF_CConnectAddrHashFinder_DECLEAR		template<typename T> 
#define DEF_CConnectAddrHashFinder_CLASS		CConnectAddrHashFinder<T>
#endif // DEF_CConnectAddrHashFinder_DECLEAR

template<typename T>
class CConnectAddrHashFinder
{
public:
	CConnectAddrHashFinder(void)
	{

	}
	~CConnectAddrHashFinder(void)
	{
		Destroy();
	}

public:
	typedef struct
	{
		IPv4_CONNECT_ADDR	connAddr;
		T					data;
	}ConnectItem;

private:
	class TCPHashCompFun
	{
	public:		
		int Hash(const IPv4_CONNECT_ADDR& addr) const
		{
			return ((addr.client.wdPort^addr.server.wdPort)
				^(addr.client.dwIP^addr.server.dwIP));
		}

		int Cmp(const ConnectItem& link,const IPv4_CONNECT_ADDR& addr) const
		{
			return link.connAddr.Cmp(addr);
		}
	};

public:
	BOOL Create(size_t nums,size_t hashsize,BOOL bSupportLink = TRUE)
	{
		return m_LinkPool.Create(nums,hashsize,bSupportLink);
	}
	void Destroy()
	{
		m_LinkPool.Destroy();
	}

	T * InsertLink(const IPv4_CONNECT_ADDR& connAddr,const T& data);

	BOOL DelLink(const IPv4_CONNECT_ADDR& connAddr);

	T * Find(const IPv4_CONNECT_ADDR& connAddr) const
	{
		DIR_CS dir;

		return Find(connAddr,dir);
		
	}

	T * Find(const IPv4_CONNECT_ADDR& connectAddr,DIR_CS& dir) const
	{
		int cmpvalue = 0;

		ConnectItem * pItem = m_LinkPool.FindOnly(connAddr,cmpvalue);

		if(pItem)
		{
			if( cmpvalue == -1 )
				dir = DIR_S2C;
			else
				dir = DIR_C2S;

			return &pItem->data;
		}

		return NULL;
	}


public:
	CTHashFinderEx<ConnectItem,IPv4_CONNECT_ADDR,TCPHashCompFun>	m_LinkPool;
};


DEF_CConnectAddrHashFinder_DECLEAR
T * DEF_CConnectAddrHashFinder_CLASS::InsertLink(const IPv4_CONNECT_ADDR& connect_addr,
											   const T& data)
{
	ConnectItem * pItem = m_LinkPool.FindOnly(connect_addr);

	if( pItem )
	{
		pItem->data = data;

		return &pItem->data;
	}

	pItem = m_LinkPool.AllocBlockByKey(connect_addr);

	if( pItem )
	{
		pItem->data = data;
		pItem->connAddr = connect_addr;

		return &pItem->data;
	}

	return NULL;
}

DEF_CConnectAddrHashFinder_DECLEAR
BOOL DEF_CConnectAddrHashFinder_CLASS::DelLink(const IPv4_CONNECT_ADDR& connect_addr)
{
	ConnectItem * pItem = m_LinkPool.FindOnly(connect_addr);

	if(pItem)
	{
		m_LinkPool.FreeBlock(pItem);

		return true;
	}

	return false;
}

