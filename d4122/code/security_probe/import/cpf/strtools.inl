//////////////////////////////////////////////////////////////////////////
//strtools.inl

#include "strtools.h"

template<typename chartype>
int CPF::GetWordNums(const chartype* source, chartype sep,BOOL bTrunctEmpty)
{
	size_t numtchars = ACE_OS::strlen(source);
	int nums_seg = 0;

	while( 1 )
	{
		int len = -1;

		const chartype * token = CPF::separate_token_const(source,numtchars,sep,len,bTrunctEmpty);

		if( !token )
			break;

		++nums_seg;

	}

	return nums_seg;
}



template<typename chartype>
int CPF::GetWordNums(const chartype* source,size_t numtchars,
					 chartype sep,BOOL bTrunctEmpty)
{
	int nums_seg = 0;

	while( 1 )
	{
		int len = -1;

		const chartype * token = CPF::separate_token_const(source,numtchars,sep,len,bTrunctEmpty);

		if( !token )
			break;

		++nums_seg;

	}

	return nums_seg;
}


template<typename chartype>
BOOL CPF::GetIntWord(int pos,const chartype* source, int& data,chartype sep,BOOL bTrunctEmpty)
{
	std::basic_string<chartype> strvalue;

	if( CPF::GetWord(pos,source,strvalue,sep,bTrunctEmpty) )
	{
		sscanf(strvalue.c_str(),"%d",&data);
		return true;
	}

	return false;
}

template<typename chartype>
BOOL CPF::GetUintWord(int pos,const chartype* source, unsigned int& data,chartype sep,BOOL bTrunctEmpty)
{
	std::basic_string<chartype> strvalue;

	if( CPF::GetWord(pos,source,strvalue,sep,bTrunctEmpty) )
	{
		sscanf(strvalue.c_str(),"%u",&data);
		return true;
	}

	return false;
}

template<typename chartype>
BOOL CPF::GetUint64Word(int pos,const chartype* source, ACE_UINT64& data,chartype sep,BOOL bTrunctEmpty)
{
	std::basic_string<chartype> strvalue;

	if( CPF::GetWord(pos,source,strvalue,sep,bTrunctEmpty) )
	{
		sscanf(strvalue.c_str(),"%llu",&data);
		return true;
	}

	return false;
}


template<typename chartype>
int CPF::GetWord_Pointer(int pos,const chartype* source, size_t numtchars,const chartype*& result,chartype sep,BOOL bTrunctEmpty)
{
	result = NULL;

	ACE_ASSERT(0 <= pos );
	if (pos < 0 )
		return -1;

	int nums_seg = 0;

	while( 1 )
	{
		int len = -1;

		const chartype * token = CPF::separate_token_const(source,numtchars,sep,len,bTrunctEmpty);

		if( !token )
			break;

		if( nums_seg == pos )
		{
			result = token;
			return len;
		}

		++nums_seg;

	}

	return -1;
}

template<typename chartype,typename DSTSTRTYPE>
BOOL CPF::GetWord(int pos,const chartype* source, size_t numtchars,DSTSTRTYPE& result,chartype sep,BOOL bTrunctEmpty)
{
	result.clear();

	const chartype * pdata = NULL;
	int len = GetWord_Pointer<chartype>(pos,source,numtchars,pdata,sep,bTrunctEmpty);

	if( len == -1 )
		return false;

	result.assign(pdata,len);

	return true;
}

template<typename chartype>
int CPF::GetWords_Pointer(IN const chartype* source,IN size_t numtchars,
						  OUT std::vector< std::pair<const chartype*,int> >& result,IN chartype sep,BOOL bTrunctEmpty)
{
	result.clear();

	int count = GetWordNums(source,numtchars,sep,bTrunctEmpty);

	if( count <= 0 )
		return 0;

	result.reserve(count);

	while( 1 )
	{
		int len = -1;

		const chartype * token = CPF::separate_token_const(source,numtchars,sep,len,bTrunctEmpty);

		if( !token )
			break;

		result.push_back(std::make_pair(token,len));

	}

	ACE_ASSERT(count == (int)result.size() );

	return (int)result.size();
}

