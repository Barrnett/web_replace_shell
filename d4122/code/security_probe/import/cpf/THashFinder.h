#if !defined(AFX_THASHFINDER_H__2F43062B_5DB9_4A06_A250_7F84A7A70CAA__INCLUDED_)
#define AFX_THASHFINDER_H__2F43062B_5DB9_4A06_A250_7F84A7A70CAA__INCLUDED_

//这个类是为了HASH算法提供的表的操作。其中要求用户提供HASH函数和比较函数
//这个类允许用户进行增加元素和删除元素。

//但是这个类没有为增加元素和删除元素提供同步。
//如果一个线程在增加元素，另外一个线程在删除元素（或者增加元素），整个程序将会出错。
//用户应该自己提供同步机制

#include "cpf/cpf.h"
#include "cpf/Common_Func_Macor.h"

#ifdef OS_WINDOWS
#pragma warning(disable : 4251)
#endif

#ifdef OS_LINUX
#	define _FARQ
#endif

namespace thashfinder
{
	template<class _T1> inline
		void _Construct(_T1 *_Ptr)
	{
		void *_Vptr = _Ptr;
		::new (_Vptr) _T1();
	}

	template<class _Ty> inline
		void _Destroy(_Ty *_Ptr)
	{
		(_Ptr)->~_Ty();
	}
}


template <class T>
class CTHashFinder  
{
public:
	enum{
		HASH_TABLE_DEFAULT_SIZE = 65536,
			HASH_TABLE_DEFAULT_BOUNDER = (HASH_TABLE_DEFAULT_SIZE-1)
	};
	
private:
	enum{ FLAG_LAST = -1,FLAG_UNUSED = -2 };
	
	typedef struct{
		
		int next;//一个hash值的双向链表
		int prev;//一个hash值的双向链表		
	
		unsigned int	dwData;

		BOOL	bRealData;//数据是否确实有效
		
	}BLOCK_ORDER, *LPBLOCK_ORDER;

	typedef struct {

		int Lnext;//在整体使用链表中下一个的指针
		int	Lprev;//在整体使用链表中前一个指针

	}USED_NODE, *LPUSED_NODE;	
	
public:	
	//bSupportLink是否支持将所有已经分配的节点连接起来。
	//如果支持，那么在遍历所有已经使用节点将很快	
	bool Create(size_t blocknums, size_t nHashsize, BOOL bSupportLink, T* pData = NULL);

	void Destroy();

	BOOL IsCreateOK() const
	{
		return (m_pHashTable!=NULL);
	}

public:
	inline static BOOL EOL(int index)
	{	return (index==FLAG_LAST);	}

	inline BOOL IsValidIndex(int index) const
	{	return ( index >= 0 && index < m_nBlocknums );	}

	//是不是有效的被用户使用的块
	inline BOOL IsValidBlock(int index) const
	{	ACE_ASSERT( IsValidIndex(index) );	return( m_pOrder[index].prev != FLAG_UNUSED );	}

	inline BOOL IsValidHashIndex(int hashindex) const
	{	return( hashindex >= 0 && hashindex <= m_nHashTableBoundary ); }

	inline BOOL IsSupportLink() const	
	{	return m_bSupportLink;	}

	//是否是实际插入的值
	inline BOOL IsRealDataBlock(int index) const
	{	return( m_pOrder[index].bRealData );	}

	inline T* GetDataByIndex(int index) const
	{	if( CTHashFinder<T>::EOL(index) )	return NULL;	return m_pData+index;	}
	inline int GetIndexByData(const T * pData) const
	{	if( pData == NULL ) return FLAG_LAST;	return static_cast<int>(pData-m_pData);	}

	inline T* GetData() const{	return m_pData;	}
	inline void SetData(T* pDataHead) {	m_pData = pDataHead;	}


private:
	void DeleteNodeInLink(int index);
	void AddNodeToLinkTail(int index);
	void AddNodeToLinkHead(int index);

	int AllocIndex(int hashindex,BOOL bRealData);

public:

	//NULL表示失败
	inline T* AllocBlock(int nHashIndex,BOOL bRealData)
	{	return GetDataByIndex(AllocIndex(nHashIndex,bRealData));	}

