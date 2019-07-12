

//////////////////////////////////////////////////////////////////////////
//
//		
//				实现部分
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
 *  如果fAllocByDll == TRUE：对象的stream的内存是本程序来分配
 *  如果fAllocByDll == FALSE：对象的stream的内存直接指向pDatas.
 *  这时用户必须自己管理(释放)这块内存
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
 *  如果fAllocByDll == TRUE：对象的stream的内存是本程序来分配
 *  如果fAllocByDll == FALSE：对象的stream的内存直接指向pDatas.
 *  这时用户必须自己管理(释放)这块内存
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
 *  复制构造函数
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
 *  pDatas:	以NULL结束的原始数据的指针
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
 *  析构
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
 *  释放内部所有数据，但并不释放对象本身
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
 *  根据参数，初始化对象, fAllocByDll的含义同构造函数
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
 *  根据参数，初始化对象
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
 *  复制一个对象 (在使用完返回值后，用户必须调用delete函数释放返回的对象)
 */
DEF_TEMPLETE_COCTETS
COctets<_A>* COctets<_A>::Clone() const
{
	return new COctets(*this);
}

/*
 *  将字符串拷贝到自身, 返回复制后的本对象引用
 *  在函数内部，已经释放掉本对象以前的内存
 */
DEF_TEMPLETE_COCTETS
COctets<_A>& COctets<_A>::operator = (const COctets& octet)
{
	SetData(octet.stream, octet.datalen);
	return (*this);
}

/*
 *  将字符串拷贝到自身, 返回复制后的本对象引用
 *  在函数内部，已经释放掉本对象以前的内存
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
 *  根据下标取stream中的对应字符
 */
DEF_TEMPLETE_COCTETS
BYTE& COctets<_A>::operator[](const UINT index)
{
	ACE_ASSERT(index < datalen);
	return stream[index];
}

/*
 *	根据下标取stream中的对应字符
 */
DEF_TEMPLETE_COCTETS
BYTE COctets<_A>::operator[](const UINT index) const
{
	ACE_ASSERT(index < datalen);
	return stream[index];
}

/*
 *  在一定的长度内，比较两个八位组串(COctets)的大小
 *  如果 > octet(string), 返回一个大于0的数。
 *  如果 = octet(string), 返回0
 *  如果 < octet(string), 否则返回一个小于0的数	
 */
DEF_TEMPLETE_COCTETS
int COctets<_A>::Compare(const BYTE * string, UINT nChars) const
{
	UINT nMinLength = mymin(this->datalen, nChars);

	//先比较字符串
	int iRes = memcmp(this->stream, string, nMinLength*sizeof(BYTE));

	//如果在字符串内都相等，则比较长度
	if (iRes == 0 )
	{
		iRes = this->datalen - nChars;
	}

	return iRes;
}

/*
 *  将对象存入文件
 */
//DEF_TEMPLETE_COCTETS
//void COctets<_A>::Save(std::ofstream& fout) const
//{
//	fout.write((char*)(&datalen), sizeof(datalen));
//	if (datalen > 0)
//		fout.write((char*)stream, datalen);
//}

/*
 *  将对象从文件中读取对象数据
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
 *	压缩
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



