//////////////////////////////////////////////////////////////////////////
//HeapMgr.h

#pragma once

#include "ace/Null_Mutex.h"
#include "ace/Thread_Mutex.h"
#include "ace/Recursive_Thread_Mutex.h"

#include "cpf/cpf.h"
#include "cpf/HeapMgr_Alg.h"
#include "cpf/HeapMgr_Alg2.h"

template< typename LOCK=ACE_Null_Mutex,typename HEAP_MGR_ALG=CHeapMgr_Alg2,typename _A = std::allocator<BYTE> >
class CHeapMgr
{
public:
	CHeapMgr()
	{
	}

	~CHeapMgr()
	{
		close();
	}

	inline BOOL init(ULONG total_len,ULONG minsize=64)
	{
		return m_heap_alg.init(total_len,minsize);
	}
	inline void close()
	{
		m_heap_alg.close();
	}

	//���Ƿ����ڴ�й¶�����Ƿ���ͨ����ģ��������ڴ棬û���ͷţ�
	//������ڴ�й¶������true.
	inline BOOL CheckLeak() const
	{
		return m_heap_alg.CheckLeak();
	}

public:
	inline void * Alloc(ULONG dwBytes)
	{
		if( dwBytes == 0 )
			return NULL;

		void * pnewdata = m_heap_alg.Alloc(dwBytes);

		if( !pnewdata )
		{
			if( m_heap_alg.GetTotalLength() != 0 )
			{
				//ACE_DEBUG((LM_DEBUG,"%D:heap �����ڴ� size=%u ʧ��\r\n",dwBytes));

			}

			pnewdata = m_allocator.allocate(dwBytes,0);

			if( pnewdata )
			{
				if( m_heap_alg.GetTotalLength() != 0 )
				{
					//ACE_DEBUG((LM_DEBUG,"%D:ϵͳ �����ڴ� size=%u �ɹ�\r\n",dwBytes));
				}
			}
			else
			{
				//ACE_DEBUG((LM_DEBUG,"%D:ϵͳ �����ڴ� size=%u ʧ��\r\n",dwBytes));
			}
		}

		return pnewdata;
	}

	inline void Free(void* pBlock)
	{
		if( !m_heap_alg.IsInMgr(pBlock) )
		{//��������ķ�Χ���϶���������������ڴ档
			m_allocator.deallocate((BYTE *)pBlock,0);
			return;
		}

		m_heap_alg.Free(pBlock);

		return;
	}

	inline void * ReAlloc(void* pBlock, ULONG dwBytes)
	{
		void * pnewdata = Alloc(dwBytes);

		if( pnewdata )
		{
			memcpy(pnewdata,pBlock,dwBytes);			
		}

		Free(pBlock);

		return pnewdata;
	}

	inline LOCK& GetLock()
	{
		return m_lock;
	}

	inline ULONG GetAvailableLength() const
	{
		return m_heap_alg.GetAvailableLength();
	}

	inline ULONG GetTotalLength() const
	{
		return m_heap_alg.GetTotalLength();
	}

private:
	HEAP_MGR_ALG	m_heap_alg;

private:
	LOCK	m_lock;

	_A		m_allocator;	// �ڴ������

};

typedef CHeapMgr<ACE_Null_Mutex>					CHeapMgr_no_mtsafe;
typedef CHeapMgr<ACE_Recursive_Thread_Mutex>		CHeapMgr_mtsafe;

