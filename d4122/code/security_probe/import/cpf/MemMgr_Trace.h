//////////////////////////////////////////////////////////////////////////
//CMemMgr_Trace.h

//用来跟踪内存管理程序中，是否有内存泄露

#include "cpf/cpf.h"
#include <set>

#pragma once

struct CMemMgr_Trace_myless
{	
	bool operator()(const LPSTR& _Left, const LPSTR& _Right) const
	{	// apply operator< to operands
		return (strcmp(_Left,_Right)<0);
	}
};


template<typename LOCK,typename MEM_MGR>
class CMemMgr_Trace
{
public:
	typedef CMemMgr_Trace<LOCK,MEM_MGR> _Myt;

private:
	typedef struct tagMem_Header
	{
		tagMem_Header * next;
		tagMem_Header * prev;
		const char *	filename;
		unsigned int	lineno;
		unsigned int	reqsize;//申请内存的大小
	}Mem_Header;

public:
	CMemMgr_Trace(MEM_MGR& Memmgr):m_Memmgr(Memmgr)
	{
		m_header.next = m_header.prev = &m_header;

		m_header.filename = NULL;
		m_header.lineno = 0;
		m_header.reqsize = 0;

		m_ulAllocBlock = 0;
	}

	~CMemMgr_Trace()
	{
		Close();
	}

	void Close();

public:
	void DumpLeak(const char * lpszFileName) const;

	inline void * Alloc(ULONG dwBytes)
	{
		if( dwBytes == 0 )
			return NULL;

		Mem_Header * pMem_Header = (Mem_Header *)m_Memmgr.Alloc(sizeof(Mem_Header)+dwBytes);

		if(pMem_Header)
		{
			on_mem_alloc(pMem_Header);

			pMem_Header->filename = NULL;
			pMem_Header->lineno = 0;

			pMem_Header->reqsize = dwBytes;

			++m_ulAllocBlock;

			return (BYTE *)pMem_Header + sizeof(Mem_Header);
		}

		return NULL;
	}

	inline void * Alloc(ULONG dwBytes,const char * filename,unsigned int lineno)
	{
		if( dwBytes == 0 )
			return NULL;

		Mem_Header * pMem_Header = (Mem_Header *)m_Memmgr.Alloc(sizeof(Mem_Header)+dwBytes);

		if(pMem_Header)
		{
			on_mem_alloc(pMem_Header);

			set_filename_lineno(pMem_Header,filename,lineno);

			pMem_Header->reqsize = dwBytes;

			++m_ulAllocBlock;

			return (BYTE *)pMem_Header + sizeof(Mem_Header);
		}

		return NULL;
	}

	inline void Free(void* pBlock)
	{
		if( !pBlock )
			return;

		Mem_Header* pMem_Header = (Mem_Header *)((BYTE *)pBlock - sizeof(Mem_Header));

		on_mem_free(pMem_Header);

		m_Memmgr.Free(pMem_Header);

		--m_ulAllocBlock;

		return;
	}

