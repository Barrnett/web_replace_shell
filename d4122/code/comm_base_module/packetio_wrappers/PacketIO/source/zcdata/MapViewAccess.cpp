
#include "MapViewAccess.h"

/*
 *	
 */
CMapViewAccess::CMapViewAccess()
{
	m_nMaxBufSize = 1024*1024;
	m_hMap = NULL;
	m_pMapBuffer = NULL;
	m_ullOffset = 0;
	m_ullMapSize = 0;
	m_nMemfSize = 0;        // msync and munmap need the real mapped size
}

/*
 *	
 */
CMapViewAccess::~CMapViewAccess()
{
	Close();
}

/*
 *	
 */
void CMapViewAccess::Close()
{
	if (m_pMapBuffer != NULL)
	{
		CommitAndUnMap();
	}
	m_pMapBuffer = NULL;
}

/*
 *	
 */
BOOL CMapViewAccess::SetMaxBufferSize(int nMaxBufSize)
{
	if (nMaxBufSize < 1024)
		return FALSE;

	if (m_pMapBuffer != NULL)
	{
		CommitAndUnMap();
	}

	m_pMapBuffer = NULL;
	m_nMaxBufSize = nMaxBufSize;
	return TRUE;
}

/*
 *	
 */
BOOL CMapViewAccess::SetMapHandle(HANDLE hMap,ULONGLONG ullMapSize,DWORD dwDesiredAccess)
{
	if (hMap == NULL)
		return FALSE;

	if (m_pMapBuffer != NULL)
		UnMap();

	m_pMapBuffer = NULL;
	m_nMemfSize = 0;
	
	m_hMap = hMap;
	m_ullMapSize = ullMapSize;
	m_dwDesiredAccess = dwDesiredAccess;
	return TRUE;
}

/*
 *	
 */
BYTE* CMapViewAccess::GetMemory(ULONGLONG ullOffset, int nSize)
{
	ACE_ASSERT(m_nMaxBufSize >= nSize);
	if(m_nMaxBufSize<nSize)
		return NULL;
	
	if (m_pMapBuffer != NULL && ullOffset>m_ullOffset 
		&& (ullOffset+nSize) <= (m_ullOffset+m_nMemfSize)) //在内存中
		return &m_pMapBuffer[ullOffset-m_ullOffset];

	//取消以前的映射
	if (m_pMapBuffer != NULL)
	{
		CommitAndUnMap();
	}

	m_pMapBuffer = NULL;
	m_nMemfSize = 0;

	if (m_hMap == NULL)
		return NULL;

	//计算新的映射地址
	m_ullOffset = (ullOffset / (64*1024)) * (64*1024); //按64K对齐
	int nAlignOffset = (int)(ullOffset - m_ullOffset);	//对齐浪费的字节
	m_nMemfSize = nAlignOffset + m_nMaxBufSize; //实际映射内存的大小

	if(m_ullOffset+m_nMemfSize > m_ullMapSize)	//到达尾部,直接映射到尾部
		m_nMemfSize =(int) (m_ullMapSize - m_ullOffset);

#if defined(OS_WINDOWS)
	m_pMapBuffer = (BYTE *)MapViewOfFile(m_hMap, m_dwDesiredAccess,
		HIDWORD(m_ullOffset), LODWORD(m_ullOffset),m_nMemfSize);
#elif defined(OS_LINUX)
	int prot = 0;
	if(FILE_MAP_READ==m_dwDesiredAccess)
		prot = PROT_READ;
	else
		prot = PROT_READ|PROT_WRITE;
	int iTemp  = (int)mmap64(0, m_nMemfSize, prot, MAP_SHARED, (int)m_hMap, m_ullOffset);
	if(iTemp == -1)
		m_pMapBuffer = NULL;
	else
		m_pMapBuffer =(BYTE *)iTemp;
#endif

	if (m_pMapBuffer == NULL)
		return NULL;

	return &m_pMapBuffer[nAlignOffset];
}

void CMapViewAccess::Commit()
{
#if defined(OS_WINDOWS)
	if(FILE_MAP_READ != m_dwDesiredAccess)
		FlushViewOfFile(m_pMapBuffer,m_nMemfSize);
#elif defined(OS_LINUX)
	if(FILE_MAP_READ != m_dwDesiredAccess)
		msync(m_pMapBuffer, m_nMemfSize, MS_SYNC);
#endif

	return;
}


void CMapViewAccess::UnMap()
{
#if defined(OS_WINDOWS)
	UnmapViewOfFile(m_pMapBuffer);
#elif defined(OS_LINUX)
	munmap(m_pMapBuffer, m_nMemfSize);
#endif

	return;
}