	inline void SetItemData(int nIndex,unsigned int dwData)
	{	ACE_ASSERT(IsValidBlock(nIndex));	m_pOrder[nIndex].dwData = dwData;	}
	inline unsigned int GetItemData(int nIndex) const
	{	ACE_ASSERT(IsValidBlock(nIndex));	return m_pOrder[nIndex].dwData;	}	

	//得到下一个要分配的块，但是没有分配,-1表示没有了
	inline int GetFirstFreeBlock() const
	{	return m_nFreeHeader;	}
	
	//fanhui目前可用总数
	inline void FreeBlock(const T* pData)
	{	FreeBlock((int)(pData-m_pData));	}
	
	void FreeBlock(int index);	
	void FreeFindBlock(int nHashIndex);
	void FreeAllBlock();

	//释放头一个节点（最旧的），如果有则返回节点的序号，如果没有返回-1
	//只有在bSupportLink＝TRUE时才能使用
	inline int FreeHead()
	{
		int index = GetHeadBlock();

		if( index != -1 )
			FreeBlock(index);

		return index;
	}
	//释放最后一个节点（最新的），如果有则返回节点的序号，如果没有返回-1
	//只有在bSupportLink＝TRUE时才能使用
	inline int FreeTail()
	{
		int index = GetTailBlock();

		if( index != -1 )
			FreeBlock(index);

		return index;
	}
	
	//return the number as blocknums the param of create
	inline int	GetTotalCount() const
	{ return m_nBlocknums;}
	inline int GetUsedCount() const
	{ return m_nBlocknums - m_nUnUsedCount; }
	inline int GetUnUsedCount() const
	{ return m_nUnUsedCount; }
	

public:
	//遍历所有已经分配的块：从最旧的到最新的。只有在bSupportLink＝TRUE时才能使用
	//EOL(index)表示结束
	inline int	GetAllHeadBlock() const
	{	return m_nUsedHeader;	}

	inline T* GetAllNextBlockFromHead(int & index ) const
	{
		ACE_ASSERT(m_bSupportLink && IsValidBlock(index));	
		T* pData = m_pData+index;
		index = m_pUsedLink[index].Lnext;	
		return pData;
	}


	//遍历所有已经分配的块：从最新的到最旧的。只有在bSupportLink＝TRUE时才能使用
	//EOL(index)表示结束
	inline int	GetAllTailBlock() const
	{	return m_nUsedTail;	}

	inline T*	GetAllNextBlockFromTail(int& index) const
	{
		ACE_ASSERT(m_bSupportLink&&IsValidBlock(index));	
		T* pData = m_pData+index;
		index = m_pUsedLink[index].Lprev;
		return pData;
	}

	//遍历所有已经分配的块：只有在bSupportLink＝FALSE时才能使用
	//EOL(index)表示结束
	inline int GetAllFirstUsedBlock() const
	{
		for(int i = 0;i < m_nBlocknums; ++i )
		{
			if( IsValidBlock(i) )
				return i;
		}

		return -1;
	}

	inline T* GetAllNextUsedBlock(int& index) const
	{
		ACE_ASSERT(!m_bSupportLink&&IsValidBlock(index));	

		T* pData = m_pData + index;

		while( ++index < m_nBlocknums )
		{
			if( IsValidBlock(index) )
				break;
		}

		if( index >= m_nBlocknums )
			index = -1;

		return pData;		
	}


public:
	//遍历所有已经分配，并且有实际数据的块，,从最旧的到最新的。只有在bSupportLink＝TRUE时才能使用
	//EOL(index)标志结束
	inline int	GetHeadBlock() const
	{		
		int index = GetAllHeadBlock();
		
		while( !EOL(index) && IsRealDataBlock(index)== FALSE )
		{
			GetAllNextBlockFromHead(index);
		}
		
		return index;
	}
	
	inline T* GetNextBlockFromHead(int & index ) const
	{
		ACE_ASSERT(m_bSupportLink);
		
		T* pData = GetAllNextBlockFromHead(index);
		
		while( !EOL(index)  && IsRealDataBlock(index) == FALSE )
		{
			GetAllNextBlockFromHead(index);
		}

		return pData;
	}
	
	
	//遍历所有已经分配，并且有实际数据的块，从最新的到最旧的。只有在bSupportLink＝TRUE时才能使用
	//EOL(index)标志结束
	inline int	GetTailBlock() const
	{		
		int index = GetAllTailBlock();
		
		while( !EOL(index)  && IsRealDataBlock(index) == FALSE )
		{
			GetAllNextBlockFromTail(index);
		}
		
		return index;
	}
	
