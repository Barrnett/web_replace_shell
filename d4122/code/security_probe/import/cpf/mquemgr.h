//////////////////////////////////////////////////////////////////////////
//mquemgr.h

//使用多个队列来管理多个相同大小的内存管理器

#ifndef  MEM_MQUE_MGR_H_2007_06_05
#define  MEM_MQUE_MGR_H_2007_06_05
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

#include "ace/Null_Mutex.h"
#include "ace/Thread_Mutex.h"
#include "ace/Recursive_Thread_Mutex.h"

#include "cpf/cpf.h"

template< typename LOCK=ACE_Null_Mutex,typename _A = std::allocator<BYTE> >
class Cmquemgr
{
public:
	enum 
	{
		DEFAULT_MAX_SIZE	= 1024,	// 受管理的最大内存块
		DEFAULT_CACHE_BANK	= 8,	// 每一块被管理内存的大小
		DEFAULT_MEM_COUNT   = 0,    // 预先为每一阶内存分配的块数
		DEFAULT_CACHE_NUMS = 100,	// 管理器最多为每一阶内存分配的块数，如果超出这些，则将内存释放给系统。
	};

	//参数参看SetParams和PreAlloc
	Cmquemgr(ULONG dwMaxBlockSize = DEFAULT_MAX_SIZE, 
		ULONG dwCacheBank = DEFAULT_CACHE_BANK,
		ULONG dwPreAllocBlocks = DEFAULT_MEM_COUNT,
		ULONG dwCacheNums = -1 );

	~Cmquemgr();

	//将应用申请后但是已经被释放的内存（挂在这个对象中的内存）。全部归还给系统。
	void ReclaimBuffer();

	//dwMaxBlockSize;使用内存来管理的内存最大大小，
	//比如1024，如果申请的内存小于等于dwMaxBlockSize，则使用管理自管理，否则使用系统内存管理
	//
	//dwCacheBank是分配大小的粒度
	//比如32。如果申请内存的大小在1-32之间，实际开辟32。33-64之间，实际开辟64

	//参数要求dwMaxBlockSize必须>dwCacheBank
	//dwCacheBank：这个值必须是2^n,并且必须>=sizeof(ulong)

	//dwCacheNums:管理器最多为每一阶内存分配的块数，如果超出这些，则将内存释放给系统。-1表示无数个
	void SetParams(ULONG dwMaxBlockSize, ULONG dwCacheBank,ULONG nCount=0,ULONG dwCacheNums = -1 );

public:
	void * Alloc(ULONG dwBytes);	
	
	void Free(void* pBlock);
	
	void * ReAlloc(void* pBlock, ULONG dwBytes);

	BOOL  Reserve(ULONG blocksize, ULONG blocknums);

	inline LOCK& GetLock()
	{
		return m_lock;
	}
private:

	enum 
	{
		APPED_SIZE = sizeof(char *)
	};
	
private:
	//预先为每一阶内存开辟的内存个数
	BOOL PreAlloc(ULONG nCount);

private:
	ULONG	m_dwCacheSize;		// 内存管理区域的个数
	ULONG	m_dwCacheBank;		// 每个内存管理的等比大小, 最小为sizeof(BYTE*), 必须是2的整数次方
	ULONG	m_dwCacheBankPower;//如果m_dwCacheBank==(2<<n),m_dwCacheBankPower==n
	ULONG	m_dwMaxBlockSize;	// 最大能够管理的大小,超过后将进行真正的开辟
	ULONG	m_dwCacheNums;		//dwCacheNums:管理器最多为每一阶内存分配的块数，如果超出这些，则将内存释放给系统。-1表示无数个

	typedef struct{
		ULONG_PTR		count;
		BYTE *			phead;

	}Mgr_Header;

	Mgr_Header*		m_pCache;

	ULONG	m_dwAllocBlock;		// 上层如果申请成功，则该计数增1，如果释放则该计数减1

	LOCK	m_lock;

	_A		m_allocator;	// 内存分配器

private:
	void Close();

	inline ULONG CalcuGauss(ULONG dwSize) const
	{
		return (dwSize+m_dwCacheBank-1) & (~(m_dwCacheBank-1));
	}

	inline ULONG GetIndex(ULONG dwBytes) const
	{	return ((dwBytes >> m_dwCacheBankPower)-1);		}

	inline static void AssertValid(void * pdata)
	{
		BYTE * x = (BYTE *)pdata - sizeof(ULONG_PTR);

		ACE_ASSERT(memcmp((BYTE *)x+sizeof(ULONG_PTR) + *(ULONG_PTR *)x,m_append_data,APPED_SIZE)==0);

	}
private:
	static const unsigned char m_append_data[];

}; 

#include "cpf/mquemgr.inl"

typedef Cmquemgr<ACE_Null_Mutex>					CMqueMgr_no_mtsaft;
typedef Cmquemgr<ACE_Recursive_Thread_Mutex>		CMqueMgr_mtsaft;


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
#endif // MEM_MQUE_MGR_H_2007_06_05
