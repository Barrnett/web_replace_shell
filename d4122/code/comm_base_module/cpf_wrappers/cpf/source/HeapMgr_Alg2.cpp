//////////////////////////////////////////////////////////////////////////
//HeapMgr_Alg2.cpp

#include "cpf/HeapMgr_Alg2.h"
#include "cpf/Common_Func_Macor.h"

const unsigned char CHeapMgr_Alg2::m_append_data[] = {0x00,0x78,0x78,0x78,0x78,0x78,0x78,0x78};

CHeapMgr_Alg2::CHeapMgr_Alg2()
{
	m_minsize = 0;

	m_Available_len = m_Total_len = 0;

	m_pBuffer = m_pData = 0;

	m_map = NULL;

	m_empty_head.m_addr = 0;
	m_empty_head.m_size = 0;

	return;
}

CHeapMgr_Alg2::~CHeapMgr_Alg2()
{
	close();

	return;
}

BOOL CHeapMgr_Alg2::init(ULONG total_len,ULONG minsize,char * pBuffer)
{
	m_minsize = minsize;

	if( total_len > 0 )
	{
		ULONG mgr_len = total_len/64/2*sizeof(map_item);

		if( !pBuffer )
		{
			pBuffer = (char *)::malloc(total_len);

			if( !pBuffer )
				return false;

			m_bAllocBuffer = true;
		}

		m_pBuffer = (char *)pBuffer;

		m_map = (map_item *)m_pBuffer;

		m_pData = m_pBuffer + mgr_len;

		m_map[0].m_addr = m_pData;
		m_map[0].m_size = total_len - mgr_len;

		m_map[1].m_addr = NULL;
		m_map[1].m_size = 0;

		m_Available_len = m_Total_len = total_len - mgr_len;
	}
	else
	{
		m_map = &m_empty_head;

		m_pData = m_pBuffer = NULL;

		m_bAllocBuffer = false;

		m_Available_len = m_Total_len = 0;
	}

	return true;
}

void CHeapMgr_Alg2::close()
{
	if( m_bAllocBuffer && m_pBuffer )
	{
		free((void *)m_pBuffer);
	}

	m_pBuffer = NULL;

	m_pData = NULL;

	m_map = NULL;

	return;
}


void * CHeapMgr_Alg2::Alloc(ULONG dwBytes)
{
	ULONG dwAllocsize = mymax(GET_BYTE_ALIGN(dwBytes+APPED_SIZE,8),m_minsize);

	void * pdata = CHeapMgr_Alg2::malloc(m_map,dwAllocsize);

	if( pdata )
	{
#ifdef _DEBUG
		memcpy((char *)pdata + dwAllocsize - APPED_SIZE,m_append_data,APPED_SIZE);
#endif

		*(ULONG_PTR *)pdata = (ULONG_PTR)dwAllocsize;

		m_Available_len -= dwAllocsize;

		return (void *)((char *)pdata + sizeof(ULONG_PTR));
	}

	return pdata;
}

void CHeapMgr_Alg2::Free(void* pBlock)
{
	if( !IsInMgr(pBlock) )
		return;

	char * pdata = (char *)pBlock-sizeof(ULONG_PTR);

#ifdef _DEBUG
	AssertValid(pBlock);
#endif

	m_Available_len += (ULONG)(*(ULONG_PTR *)pdata);

	CHeapMgr_Alg2::mfree(m_map,(ULONG)(*(ULONG_PTR *)pdata),pdata);

	return;
}

BOOL CHeapMgr_Alg2::CheckLeak() const
{
	if( m_Available_len != m_Total_len )
		return true;

	if( m_map[0].m_size != m_Total_len )
		return true;

	if( m_map[0].m_size && m_map[1].m_size != 0 )
		return true;

	return false;
}


void * CHeapMgr_Alg2::malloc(CHeapMgr_Alg2::map_item * mp,ULONG size)
{
	register UINT_PTR a;

	register CHeapMgr_Alg2::map_item *bp;

	for (bp=mp; bp->m_size; bp++) {

		if (bp->m_size >= size) {

			a = (UINT_PTR)bp->m_addr;

			bp->m_addr += size;

			if ((bp->m_size -= size) == 0)

				do {

					bp++;

					(bp-1)->m_addr = bp->m_addr;

				} while(((bp-1)->m_size = bp->m_size));

			return(void *)(a);

		}

	}

	return(0);

}


void CHeapMgr_Alg2::mfree(CHeapMgr_Alg2::map_item * mp,ULONG size,char * pdata)
{
	register CHeapMgr_Alg2::map_item *bp;

	register UINT_PTR t;

	register char * a = pdata;

	for (bp = mp; bp->m_addr<=a && bp->m_size!=0; bp++);

	if (bp>mp && (bp-1)->m_addr+(bp-1)->m_size == a) {

		(bp-1)->m_size += size;

		if (a+size == bp->m_addr) {

			(bp-1)->m_size += bp->m_size;

			while (bp->m_size) {

				bp++;

				(bp-1)->m_addr = bp->m_addr;

				(bp-1)->m_size = bp->m_size;

			}

		}

	} else {

		if (a+size == bp->m_addr && bp->m_size) {

			bp->m_addr -= size;

			bp->m_size += size;

		} else if (size) {
			do {

				t = (UINT_PTR)bp->m_addr;

				bp->m_addr = a;

				a = (char *)t;

				t = bp->m_size;

				bp->m_size = size;

				bp++;

			} while ((size = t));

			bp->m_size = 0;
		}

	}

}


