#if !defined(AFX_THASHFINDEREX_H__27983C38_3B45_4316_9B73_12BC43E86D1B__INCLUDED_)
#define AFX_THASHFINDEREX_H__27983C38_3B45_4316_9B73_12BC43E86D1B__INCLUDED_

#include "ace/ACE.h"
#include "cpf/ostypedef.h"

//HO = HostOrder,NO=NetOrder

#define CPF_HASH_HO_INT(x)	(unsigned int)(x)
#define CPF_HASH_NO_INT(x)	(unsigned int)(ACE_NTOHL(x))

#define CPF_HASH_HO_IPv4(x)	CPF_HASH_HO_INT(x)
#define CPF_HASH_NO_IPv4(x)	CPF_HASH_NO_INT(x)

#define CPF_HASH_HO_PORT(x)	(unsigned int)(x)
#define CPF_HASH_NO_PORT(x)	(unsigned int)(ACE_NTOHS(x))

#define CPF_HASH_STRING(p)			(ACE::hash_pjw(p))

#define CPF_HASH_HO_TWOIPv4(ip1,ip2)	(CPF_HASH_HO_INT((ip1)+(ip2)))
#define CPF_HASH_NO_TWOIPv4(ip1,ip2)	(unsigned int)(ACE_NTOHL(ip1)+ACE_NTOHL(ip2))

#define CPF_HASH_HO_ADDR(ip1,ip2,port1,port2)	(unsigned int)((ip1)+(port1)+(port1)+(port2))
#define CPF_HASH_NO_ADDR(ip1,ip2,port1,port2)		(unsigned int)((ACE_NTOHL(ip1)+ACE_NTOHL(ip2)+ACE_NTOHS(port1)+ACE_NTOHS(port2))

#define CPF_HASH_IPv4_TRANSPORT_ADDR(addr)		(unsigned int)((addr).dwIP^(addr).wdPort)

#include "THashFinder.h"

//HashCompFunc��Ҫʵ��һ�¼�������
//int Hash(const KeyType& key) const

//int Cmp(const ValueType& value,const KeyType& key) const;
//�������ȷ���0,

#ifndef DEF_CTHASH_FINDEREX_DECLEAR
	#define DEF_CTHASH_FINDEREX_DECLEAR		template<class ValueType, class KeyType, class HashCompFunc> 
	#define DEF_CTHASH_FINDEREX_CLASS		CTHashFinderEx<ValueType, KeyType, HashCompFunc>
#endif // DEF_CTHASH_FINDEREX_DECLEAR

DEF_CTHASH_FINDEREX_DECLEAR
class CTHashFinderEx : public CTHashFinder<ValueType>
{

public:
	CTHashFinderEx()
		:m_dwRandomHashIndex(0)
	{	
		return;
	}
	
	~CTHashFinderEx()
	{
		return;
	}

public:	
	bool Create(size_t num,size_t nHashsize = CTHashFinder<ValueType>::HASH_TABLE_DEFAULT_SIZE, 
		bool bSupportLink = true, ValueType * pData = NULL)
	{
		return CTHashFinder<ValueType>::Create(num, nHashsize, bSupportLink, pData);
	}

	
	//���ڹؼ��ֲ�Ψһ����ʹ����������������,���ڹؼ���Ψһ����FindOnly

	//��Ҫ���رȽϽ��:nCmpValue	
	inline int FindFirst(const KeyType& key, int& nCmpValue) const;
	//��Ҫ���رȽϽ��:nCmpValue
	inline ValueType* FindNext(const KeyType& key, int& index, int& nCmpValue) const;

	//����Ҫ���رȽϽ��
	inline int FindFirst(const KeyType& key) const
	{
		int nCmpValue;
		return FindFirst(key,nCmpValue);
	}
	//����Ҫ���رȽϽ��
	inline ValueType* FindNext(const KeyType& key, int& index) const
	{
		int nCmpValue;
		return FindNext(key,index,nCmpValue);
	}

	//���ڹؼ���Ψһ����ʹ������򵥵ĺ���,
	//���ڹؼ��ֲ�Ψһ,����ʹ��FindFirst&FindNext
	//��Ҫ���رȽϽ��:nCmpValue
	inline ValueType* FindOnly(const KeyType& key,int& nCmpValue) const
	{
		return GetDataByIndex(FindFirst(key,nCmpValue));
	}
	//����Ҫ���رȽϽ��
	inline ValueType* FindOnly(const KeyType& key) const
	{
		int nCmpValue;
		return GetDataByIndex(FindFirst(key,nCmpValue));
	}

	inline ValueType* AllocBlockByKey(const KeyType& key)
	{	
		return CTHashFinder<ValueType>::AllocBlock(m_HashCompFunc.Hash(key),true);
	}
	
