//////////////////////////////////////////////////////////////////////////
//mquemgr.inl

// =======================================================================


template<typename LOCK,typename _A>
const unsigned char Cmquemgr<LOCK,_A>::m_append_data[] = {0x00,0x78,0x78,0x78,0x78,0x78,0x78,0x78};

#include "cpf/other_tools.h"

/*
 *	���캯��
 */
template<typename LOCK,typename _A>
Cmquemgr<LOCK,_A>::Cmquemgr(ULONG dwMaxBlockSize, ULONG dwCacheBank, ULONG dwPreAllocBlocks,ULONG dwCacheNums)
{
	m_dwCacheBank = 0;
	m_dwCacheSize = 0;
	m_dwMaxBlockSize = 0;
	m_pCache = NULL;
	m_dwAllocBlock = 0;
	m_dwCacheBankPower = 0;
	m_dwCacheNums = 0;

	SetParams(dwMaxBlockSize, dwCacheBank,dwPreAllocBlocks,dwCacheNums);

	return;
}

/*
 *	��������
 */
template<typename LOCK,typename _A>
Cmquemgr<LOCK,_A>::~Cmquemgr()
{
	Close();

	return;
}


/*
 *	��������
 */
template<typename LOCK,typename _A>
void Cmquemgr<LOCK,_A>::SetParams(ULONG dwMaxBlockSize, ULONG dwCacheBank,ULONG nCount,ULONG dwCacheNums)
{
	ACE_ASSERT(dwCacheBank >= sizeof(BYTE*));
	ACE_ASSERT(((~dwCacheBank+1) & dwCacheBank ) == dwCacheBank);
	
	Close();
	
	m_dwCacheNums = dwCacheNums;

	m_dwCacheBank = dwCacheBank;

	if( dwMaxBlockSize % dwCacheBank == 0)
		m_dwCacheSize = dwMaxBlockSize / dwCacheBank;
	else
		m_dwCacheSize = dwMaxBlockSize / dwCacheBank + 1;

	m_pCache = new(std::nothrow) Mgr_Header[m_dwCacheSize];

	if (m_pCache == NULL)
	{
		ACE_ASSERT(FALSE);
		exit(0);
	}

	memset(m_pCache,0x00,m_dwCacheSize*sizeof(Mgr_Header));

	m_dwMaxBlockSize = m_dwCacheSize*dwCacheBank;

	m_dwCacheBankPower = 0;
	while( dwCacheBank != 1 )
	{
		dwCacheBank = (dwCacheBank>>1);
		++m_dwCacheBankPower;
	}
		
	PreAlloc(nCount);

	return;
}

/*
 *	Ԥ��Ϊÿһ���ڴ����nCount���ڴ��
 */
template<typename LOCK,typename _A>
BOOL Cmquemgr<LOCK,_A>::PreAlloc(ULONG nCount)
{
	for (UINT i = 0; i < m_dwCacheSize; i++)
	{
		ULONG dwBytes = (i+1) * m_dwCacheBank;

		if( !Reserve(dwBytes,nCount) )
			return false;
	}

	return TRUE;
}

template<typename LOCK,typename _A>
BOOL Cmquemgr<LOCK,_A>::Reserve(ULONG blocksize, ULONG blocknums)
{	
	PVOID* pReservePointer = (PVOID *)new(std::nothrow) char[blocknums*sizeof(PVOID)];

	if( !pReservePointer )
		return false;

	BOOL bOK = TRUE;

	memset(pReservePointer,0x00,blocknums*sizeof(PVOID));

	for(size_t i = 0; i < blocknums; ++i)
	{
		pReservePointer[i] = Alloc((ULONG)blocksize);

		if( !pReservePointer[i] )
		{
			bOK = FALSE;
			break;
		}
	}

	for(size_t i = 0; i < blocknums; ++i)
	{
		if( pReservePointer[i] )
		{
			Free(pReservePointer[i]);
		}
	}

	delete[] (pReservePointer);

	return bOK;
}

/*
 *	�ڴ濪��
 */
