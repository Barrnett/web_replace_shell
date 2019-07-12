//////////////////////////////////////////////////////////////////////////
//memmgr.cpp

#include "cpf/memmgr.h"
#include "cpf/memmgr2.h"
#include "cpf/path_tools.h"

CHeapMgr< ACE_Recursive_Thread_Mutex,CHeapMgr_Alg2,CPF::CPF_malloc_allocator<BYTE> > *						g_pHeapMgr_mtsafe;

bool init_heap_memmgr(ULONG heap_total_len)
{
	g_pHeapMgr_mtsafe = new CHeapMgr< ACE_Recursive_Thread_Mutex,CHeapMgr_Alg2,CPF::CPF_malloc_allocator<BYTE> > ;

	if( !g_pHeapMgr_mtsafe )
	{
		return false;
	}

	if( !g_pHeapMgr_mtsafe->init(heap_total_len) )
	{
		delete g_pHeapMgr_mtsafe;
		g_pHeapMgr_mtsafe = NULL;

		return false;
	}

	return true;
}


void close_heap_memmgr()
{
	if( g_pHeapMgr_mtsafe )
	{
		ACE_ASSERT(!g_pHeapMgr_mtsafe->CheckLeak());

		g_pHeapMgr_mtsafe->close();
		delete g_pHeapMgr_mtsafe;
		g_pHeapMgr_mtsafe = NULL;
	}

	return;
}

template<typename DATA_TYPE>
class Heap_allocator
{
public:
	inline DATA_TYPE * allocate(size_t count,const void *pbegin)
	{
		return (DATA_TYPE *) g_pHeapMgr_mtsafe->Alloc((UINT)count);
	}
	inline DATA_TYPE * allocate(size_t count)
	{
		return allocate(count,(const void *)0);
	}

	inline void deallocate(DATA_TYPE *pdata,size_t datalen)
	{
		g_pHeapMgr_mtsafe->Free((void *)pdata);
	}

	inline void deallocate(DATA_TYPE *pdata)
	{
		g_pHeapMgr_mtsafe->Free((void *)pdata);
	}
};

typedef CMemMgr_Trace_Mque< ACE_Recursive_Thread_Mutex,Heap_allocator<BYTE> >		CTraceMqueMgr_mtsafe_Heap;

CTraceMqueMgr_mtsafe_Heap* g_pMquMgr_mtsafe = NULL;

bool init_mque_memmgr()
{
	g_pMquMgr_mtsafe = new CTraceMqueMgr_mtsafe_Heap;

	if( !g_pMquMgr_mtsafe )
	{
		return false;
	}

	if( !g_pMquMgr_mtsafe->init(2048,32,0,100) )
	{
		delete g_pMquMgr_mtsafe;
		g_pMquMgr_mtsafe = NULL;

		return false;
	}

	return true;
}

void close_mque_memmgr(int log_leak)
{
	if( g_pMquMgr_mtsafe )
	{
		if( log_leak )
		{
			char module_file_name[MAX_PATH];

			CPF::GetModuleFileName(module_file_name,sizeof(module_file_name));

			strcat(module_file_name,".mem_leak.txt");

			g_pMquMgr_mtsafe->DumpLeak(module_file_name);
		}		

		g_pMquMgr_mtsafe->close();
		delete g_pMquMgr_mtsafe;

		g_pMquMgr_mtsafe = NULL;
	}

	return;
}

bool init_memmgr(ULONG heap_size)
{
	init_heap_memmgr(heap_size);
	init_mque_memmgr();

	return true;
}

void close_memmgr(int log_leak)
{
	close_mque_memmgr(log_leak);
	close_heap_memmgr();

	return;
}
//////////////////////////////////////////////////////////////////////////

BYTE * CPF::MyMemAlloc(UINT bytes)
{
	return (BYTE *)g_pMquMgr_mtsafe->Alloc(bytes);
}

BYTE * CPF::MyMemAlloc(UINT bytes,const char * filename,unsigned int lineno)
{
	return (BYTE *)g_pMquMgr_mtsafe->Alloc(bytes,filename,lineno);
} 

BYTE * CPF::MyMemReAlloc(void* block,UINT bytes)
{
	return (BYTE *)g_pMquMgr_mtsafe->ReAlloc(block,bytes);
}

BYTE * CPF::MyMemReAlloc(void* block,UINT bytes,const char * filename,unsigned int lineno)
{
	return (BYTE *)g_pMquMgr_mtsafe->ReAlloc(block,bytes,filename,lineno);
}

void CPF::MyMemFree(void* block)
{
	g_pMquMgr_mtsafe->Free(block);
}

void  CPF::MyMemReserve(size_t blocksize, size_t blocknums)
{	
	//g_pMquMgr_mtsafe->Reserve((ULONG)blocksize,(ULONG)blocknums);	
}

