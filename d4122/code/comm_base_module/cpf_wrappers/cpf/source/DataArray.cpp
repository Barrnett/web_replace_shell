/*=========================================================
*	File name	：	DataArray.cpp
*	Authored by	：	xuxinyao
*	Date		：	2003-3-3 18:45:02
*	Description	：	CDataArray： UINT_PTR类型数据数组
*                   CBufferArray：BUFFERDATA类型数据数组
*                   struct BUFFERDATA
* 					{
* 						char *pData;//数据指针
* 						int len;//数据长度
* 					};
*   
*
*	Modify  	：	
*=========================================================*/

#include "cpf/DataArray.h"
#include "cpf/memmgr.h"

CDataArray::CDataArray()
{
	m_pData = NULL;
	m_nSize = m_nMaxSize = m_nGrowBy = 0;
}

CDataArray::CDataArray(int nNewSize, int nGrowBy /*= -1*/)
{
	m_pData = NULL;
	m_nSize = m_nMaxSize = m_nGrowBy = 0;
    SetSize(nNewSize,nGrowBy);
}


CDataArray::~CDataArray()
{

	if (m_pData != NULL)
	{
        CPF_MEM_FREE(m_pData);
	}
}


void CDataArray::SetSize(int nNewSize, int nGrowBy)
{
	ACE_ASSERT(nNewSize >= 0);

	if (nGrowBy != -1)
		m_nGrowBy = nGrowBy;  // set new size

	if (nNewSize == 0)
	{
		// shrink to nothing
		if (m_pData != NULL)
		{
			CPF_MEM_FREE(m_pData);
			m_pData = NULL;
		}
		m_nSize = m_nMaxSize = 0;
	}
	/*
	else if (m_pData == NULL)
	{
		// create one with exact size
#ifdef SIZE_T_MAX
		ACE_ASSERT(nNewSize <= SIZE_T_MAX/sizeof(UINT_PTR));    // no overflow
#endif
		m_pData =  (UINT_PTR*)CPF_MEM_ALLOC(nNewSize*sizeof(UINT_PTR));
		m_nSize = m_nMaxSize = nNewSize;
	}
	*/
	else if (nNewSize <= m_nMaxSize)
	{
		m_nSize = nNewSize;
	}
	else
	{
		// otherwise, grow array
		int nGrowBy = m_nGrowBy;
		if (nGrowBy == 0)
		{
			// heuristically determine growth when nGrowBy == 0
			//  (this avoids heap fragmentation in many situations)
			nGrowBy = m_nSize / 8;
			nGrowBy = (nGrowBy < 4) ? 4 : ((nGrowBy > 1024) ? 1024 : nGrowBy);
		}
		int nNewMax;
		if (nNewSize < m_nMaxSize + nGrowBy)
			nNewMax = m_nMaxSize + nGrowBy;  // granularity
		else
			nNewMax = nNewSize;  // no slush

		ACE_ASSERT(nNewMax >= m_nMaxSize);  // no wrap around
#ifdef SIZE_T_MAX
		ACE_ASSERT(nNewMax <= SIZE_T_MAX/sizeof(UINT_PTR)); // no overflow
#endif
		UINT_PTR* pNewData =  (UINT_PTR*)CPF_MEM_ALLOC(nNewMax*sizeof(UINT_PTR));

		// copy new data from old
		memcpy(pNewData, m_pData, m_nSize * sizeof(UINT_PTR));

		// construct remaining elements
		ACE_ASSERT(nNewSize > m_nSize);
		// get rid of old stuff (note: no destructors called)
        CPF_MEM_FREE(m_pData);
		m_pData = pNewData;
		m_nSize = nNewSize;
		m_nMaxSize = nNewMax;
	}
}


int CDataArray::Append(const CDataArray& src)
{
	ACE_ASSERT(this != &src);   // cannot append to itself

	int nOldSize = m_nSize;
	SetSize(m_nSize + src.m_nSize);
	memcpy(m_pData + nOldSize, src.m_pData, src.m_nSize*sizeof(UINT_PTR));
	return nOldSize;
}


void CDataArray::Copy(const CDataArray& src)
{
	ACE_ASSERT(this != &src);   // cannot append to itself

	SetSize(src.m_nSize);
	memcpy(m_pData, src.m_pData, src.m_nSize*sizeof(UINT_PTR));
}


void CDataArray::FreeExtra()
{

	if (m_nSize != m_nMaxSize)
	{
		// shrink to desired size
#ifdef SIZE_T_MAX
		ACE_ASSERT(m_nSize <= SIZE_T_MAX/sizeof(UINT_PTR)); // no overflow
#endif
		UINT_PTR* pNewData = NULL;
		if (m_nSize != 0)
		{

			pNewData = (UINT_PTR*)CPF_MEM_ALLOC(m_nSize*sizeof(UINT_PTR));
			// copy new data from old
			memcpy(pNewData, m_pData, m_nSize * sizeof(UINT_PTR));
		}

		// get rid of old stuff (note: no destructors called)
        CPF_MEM_FREE(m_pData);
		m_pData = pNewData;
		m_nMaxSize = m_nSize;
	}
}