	inline T*	GetNextBlockFromTail(int& index) const
	{
		ACE_ASSERT(m_bSupportLink);
		
		T* pData = GetAllNextBlockFromTail(index);
		
		while( !EOL(index)  && IsRealDataBlock(index) == FALSE )
		{
			GetAllNextBlockFromTail(index);		
		}
		
		return pData;
	}
	
	//遍历所有已经分配，并且有实际数据的块：只有在bSupportLink＝FALSE时才能使用
	//EOL(index)表示结束,这是一种比较慢的遍历方式,
	//如果bSupportLink==TRUE,请使用	GetHeadBlock(GetNextBlockFromHead)或者GetTailBlock(GetNextBlockFromTail)
	inline int GetFirstUsedBlock() const
	{
		int index = GetAllFirstUsedBlock();
		
		while( !EOL(index) && IsRealDataBlock(index) == FALSE )
		{
			GetAllNextUsedBlock(index);
		}		

		return index;
	}
	
	inline T* GetNextUsedBlock(int& index) const
	{
		ACE_ASSERT(!m_bSupportLink);

		T* pData = GetAllNextUsedBlock(index);

		while( !EOL(index) && IsRealDataBlock(index) == FALSE )
		{
			GetAllNextUsedBlock(index);
		}	
		
		return pData;	
	}
		
	//将index移到末尾。将节点变成最新的节点。只有在bSupportLink＝TRUE时才能使用
	inline void MoveToTail(int index)
	{
		ACE_ASSERT(m_bSupportLink);
		
		//确认这个数据被占用
		ACE_ASSERT( IsValidBlock(index) );
		
		//已经是最后了
		if( index != m_nUsedTail )
		{	
			DeleteNodeInLink(index);
			AddNodeToLinkTail(index);	
		}
		
		return;		
	}
	

	//将index移到末尾。将节点变成最新的节点。只有在bSupportLink＝TRUE时才能使用
	inline void MoveToHead(int index)
	{
		ACE_ASSERT(m_bSupportLink);

		//确认这个数据被占用
		ACE_ASSERT( IsValidBlock(index) );

		//已经是最后了
		if( index != m_nUsedHeader )
		{	
			DeleteNodeInLink(index);
			AddNodeToLinkHead(index);	
		}

		return;		
	}
	
	BOOL CopyTo(CTHashFinder<T>& finder,BOOL bCopyData) const;	


public:
	CTHashFinder()
		:m_pOrder(NULL),m_pUsedLink(NULL),
		m_nBlocknums(0),m_nHashTableBoundary(0),
		m_pHashTable(NULL),m_nFreeHeader(FLAG_LAST),
		m_nUnUsedCount(0),m_nUsedHeader(FLAG_LAST),
		m_nUsedTail(FLAG_LAST),m_bSupportLink(FALSE),
		m_bCreateBuffer(FALSE),m_pData(NULL)
	{
		return;
	}
	
	CTHashFinder(const CTHashFinder<T>& hashFinder)
	{
		ACE_ASSERT(FALSE);	
		
		*this = hashFinder;
		return;		
	}

	CTHashFinder<T>& operator = (const CTHashFinder<T>& hashFinder)
	{
		ACE_ASSERT(FALSE);
		
		ACE_ASSERT( this != &hashFinder );
		
		if( this == &hashFinder )
			return *this;
		
		hashFinder.CopyTo(*this,TRUE);
		
		return *this;
	}
	
	~CTHashFinder()
	{
		Destroy();
		return;		
	}
		
public:

		
	BOOL CopyDataTo(CTHashFinder<T>& finder) const;

public:
		
	//根据HASH函数返回后的值进行搜索，搜索有实际值的块
	//EOL(index)表示结束
	inline int GetFindHeadBlock(int hashindex) const
	{
		ACE_ASSERT( IsValidHashIndex(hashindex) );
		
		int index = GetAllFindHeadBlock(hashindex);
		
		while( !EOL(index) && IsRealDataBlock(index) == FALSE )
		{
			GetAllFindNextBlock(index);
		}
		
		return index;
	}
	
