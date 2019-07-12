//////////////////////////////////////////////////////////////////////////
//VarHashFinderEx.inl

DEF_VARTHASH_FINDEREX_DECLEAR
DEF_VARTHASH_FINDEREX_CLASS::CVarHashFinderEx()
{
	m_myUsedList = NULL;

	m_hashTable = NULL;

	m_nHashTableBoundary = 0;

	m_bSupportLink = false;

	m_ncount = 0;
}

DEF_VARTHASH_FINDEREX_DECLEAR
DEF_VARTHASH_FINDEREX_CLASS::~CVarHashFinderEx()
{
	Destroy();
}

DEF_VARTHASH_FINDEREX_DECLEAR
BOOL DEF_VARTHASH_FINDEREX_CLASS
::Create(size_t nHashsize,BOOL bSupportLink)
{	
	ACE_ASSERT((int)nHashsize > 0 && IS_POWER_2(nHashsize));

	m_hashTable = new LPMY_HASH_LIST[nHashsize];
	memset(m_hashTable,0x00,nHashsize*sizeof(LPMY_HASH_LIST));

	m_nHashTableBoundary = nHashsize-1;

	m_bSupportLink = bSupportLink;

	if( m_bSupportLink )
	{
		m_myUsedList = new MY_USED_LIST;
	}

	m_ncount = 0;

	return true;
}

DEF_VARTHASH_FINDEREX_DECLEAR
void DEF_VARTHASH_FINDEREX_CLASS
::Destroy()
{
	this->RemoveAll();

	if( m_hashTable )
	{
		delete []m_hashTable;
		m_hashTable = NULL;
	}

	m_nHashTableBoundary = 0;

	if( m_myUsedList )
	{
		delete m_myUsedList;
		m_myUsedList = NULL;

		m_bSupportLink = FALSE;
	}

	return;
}

DEF_VARTHASH_FINDEREX_DECLEAR
ValueType* DEF_VARTHASH_FINDEREX_CLASS
::FindOnly(const KeyType& key, int& nCmpValue) const
{
	VAR_HASH_ELEMENT element = FindFirst(key,nCmpValue);

	while( NOT_LAST(element) )
	{
		return FindNext(key,element,nCmpValue);
	}

	return NULL;
}

DEF_VARTHASH_FINDEREX_DECLEAR
VAR_HASH_ELEMENT DEF_VARTHASH_FINDEREX_CLASS
::FindFirst(const KeyType& key, int& nCmpValue) const
{
	VAR_HASH_ELEMENT loc(My_Hash(key),NULL);

	if( m_hashTable[loc.index] == NULL )
		return loc;

	loc.loc = m_hashTable[loc.index]->GetHeadPosition();

	while( m_hashTable[loc.index]->NOT_LAST(loc.loc) )
	{
		VAR_HASH_ELEMENT prevloc = loc;

		if( m_HashCmpFunc.Cmp(m_hashTable[loc.index]->GetNext(loc.loc).data,key) )
			return prevloc;
	}

	return loc;
}

DEF_VARTHASH_FINDEREX_DECLEAR
ValueType* DEF_VARTHASH_FINDEREX_CLASS
::FindNext(const KeyType& key,VAR_HASH_ELEMENT& loc,int& nCmpValue) const
{
	HASH_LIST_ELEMENT& hastListElement = m_hashTable[loc.index]->GetNext(loc.loc);

	 while( m_hashTable[loc.index]->NOT_LAST(loc.loc) )
	 {
		 HASH_LIST_ELEMENT& hastListElement2 = m_hashTable[loc.index]->GetAt(loc.loc);

		 if( m_HashCmpFunc.Cmp(hastListElement2.data,key) )
		 {
			 break;
		 }	

		 m_hashTable[loc.index]->GetNext(loc.loc);
	 }

	 return &hastListElement.data;
}


