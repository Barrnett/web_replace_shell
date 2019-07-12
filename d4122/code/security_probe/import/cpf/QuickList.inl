//////////////////////////////////////////////////////////////////////////
//QuickList.inl

//QuickList.h的实现文件

template<class TYPE>
CQuickList<TYPE>::CQuickList()
{
	m_nCount = 0;

	m_pNodeHead = (CNode *)CPF::MyMemAlloc(sizeof(CNode));

	m_pNodeHead->pPrev = m_pNodeHead;
	m_pNodeHead->pNext = m_pNodeHead;

	return;
	
}

template<class TYPE>
void CQuickList<TYPE>::RemoveAll()
{
	LOCATION loc = GetHeadPosition();

	while( !LAST(loc) )
	{
		CNode * p = (CNode *)loc;

		GetNext(loc);
		FreeNode(p);
	}

	m_pNodeHead->pPrev = m_pNodeHead;
	m_pNodeHead->pNext = m_pNodeHead;

}

template<class TYPE>
CQuickList<TYPE>::~CQuickList()
{
	RemoveAll();

	CPF::MyMemFree(m_pNodeHead);

	m_pNodeHead = NULL;

	return;
}

template<class TYPE>
void CQuickList<TYPE>::AddHead(const CQuickList& NewList)
{
	// add a list of same elements to head (maintain order)
	LOCATION pos = NewList.GetTailPosition();
	while (!NewList.LAST(pos))
	{
		AddHead(NewList.GetPrev(pos));
	}
}

template<class TYPE>
void CQuickList<TYPE>::AddTail(const CQuickList& NewList)
{
	// add a list of same elements
	LOCATION pos = NewList.GetHeadPosition();
	while (!NewList.LAST(pos))
	{
		AddTail(NewList.GetNext(pos));
	}
}


template<class TYPE>
LOCATION CQuickList<TYPE>::FindIndex(int nIndex) const
{
	if (nIndex >= m_nCount || nIndex < 0)
		return NULL;  // went too far

	CNode* pNode = GetHeadPosition();
	while (nIndex--)
	{
		pNode = pNode->pNext;
	}
	return (LOCATION) pNode;
}

template<class TYPE>
LOCATION CQuickList<TYPE>::Find(const TYPE& searchValue, LOCATION startAfter) const
{
	CNode* pNode = (CNode*) startAfter;
	if (pNode == NULL)
	{
		pNode = (CNode *)GetHeadPosition();  // start at head
	}
	else
	{
		pNode = pNode->pNext;  // start after the one specified
	}

	for (; !LAST(pNode); pNode = pNode->pNext)
	{
		// Modified by zhang.w.b
		if (pNode->data == searchValue)
			return (LOCATION)pNode;
	}

	return NULL;
}