	//�Ȱ��չؼ��ֲ���,����ҵ����ϵ�����,�����ҵ�������.���û���ҵ�,���´���һ������.
	inline ValueType* AllocOnlyBlockByKey(const KeyType& key,BOOL* pnew = NULL)
	{	
		ValueType * pData = FindOnly(key);

		if( pData )
		{
			if(pnew)
			{
				*pnew = FALSE;
			}
		}
		else
		{
			if(pnew)
			{
				*pnew = TRUE;
			}

			pData = AllocBlockByKey(key);

		}

		return pData;
	}


	//ռ��һ��λ�ã�NULL��ʾʧ��
	inline ValueType* AllocBlockByRandom()
	{
		++m_dwRandomHashIndex;
		if ( m_dwRandomHashIndex >= CTHashFinder<ValueType>::m_nHashTableBoundary+1 )
		{
			m_dwRandomHashIndex = 0;
		}
		
		return CTHashFinder<ValueType>::AllocBlock(m_dwRandomHashIndex, false);	
	}

	//���չؼ������ͷ����ݣ�ɾ����һ�����Ϲؼ��ֵ�����
	BOOL FreeFirstBlockByKey(const KeyType& key);

	//���չؼ������ͷ����ݣ��ͷ����йؼ�����ͬ������.
	//�����ͷŵĸ���
	int FreeBlockByKey(const KeyType& key);
	
protected:
	CTHashFinderEx(const DEF_CTHASH_FINDEREX_CLASS& hashFinderex);
	DEF_CTHASH_FINDEREX_CLASS& operator = (const DEF_CTHASH_FINDEREX_CLASS& hashFinderex);
	

private:
	ACE_UINT32		m_dwRandomHashIndex;

public:
	HashCompFunc	m_HashCompFunc;
	
	
};

DEF_CTHASH_FINDEREX_DECLEAR
int DEF_CTHASH_FINDEREX_CLASS::FindFirst(const KeyType& key, int& nCmpValue) const
{
	if( this->m_pData )
	{			
		int nHashIndex = (int)(((UINT)m_HashCompFunc.Hash(key)) & CTHashFinder<ValueType>::m_nHashTableBoundary);

		int index = CTHashFinder<ValueType>::GetFindHeadBlock(nHashIndex);	
			
		while ( !CTHashFinder<ValueType>::EOL(index) &&
			(nCmpValue = m_HashCompFunc.Cmp(this->m_pData[index], key)) == 0 )
		{
			CTHashFinder<ValueType>::GetFindNextBlock(index);
		}

		return index;
	}
	else
	{
		return -1;
	}
}

DEF_CTHASH_FINDEREX_DECLEAR
ValueType* DEF_CTHASH_FINDEREX_CLASS
::FindNext(const KeyType& key, int& index, int& nCmpValue) const
{
	ACE_ASSERT( -1 != index);
	
	ValueType* pData = this->m_pData+index;
	
	CTHashFinder<ValueType>::GetFindNextBlock(index);	
	
	while( !CTHashFinder<ValueType>::EOL(index) && 
		(nCmpValue = m_HashCompFunc.Cmp(this->m_pData[index], key)) == 0 
		)
	{
		CTHashFinder<ValueType>::GetFindNextBlock(index);		
	}
	
	return pData;	
}

//���չؼ������ͷ����ݣ�ɾ����һ�����Ϲؼ��ֵ�����
DEF_CTHASH_FINDEREX_DECLEAR
BOOL DEF_CTHASH_FINDEREX_CLASS::FreeFirstBlockByKey(const KeyType& key)
{
	int index = FindFirst(key);

	if( index != -1 )
	{
		CTHashFinder<ValueType>::FreeBlock(index);
		return true;
	}

	return false;
}

//���չؼ������ͷ����ݣ��ͷ����йؼ�����ͬ������.
DEF_CTHASH_FINDEREX_DECLEAR
int DEF_CTHASH_FINDEREX_CLASS::FreeBlockByKey(const KeyType& key)
{
	int index = FindFirst(key);

	int count = 0;

	while( -1 != index )
	{
		int previndex = index;

		FindNext(key,index);

		CTHashFinder<ValueType>::FreeBlock(previndex);

		++count;
	}

	return count;
}

#ifdef DEF_CTHASH_FINDEREX_DECLEAR
	#undef DEF_CTHASH_FINDEREX_DECLEAR 
	#undef DEF_CTHASH_FINDEREX_CLASS
#endif //DEF_CTHASH_FINDEREX_DECLEAR


#endif // !defined(AFX_THASHFINDEREX_H__27983C38_3B45_4316_9B73_12BC43E86D1B__INCLUDED_)


