//////////////////////////////////////////////////////////////
//HeapMgr_Alg.cpp

#include "cpf/HeapMgr_Alg.h"
#include "cpf/Common_Func_Macor.h"

const unsigned char CHeapMgr_Alg::m_append_data[] = {0x00,0x78,0x78,0x78,0x78,0x78,0x78,0x78};

CHeapMgr_Alg::CHeapMgr_Alg()
{
	m_Available_len = 0;
	m_Total_len = 0;

	m_head.m_size = 0;
	m_head.m_next = NULL;

	m_pBuffer = NULL;

	m_bAllocBuffer = false;

	return;
}

CHeapMgr_Alg::~CHeapMgr_Alg()
{
	close();
}

BOOL CHeapMgr_Alg::init(ULONG buf_len,ULONG minsize,char * pBuffer)
{
	m_head.m_size = 0;
	m_head.m_next = NULL;

	m_Total_len = m_Available_len = buf_len;

	if( buf_len > 0 )
	{
		if( !pBuffer )
		{
			buf_len = Virtualalloc_Init_Buffer(buf_len);

			m_Total_len = m_Available_len = buf_len;

			m_bAllocBuffer = true;
		}
		else
		{
			m_pBuffer = pBuffer;

			m_bAllocBuffer = false;
		}

		if( !m_pBuffer )
		{
			return false;
		}

		map_item * pmap_item = (map_item *)m_pBuffer;

		pmap_item->m_size = buf_len;
		pmap_item->m_next = NULL;

		m_head.m_next = pmap_item;
	}

	return true;
}


void CHeapMgr_Alg::close()
{
	if( m_pBuffer && m_bAllocBuffer )
	{
		VirtualfreeBuffer();
		//FreeBuffer();
	}

	m_pBuffer = NULL;

	m_head.m_size = 0;
	m_head.m_next = NULL;

	m_Available_len = 0;
	m_Total_len = 0;

	return;
}

void * CHeapMgr_Alg::Alloc(ULONG dwBytes)
{
	ULONG ulAllocSize = sizeof(ULONG_PTR) + GET_BYTE_ALIGN(dwBytes,sizeof(ULONG_PTR)) + APPED_SIZE;

	void * pdata = this->malloc(ulAllocSize);

	if( pdata )
	{
#ifdef _DEBUG
		memcpy((BYTE *)pdata+ulAllocSize-APPED_SIZE,m_append_data,APPED_SIZE);
#endif

		*(ULONG_PTR *)pdata = (ULONG_PTR)ulAllocSize;

		m_Available_len -= ulAllocSize;

		return (void *)((char *)pdata+sizeof(ULONG_PTR));

	}	

	return NULL;
}

void CHeapMgr_Alg::Free(void* pBlock)
{
	if( !IsInMgr(pBlock) )
	{
		ACE_ASSERT(false);
		return;
	}

	char * pdata = (char *)pBlock - sizeof(ULONG_PTR);

#ifdef _DEBUG
	AssertValid(pBlock);
#endif

	m_Available_len += (ULONG)(*(ULONG_PTR *)pdata);

	this->mfree(pdata,(ULONG)(*(ULONG_PTR *)pdata));

	return;
}


void * CHeapMgr_Alg::malloc(ULONG ulAllocSize)
{
	map_item * pmap_prev = &m_head;

	map_item * pmap_cur = m_head.m_next;

	while(pmap_cur)
	{
		if( pmap_cur->m_size >= ulAllocSize )
		{
			char * pdata = (char *)pmap_cur;

			if( pmap_cur->m_size == ulAllocSize )
			{
				pmap_prev->m_next = pmap_cur->m_next;
			}
			else
			{
				ULONG		old_size = pmap_cur->m_size;
				map_item *	old_next = pmap_cur->m_next;

				pmap_cur = (map_item *)(pdata + ulAllocSize);

				pmap_cur->m_size = old_size - ulAllocSize;
				pmap_cur->m_next = old_next;

				pmap_prev->m_next = pmap_cur;
			}

			return (void *)(pdata);
		}

		pmap_prev = pmap_cur;

		pmap_cur = pmap_cur->m_next;
	}

	return NULL;
}

void CHeapMgr_Alg::mfree(char* pdata,ULONG dwAllocSize)
{
	map_item * pmap_prev = &m_head;

	map_item * pmap_cur = m_head.m_next;

	while( pmap_cur && (char *)pmap_cur < pdata )
	{
		pmap_prev = pmap_cur;
		pmap_cur = pmap_cur->m_next;
	}

	if( (char *)pmap_prev+pmap_prev->m_size == pdata && pmap_cur != m_head.m_next )
	{//和前面一块合并
		pmap_prev->m_size += dwAllocSize;

		if( pdata + dwAllocSize == (char *)pmap_cur )
		{//和后面一块合并

			pmap_prev->m_size += pmap_cur->m_size;
			pmap_prev->m_next = pmap_cur->m_next;
		}
	}
	else
	{
		if( pdata + dwAllocSize == (char *)pmap_cur )
		{//和后面一块合并

			map_item * pnew_item = (map_item *)pdata;

			pnew_item->m_size = pmap_cur->m_size + dwAllocSize;
			pnew_item->m_next = pmap_cur->m_next;

			pmap_prev->m_next = pnew_item;
		}
		else
		{
			map_item * pnew_item = (map_item *)pdata;

			pnew_item->m_next = pmap_prev->m_next;
			pmap_prev->m_next = pnew_item;
		}
	}

	return;
}


BOOL CHeapMgr_Alg::CheckLeak() const
{
	if( m_Available_len != m_Total_len )
		return true;

	if( m_head.m_next )
	{
		if( m_head.m_next->m_next != NULL )
			return true;

		if( m_head.m_next->m_size != m_Total_len )
			return true;
	}

	return false;
}

ULONG CHeapMgr_Alg::Virtualalloc_Init_Buffer(ULONG usize)
{
#ifdef OS_WINDOWS
	while( 1 )
	{
		void * lpvBase = VirtualAlloc(NULL,usize,MEM_RESERVE,PAGE_READWRITE); 

		if( !lpvBase )
		{
			usize = usize *3 / 4;
			continue;
		}

		void * lpvResult = VirtualAlloc(lpvBase,usize,MEM_COMMIT,PAGE_READWRITE);

		if( !lpvResult )
		{
			VirtualFree(lpvBase,0,MEM_RELEASE);

			usize = usize *3 / 4;

			continue;
		}

		m_pBuffer = (char *)lpvResult;

		break;

	}

	return usize;

#else
	m_pBuffer = new BYTE[usize];
	return usize;

#endif

}

void CHeapMgr_Alg::VirtualfreeBuffer()
{
#ifdef OS_WINDOWS
	VirtualFree(m_pBuffer,0,MEM_RELEASE);
	m_pBuffer = NULL;

#else
	if (NULL != m_pBuffer)
	{
		delete m_pBuffer;
	}
	m_pBuffer = NULL;
	return;
#endif
}

ULONG CHeapMgr_Alg::Malloc_Init_Buffer(ULONG usize)
{
	while( 1 )
	{
		void * lpvResult = malloc(usize);

		if( !lpvResult )
		{
			usize = usize *3 / 4;
			continue;
		}

		m_pBuffer = (char *)lpvResult;

		break;
	}

	return usize;
}

void CHeapMgr_Alg::FreeBuffer()
{
	free(m_pBuffer);
	m_pBuffer = NULL;
}

