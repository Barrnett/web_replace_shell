//////////////////////////////////////////////////////////////////////////
//mquemgr.h

//ʹ�ö����������������ͬ��С���ڴ������

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
		DEFAULT_MAX_SIZE	= 1024,	// �ܹ��������ڴ��
		DEFAULT_CACHE_BANK	= 8,	// ÿһ�鱻�����ڴ�Ĵ�С
		DEFAULT_MEM_COUNT   = 0,    // Ԥ��Ϊÿһ���ڴ����Ŀ���
		DEFAULT_CACHE_NUMS = 100,	// ���������Ϊÿһ���ڴ����Ŀ��������������Щ�����ڴ��ͷŸ�ϵͳ��
	};

	//�����ο�SetParams��PreAlloc
	Cmquemgr(ULONG dwMaxBlockSize = DEFAULT_MAX_SIZE, 
		ULONG dwCacheBank = DEFAULT_CACHE_BANK,
		ULONG dwPreAllocBlocks = DEFAULT_MEM_COUNT,
		ULONG dwCacheNums = -1 );

	~Cmquemgr();

	//��Ӧ����������Ѿ����ͷŵ��ڴ棨������������е��ڴ棩��ȫ���黹��ϵͳ��
	void ReclaimBuffer();

	//dwMaxBlockSize;ʹ���ڴ���������ڴ�����С��
	//����1024�����������ڴ�С�ڵ���dwMaxBlockSize����ʹ�ù����Թ�������ʹ��ϵͳ�ڴ����
	//
	//dwCacheBank�Ƿ����С������
	//����32����������ڴ�Ĵ�С��1-32֮�䣬ʵ�ʿ���32��33-64֮�䣬ʵ�ʿ���64

	//����Ҫ��dwMaxBlockSize����>dwCacheBank
	//dwCacheBank�����ֵ������2^n,���ұ���>=sizeof(ulong)

	//dwCacheNums:���������Ϊÿһ���ڴ����Ŀ��������������Щ�����ڴ��ͷŸ�ϵͳ��-1��ʾ������
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
	//Ԥ��Ϊÿһ���ڴ濪�ٵ��ڴ����
	BOOL PreAlloc(ULONG nCount);

private:
	ULONG	m_dwCacheSize;		// �ڴ��������ĸ���
	ULONG	m_dwCacheBank;		// ÿ���ڴ����ĵȱȴ�С, ��СΪsizeof(BYTE*), ������2�������η�
	ULONG	m_dwCacheBankPower;//���m_dwCacheBank==(2<<n),m_dwCacheBankPower==n
	ULONG	m_dwMaxBlockSize;	// ����ܹ�����Ĵ�С,�����󽫽��������Ŀ���
	ULONG	m_dwCacheNums;		//dwCacheNums:���������Ϊÿһ���ڴ����Ŀ��������������Щ�����ڴ��ͷŸ�ϵͳ��-1��ʾ������

	typedef struct{
		ULONG_PTR		count;
		BYTE *			phead;

	}Mgr_Header;

	Mgr_Header*		m_pCache;

	ULONG	m_dwAllocBlock;		// �ϲ��������ɹ�����ü�����1������ͷ���ü�����1

	LOCK	m_lock;

	_A		m_allocator;	// �ڴ������

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
