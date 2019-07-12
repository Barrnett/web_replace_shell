//////////////////////////////////////////////////////////////////////////
//QuickList.h

#ifndef _QUICK_LIST_H_
#define _QUICK_LIST_H_


#include "cpf/cpf.h"
#include "cpf/memmgr.h"

typedef void *	LOCATION;
/*
 *	CQuickList 的定义
 */
template<class TYPE>
class CQuickList
{
public:
	typedef struct tagCNode
	{
		struct tagCNode* pNext;
		struct tagCNode* pPrev;

		TYPE	data;

	} CNode;
	
public:
	CQuickList();

	~CQuickList();

public:
	//是否是最后一个节点
	inline BOOL LAST(LOCATION loc) const
	{	
		return ((CNode*)loc == m_pNodeHead);	
	}

	//是否不是最后一个节点
	inline BOOL NOT_LAST(LOCATION loc) const
	{	
		return !LAST(loc);	
	}

	// Attributes (head and tail)
	// count of elements
	inline int GetCount() const
	{
		return m_nCount; 
	}
	inline BOOL IsEmpty() const
	{
		return (m_nCount==0); 
	}
	
	// peek at head or tail
	inline const TYPE& GetHead() const
	{
		ACE_ASSERT(!IsEmpty());
		return GetAt(GetHeadPosition());
	}
	inline TYPE& GetHead()
	{
		ACE_ASSERT(!IsEmpty());
		return GetAt(GetHeadPosition());
	}

	inline const TYPE& GetTail() const
	{
		ACE_ASSERT(!IsEmpty());
		return GetAt(GetTailPosition());
	}
	inline TYPE& GetTail()
	{
		ACE_ASSERT(!IsEmpty());
		return GetAt(GetTailPosition());
	}
	
	// Operations
	// get head or tail (and remove it) - don't call on empty list !
	inline void RemoveHead()
	{
		ACE_ASSERT(!IsEmpty());
		RemoveAt(GetHeadPosition());
	}
	inline void RemoveTail()
	{
		ACE_ASSERT(!IsEmpty());
		RemoveAt(GetTailPosition());
	}

	// add before head or after tail
	inline LOCATION AddHead(const TYPE& newElement)
	{
		return InsertAfter(m_pNodeHead,newElement);
	}
	inline LOCATION AddTail(const TYPE& newElement)
	{
		return InsertBefore(m_pNodeHead,newElement);
	}
	
	// add another list of elements before head or after tail
	void AddHead(const CQuickList& NewList);
	void AddTail(const CQuickList& NewList);
	
	// remove all elements
	void RemoveAll();
	
	// iteration
	inline LOCATION GetHeadPosition() const
	{ 
		return (LOCATION) m_pNodeHead->pNext; 
	}
	inline LOCATION GetTailPosition() const
	{ 
		return (LOCATION) m_pNodeHead->pPrev; 
	}

	inline const TYPE& GetNext(LOCATION& rPosition) const // return *Position++
	{ 
		CNode* pNode = (CNode*) rPosition;
		rPosition = (LOCATION) pNode->pNext;
		return (pNode->data); 
	}

	inline TYPE& GetNext(LOCATION& rPosition) // return *Position++
	{ 
		CNode* pNode = (CNode*) rPosition;
		rPosition = (LOCATION) pNode->pNext;
		return (pNode->data); 
	}

	inline const TYPE& GetPrev(LOCATION& rPosition) const // return *Position--
	{ 
		CNode* pNode = (CNode*) rPosition;
		rPosition = (LOCATION) pNode->pPrev;
		return (pNode->data); 
	}

	inline TYPE& GetPrev(LOCATION& rPosition) // return *Position--
	{ 
		CNode* pNode = (CNode*) rPosition;
		rPosition = (LOCATION) pNode->pPrev;
		return (pNode->data); 
	}
	
