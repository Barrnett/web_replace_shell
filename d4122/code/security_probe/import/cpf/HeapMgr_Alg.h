//////////////////////////////////////////////////////////////
//HeapMgr_Alg.h

#pragma once

#include "cpf/cpf.h"

class CPF_CLASS CHeapMgr_Alg
{
public:
	CHeapMgr_Alg();

	~CHeapMgr_Alg();

	BOOL init(ULONG total_len,ULONG minsize=64,char * pBuffer = NULL);
	void close();

	//看是否有内存泄露（即是否有通过该模块申请的内存，没有释放）
	//如果有内存泄露，返回true.
	BOOL CheckLeak() const;

public:
	void * Alloc(ULONG dwBytes);	

	void Free(void* pBlock);

	void * ReAlloc(void* pBlock, ULONG dwBytes)
	{
		void * pnewdata = Alloc(dwBytes);

		if( pnewdata )
		{
			memcpy(pnewdata,pBlock,dwBytes);			
		}

		Free(pBlock);

		return pnewdata;
	}

	inline ULONG GetAvailableLength() const
	{
		return m_Available_len;
	}

	inline ULONG GetTotalLength() const
	{
		return m_Total_len;
	}

	inline BOOL IsInMgr(void * pdata) const
	{
		return ( pdata >= m_pBuffer && pdata < m_pBuffer + m_Total_len );
	}

private:
	void * malloc(ULONG usize);
	void mfree(char * pdata,ULONG usize);

private:
	inline static void AssertValid(void * pdata)
	{
		BYTE * x = (BYTE *)pdata - sizeof(ULONG_PTR);

		ACE_ASSERT(memcmp((BYTE *)x + *(ULONG_PTR *)x - APPED_SIZE,m_append_data,APPED_SIZE)==0);
	}

	ULONG Malloc_Init_Buffer(ULONG usize);
	void FreeBuffer();

	ULONG Virtualalloc_Init_Buffer(ULONG usize);
	void VirtualfreeBuffer();

private:
	struct map_item
	{
		ULONG		m_size;
		map_item *	m_next;
	};

	map_item	m_head;

	ULONG		m_Total_len;
	ULONG		m_Available_len;

	char *		m_pBuffer;
	BOOL		m_bAllocBuffer;

private:
	enum 
	{
		APPED_SIZE = sizeof(char *)
	};

	static const unsigned char m_append_data[];

};