	inline void * ReAlloc(void* pBlock, ULONG dwBytes,const char * filename,unsigned int lineno)
	{
		void * pnewdata = Alloc(dwBytes,filename,lineno);

		if( pnewdata )
		{
			memcpy(pnewdata,pBlock,dwBytes);
		}

		Free(pBlock);

		return pnewdata;
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

private:
	inline void on_mem_alloc(Mem_Header * pMemHeader)
	{
		m_Memmgr.GetLock().acquire();

		pMemHeader->next = m_header.next;
		m_header.next->prev = pMemHeader;

		m_header.next = pMemHeader;
		pMemHeader->prev = &m_header;

		m_Memmgr.GetLock().release();
	}

	inline void on_mem_free(Mem_Header * pMemHeader)
	{
		m_Memmgr.GetLock().acquire();

		pMemHeader->prev->next = pMemHeader->next;
		pMemHeader->next->prev = pMemHeader->prev;

		m_Memmgr.GetLock().release();
	}

	void set_filename_lineno(Mem_Header * pMem_Header,const char *filename,unsigned int lineno);

private:

	Mem_Header	m_header;//对于已经分配出去，还没有回收的内存链表


	typedef std::set<LPSTR,CMemMgr_Trace_myless>	MY_SET;

	MY_SET		m_set_filename;

	MEM_MGR&	m_Memmgr;

	ULONG		m_ulAllocBlock;
};


template<typename LOCK,typename MEM_MGR>
void CMemMgr_Trace<LOCK,MEM_MGR>::set_filename_lineno(Mem_Header * pMem_Header,const char *filename,unsigned int lineno)
{
	if( !filename || filename[0] == 0 )
	{
		pMem_Header->filename = NULL;
		pMem_Header->lineno = 0;

		return;
	}

	m_Memmgr.GetLock().acquire();

	std::pair<MY_SET::iterator,bool> result = m_set_filename.insert((LPSTR)filename);

	if( result.second )
	{
		char * tempfilename = ACE_OS::strdup(filename);

		char * const& temp = *result.first;

		*(LPSTR *)&temp = tempfilename;		

		pMem_Header->filename = tempfilename;		
	}
	else
	{
		pMem_Header->filename = *(result.first);
	}

	m_Memmgr.GetLock().release();

	pMem_Header->lineno = lineno;

	return;
}

/*
*	输入当前的没有释放的内存块, 当lpszFileName不为空时输出到文件, 否则Trace
*/
template<typename LOCK,typename MEM_MGR>
void CMemMgr_Trace<LOCK,MEM_MGR>::DumpLeak(const char * lpszFileName) const
{
	FILE * file = fopen(lpszFileName,"wbt");

	if( !file )
		return;

	{
		time_t curtime;
		ACE_OS::time(&curtime);

		char buf_temp[1024];

		sprintf(buf_temp,"%s  - %u memmory leak(s) founded.\r\n",
			CPF::FormatTime(6,curtime),(unsigned int)m_ulAllocBlock );

		fwrite(buf_temp,1,strlen(buf_temp),file);

	}

	/*if( m_pCache )
	{
		char buf[256]={0};

		strcpy(buf,"mem used:\r\n");

		fwrite(buf,1,strlen(buf),file);

		for (ULONG i = 0; i < m_dwCacheSize; i++) 
		{
			sprintf(buf,"size=%d,alloc=%d\r\n",(i+1)*m_dwCacheBank,m_pCache[i].count);

			fwrite(buf,1,strlen(buf),file);
		}
	}*/

	char buf[MAX_PATH+32+32];

	Mem_Header * pheader = m_header.next;

	while( pheader != &m_header )
	{
		if( pheader->filename )
		{
			sprintf(buf,"%s:%u size=%u\r\n",pheader->filename,pheader->lineno,pheader->reqsize);
		}
		else
		{
			sprintf(buf,"size=%u\r\n",pheader->reqsize);
		}

		fwrite(buf,1,strlen(buf),file);

		pheader = pheader->next;

	}

	fclose(file);

	return;
}

template<typename LOCK,typename MEM_MGR>
void CMemMgr_Trace<LOCK,MEM_MGR>::Close()
{
	MY_SET::iterator it = m_set_filename.begin();

	for(; it != m_set_filename.end(); ++it )
	{
		//just for debug
#ifdef _DEBUG
		const char * pfilename = (*it);
#endif

		free((void *)(*it));

	}

	m_set_filename.clear();

	if( m_header.next != &m_header )
	{//出现了内存泄露了

		//不应该出现内存泄露
		ACE_ASSERT(FALSE);

		Mem_Header * pheader = m_header.next;

		while( pheader != &m_header )
		{
			Mem_Header * temp = pheader;

			pheader = pheader->next;

			m_Memmgr.Free(temp);
		}

		m_header.next = &m_header;
		m_header.prev = &m_header;
	}

	return;
}
