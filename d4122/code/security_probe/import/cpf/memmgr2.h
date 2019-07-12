//////////////////////////////////////////////////////////////////////////
//memmgr2.h

#ifndef  MEMMGR2_H_2006_06_06
#define  MEMMGR2_H_2006_06_06
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

#include "cpf/cpf.h"
#include "cpf/HeapMgr.h"
#include "cpf/mquemgr.h"
#include "cpf/MemMgr_Trace.h"

template< typename LOCK=ACE_Null_Mutex,typename _A = std::allocator<BYTE> >
class CMemMgr_Trace_Heap
{
public:
	CMemMgr_Trace_Heap()
	{
		m_mgr_trace = NULL;
		m_heap_mgr = NULL;
		m_b_create_heap = false;
	}

	~CMemMgr_Trace_Heap()
	{
		close();
	}

	bool init(CHeapMgr<LOCK,_A> * heap_mgr)
	{
		m_heap_mgr = heap_mgr;

		m_mgr_trace = new CMemMgr_Trace<LOCK,_A>(*m_heap_mgr);

		m_b_create_heap = false;

		return true;
	}

	bool init(ULONG heap_size)
	{
		m_heap_mgr = new CHeapMgr<LOCK,_A>;

		if( m_heap_mgr )
		{
			m_heap_mgr->init(heap_size);

			m_b_create_heap = true;
		}

		m_mgr_trace = new CMemMgr_Trace< LOCK,CHeapMgr<LOCK,_A> >(*m_heap_mgr);

		return true;
	}

	void close()
	{
		if( m_mgr_trace )
		{
			m_mgr_trace->Close();
			delete m_mgr_trace;
			m_mgr_trace = NULL;
		}

		if( m_b_create_heap )
		{
			m_heap_mgr->close();
			delete m_heap_mgr;
			m_heap_mgr = NULL;
		}
	}

public:
	void DumpLeak(const char * lpszFileName) const
	{
		m_mgr_trace->DumpLeak(lpszFileName);
	}

	inline void * Alloc(ULONG dwBytes)
	{
		return m_mgr_trace->Alloc(dwBytes);
	}

	inline void * Alloc(ULONG dwBytes,const char * filename,unsigned int lineno)
	{
		return m_mgr_trace->Alloc(dwBytes,filename,lineno);

	}

	inline void Free(void* pBlock)
	{
		return m_mgr_trace->Free(pBlock);
	}

	inline void * ReAlloc(void* pBlock, ULONG dwBytes,const char * filename,unsigned int lineno)
	{
		return m_mgr_trace->ReAlloc(pBlock,dwBytes,filename,lineno);
	}

	inline void * ReAlloc(void* pBlock, ULONG dwBytes)
	{
		return m_mgr_trace->ReAlloc(pBlock,dwBytes);
	}

private:
	CHeapMgr<LOCK,_A> *							m_heap_mgr;
	CMemMgr_Trace< LOCK,CHeapMgr<LOCK,_A> > *	m_mgr_trace;
	bool										m_b_create_heap;
};

typedef CMemMgr_Trace_Heap< ACE_Null_Mutex,std::allocator<BYTE> >					CTraceHeapMgr_no_mtsafe;
typedef CMemMgr_Trace_Heap< ACE_Recursive_Thread_Mutex,std::allocator<BYTE> >		CTraceHeapMgr_mtsafe;


template< typename LOCK=ACE_Null_Mutex,typename _A = std::allocator<BYTE> >
class CMemMgr_Trace_Mque
{
public:
	CMemMgr_Trace_Mque()
	{
		m_mgr_trace = NULL;
		m_mque_mgr = NULL;
		m_b_create_mque = false;
	}

	~CMemMgr_Trace_Mque()
	{
		close();
	}

	bool init(Cmquemgr<LOCK,_A> * mque_mgr)
	{
		m_mque_mgr = mque_mgr;

		m_mgr_trace = new CMemMgr_Trace<LOCK,_A>(*m_mque_mgr);

		m_b_create_mque = false;

		return true;
	}

	bool init(ULONG dwMaxBlockSize = 1024, 
		ULONG dwCacheBank = 16,
		ULONG dwPreAllocBlocks = 0,
		ULONG dwCacheNums = -1)
	{
		m_mque_mgr = new Cmquemgr<LOCK,_A>(dwMaxBlockSize,dwCacheBank,dwPreAllocBlocks,dwCacheNums);

		if( m_mque_mgr )
		{
			m_b_create_mque = true;
		}

		m_mgr_trace = new CMemMgr_Trace< LOCK,Cmquemgr<LOCK,_A> >(*m_mque_mgr);

		return true;
	}

	void close()
	{
		if( m_mgr_trace )
		{
			m_mgr_trace->Close();
			delete m_mgr_trace;
			m_mgr_trace = NULL;
		}

		if( m_b_create_mque )
		{
			delete m_mque_mgr;
			m_mque_mgr = NULL;
		}
	}

public:
	void DumpLeak(const char * lpszFileName) const
	{
		m_mgr_trace->DumpLeak(lpszFileName);
	}

	inline void * Alloc(ULONG dwBytes)
	{
		return m_mgr_trace->Alloc(dwBytes);
	}

	inline void * Alloc(ULONG dwBytes,const char * filename,unsigned int lineno)
	{
		return m_mgr_trace->Alloc(dwBytes,filename,lineno);

	}

	inline void Free(void* pBlock)
	{
		return m_mgr_trace->Free(pBlock);
	}

	inline void * ReAlloc(void* pBlock, ULONG dwBytes,const char * filename,unsigned int lineno)
	{
		return m_mgr_trace->ReAlloc(pBlock,dwBytes,filename,lineno);
	}

	inline void * ReAlloc(void* pBlock, ULONG dwBytes)
	{
		return m_mgr_trace->ReAlloc(pBlock,dwBytes);
	}

private:
	Cmquemgr<LOCK,_A> *						m_mque_mgr;
	CMemMgr_Trace< LOCK,Cmquemgr<LOCK,_A> > *	m_mgr_trace;
	bool									m_b_create_mque;
};

typedef CMemMgr_Trace_Mque< ACE_Null_Mutex,std::allocator<BYTE> >					CTraceMqueMgr_no_mtsafe;
typedef CMemMgr_Trace_Mque< ACE_Recursive_Thread_Mutex,std::allocator<BYTE> >		CTraceMqueMgr_mtsafe;

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
#endif//MEMMGR2_H_2006_06_06