void CDataArray::SetAtGrow(int nIndex, UINT_PTR newElement)
{
	ACE_ASSERT(nIndex >= 0);

	if (nIndex >= m_nSize)
		SetSize(nIndex+1, -1);
	m_pData[nIndex] = newElement;
}


void CDataArray::InsertAt(int nIndex, UINT_PTR newElement, int nCount /*=1*/)
{
	
	ACE_ASSERT(nIndex >= 0);    // will expand to meet need
	ACE_ASSERT(nCount > 0);     // zero or negative size not allowed

	if (nIndex >= m_nSize)
	{
		// adding after the end of the array
		SetSize(nIndex + nCount, -1);   // grow so nIndex is valid
	}
	else
	{
		// inserting in the middle of the array
		int nOldSize = m_nSize;
		SetSize(m_nSize + nCount, -1);  // grow it to new size
		// shift old data up to fill gap
		memmove(&m_pData[nIndex+nCount], &m_pData[nIndex],
			(nOldSize-nIndex) * sizeof(UINT_PTR));
	}

	// insert new value in the gap
	ACE_ASSERT(nIndex + nCount <= m_nSize);
	while (nCount--)
		m_pData[nIndex++] = newElement;
}


void CDataArray::RemoveAt(int nIndex, int nCount)
{
	
	ACE_ASSERT(nIndex >= 0);
	ACE_ASSERT(nCount >= 0);
	ACE_ASSERT(nIndex + nCount <= m_nSize);

	// just remove a range
	int nMoveCount = m_nSize - (nIndex + nCount);
	if (nMoveCount)
		memmove(&m_pData[nIndex], &m_pData[nIndex + nCount],
			nMoveCount * sizeof(UINT_PTR));
	m_nSize -= nCount;
}


void CDataArray::InsertAt(int nStartIndex, CDataArray* pNewArray)
{
	
	ACE_ASSERT(pNewArray != NULL);
	ACE_ASSERT(nStartIndex >= 0);

	if (pNewArray->GetSize() > 0)
	{
		InsertAt(nStartIndex, pNewArray->GetAt(0), pNewArray->GetSize());
		for (int i = 0; i < pNewArray->GetSize(); i++)
			SetAt(nStartIndex + i, pNewArray->GetAt(i));
	}
}




/////////////////////////////////////////////////////////////////////////////
// CBufferArray out-of-line functions


CBufferArray::CBufferArray()
{
	m_pData = NULL;
	m_nSize = m_nMaxSize = m_nGrowBy = 0;
}

CBufferArray::CBufferArray(int nNewSize, int nGrowBy /*= -1*/)
{
	m_pData = NULL;
	m_nSize = m_nMaxSize = m_nGrowBy = 0;
    SetSize(nNewSize,nGrowBy);
}


CBufferArray::~CBufferArray()
{

	if (m_pData != NULL)
	{
        CPF_MEM_FREE(m_pData);
	}
}


void CBufferArray::SetSize(int nNewSize, int nGrowBy)
{
	ACE_ASSERT(nNewSize >= 0);

	if (nGrowBy != -1)
		m_nGrowBy = nGrowBy;  // set new size

	if (nNewSize == 0)
	{
		// shrink to nothing
		if (m_pData != NULL)
		{
			CPF_MEM_FREE(m_pData);
			m_pData = NULL;
		}
		m_nSize = m_nMaxSize = 0;
	}
	/*
	else if (m_pData == NULL)
	{
		// create one with exact size
#ifdef SIZE_T_MAX
		ACE_ASSERT(nNewSize <= SIZE_T_MAX/sizeof(UINT_PTR));    // no overflow
#endif
		m_pData =  (BUFFERDATA*)CPF_MEM_ALLOC(nNewSize*sizeof(BUFFERDATA));
		m_nSize = m_nMaxSize = nNewSize;
	}
	*/
	else if (nNewSize <= m_nMaxSize)
	{
		m_nSize = nNewSize;
	}
	else
	{
		// otherwise, grow array
		int nGrowBy = m_nGrowBy;
		if (nGrowBy == 0)
		{
			// heuristically determine growth when nGrowBy == 0
			//  (this avoids heap fragmentation in many situations)
			nGrowBy = m_nSize / 8;
			nGrowBy = (nGrowBy < 4) ? 4 : ((nGrowBy > 1024) ? 1024 : nGrowBy);
		}
		int nNewMax;
		if (nNewSize < m_nMaxSize + nGrowBy)
			nNewMax = m_nMaxSize + nGrowBy;  // granularity
		else
			nNewMax = nNewSize;  // no slush

		ACE_ASSERT(nNewMax >= m_nMaxSize);  // no wrap around
#ifdef SIZE_T_MAX
		ACE_ASSERT(nNewMax <= SIZE_T_MAX/sizeof(UINT_PTR)); // no overflow
#endif
		BUFFERDATA* pNewData =  (BUFFERDATA*)CPF_MEM_ALLOC(nNewMax*sizeof(BUFFERDATA));

		// copy new data from old
		memcpy(pNewData, m_pData, m_nSize * sizeof(BUFFERDATA));

		// construct remaining elements
		ACE_ASSERT(nNewSize > m_nSize);
		// get rid of old stuff (note: no destructors called)
        CPF_MEM_FREE(m_pData);
		m_pData = pNewData;
		m_nSize = nNewSize;
		m_nMaxSize = nNewMax;
	}
}


