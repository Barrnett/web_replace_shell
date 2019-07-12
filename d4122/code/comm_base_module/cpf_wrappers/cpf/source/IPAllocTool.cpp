//////////////////////////////////////////////////////////////////////////
//IPAllocTool.cpp

#include "cpf/IPAllocTool.h"

CIPAllocTool::CIPAllocTool(void)
{
	m_begin_ip = 0;
	m_end_ip = 0;

	m_nHead = m_nTail = -1;

	m_pFreeTable = NULL;

}

CIPAllocTool::~CIPAllocTool(void)
{
}

void CIPAllocTool::init(ACE_UINT32 begin_ip,ACE_UINT32 end_ip)
{
	if( end_ip < begin_ip )
	{
		end_ip = begin_ip;
	}

	if(end_ip - begin_ip > 256*256 )
	{
		end_ip = begin_ip + 256*256;
	}

	m_begin_ip = begin_ip;
	m_end_ip = end_ip;

	init_free_item(begin_ip,end_ip);

	return;

}

void CIPAllocTool::close()
{
	if( m_pFreeTable )
	{
		delete m_pFreeTable;
		m_pFreeTable = NULL; 
	}

	m_nHead = m_nTail = -1;

	return;
}

void CIPAllocTool::init_free_item(ACE_UINT32 begin_ip,ACE_UINT32 end_ip)
{
	int max_count = (int)(end_ip - begin_ip + 1);

	m_pFreeTable = new FREE_ITEM[max_count];

	int prev_index = -1;

	for(int i = 0; i < max_count; ++i)
	{
		m_pFreeTable[i].prev = prev_index;

		m_pFreeTable[i].next = i+1;

		prev_index = i;
	}

	m_pFreeTable[max_count-1].next = -1;

	m_nHead = 0;
	m_nTail = max_count-1;

	return;
}


ACE_UINT32 CIPAllocTool::alloc(int index)
{
	if( m_nHead == m_nTail )
	{
		return 0;
	}

#ifdef _DEBUG
	if( index != m_nTail )
	{
		ACE_ASSERT(m_pFreeTable[index].next!=-1);
	}

	if(index != m_nHead )
	{
		ACE_ASSERT(m_pFreeTable[index].prev!=-1);
	}
#endif

	int prev = m_pFreeTable[index].prev;

	if( prev != -1 )
	{
		m_pFreeTable[prev].next = m_pFreeTable[index].next;
	}
	else
	{
		ACE_ASSERT(m_nHead==index);
		m_nHead = m_pFreeTable[index].next;
	}

	int next = m_pFreeTable[index].next;

	if( next != -1 )
	{
		m_pFreeTable[next].prev = m_pFreeTable[index].prev;
	}
	else
	{
		ACE_ASSERT(m_nTail==index);

		m_nTail = m_pFreeTable[index].prev;
	}

	//标记为-1，表示释放出去了 
	m_pFreeTable[index].next = m_pFreeTable[index].prev = -1;

	return (ACE_UINT32)(index + m_begin_ip);
}
