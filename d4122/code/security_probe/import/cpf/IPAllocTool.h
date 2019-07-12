//////////////////////////////////////////////////////////////////////////
//IPAllocTool.h

#pragma once

#include "cpf/cpf.h"


class CPF_CLASS CIPAllocTool
{
public:
	CIPAllocTool(void);
	~CIPAllocTool(void);

public:
	void init(ACE_UINT32 begin_ip,ACE_UINT32 end_ip);
	void close();

private:
	void init_free_item(ACE_UINT32 begin_ip,ACE_UINT32 end_ip);

	typedef struct
	{
		int next;
		int prev;

	}FREE_ITEM;

	FREE_ITEM * m_pFreeTable;

	int			m_nHead;
	int			m_nTail;

private:
	ACE_UINT32	m_begin_ip;
	ACE_UINT32	m_end_ip;

public:
	inline ACE_UINT32 GetBeginIP() const
	{
		return m_begin_ip;
	}
	inline ACE_UINT32 GetEndIP() const
	{
		return m_end_ip;
	}

	inline ACE_UINT32 alloc_by_ip(ACE_UINT32 ipaddr)
	{
		return alloc((int)(ipaddr-m_begin_ip));
	}

	inline ACE_UINT32 alloc()
	{
		return alloc(m_nHead);
	}

	void free(ACE_UINT32 ipaddr)
	{
		int index = (int)(ipaddr - m_begin_ip);

		ACE_ASSERT(m_pFreeTable[index].next==-1 && m_pFreeTable[index].prev==-1);

		m_pFreeTable[m_nTail].next = index;
		m_pFreeTable[index].prev = m_nTail;
		m_pFreeTable[index].next = -1;
		m_nTail = index;
	}

private:
	//返回0，标识没有分配到IP
	ACE_UINT32 alloc(int index);

public:
	inline BOOL IsIPUsed(ACE_UINT32 ipaddr) const
	{
		int index = (int)(ipaddr - m_begin_ip);

		return( m_pFreeTable[index].next==-1 && m_pFreeTable[index].prev==-1);

	}

	inline BOOL IsIPFree(ACE_UINT32 ipaddr) const
	{
		return !IsIPUsed(ipaddr);
	}

	inline BOOL IsOutofRange(ACE_UINT32 ipaddr) const
	{
		return (!IsInRange(ipaddr));
	}

	inline BOOL IsInRange(ACE_UINT32 cur_ip) const
	{
		return ( cur_ip >= m_begin_ip && cur_ip <= m_end_ip );
	}
};
