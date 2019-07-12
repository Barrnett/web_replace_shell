//////////////////////////////////////////////////////////////////////////
//Data_Stream.h

#pragma once

#include "cpf/cpf.h"
#include "cpf/ostypedef.h"

class CPF_CLASS CData_Stream_LE
{
public:
	static inline ACE_UINT8 GetUint8(const char *& pPtr)
	{
		return (ACE_UINT8)(*pPtr++);
	}

	static inline ACE_UINT16 GetUint16(const char *& pPtr)
	{
		ACE_UINT16 nValue=LBUF_TO_WORD(pPtr);

		pPtr += sizeof(ACE_UINT16);

		return nValue;
	}

	static inline ACE_UINT32 GetUint32(const char *& pPtr)
	{
		ACE_UINT32 nValue=LBUF_TO_UINT(pPtr);

		pPtr += sizeof(ACE_UINT32);

		return nValue;
	}

	static inline ACE_UINT64 GetUint64(const char *& pPtr)
	{
		ACE_UINT64 nValue=LBUF_TO_UI64(pPtr);

		pPtr += sizeof(ACE_UINT64);

		return nValue;
	}

	static inline const char * GetFixString(const char *& pPtr,int nStrLen)
	{
		const char * prtn = pPtr;

		pPtr += nStrLen;

		return prtn;
	}

	static inline const char * GetVarString(const char * &pPtr,int & nStrLen,int sizeof_len=sizeof(ACE_UINT32))
	{
		if( sizeof_len == sizeof(ACE_UINT32) )
			nStrLen = (int)GetUint32(pPtr);
		else if( sizeof_len == sizeof(ACE_UINT16) )
			nStrLen = (int)GetUint16(pPtr);
		else if( sizeof_len == sizeof(ACE_UINT8) )
			nStrLen = (int)GetUint8(pPtr);
		else if( sizeof_len == sizeof(ACE_UINT64) )
			nStrLen = (int)GetUint64(pPtr);
		else
		{
			nStrLen = 0;
			ACE_ASSERT(FALSE);
		}

		return GetFixString(pPtr,nStrLen);
	}

	static inline const char * GetString(const char * &pPtr,int sizeof_len=sizeof(ACE_UINT32))
	{
		int nStrLen = 0;

		return GetVarString(pPtr,nStrLen,sizeof_len);
	}

	static inline const char * GetSafeString(const char * &pPtr,int sizeof_len=sizeof(ACE_UINT32))
	{
		int name_string_len = 0;
		
		if( sizeof_len == sizeof(ACE_UINT32) )
			name_string_len = (int)GetUint32(pPtr);
		else if( sizeof_len == sizeof(ACE_UINT16) )
			name_string_len = (int)GetUint16(pPtr);
		else if( sizeof_len == sizeof(ACE_UINT8) )
			name_string_len = (int)GetUint8(pPtr);
		else if( sizeof_len == sizeof(ACE_UINT64) )
			name_string_len = (int)GetUint64(pPtr);
		else
		{
			name_string_len = (int)GetUint32(pPtr);
			ACE_ASSERT(FALSE);
		}

		//格式错误
		if( pPtr[name_string_len-1] != 0 )
			return NULL;

		const char * name = pPtr;

		pPtr += name_string_len;

		return name;
	}

	template<typename T>
	static inline void GetVectorInt32(const char *& pPtr,std::vector<T>& vt_data)
	{
		int count = (int)GetUint32(pPtr);

		for(int i = 0; i < count; ++i)
		{
			vt_data.push_back((T)GetUint32(pPtr));
		}
	}

	template<typename T>
	static inline void GetVectorInt64(const char *& pPtr,std::vector<T>& vt_data)
	{
		int count = (int)GetUint32(pPtr);

		for(int i = 0; i < count; ++i)
		{
			vt_data.push_back((T)GetUint64(pPtr));
		}
	}

	static inline void GetVectorString(const char *& pPtr,std::vector<std::string>& vt_data,int sizeof_len=sizeof(ACE_UINT32))
	{
		int count = (int)GetUint32(pPtr);

		for(int i = 0; i < count; ++i)
		{
			vt_data.push_back((std::string)GetString(pPtr,sizeof_len));
		}
	}

	static inline void PutUint8(char *& pPtr,ACE_UINT8 cValue)
	{
		*(pPtr++)=cValue;
	}

	static inline void PutUint16(char *& pPtr,ACE_UINT16 nValue)
	{
		*(ACE_UINT16 *)pPtr = CPF_HTOLS(nValue);

		pPtr += sizeof(ACE_UINT16);
	}

	static inline void PutUint32(char *& pPtr,ACE_UINT32 nValue)
	{
		*(ACE_UINT32 *)pPtr = CPF_HTOLL(nValue);

		pPtr += sizeof(ACE_UINT32);

	}

	static inline void PutUint64(char *& pPtr,ACE_UINT64 nValue)
	{
		*(ACE_UINT64 *)pPtr = CPF_LTOHI64(nValue);

		pPtr += sizeof(ACE_UINT64);
	}