DEF_VARTHASH_FINDEREX_DECLEAR
ValueType * DEF_VARTHASH_FINDEREX_CLASS
::AllocBlockByKey(const KeyType& key)
{
	int hash_index = My_Hash(key);

	if( m_hashTable[hash_index] == NULL )
	{
		m_hashTable[hash_index] = new MY_HASH_LIST();
	}

	if( m_hashTable[hash_index] == NULL )
		return false;	

	LOCATION hash_loc = m_hashTable[hash_index]->AddTail(HASH_LIST_ELEMENT(NULL));

	HASH_LIST_ELEMENT& hash_element = m_hashTable[hash_index]->GetTail();

	if( m_bSupportLink )
	{
		VAR_HASH_ELEMENT used_element(hash_index,hash_loc);

		LOCATION loc_used = m_myUsedList->AddTail( used_element );

		hash_element.usedloc = loc_used;
	}

	++m_ncount;

	return &hash_element.data;
}

DEF_VARTHASH_FINDEREX_DECLEAR
ValueType * DEF_VARTHASH_FINDEREX_CLASS
::AllocOnlyBlockByKey(const KeyType& key,BOOL* pnew = NULL)
{
	ValueType * pData = FindOnly(key);

	if( pData)
	{
		if( pnew )
			*pnew = false;

		return pData;
	}

	pData = AllocBlockByKey(key);

	if( pData )
	{
		if( pnew )
			*pnew = true;
	}	

	return pData;
}


DEF_VARTHASH_FINDEREX_DECLEAR
BOOL DEF_VARTHASH_FINDEREX_CLASS
::Remove(const KeyType& key)
{
	int index = My_Hash(key);

	if( m_hashTable[index] == NULL )
		return false;

	LOCATION loc = m_hashTable[index]->GetHeadPosition();

	while( m_hashTable[index]->NOT_LAST(loc) )
	{
		LOCATION tempLoc = loc;

		HASH_LIST_ELEMENT& hastListElement = m_hashTable[index]->GetNextPosition(loc);
	
		if( m_HashCmpFunc.Cmp(hastListElement.data,key) )
		{
			if( m_bSupportLink )
			{
				m_myUsedList->RemoveAt(hastListElement.usedloc);
			}
			
			m_hashTable[index]->RemoveAt(tempLoc);

			--m_ncount;

			return TRUE;
		}
	}

	return FALSE;
}

DEF_VARTHASH_FINDEREX_DECLEAR
BOOL DEF_VARTHASH_FINDEREX_CLASS
::Remove(const VAR_HASH_ELEMENT& element)
{
	return Remove(element.index,element.loc);
}

DEF_VARTHASH_FINDEREX_DECLEAR
BOOL DEF_VARTHASH_FINDEREX_CLASS
::Remove(int hash_index,LOCATION loc)
{
	if( m_hashTable[hash_index] == NULL )
		return false;

	if( m_bSupportLink )
	{
		HASH_LIST_ELEMENT& hastListElement = m_hashTable[hash_index]->GetAt(loc);

		m_myUsedList->RemoveAt(hastListElement.usedloc);
	}

	--m_ncount;

	m_hashTable[hash_index]->RemoveAt(loc);

	return TRUE;
}

DEF_VARTHASH_FINDEREX_DECLEAR
void DEF_VARTHASH_FINDEREX_CLASS
::RemoveAll()
{//清除所有已经插入到里面的数据

	if( m_hashTable )
	{
		for( size_t i = 0; i < m_nHashTableBoundary+1; i++ )
		{
			if( m_hashTable[i] )
			{
				m_hashTable[i]->RemoveAll();

				delete m_hashTable[i];

				m_hashTable[i] = NULL;
			}
		}
	}


	if( m_bSupportLink )
	{
		if( m_myUsedList )
		{
			m_myUsedList->RemoveAll();
		}	
	}

	m_ncount = 0;

	return;
}

DEF_VARTHASH_FINDEREX_DECLEAR
int DEF_VARTHASH_FINDEREX_CLASS
::AssertEmptyError() const
{
	if( m_bSupportLink && m_myUsedList )
	{
		if( !m_myUsedList->IsEmpty() )
			return 1;

		if( !m_myUsedList->LAST(m_myUsedList->GetHeadPosition()) )
			return 2;
	}


	for(size_t i = 0; i <= m_nHashTableBoundary; ++i)
	{
		if( !m_hashTable[i] )
			continue;

		if( !m_hashTable[i]->IsEmpty() )
			return 3;

		if( m_hashTable[i]->GetHeadPosition() != NULL )
			return 4;
	}
	
	if( m_ncount != 0 )
		return 5;

	return TRUE;
}