	inline T* GetFindNextBlock(int & index ) const
	{
		T* pData = GetAllFindNextBlock(index);
		
		while( !EOL(index) && IsRealDataBlock(index) == FALSE )
		{
			GetAllFindNextBlock(index);
		}
		
		return pData;		
	}
	
	//EOL(index)表示结束
	inline int GetAllFindHeadBlock(int hashindex) const
	{
		ACE_ASSERT( IsValidHashIndex(hashindex) );
		
		if( m_pHashTable[hashindex] >= m_nBlocknums )
			return -1;
		else
			return m_pHashTable[hashindex];
	}
	
	inline T* GetAllFindNextBlock(int & index ) const
	{
		//确认这个数据被占用
		ACE_ASSERT( IsValidBlock(index) );

		T* pData = m_pData+index; 
		
		index = m_pOrder[index].next;
		
		if( index >= m_nBlocknums )
			index = -1;
		
		return pData;		
	}
	
protected:
	// 管理全部的数据块
	BLOCK_ORDER *	m_pOrder;

	// 管理已经使用的数据块的链表
	USED_NODE *		m_pUsedLink;

	// 总的块数
	int				m_nBlocknums;

	// hash标的大小,=nHashTableSize-1
	int				m_nHashTableBoundary;
	
	// 哈希表指针
	int	*			m_pHashTable;

	// 下一个空闲块的指针
	int				m_nFreeHeader;

	// 空闲的块计数
	int				m_nUnUsedCount;

	// 使用了的块的头索引
	int				m_nUsedHeader;
	// 已经使用了的块的尾索引
	int				m_nUsedTail;

	// 是否支持将已经使用的块连接起来
	BOOL			m_bSupportLink;

	// 是否有内部创建的缓冲区
	BOOL			m_bCreateBuffer;

	// 存储数据的缓冲区
	T *				m_pData;
	char *			m_pBufferAlloc;

	
};

template<class T>
bool CTHashFinder<T>::Create(size_t blocknums, size_t nHashsize, BOOL bSupportLink,T* pData)
{
	ACE_ASSERT( (blocknums > 0) && (nHashsize > 0) && IS_POWER_2(nHashsize) );
	
	m_bSupportLink = bSupportLink;

	m_pOrder = new BLOCK_ORDER[blocknums];
	if(m_pOrder==NULL)
	{
		return FALSE;
	}

	memset(m_pOrder,0x00,blocknums*sizeof(BLOCK_ORDER));
	
	if(m_bSupportLink)
	{
		m_pUsedLink = new USED_NODE[blocknums];
		if(m_pUsedLink==NULL)
		{
			return FALSE;
		}
	}
	
	m_pHashTable = new int[nHashsize];
	if(m_pHashTable==NULL)
	{
		return FALSE;
	}
	
	m_nHashTableBoundary = (int)nHashsize-1;
	m_nBlocknums = (int)blocknums;
	
	if( pData == NULL )
	{
//		pData = new T[blocknums];
		m_pBufferAlloc = new char[blocknums*sizeof(T)];

		if( m_pBufferAlloc == NULL )
			return FALSE;
		
		m_bCreateBuffer = TRUE;

		m_pData = (T*)m_pBufferAlloc;
	}
	else
	{
		m_pData = pData;
	}
	
	
	FreeAllBlock();	
	
	return TRUE;
}	


template<class T>
void CTHashFinder<T>::Destroy()
{
	FreeAllBlock();

	if( m_pOrder != NULL)
	{
		delete []m_pOrder;
		m_pOrder = NULL;
	}
	
	if( m_bSupportLink  && m_pUsedLink != NULL )
		delete []m_pUsedLink;

	m_pUsedLink = NULL;
	
	if( m_pHashTable != NULL)
	{
		delete []m_pHashTable;
		m_pHashTable = NULL;
	}
	
	if( m_bCreateBuffer && m_pData && m_pBufferAlloc)
	{
		delete []m_pBufferAlloc;
		m_pBufferAlloc = NULL;
	}
	
	m_pBufferAlloc = NULL;
	m_bCreateBuffer = FALSE;
	m_pData = NULL;
	
	m_nFreeHeader = 0;
	m_nUnUsedCount = m_nBlocknums = 0;
	m_nHashTableBoundary = 0;
	m_nUsedHeader = m_nUsedTail = -1;	
	
	return;		
}