	static inline void PutFixString(char *& pPtr,int nStrLen,const char * pdata)
	{
		if( nStrLen > 0 )
		{
			memcpy(pPtr,pdata,nStrLen);
		}

		pPtr += nStrLen;
	}

	static inline void PutVarString(char *& pPtr,int nStrLen,const char * pdata,int sizeof_len=sizeof(ACE_UINT32))
	{
		if( sizeof_len == sizeof(ACE_UINT32) )
			PutUint32(pPtr,nStrLen);
		else if( sizeof_len == sizeof(ACE_UINT16) )
			PutUint16(pPtr,nStrLen);
		else if( sizeof_len == sizeof(ACE_UINT8) )
			PutUint8(pPtr,nStrLen);
		else if( sizeof_len == sizeof(ACE_UINT64) )
			PutUint64(pPtr,nStrLen);
		else
		{			
			ACE_ASSERT(FALSE);
			PutUint32(pPtr,nStrLen);
		}

		PutFixString(pPtr,nStrLen,pdata);
	}

	//最后一个0也写入缓存区
	static inline void PutString(char *& pPtr,const char * pdata,int sizeof_len=sizeof(ACE_UINT32))
	{		
		if( pdata )
		{
			int len = (int)strlen(pdata)+1;
			PutVarString(pPtr,len,pdata,sizeof_len);
		}
		else
		{
			char temp_buf[1] ={0};

			PutVarString(pPtr,1,temp_buf,sizeof_len);
		}
	}

	//最后一个0也写入缓存区
	static inline void PutString(char *& pPtr,const std::string& data,int sizeof_len=sizeof(ACE_UINT32))
	{
		int len = (int)data.size()+1;

		PutVarString(pPtr,len,data.c_str(),sizeof_len);
	}

	template<typename T>
	static inline void PutVectorInt32(char *& pPtr,const std::vector<T>& vt_data)
	{
		PutUint32(pPtr,(ACE_UINT32)vt_data.size());

		for(size_t i = 0; i < vt_data.size(); ++i)
		{
			PutUint32(pPtr,(ACE_UINT32)vt_data[i]);
		}
	}

	template<typename T>
	static inline void PutVectorInt64(char *& pPtr,const std::vector<T>& vt_data)
	{
		PutUint32(pPtr,(ACE_UINT32)vt_data.size());

		for(size_t i = 0; i < vt_data.size(); ++i)
		{
			PutUint64(pPtr,(ACE_UINT64)vt_data[i]);
		}
	}

	static inline void PutVectorString(char *& pPtr,const std::vector<std::string>& vt_data,int sizeof_len=sizeof(ACE_UINT32))
	{
		PutUint32(pPtr,(ACE_UINT32)vt_data.size());

		for(size_t i = 0; i < vt_data.size(); ++i)
		{
			PutString(pPtr,vt_data[i],sizeof_len);
		}
	}

	//计算一个字符串，按照PutString的方式需要多大的空间
	static inline int GetStringBufLength(const char * my_string,int sizeof_len=sizeof(ACE_UINT32))
	{
		int len = sizeof_len+1;

		if( my_string )
		{
			len += (int)strlen(my_string);
		}

		return len;
	}

	static inline int GetStringBufLength(const std::string& data,int sizeof_len=sizeof(ACE_UINT32))
	{
		return (int)(sizeof_len + 1 + (int)data.size());
	}
};

class CPF_CLASS CData_Stream_BE
{
public:
	static inline ACE_UINT8 GetUint8(const char *& pPtr)
	{
		return (ACE_UINT8)(*pPtr++);
	}

	static inline ACE_UINT16 GetUint16(const char *& pPtr)
	{
		ACE_UINT16 nValue=NBUF_TO_WORD(pPtr);

		pPtr += sizeof(ACE_UINT16);

		return nValue;
	}

	static inline ACE_UINT32 GetUint32(const char *& pPtr)
	{
		ACE_UINT32 nValue=NBUF_TO_UINT(pPtr);

		pPtr += sizeof(ACE_UINT32);

		return nValue;
	}

	static inline ACE_UINT64 GetUint64(const char *& pPtr)
	{
		ACE_UINT64 nValue=NBUF_TO_UI64(pPtr);

		pPtr += sizeof(ACE_UINT64);

		return nValue;

	}

	static inline const char * GetFixString(const char *& pPtr,int nStrLen)
	{
		const char * prtn = pPtr;

		pPtr += nStrLen;

		return prtn;
	}