int CBufferArray::Append(const CBufferArray& src)
{
	ACE_ASSERT(this != &src);   // cannot append to itself

	int nOldSize = m_nSize;
	SetSize(m_nSize + src.m_nSize);
	memcpy(m_pData + nOldSize, src.m_pData, src.m_nSize*sizeof(BUFFERDATA));
	return nOldSize;
}


void CBufferArray::Copy(const CBufferArray& src)
{
	ACE_ASSERT(this != &src);   // cannot append to itself

	SetSize(src.m_nSize);
	memcpy(m_pData, src.m_pData, src.m_nSize*sizeof(BUFFERDATA));
}


void CBufferArray::FreeExtra()
{

	if (m_nSize != m_nMaxSize)
	{
		// shrink to desired size
#ifdef SIZE_T_MAX
		ACE_ASSERT(m_nSize <= SIZE_T_MAX/sizeof(UINT_PTR)); // no overflow
#endif
		BUFFERDATA* pNewData = NULL;
		if (m_nSize != 0)
		{

			pNewData = (BUFFERDATA*)CPF_MEM_ALLOC(m_nSize*sizeof(BUFFERDATA));
			// copy new data from old
			memcpy(pNewData, m_pData, m_nSize * sizeof(BUFFERDATA));
		}

		// get rid of old stuff (note: no destructors called)
        CPF_MEM_FREE(m_pData);
		m_pData = pNewData;
		m_nMaxSize = m_nSize;
	}
}


void CBufferArray::SetAtGrow(int nIndex, BUFFERDATA newElement)
{
	ACE_ASSERT(nIndex >= 0);

	if (nIndex >= m_nSize)
		SetSize(nIndex+1, -1);
	m_pData[nIndex] = newElement;
}


void CBufferArray::InsertAt(int nIndex, BUFFERDATA newElement, int nCount /*=1*/)
{
	
	ACE_ASSERT(nIndex >= 0);    // will expand to meet need
	ACE_ASSERT(nCount > 0);     // zero or negative size not allowed

	if (nIndex >= m_nSize)
	{
		// adding after the end of the array
		SetSize(nIndex + nCount, -1);   // grow so nIndex is valid
	}
	else
	{
		// inserting in the middle of the array
		int nOldSize = m_nSize;
		SetSize(m_nSize + nCount, -1);  // grow it to new size
		// shift old data up to fill gap
		memmove(&m_pData[nIndex+nCount], &m_pData[nIndex],
			(nOldSize-nIndex) * sizeof(BUFFERDATA));
	}

	// insert new value in the gap
	ACE_ASSERT(nIndex + nCount <= m_nSize);
	while (nCount--)
		m_pData[nIndex++] = newElement;
}


void CBufferArray::RemoveAt(int nIndex, int nCount)
{
	
	ACE_ASSERT(nIndex >= 0);
	ACE_ASSERT(nCount >= 0);
	ACE_ASSERT(nIndex + nCount <= m_nSize);

	// just remove a range
	int nMoveCount = m_nSize - (nIndex + nCount);
	if (nMoveCount)
		memmove(&m_pData[nIndex], &m_pData[nIndex + nCount],
			nMoveCount * sizeof(BUFFERDATA));
	m_nSize -= nCount;
}


void CBufferArray::InsertAt(int nStartIndex, CBufferArray* pNewArray)
{
	
	ACE_ASSERT(pNewArray != NULL);
	ACE_ASSERT(nStartIndex >= 0);

	if (pNewArray->GetSize() > 0)
	{
		InsertAt(nStartIndex, pNewArray->GetAt(0), pNewArray->GetSize());
		for (int i = 0; i < pNewArray->GetSize(); i++)
			SetAt(nStartIndex + i, pNewArray->GetAt(i));
	}
}