template<class T>
void CTHashFinder<T>::FreeBlock(int index)
{
	//确认这个数据被占用
	ACE_ASSERT( IsValidBlock(index) );
	
	if( !IsValidBlock(index) )
		return;
	
	BLOCK_ORDER& delBlock = m_pOrder[index];

	//析构
	if(delBlock.bRealData&&m_bCreateBuffer)
	{
		thashfinder::_Destroy<T>(&m_pData[index]);
	}
	
	delBlock.bRealData = FALSE;	
	
	//释放hash表
	int next = delBlock.next;
	int prev = delBlock.prev;
	
	//注意赋值的顺序很重要，不能随意更换
	if( prev < m_nBlocknums )
		m_pOrder[prev].next = next;
	else
		m_pHashTable[prev-m_nBlocknums] = next;
	
	if( next < m_nBlocknums )
		m_pOrder[next].prev = prev;
	
	delBlock.next = m_nFreeHeader;
	m_nFreeHeader = index;
	
	//做已经释放的标记
	delBlock.prev = FLAG_UNUSED;
	
	if( m_bSupportLink )
	{
		//将整体已经使用的链表中删除一个块
		DeleteNodeInLink(index);
		
	}

	++m_nUnUsedCount;
}

template<class T>
void CTHashFinder<T>::FreeAllBlock()
{
	if( m_pOrder == NULL)
		return;

	int i;	
	
	for( i = 0; i < m_nBlocknums-1; i++ )	
		m_pOrder[i].next = i+1;
	
	m_pOrder[i].next = FLAG_LAST;
	
	//做已经释放的标记
	for( i = 0; i < m_nBlocknums; i++ )	
	{
		//析构
		if (m_pOrder[i].bRealData && m_bCreateBuffer )
		{
			thashfinder::_Destroy<T>(&m_pData[i]);
		}
		m_pOrder[i].prev = FLAG_UNUSED;	

#if USER_DATA
		m_pOrder[i].dwData = 0;
#endif
		m_pOrder[i].bRealData = FALSE;
	}
	
	for( i = 0; i < m_nHashTableBoundary+1; i++ )
		m_pHashTable[i] = m_nBlocknums + i;
	
	m_nUnUsedCount = m_nBlocknums;
	m_nUsedHeader = m_nUsedTail = -1;	
	m_nFreeHeader = 0;	
}

template<class T>
void CTHashFinder<T>::FreeFindBlock(int hashindex)
{
	int nIndex = GetAllFindHeadBlock(hashindex);
	
	int nPrevIndex;

	while( nIndex != -1 )
	{
		nPrevIndex = nIndex;
		
		GetAllFindNextBlock(nIndex);
		
		FreeBlock(nPrevIndex);
	}
	
	return;
}

template<class T>
int CTHashFinder<T>::AllocIndex(int hashindex,BOOL bRealData)
{
	hashindex = (((unsigned int)hashindex) & m_nHashTableBoundary);	
	
	if( m_nFreeHeader == FLAG_LAST )
		return m_nFreeHeader;	
	
	//从空余模块中得到一个数据
	int nNewIndex = m_nFreeHeader;
	
	//确认这个数据没有被占用
	ACE_ASSERT( IsValidBlock(nNewIndex)==FALSE );
	
	BLOCK_ORDER& newBlock = m_pOrder[nNewIndex];
	int& nHashData = m_pHashTable[hashindex];
	
	m_nFreeHeader = newBlock.next;
	
	//注意赋值的顺序很重要，不能随意更换
	newBlock.next = nHashData;
	newBlock.prev = hashindex + m_nBlocknums;
	
	if( nHashData < m_nBlocknums )
		m_pOrder[nHashData].prev = nNewIndex;
	
	nHashData = nNewIndex;
	
	m_nUnUsedCount--;
	
	if( m_bSupportLink )
	{
		//将这个块放入整体已经使用链表中
		AddNodeToLinkTail(nNewIndex);
	}
	
	m_pOrder[nNewIndex].bRealData = bRealData;
	
	// 构造
	if (bRealData&&m_bCreateBuffer)
	{
		thashfinder::_Construct<T>(m_pData+nNewIndex);
	}

	return nNewIndex;
}

