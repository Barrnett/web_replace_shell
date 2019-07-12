//////////////////////////////////////////////////////////////////////////
//HeapMgr_Alg2.h

#pragma once

#include "cpf/cpf.h"

class CPF_CLASS CHeapMgr_Alg2
{
public:
	CHeapMgr_Alg2();

	~CHeapMgr_Alg2();

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
		return ( pdata >= m_pData && pdata < m_pData + m_Total_len );
	}

private:
	inline static void AssertValid(void * pdata)
	{
		BYTE * x = (BYTE *)pdata - sizeof(char *);

		ACE_ASSERT(memcmp((BYTE *)x + *(ULONG_PTR *)x - APPED_SIZE,m_append_data,APPED_SIZE)==0);
	}


private:
	struct map_item
	{
		ULONG m_size;

		char *m_addr;

	};

	map_item *	m_map;

	map_item	m_empty_head;

	static void mfree(CHeapMgr_Alg2::map_item * mp,ULONG size,char * pdata);
	static void * malloc(CHeapMgr_Alg2::map_item * mp,ULONG size);

private:

	ULONG		m_Total_len;
	ULONG		m_Available_len;

	char *		m_pBuffer;

	char *		m_pData;

	ULONG		m_minsize;

	BOOL		m_bAllocBuffer;

private:
	enum 
	{
		APPED_SIZE = sizeof(char *)
	};

	static const unsigned char m_append_data[];


};