template<typename chartype,typename DSTSTRTYPE>
int CPF::GetWords(const chartype* source,size_t numtchars,std::vector<DSTSTRTYPE>& result,chartype sep,BOOL bTrunctEmpty)
{
	result.clear();

	int count = GetWordNums(source,numtchars,sep,bTrunctEmpty);

	if( count <= 0 )
		return 0;

	result.reserve(count);

	while( 1 )
	{
		int len = -1;

		const chartype * token = CPF::separate_token_const(source,numtchars,sep,len,bTrunctEmpty);

		if( !token )
			break;

		result.push_back(DSTSTRTYPE(token,len));

	}

	ACE_ASSERT(count == (int)result.size() );

	return (int)result.size();

}

template<typename chartype,typename DSTSTRTYPE>
int CPF::ESC_GetWords(const chartype* source,std::vector<DSTSTRTYPE>& result,chartype sep,chartype esc_char,BOOL bTrunctEmpty)
{
	result.clear();

	while( source )
	{
		DSTSTRTYPE one_result;

		if( CPF::esc_separate_token_const(source,sep,esc_char,one_result,bTrunctEmpty) )
		{
			result.push_back(one_result);
		}
		else
		{
			break;
		}

	}

	return (int)result.size();

}

template<typename chartype,typename DSTSTRTYPE>
BOOL CPF::GetFirstWord(const chartype* source, size_t numtchars, DSTSTRTYPE& result,chartype sep,BOOL bTrunctEmpty)
{	
	int count = GetWordNums(source,numtchars,sep,bTrunctEmpty);

	if( count <= 0 )
		return false;

	return GetWord(0,source,result,sep,bTrunctEmpty);

	return true;

}

template<typename chartype>
int CPF::GetFirstWord(const chartype* source,size_t numtchars,chartype*& result,chartype sep,BOOL bTrunctEmpty)
{
	int count = GetWordNums(source,numtchars,sep,bTrunctEmpty);

	if( count <= 0 )
		return -1;

	return GetWord_Pointer<chartype>(0,source,result,sep,bTrunctEmpty);
}

//解析出最后一个单词 -1终止
template<typename chartype,typename DSTSTRTYPE>
BOOL CPF::GetLastWord(const chartype* source, size_t numtchars,DSTSTRTYPE& result,chartype sep,BOOL bTrunctEmpty)
{
	int count = GetWordNums(source,numtchars,sep,bTrunctEmpty);

	if( count <= 0 )
		return false;

	return GetWord(count-1,source,result,sep,bTrunctEmpty);
}

template<typename chartype,typename DSTSTRTYPE>
int CPF::GetLastWord(const chartype* source, size_t numtchars,chartype*& result,chartype sep,BOOL bTrunctEmpty)
{
	int count = GetWordNums(source,numtchars,sep,bTrunctEmpty);

	if( count <= 0 )
		return -1;

	return GetWord_Pointer<chartype>(count-1,source,result,sep,bTrunctEmpty);
}

template<typename chartype>
const chartype * CPF::separate_token_const(const chartype* &source,size_t& sourcelen,
								   chartype sep, 
								   int& len,
								   BOOL bTrunctEmpty)
{
	if( !source )
	{
		len = 0;
		return NULL;
	}

	const chartype * pend = source + sourcelen;

	while( source < pend  )
	{
		const chartype * ptemp = source;

		while( *source != sep && source < pend )
			++source;

		if( ptemp != source || !bTrunctEmpty )
		{
			len = (int)(source-ptemp);

			if( *source == sep )
			{
				++source;

				sourcelen = pend - source;
			}
			else
			{
				sourcelen = -1;

				source = NULL;
			}

			return ptemp;
		}
		else
		{
			++source;
		}
	}

	if( pend == source && !bTrunctEmpty)
	{
		len = (int)(pend - source);

		const chartype * data = source;

		sourcelen = -1;
		source = NULL;

		return data;

	}
	else
	{
		len = 0;

		sourcelen = -1;
		source = NULL;

		return NULL;
	}
}

