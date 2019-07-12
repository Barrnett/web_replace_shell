

//////////////////////////////////////////////////////////////////////////
//
//		
//				ʵ�ֲ���
//
//
//////////////////////////////////////////////////////////////////////////


#define DEF_ALLOCATE_NULL	((const void*)0)

// -----------------------------------------------------------------------------------------


DEF_TEMPLETE_COCTETS
COctets<_A>::COctets()
{
	stream = NULL;
    datalen = 0;
	buflen = 0;
    dynamic = FALSE;
}

/*
 *  
 */
DEF_TEMPLETE_COCTETS
COctets<_A>::COctets(UINT nLen)
{
	ACE_ASSERT(0 < nLen);

	stream = allocator.allocate(nLen, DEF_ALLOCATE_NULL);

	buflen = nLen;
	datalen = 0;
	dynamic = TRUE;
}

/*
 *  ���fAllocByDll == TRUE�������stream���ڴ��Ǳ�����������
 *  ���fAllocByDll == FALSE�������stream���ڴ�ֱ��ָ��pDatas.
 *  ��ʱ�û������Լ�����(�ͷ�)����ڴ�
 */
DEF_TEMPLETE_COCTETS
COctets<_A>::COctets(const BYTE* pDatas, UINT nLenOfDatas, BOOL fAllocByDll)
{
	ACE_ASSERT(nLenOfDatas);

	datalen = buflen = nLenOfDatas;
	dynamic = fAllocByDll;

	if (fAllocByDll)
	{
		stream = allocator.allocate(buflen, DEF_ALLOCATE_NULL);

		if( stream )
		{
			memcpy(stream, pDatas, datalen*sizeof(BYTE));
		}
		else
		{
			datalen = buflen = 0;
		}
	}
	else
	{
		stream = (BYTE*)pDatas;
	}
}

/*
 *  ���fAllocByDll == TRUE�������stream���ڴ��Ǳ�����������
 *  ���fAllocByDll == FALSE�������stream���ڴ�ֱ��ָ��pDatas.
 *  ��ʱ�û������Լ�����(�ͷ�)����ڴ�
 */
DEF_TEMPLETE_COCTETS
COctets<_A>::COctets(const char* pDatas, UINT nLenOfDatas, BOOL fAllocByDll)
{
	ACE_ASSERT(nLenOfDatas);

	datalen = buflen = nLenOfDatas;
	dynamic = fAllocByDll;

	if (fAllocByDll)
	{
		stream = allocator.allocate(buflen, DEF_ALLOCATE_NULL);
		
		if( stream )
		{
			memcpy(stream, pDatas, datalen*sizeof(BYTE));
		}
		else
		{
			datalen = buflen = 0;
		}
	}
	else
	{
		stream = (BYTE *)pDatas;
	}
}

/*
 *  ���ƹ��캯��
 */
DEF_TEMPLETE_COCTETS
COctets<_A>::COctets(const COctets<_A>& octet)
{
	datalen = buflen = octet.datalen; // Only data clone

	if ( 0 < buflen )
	{
		stream = allocator.allocate(buflen, DEF_ALLOCATE_NULL);

		if( stream )
		{
			memcpy(stream, octet.stream, datalen*sizeof(BYTE));
		}
		else
		{
			datalen = buflen = 0;
		}

		dynamic = TRUE;
	}
	else
	{
		dynamic = FALSE;
		stream = NULL;
	}	
}

/*
 *  pDatas:	��NULL������ԭʼ���ݵ�ָ��
 */
DEF_TEMPLETE_COCTETS
COctets<_A>::COctets(const char * pDatas)
{
	ACE_ASSERT(pDatas && *pDatas);
	buflen = datalen = strlen(pDatas);

	if (buflen > 0)
	{
		stream = allocator.allocate(buflen, DEF_ALLOCATE_NULL);

		if( stream )
		{
			memcpy(stream, pDatas, datalen*sizeof(BYTE));
		}
		else
		{
			datalen = buflen = 0;
		}

		dynamic = TRUE;
	}
	else
	{
		dynamic = FALSE;
		stream = NULL;
	}
}

