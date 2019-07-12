#ifndef _OCTETS_H_
#define _OCTETS_H_

/*--------------------------------------------------------
* 版本 : v1.0

		
* 程序说明:
		关于内存块的操作,该内存中的数据是以八位组的方式进行组合,
		而不是以 0 为结束的字符串,这一点要特别的注意 

* 示例 : 
		#include "Octets.h"
		
		main()
		{
			return;
		}
		
--------------------------------------------------------*/

//#include "ArithmeticDef.h"
//#include "Mem.h"
//#include <fstream>
#include "cpf/ostypedef.h"
#include "cpf/cpf.h"
#include "cpf/memmgr.h"
#include "cpf/MyStr.h"


#define DEF_TEMPLETE_COCTETS	template<class _A>

//////////////////////////////////////////////////////////////////////////
//
//				定义部分
//
//////////////////////////////////////////////////////////////////////////

struct _OCTETS
{
    BYTE * stream;
    UINT   datalen;
    UINT   buflen;
    BOOL   dynamic;
};
typedef struct _OCTETS OCTETS;

template<class _A = std::allocator<BYTE> >
class COctets : public _OCTETS
{
public:
	COctets();
	COctets(UINT nLen);
	COctets(const COctets& octet);
	COctets(const BYTE * pDatas, UINT nLenOfDatas, BOOL fAllocByDll = TRUE);
	COctets(const char * pDatas, UINT nLenOfDatas, BOOL fAllocByDll = TRUE);
	COctets(const char * pDatas);
	
	virtual ~COctets();
	
public:
	void Free();
	
	BOOL SetData(const BYTE* string, UINT nLength, BOOL fAllocByDll = TRUE);
	BOOL SetDataSize(UINT nLength);
	BOOL SetBufferSize(UINT nLength);
	BOOL ExtendBuffer(UINT nExtendLen, BOOL bDataCopy = TRUE);
	COctets* Clone() const;
	
	COctets& operator = (const COctets& octet);
	COctets& operator = (const char* s);
	
	COctets& operator += (const COctets& octet)
	{		Append(octet); return *this;	}

	COctets& operator += (const char* octet)
	{		Append(octet); return *this;	}

	COctets& operator += (const char ch)
	{		Append(ch); return *this;	}

	BOOL Append(const COctets& octet)	{return Append(octet.GetData(),octet.GetDataLength());}
	BOOL Append(const char* octet)		{return Append((const BYTE *)octet,strlen(octet));}
	BOOL Append(const BYTE* pDatas, UINT nLenOfDatas);

	BOOL AppendUint8(const char ch)			{return Append((const BYTE *)&ch,sizeof(ch));}
	BOOL AppendUint16(ACE_UINT16 data)		{return Append((const BYTE *)&data,sizeof(data));}
	BOOL AppendUint32(ACE_UINT32 data)		{return Append((const BYTE *)&data,sizeof(data));}
	BOOL AppendUint64(ACE_UINT64 data)		{return Append((const BYTE *)&data,sizeof(data));}

	BOOL AppendLenAndString(const char * mystring,int len)
	{
		if( !mystring || len == 0 )
		{
			if( !AppendUint32(1) )
				return false;

			return AppendUint8(0);
		}
		else
		{
			if( !AppendUint32((ACE_UINT32)len) )
				return false;

			return Append((const BYTE *)mystring,len);
		}
	}

	BOOL AppendLenAndString(const char * mystring)
	{
		if( mystring )
		{
			int len = (int)strlen(mystring)+1;
			return AppendLenAndString(mystring,len);
		}
		else
		{
			if( !AppendUint32(1) )
				return false;

			if( !AppendUint8(0) )
				return false;

			return true;
		}
	}

	BOOL AppendLenAndString(const std::string& mystring)
	{
		return AppendLenAndString(mystring.c_str(),(int)mystring.size()+1);
	}

	BOOL AppendLenAndString(const CStr& mystring)
	{
		return AppendLenAndString(mystring.GetString(),mystring.GetLength()+1);
	}

	BOOL AppendOneZero()	{return AppendUint8(0);}
	
	BYTE& operator[](const UINT index);
	BYTE operator[](const UINT index) const;

	BOOL IsEmpty() const {return datalen == 0;}
	void Empty() {datalen = 0;}
	void Reset() {datalen = 0;}
	BOOL Compress();
	
	// 比较
	/*
	*  在一定的长度内，比较两个八位组串(COctets)的大小
	*  如果 > octet(string), 返回一个大于0的数。
	*  如果 = octet(string), 返回0
	*  如果 < octet(string), 否则返回一个小于0的数	
	*/
	int Compare(const BYTE* octet, UINT nChars) const;
	
	//如果nChars==-1表示nChars=octet.GetDataLength()
	int Compare(const COctets& octet, UINT nChars=-1) const
	{
		if( -1 == nChars )
		{
			nChars = octet.GetDataLength();
		}

		return Compare(octet.GetData(),nChars);	
	}

	int Compare(const char* octet) const
	{	return Compare(octet,  strlen(octet));	}
	
	// 存盘
	void Save(std::ofstream& fout) const;
	void operator >> (std::ofstream& fout) const {Save(fout);}
	
	// 读盘
	//void Load( std::ifstream& fin );	
	//void operator << (std::ifstream& fin) {Load(fin);}
	
	BYTE* GetData() const {return stream;}
	UINT GetDataLength() const {return datalen;}
	UINT GetBufLength() const {return buflen;}
	
private:
	_A			allocator;	// 内存分配器

};

typedef COctets< std::allocator<BYTE> >		CStdOctets;
typedef COctets< CPF::CPF_allocator<BYTE> >	CCPFOctets;

#include "Octets.inl"

#endif // _OCTETS_H_

