// CPF_MD5.h: interface for the CMD5 class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CPF_MD5_H__AC926F6A_E7D5_48F3_B7EC_769DAA778203__INCLUDED_)
#define AFX_CPF_MD5_H__AC926F6A_E7D5_48F3_B7EC_769DAA778203__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "cpf/cpf.h"

#define MD5_EQUAL(x,y) \
	((*(ACE_UINT64 *)((char *)(x))) == (*(ACE_UINT64 *)((char *)(y))) \
	&& 	(*(ACE_UINT64 *)((char *)(x)+8)) == (*(ACE_UINT64 *)((char *)(y)+8)))

#define MD5_CPY(x,y) \
	{\
	(*((ACE_UINT64 *)((char *)(x)))) = (*((ACE_UINT64 *)((char *)(y))));\
	(*(ACE_UINT64 *)(((char *)(x))+8)) = (*((ACE_UINT64 *)(((char *)(y))+8)));\
	}


namespace CPF{

class CPF_CLASS CMD5  
{
public:
	typedef unsigned char	MD5128_VALUE[16];

public:
	CMD5();
	
	void MD5Init()
	{
		init(state);

		lastBlockLen=0;
		messagelen=0;
	}

	//往里面添加数据
	void MD5AddBlock(const unsigned char * message,unsigned int length);

	//数据添加结束,输出一个128位的整数(16个字节).
	void MD5Finish(unsigned char * digest);

	static void MD5Direct(const unsigned char * message,unsigned int length,unsigned char * digest)
	{
		CMD5 instance;

		instance.MD5Init();

		instance.MD5AddBlock(message,length);

		instance.MD5Finish(digest);
	}

	static void MD5Print (unsigned char digest[16])
	{
		unsigned int i;
		for (i = 0; i < 16; i++)
			printf ("%02x ", digest[i]);

		printf("\n");
	}
private:
	static void encode (unsigned char * output,const unsigned int * input,unsigned int len);
	static void decode (unsigned int * output,const unsigned char * input,unsigned int len);	
	static int  padding(const unsigned char * lastblock,unsigned int lastblocklen,unsigned int messagelen,unsigned char * padBuff);	
	static void init(unsigned int state[4]);	
	static void block(const unsigned char * Y,unsigned int state[4]);
	
private:
	unsigned int state[4];
	unsigned char lastBlock[64];
	unsigned int lastBlockLen;
	unsigned int messagelen;
};

}

#endif // !defined(AFX_CPF_MD5_H__AC926F6A_E7D5_48F3_B7EC_769DAA778203__INCLUDED_)