/*
 *  ����
 */
DEF_TEMPLETE_COCTETS
COctets<_A>::~COctets()
{
	Free();
}


DEF_TEMPLETE_COCTETS
BOOL COctets<_A>::SetBufferSize(UINT nLen)
{
	Free();

	if (nLen > 0)
	{
		if ((stream = allocator.allocate(nLen)) != NULL)
		{
			buflen = nLen;
			datalen = 0;
			dynamic = TRUE;
			return true;
		}

		return false;
	}

	return true;
}

/*
 *  �ͷ��ڲ��������ݣ��������ͷŶ�����
 */
DEF_TEMPLETE_COCTETS
void COctets<_A>::Free()
{
	if (buflen > 0 && NULL != stream )
	{
		if (dynamic)		
		{
			allocator.deallocate(stream, buflen);
		}

		stream = NULL;
		buflen = 0;
		datalen = 0;
		dynamic = FALSE;
	}
}

/*
 *  ���ݲ�������ʼ������, fAllocByDll�ĺ���ͬ���캯��
 */
DEF_TEMPLETE_COCTETS
BOOL COctets<_A>::SetData(const BYTE* string, UINT nLength, BOOL fAllocByDll)
{
	if (!fAllocByDll)
	{
		Free();
		stream = (BYTE *)string;
		buflen = nLength;
		datalen = nLength;

		return true;
	}
	else
	{
		if (nLength > buflen)
		{
			Free();
			stream = allocator.allocate(nLength*2);

			if( stream )
			{
				buflen = nLength*2;
			}
			else
			{
				datalen = buflen = 0;
			}
			
			dynamic = TRUE;
		}

		if ( NULL != stream )
		{
			datalen = nLength;
			memcpy(stream, string, nLength);

			return true;
		}	

		return false;
	}
}

/*
 *  ���ݲ�������ʼ������
 */
DEF_TEMPLETE_COCTETS
BOOL COctets<_A>::SetDataSize(UINT nLength)
{
	if (nLength > buflen)
	{
		Free();

		stream = allocator.allocate(nLength*2);
		
		if( stream )
		{
			buflen = nLength*2;
		}
		else
		{
			datalen = buflen = 0;
		}

		dynamic = TRUE;
	}

	if (NULL != stream)
	{
		datalen = nLength;

		return true;
	}

	return false;
}

/*
 *
 */
DEF_TEMPLETE_COCTETS
BOOL COctets<_A>::ExtendBuffer(UINT nExtendLen, BOOL bDataCopy)
{
	if ( 0 == nExtendLen )
	{
		return true;
	}
	
	BYTE* pDatas = allocator.allocate(this->buflen + nExtendLen);
	if ( NULL == pDatas)
	{
		return false;
	}

	if (bDataCopy)
	{
		memcpy(pDatas, this->stream, this->datalen);
	}

	allocator.deallocate(this->stream);		
				
	this->stream = pDatas;
	this->buflen = (this->datalen + nExtendLen);
	this->dynamic = TRUE;
	
	return true;
}

/*
 *  ����һ������ (��ʹ���귵��ֵ���û��������delete�����ͷŷ��صĶ���)
 */
DEF_TEMPLETE_COCTETS
COctets<_A>* COctets<_A>::Clone() const
{
	return new COctets(*this);
}

/*
 *  ���ַ�������������, ���ظ��ƺ�ı���������
 *  �ں����ڲ����Ѿ��ͷŵ���������ǰ���ڴ�
 */
DEF_TEMPLETE_COCTETS
COctets<_A>& COctets<_A>::operator = (const COctets& octet)
{
	SetData(octet.stream, octet.datalen);
	return (*this);
}

/*
 *  ���ַ�������������, ���ظ��ƺ�ı���������
 *  �ں����ڲ����Ѿ��ͷŵ���������ǰ���ڴ�
 */