template<typename LOCK,typename _A>
void * Cmquemgr<LOCK,_A>::Alloc(ULONG dwBytes)
{
	if (dwBytes == 0) 
		return NULL;	

	//���ж���
	ULONG dwAllignSize = CalcuGauss(dwBytes);
	ULONG dwAllocSize = sizeof(ULONG_PTR)+dwAllignSize+APPED_SIZE;// 

	// ����ǳ����, ��ֱ�ӿ���
	if (dwAllignSize > m_dwMaxBlockSize)
	{
		BYTE * pdata = m_allocator.allocate(dwAllocSize);
		
		if( !pdata )
		{
			ReclaimBuffer();

			ACE_DEBUG((LM_DEBUG,"%D:mqu_mgr�����ڴ�ʧ�� size=%u.�ͷ�mqu_mgr���ڴ�.\r\n",dwBytes));

			pdata = m_allocator.allocate(dwAllocSize);

			if( !pdata )
			{
				return NULL;
			}
		}

		*(ULONG_PTR *)pdata = (ULONG_PTR)dwAllignSize;

#ifdef _DEBUG
		memcpy((BYTE *)pdata+dwAllocSize-APPED_SIZE,m_append_data,APPED_SIZE);
#endif
		return (void *)(pdata+sizeof(ULONG_PTR));
	}
	
	// �����������û��,����������
	ULONG dwIndex = GetIndex(dwAllignSize);

	m_lock.acquire();
	BYTE * x = m_pCache[dwIndex].phead;
	if (x == NULL)
	{
		m_lock.release();

		BYTE * pdata = m_allocator.allocate(dwAllocSize);

		if( !pdata )
			return NULL;

		*(ULONG_PTR *)pdata = (ULONG_PTR)dwAllignSize;

#ifdef _DEBUG
		memcpy((BYTE *)pdata+dwAllocSize-APPED_SIZE,m_append_data,APPED_SIZE);
#endif

		return (void *)(pdata+sizeof(ULONG_PTR));
	}
	
	// �ӹ�������ȡ���ڴ��
	BYTE** x_fp = (BYTE**) x;

	m_pCache[dwIndex].phead = *x_fp;

	*(ULONG_PTR*)(x) = (ULONG_PTR)dwAllignSize;

	--m_pCache[dwIndex].count;

	m_lock.release();
	
	return x+sizeof(char*);
}

/*
 *	�ͷ�
 */
template<typename LOCK,typename _A>
void Cmquemgr<LOCK,_A>::Free(void *pdata)
{
	if (pdata == NULL)
		return ;

#ifdef _DEBUG
	AssertValid(pdata);
#endif

	BYTE * x = (BYTE *)pdata - sizeof(ULONG_PTR);

	ULONG dwAllignSize = (ULONG)(*(ULONG_PTR *)(x));

	// ����ǳ����ڴ���ߣ��������ͷ�
	if (dwAllignSize > m_dwMaxBlockSize )		
	{
		m_allocator.deallocate(x,0);

		return;
	}

	ULONG dwIndex = GetIndex(dwAllignSize);

	m_lock.acquire();

	 if( m_pCache[dwIndex].count >= m_dwCacheNums ) 
	 {//�Ѿ�������̫��,�����ͷ�
		m_lock.release();

		m_allocator.deallocate(x,0);

		return;
	 }

	// ���յ���������
	BYTE** blk_fp = (BYTE**) x;

	*blk_fp = m_pCache[dwIndex].phead;

	m_pCache[dwIndex].phead = (BYTE*) x;

	++m_pCache[dwIndex].count;

	m_lock.release();
	
	return;
}

/*
 *	���¿���
 */

template<typename LOCK,typename _A>
void * Cmquemgr<LOCK,_A>::ReAlloc(void* pBlock, ULONG dwBytes)
{
	if( pBlock == NULL )
	{
		return Alloc(dwBytes);
	}

	if( dwBytes == 0 )
	{
		Free(pBlock);
		return NULL;
	}

	BYTE * x = (BYTE *)pBlock - sizeof(ULONG_PTR);

	if( *(ULONG_PTR *)x >= (ULONG_PTR)CalcuGauss(dwBytes) )
	{
		return pBlock;
	}

	void * prtn = Alloc(dwBytes);

	if( prtn )
	{
		BYTE * x = (BYTE *)pBlock - sizeof(ULONG_PTR);

		memcpy(prtn,pBlock,*(ULONG_PTR *)x);

		Free(pBlock);
	}

	return prtn;	
}

template<typename LOCK,typename _A>
void Cmquemgr<LOCK,_A>::ReclaimBuffer()
{
	if( m_pCache )
	{
		// �ͷ������ѷ�����ڴ��
		for (ULONG i = 0; i < m_dwCacheSize; i++) 
		{
			while (m_pCache[i].phead != NULL) 
			{
				BYTE** x = (BYTE **)m_pCache[i].phead;

				m_pCache[i].phead = *x;

				m_allocator.deallocate((BYTE *)x,0);
			}

			m_pCache[i].count = 0;
		}
	}

	return;
}

/*
 *	�ر�
 */
template<typename LOCK,typename _A>
void Cmquemgr<LOCK,_A>::Close()
{
	if (m_pCache != NULL)
	{
		ReclaimBuffer();

		delete []m_pCache;
		m_pCache = NULL;
	}

	m_dwAllocBlock = 0;
	m_dwCacheBank = 0;
	m_dwCacheSize = 0;
	m_dwMaxBlockSize = 0;

	return;
}