	// getting/modifying an element at a given position
	inline const TYPE& GetAt(LOCATION position) const
	{
		CNode* pNode = (CNode*) position;
		return (pNode->data); 
	}
	inline TYPE& GetAt(LOCATION position)
	{
		CNode* pNode = (CNode*) position;
		return (pNode->data); 
	}

	inline void SetAt(LOCATION pos, const TYPE &newElement)
	{ 		
		((CNode*) pos)->data = newElement; 
	}

	inline void RemoveAt(LOCATION position)
	{
		ACE_ASSERT(!LAST(position));

		RemoveNode((CNode*) position);

		FreeNode((CNode*) position);
	}
	
	// inserting before or after a given position
	inline LOCATION InsertBefore(LOCATION position, const TYPE& newElement)
	{
		return (LOCATION)NewNodeBefore((CNode *)position,newElement);
	}
	inline LOCATION InsertAfter(LOCATION position, const TYPE& newElement)
	{
		return (LOCATION)NewNodeAfter((CNode *)position,newElement);
	}
	// helper functions (note: O(n) speed)
	LOCATION Find(const TYPE& searchValue, LOCATION startAfter = NULL) const;
	// defaults to starting at the HEAD, return NULL if not found
	LOCATION FindIndex(int nIndex) const;
	// get the 'nIndex'th element (may return NULL)

	inline void MoveToTail(LOCATION pos)
	{
		RemoveNode((CNode*)pos);
		InsertNodeBefore(m_pNodeHead,(CNode*)pos);
	}

	inline void MoveToHead(LOCATION pos)
	{
		RemoveNode((CNode*)pos);
		InsertNodeAfter(m_pNodeHead,(CNode*)pos);
	}
	
	// Implementation
protected:
	CNode * m_pNodeHead;
	int m_nCount;

	//从链表移下来一个节点，不删除这个节点
	static inline void RemoveNode(CNode* pOldNode)
	{
		pOldNode->pNext->pPrev = pOldNode->pPrev;
		pOldNode->pPrev->pNext = pOldNode->pNext;
	}

	//将thisNode插入到next之前
	static inline void InsertNodeBefore(CNode* the_node,CNode* newNode)
	{
		newNode->pPrev = the_node->pPrev;
		newNode->pNext = the_node;

		the_node->pPrev->pNext = newNode;
		the_node->pPrev = newNode;
	}

	//将thisNode插入到next之前
	static inline void InsertNodeAfter(CNode* the_node,CNode* newNode)
	{
		newNode->pPrev = the_node;
		newNode->pNext = the_node->pNext;

		the_node->pNext->pPrev = newNode;
		the_node->pNext = newNode;
	}


	//申请一个节点，并且将新的节点放到the_node之前
	inline CNode* NewNodeBefore(CNode* the_node,const TYPE& newElement )
	{
		CQuickList<TYPE>::CNode* pNewNode = AllocNode(newElement);
		InsertNodeBefore(the_node,pNewNode);
		return pNewNode;
	}

	//申请一个节点，并且将新的节点放到the_node之后
	inline CNode* NewNodeAfter(CNode* the_node, const TYPE& newElement)
	{
		CQuickList<TYPE>::CNode* pNewNode = AllocNode(newElement);
		InsertNodeAfter(the_node,pNewNode);
		return pNewNode;
	}
	
	inline CNode * AllocNode(const TYPE& element)
	{
		CQuickList<TYPE>::CNode* pNode
			= (CQuickList<TYPE>::CNode*)CPF::MyMemAlloc(sizeof(CQuickList<TYPE>::CNode));

		m_nCount++;

		::new (&pNode->data) TYPE(element);

		return pNode;
	}

	inline void FreeNode(CNode* pNode)
	{
		((TYPE *)(&pNode->data))->~TYPE();

		CPF::MyMemFree(pNode);

		m_nCount--;

	}
		
};

// ======================================================================
#if defined(OS_WINDOWS)
#pragma warning(disable : 4251)
#endif

#include "cpf/QuickList.inl"

typedef CQuickList <PVOID> CQuickPvoidList;

#endif // _QUICK_LIST_H_