DEF_TEMPLETE_COCTETS
COctets<_A>& COctets<_A>::operator = (const char* s)
{
	SetData((const BYTE*)s, strlen(s));
	return (*this);
}



/*
 *	
 */
DEF_TEMPLETE_COCTETS
BOOL COctets<_A>::Append(const BYTE* pDatas, UINT nLenOfDatas)
{
	ACE_ASSERT(NULL != pDatas);
	if ( NULL == pDatas || 0 == nLenOfDatas )
	{
		return true;
	}

	if (this->datalen + nLenOfDatas <= this->buflen)
	{
		memcpy(stream + datalen, pDatas, nLenOfDatas);
		this->datalen += nLenOfDatas;
		return true;
	}

	BYTE* pNew = allocator.allocate((this->datalen + nLenOfDatas)*2, DEF_ALLOCATE_NULL);
	if ( NULL == pNew )
	{
		return false;
	}

	memcpy(pNew, this->stream, this->datalen);
	memcpy(pNew + this->datalen, pDatas, nLenOfDatas);

	allocator.deallocate(this->stream, this->buflen);		
	this->stream = pNew;
	this->datalen += nLenOfDatas;
	this->buflen = (this->datalen*2);
	this->dynamic = TRUE;
	
	return true;
}

/*
 *  �����±�ȡstream�еĶ�Ӧ�ַ�
 */
DEF_TEMPLETE_COCTETS
BYTE& COctets<_A>::operator[](const UINT index)
{
	ACE_ASSERT(index < datalen);
	return stream[index];
}

/*
 *	�����±�ȡstream�еĶ�Ӧ�ַ�
 */
DEF_TEMPLETE_COCTETS
BYTE COctets<_A>::operator[](const UINT index) const
{
	ACE_ASSERT(index < datalen);
	return stream[index];
}

/*
 *  ��һ���ĳ����ڣ��Ƚ�������λ�鴮(COctets)�Ĵ�С
 *  ��� > octet(string), ����һ������0������
 *  ��� = octet(string), ����0
 *  ��� < octet(string), ���򷵻�һ��С��0����	
 */
DEF_TEMPLETE_COCTETS
int COctets<_A>::Compare(const BYTE * string, UINT nChars) const
{
	UINT nMinLength = mymin(this->datalen, nChars);

	//�ȱȽ��ַ���
	int iRes = memcmp(this->stream, string, nMinLength*sizeof(BYTE));

	//������ַ����ڶ���ȣ���Ƚϳ���
	if (iRes == 0 )
	{
		iRes = this->datalen - nChars;
	}

	return iRes;
}

/*
 *  ����������ļ�
 */
//DEF_TEMPLETE_COCTETS
//void COctets<_A>::Save(std::ofstream& fout) const
//{
//	fout.write((char*)(&datalen), sizeof(datalen));
//	if (datalen > 0)
//		fout.write((char*)stream, datalen);
//}

/*
 *  ��������ļ��ж�ȡ��������
 */
//DEF_TEMPLETE_COCTETS
//void COctets<_A>::Load(std::ifstream& fin)
//{
//	Free();
//
//	fin.read((char*)(&datalen),sizeof(datalen));
//	if (datalen > 0)
//	{
//		
//		stream = allocator.allocate(datalen);
//		fin.read((char *)stream, datalen);
//		buflen = datalen;
//		dynamic	= TRUE;
//	}		
//}


/*
 *	ѹ��
 */
DEF_TEMPLETE_COCTETS
BOOL COctets<_A>::Compress()
{
	if (datalen == 0)
	{
		Free();
		return true;
	}
	
	if (buflen > datalen)
	{
		BYTE* pNew = allocator.allocate(datalen);

		if( pNew )
		{
			memcpy(pNew, stream, datalen);
			allocator.deallocate(stream);

			stream = pNew;
			buflen = datalen;

			dynamic = TRUE;

			return true;
		}

		return false;
		
	}

	return true;
}