template<class T>
void CTHashFinder<T>::DeleteNodeInLink(int index)
{
	ACE_ASSERT( IsValidIndex(index) );
	
	//将整体已经使用的链表中删除一个块
	int next = m_pUsedLink[index].Lnext;
	int prev = m_pUsedLink[index].Lprev;
	
	//注意赋值的顺序很重要，不能随意更换
	if( prev != -1 )
		m_pUsedLink[prev].Lnext = next;
	
	if( next != -1 )
		m_pUsedLink[next].Lprev = prev;
	
	if( index == m_nUsedHeader )
		m_nUsedHeader = next;
	
	if( index == m_nUsedTail )
		m_nUsedTail = prev;	
	
	return;		
}

template<class T>
void CTHashFinder<T>::AddNodeToLinkTail(int index)
{
	ACE_ASSERT( IsValidIndex(index) );
	
	//将这个块放入整体已经使用链表中
	//注意赋值的顺序很重要，不能随意更换
	m_pUsedLink[index].Lnext = -1;
	m_pUsedLink[index].Lprev = m_nUsedTail;
	
	if(m_nUsedTail != -1)
		m_pUsedLink[m_nUsedTail].Lnext = index;
	
	m_nUsedTail = index;
		
	if( m_nUsedHeader == -1 )
		m_nUsedHeader =  index;
	
	return;
	
}

template<class T>
void CTHashFinder<T>::AddNodeToLinkHead(int index)
{
	ACE_ASSERT( IsValidIndex(index) );

	//将这个块放入整体已经使用链表中
	//注意赋值的顺序很重要，不能随意更换
	m_pUsedLink[index].Lnext = m_nUsedHeader;
	m_pUsedLink[index].Lprev = -1;

	if(m_nUsedHeader != -1)
		m_pUsedLink[m_nUsedHeader].Lprev = index;

	m_nUsedHeader = index;

	if( m_nUsedTail == -1 )
		m_nUsedTail =  index;

	return;

}

template<class T>
BOOL CTHashFinder<T>::CopyTo(CTHashFinder<T>& finder,BOOL bCopyData) const
{
	int m_nHashTableSize = m_nHashTableBoundary+1;
	finder.Destroy();
	
	if( bCopyData )
	{
		if( finder.Create(m_nHashTableSize,m_nBlocknums,m_bSupportLink,NULL) == FALSE )
			return FALSE;
		
		CopyDataTo(finder);
	}
	else
	{
		if( finder.Create(m_nHashTableSize,m_nBlocknums,m_bSupportLink,m_pData) == FALSE )
			return FALSE;
	}
	
	if( m_nBlocknums > 0 )
	{
		memcpy( finder.m_pOrder, m_pOrder, sizeof(m_pOrder[0])*m_nBlocknums );
	}
	
	if(m_bSupportLink==TRUE)
	{
		memcpy( finder.m_pUsedLink, m_pUsedLink, sizeof(m_pUsedLink[0])*m_nBlocknums );
	}
	
	memcpy( finder.m_pHashTable, m_pHashTable, sizeof(m_pHashTable[0])*m_nHashTableSize );
	
	finder.m_nFreeHeader = m_nFreeHeader;
	finder.m_nUnUsedCount = m_nUnUsedCount;
	finder.m_nUsedHeader = m_nUsedHeader;
	finder.m_nUsedTail = m_nUsedTail;
	
	return TRUE;
}

template<class T>
BOOL CTHashFinder<T>::CopyDataTo(CTHashFinder<T>& finder) const
{
	int index;
	
	if( m_bSupportLink )
	{
		index = GetHeadBlock();
		
		while( !EOL(index) )
		{
			finder.m_pData[index] = m_pData[index];
			
			GetNextBlockFromHead(index);
		}
	}//end if( m_bSupportLink )
	else
	{
		index = GetFirstUsedBlock();
		
		while( !EOL(index) )
		{
			finder.m_pData[index] = m_pData[index];
			
			GetNextUsedBlock(index);
		}
	}

	return TRUE;
	
}//end BOOL CopyDataTo(CTHashFinder<T>& finder) const


#endif // !defined(AFX_THASHFINDER_H__2F43062B_5DB9_4A06_A250_7F84A7A70CAA__INCLUDED_)
