///////////////////////////////////////////////////////////////////////////
//VarHashFinderEx.h 

#ifndef VAR_HASH__FINDER_EX_H_2006_04_20
#define VAR_HASH__FINDER_EX_H_2006_04_20
///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////

#include "cpf/QuickList.h"
#include <vector>
#include "cpf/Common_Func_Macor.h"

#ifndef DEF_VARTHASH_FINDEREX_DECLEAR
#define DEF_VARTHASH_FINDEREX_DECLEAR		template<class ValueType, class KeyType, class HashCmpFunc> 
#define DEF_VARTHASH_FINDEREX_CLASS			CVarHashFinderEx< ValueType, KeyType, HashCmpFunc>
#endif // DEF_CTHASH_FINDEREX_DECLEAR

typedef struct tagVAR_HASH_ELEMENT{

	tagVAR_HASH_ELEMENT(int in_index,LOCATION in_loc)
		:index(in_index),loc(in_loc)
	{
		
	}

	int		 index;
	LOCATION loc;

}VAR_HASH_ELEMENT;

DEF_VARTHASH_FINDEREX_DECLEAR
class CVarHashFinderEx
{
	typedef struct tagHASH_LIST_ELEMENT
	{
		tagHASH_LIST_ELEMENT(const ValueType& input,LOCATION loc)
			:data(input),usedloc(loc)
		{
		}

		tagHASH_LIST_ELEMENT(LOCATION loc)
			:usedloc(loc)
		{
		}

		ValueType	data;

		LOCATION	usedloc;//���Ѿ�ʹ���б��е�λ��

	}HASH_LIST_ELEMENT;

    typedef CQuickList<HASH_LIST_ELEMENT>		MY_HASH_LIST;
    typedef MY_HASH_LIST *                      LPMY_HASH_LIST;

    typedef CQuickList<VAR_HASH_ELEMENT>		MY_USED_LIST;

public:
	CVarHashFinderEx();
	~CVarHashFinderEx();

	BOOL Create(size_t nHashsize,BOOL bSupportLink=false);
	void Destroy();

	BOOL IsCreateOK() const
	{
		return (m_hashTable!=NULL);
	}

	size_t GetCount() const
	{	
		return m_ncount;	
	}

	//���ڹؼ��ֲ�Ψһ����ʹ����������������,���ڹؼ���Ψһ����FindOnly

	//��Ҫ���رȽϽ��:nCmpValue	
	//	VAR_HASH_ELEMENT loc = table.FindFirst(key);
	//	while( table.NOT_LAST(loc) )
	//	{	ValueType * pData = table.FindNext(key,loc); }
	//
	inline VAR_HASH_ELEMENT FindFirst(const KeyType& key, int& nCmpValue) const;

	inline ValueType * FindNext(const KeyType& key,VAR_HASH_ELEMENT& element, int& nCmpValue) const;	

	//�������һ��
	inline BOOL NOT_LAST(const VAR_HASH_ELEMENT& loc) const
	{
		if( m_hashTable[loc.index] && m_hashTable[loc.index]->NOT_LAST(loc.loc) )
			return true;

		return false;
	}

	inline ValueType * FindOnly(const KeyType& key, int& nCmpValue) const;

	inline ValueType * FindOnly(const KeyType& key) const
	{
		int nCmpValue = 0;
		return FindOnly(key,nCmpValue);
	}

	inline VAR_HASH_ELEMENT FindFirst(const KeyType& key) const
	{
		int nCmpValue = 0;

		return FindFirst(key,nCmpValue);
	}

	inline ValueType * FindNext(const KeyType& key,VAR_HASH_ELEMENT& element) const
	{
		int nCmpValue = 0;
		return FindNext(key,element,nCmpValue);
	}

	BOOL Add(const KeyType& key,const ValueType& value)
	{
		ValueType * pDataValue = AllocBlockByKey(key);

		if( pDataValue )
		{
			*pDataValue = value;

			return true;
		}

		return false;
	}

	ValueType * AllocBlockByKey(const KeyType& key);
	ValueType * AllocOnlyBlockByKey(const KeyType& key,BOOL* pnew=NULL);

	BOOL Remove(const KeyType& key);
	BOOL Remove(const VAR_HASH_ELEMENT& element);
	BOOL Remove(int hash_index,LOCATION loc);

	//��������Ѿ����뵽���������
	void RemoveAll();

	//���������Ѿ�����Ŀ飺����ɵĵ����µġ�ֻ����bSupportLink��TRUEʱ����ʹ��
	//EOL(index)��ʾ����
	inline LOCATION	GetAllHeadBlock() const
	{	return m_myUsedList->GetHeadPosition();	}

	inline VAR_HASH_ELEMENT& GetAllNextBlockFromHead_List(LOCATION & loc ) const
	{
		return m_myUsedList->GetNext(loc);
	}

	inline ValueType* GetAllNextBlockFromHead_Data(LOCATION & loc )
	{
		VAR_HASH_ELEMENT& hash_element = m_myUsedList->GetNext(loc);
		
		return GetData(hash_element);
	}

	inline BOOL NOT_LAST_AllHeadBlock(LOCATION loc) const
	{	
		return m_myUsedList->NOT_LAST(loc);	
	}


	inline ValueType * GetData(const VAR_HASH_ELEMENT& hash_element)
	{
		if( m_hashTable[hash_element.index] )
		{
			return &m_hashTable[hash_element.index]->GetAt(hash_element.loc).data;
		}

		return NULL;
	}

	inline const ValueType * GetData(const VAR_HASH_ELEMENT& hash_element) const
	{
		if( m_hashTable[hash_element.index] )
		{
			return &m_hashTable[hash_element.index]->GetAt(hash_element.loc).data;
		}

		return NULL;
	}

public:
	//���ȷ���Ѿ�û���κ�������
	int AssertEmptyError() const;

private:
	LPMY_HASH_LIST*			m_hashTable;
	size_t					m_nHashTableBoundary;

	MY_USED_LIST *			m_myUsedList;

	HashCmpFunc				m_HashCmpFunc;

	BOOL					m_bSupportLink;

	size_t					m_ncount;

	inline int My_Hash(const KeyType& key) const
	{
		return (((unsigned int)m_HashCmpFunc.Hash(key))&m_nHashTableBoundary);
	}

};

#include "VarHashFinderEx.inl"


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
#endif//VAR_HASH__FINDER_EX_H_2006_04_20