template<typename chartype,typename DSTSTRTYPE>
BOOL CPF::esc_separate_token_const(const chartype* &source,
							  chartype sep, 
							  chartype esc_char,//转义字符
							  DSTSTRTYPE& result,							  
							  BOOL bTrunctEmpty)
{
	BOOL esc_status = false;

	int last_len = 0;

	while( source )
	{
		const chartype * ptemp = source;

		while( *source != sep && *source && *source != esc_char )
			++source;
		
		int cur_len = (int)(source-ptemp);

		if( *source == sep )
		{
			if( cur_len == 0 )
			{
				if( bTrunctEmpty )
				{
					++source;

					if( last_len )
					{
						return true;
					}
				}
				else
				{
					++source;

					return true;
				}
			}
			else
			{
				result.append(ptemp,cur_len);

				++source;

				return true;
			}
		}
		else if( *source == esc_char )
		{
			result.append(ptemp,cur_len);

			++source;

			if( *source )
			{
				result.append(source,1);
				++source;

				last_len += cur_len+1;
			}
			else
			{
				source = NULL;
				return false;
			}			
		}
		else
		{
			if( cur_len == 0 )
			{
				if( bTrunctEmpty )
				{
					source = NULL;

					return false;
				}
				else
				{
					result.clear();

					source = NULL;

					return true;
				}
			}
			else
			{
				result.append(ptemp,cur_len);

				source = NULL;

				return true;
			}
		}	
	}

	ACE_ASSERT(FALSE);
	return false;
}

template<typename chartype>
const chartype * CPF::separate_token_const(const chartype* &source,
													chartype sep, 
													int& len,
													BOOL bTrunctEmpty)
{
	if( !source )
	{
		len = 0;
		return NULL;
	}

	while( *source )
	{
		const chartype * ptemp = source;

		while( *source != sep && *source )
			++source;

		if( ptemp != source || !bTrunctEmpty )
		{
			len = (int)(source-ptemp);

			if( *source == sep )
				++source;
			else
				source = NULL;

			return ptemp;
		}
		else
		{
			++source;
		}
	}

	if( *source == 0 && !bTrunctEmpty )
	{
		len = 0;

		const chartype * data = source;

		source = NULL;

		return data;

	}
	else
	{
		len = 0;
		return NULL;
	}
}

template<typename chartype>
chartype * CPF::separate_token(chartype* &source,
								   chartype separetor,
								   BOOL bTrunctEmpty)
{
	int len = -1;

	const chartype * out = 
		CPF::separate_token_const((const chartype *&)source,separetor,len,bTrunctEmpty);

	if( out )
	{
		*(chartype *)(out+len) = 0;
	}

	return (chartype *)out;

}
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

inline int CPF::cpf_isupper(char c)
{
	if( ((unsigned int)c)+1 > 255 )
		return false;
	return ::isupper((int)c);
}
inline int CPF::cpf_isupper(wchar_t c)
{
	if( ((unsigned int)c) > 255 )
		return false;
	return ::iswupper((int)c);
}

inline int CPF::cpf_islower(char c)
{	
	if( ((unsigned int)c) > 255 )
		return false;
	return ::islower((int)c);	
}
inline int CPF::cpf_islower(wchar_t c)
{	
	if( ((unsigned int)c) > 255 )
		return false;
	return ::iswlower((int)c);	
}


inline int CPF::cpf_isdigitstring(const char *p,int radix)
{
	if(radix == 10 )
	{
		if( *p == '-' )
			++p;

		while(*p)
		{
			if(*p >= '0' && *p <= '9') 
				++p;
			else 
				return false;
		}

		return true;
	}

	if( radix == 16 )
	{
		if( *p == '0' && (*(p+1) == 'x' || *(p+1) == 'X') )
		{
			++p;
			++p;
		}			

		while(*p)
		{
			if( (*p >= '0' && *p <= '9') 
				|| (*p >= 'a' && *p <= 'f')
				|| (*p >= 'A' && *p <= 'F') )
			{
				++p;
			}
			else
			{
				return false;
			}
		}

		return true;
	}

	if( radix == 2 )
	{
		while(*p)
		{
			if(*p != '0' && *p != '1')
				return false;
		}

		return true;
	}

	return false;

}


inline int CPF::cpf_isdigitstring(const wchar_t *p,int radix)
{
	if(radix == 10 )
	{
		if( *p == L'-' )
			++p;

		while(*p)
		{
			if(*p >= L'0' && *p <= L'9') 
				++p;
			else 
				return false;
		}

		return true;
	}

	if( radix == 16 )
	{
		if( *p == L'0' && (*(p+1) == L'x' || *(p+1) == L'X') )
		{
			++p;
			++p;
		}			

		while(*p)
		{
			if( (*p >= L'0' && *p <= L'9') 
				|| (*p >= L'a' && *p <= L'f')
				|| (*p >= L'A' && *p <= L'F') )
			{
				++p;
			}
			else
			{
				return false;
			}
		}

		return true;
	}

	if( radix == 2 )
	{
		while(*p)
		{
			if(*p != L'0' || *p != L'1')
				return false;
		}

		return true;
	}

	return false;

}