	static inline const char * GetVarString(const char * &pPtr,int & nStrLen,int sizeof_len=sizeof(ACE_UINT32))
	{
		if( sizeof_len == sizeof(ACE_UINT32) )
			nStrLen = (int)GetUint32(pPtr);
		else if( sizeof_len == sizeof(ACE_UINT16) )
			nStrLen = (int)GetUint16(pPtr);
		else if( sizeof_len == sizeof(ACE_UINT8) )
			nStrLen = (int)GetUint8(pPtr);
		else if( sizeof_len == sizeof(ACE_UINT64) )
			nStrLen = (int)GetUint64(pPtr);
		else
		{
			nStrLen = 0;
			ACE_ASSERT(FALSE);
		}

		return GetFixString(pPtr,nStrLen);
	}

	static inline const char * GetString(const char * &pPtr,int sizeof_len=sizeof(ACE_UINT32))
	{
		int nStrLen = 0;

		return GetVarString(pPtr,nStrLen,sizeof_len);
	}

	static inline const char * GetSafeString(const char * &pPtr,int sizeof_len=sizeof(ACE_UINT32))
	{
		int name_string_len = 0;

		if( sizeof_len == sizeof(ACE_UINT32) )
			name_string_len = (int)GetUint32(pPtr);
		else if( sizeof_len == sizeof(ACE_UINT16) )
			name_string_len = (int)GetUint16(pPtr);
		else if( sizeof_len == sizeof(ACE_UINT8) )
			name_string_len = (int)GetUint8(pPtr);
		else if( sizeof_len == sizeof(ACE_UINT64) )
			name_string_len = (int)GetUint64(pPtr);
		else
		{
			name_string_len = (int)GetUint32(pPtr);
			ACE_ASSERT(FALSE);
		}

		//格式错误
		if( pPtr[name_string_len-1] != 0 )
			return NULL;

		const char * name = pPtr;

		pPtr += name_string_len;

		return name;
	}

	static inline void PutUint8(char *& pPtr,ACE_UINT8 cValue)
	{
		*(pPtr++)=cValue;
	}

	static inline void PutUint16(char *& pPtr,ACE_UINT16 nValue)
	{
		*(ACE_UINT16 *)pPtr = ACE_HTONS(nValue);

		pPtr += sizeof(ACE_UINT16);
	}

	static inline void PutUint32(char *& pPtr,ACE_UINT32 nValue)
	{
		*(ACE_UINT32 *)pPtr = ACE_HTONL(nValue);

		pPtr += sizeof(ACE_UINT32);

	}

	static inline void PutUint64(char *& pPtr,ACE_UINT64 nValue)
	{
		*(ACE_UINT64 *)pPtr = ACE_HTONI64(nValue);

		pPtr += sizeof(ACE_UINT64);
	}

	static inline void PutFixString(char *& pPtr,int nStrLen,const char * pdata)
	{
		if( nStrLen > 0 )
		{
			memcpy(pPtr,pdata,nStrLen);
		}

		pPtr += nStrLen;
	}

	static inline void PutVarString(char *& pPtr,int nStrLen,const char * pdata,int sizeof_len=sizeof(ACE_UINT32))
	{
		if( sizeof_len == sizeof(ACE_UINT32) )
			PutUint32(pPtr,nStrLen);
		else if( sizeof_len == sizeof(ACE_UINT16) )
			PutUint16(pPtr,nStrLen);
		else if( sizeof_len == sizeof(ACE_UINT8) )
			PutUint8(pPtr,nStrLen);
		else if( sizeof_len == sizeof(ACE_UINT64) )
			PutUint64(pPtr,nStrLen);
		else
		{			
			ACE_ASSERT(FALSE);
			PutUint32(pPtr,nStrLen);
		}

		PutFixString(pPtr,nStrLen,pdata);
	}

	//最后一个0也写入缓存区
	static inline void PutString(char *& pPtr,const char * pdata,int sizeof_len=sizeof(ACE_UINT32))
	{
		if( pdata )
		{
			int len = (int)strlen(pdata)+1;

			PutVarString(pPtr,len,pdata,sizeof_len);
		}
		else
		{
			char temp_buf[1] ={0};

			PutVarString(pPtr,1,temp_buf,sizeof_len);
		}
	}

	//最后一个0也写入缓存区
	static inline void PutString(char *& pPtr,const std::string& data,int sizeof_len=sizeof(ACE_UINT32))
	{
		int len = (int)data.size()+1;

		PutVarString(pPtr,len,data.c_str(),sizeof_len);
	}

	//计算一个字符串，按照PutString的方式需要多大的空间
	static inline int GetStringBufLength(const char * my_string,int sizeof_len=sizeof(ACE_UINT32))
	{
		int len = sizeof_len+1;

		if( my_string )
		{
			len += (int)strlen(my_string);
		}

		return len;
	}

	static inline int GetStringBufLength(const std::string& data,int sizeof_len=sizeof(ACE_UINT32))
	{
		return (int)(sizeof_len + 1 + (int)data.size());
	}
};


# if defined (ACE_LITTLE_ENDIAN)
typedef CData_Stream_LE	CData_Stream_HO;//host_order
#else
typedef CData_Stream_BE	CData_Stream_HO;//host_order
#endif

typedef CData_Stream_BE	CData_Stream_NO;//net_order

