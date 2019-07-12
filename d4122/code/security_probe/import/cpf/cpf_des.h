//////////////////////////////////////////////////////////////////////////
//cpf_des.h

#pragma once

#include "cpf/cpf.h"

class CPF_CLASS CPF_DES
{
public:
		//使用key中56位密钥加/解密data中的数据。data默认为为64位数据块
		//返回值为1表示成功加/解密，否则失败
		int encrypt (const unsigned char key[8],unsigned char* data, int blocks = 1 );
		int decrypt (const unsigned char key[8],unsigned char* data, int blocks = 1 );

		//使用key中56位密钥加/解密data中任意长度数据。在调用加/解密函数前，可以将
		//待处理数据复制到新的缓冲区，缓冲区大小可由extend函数计算
		int yencrypt (const unsigned char key[8],unsigned char* data, int size );
		int ydecrypt (const unsigned char key[8],unsigned char* in, int blocks, int* size = 0 );

		static int extend ( int size ) { return (size/8+1)*8; };

private:
		 void des(unsigned char* in, unsigned char* out, int blocks);
		 void des_block(unsigned char* in, unsigned char* out);

private:
	
		unsigned int KnL[32];
		enum Mode { ENCRYPT, DECRYPT };
		void deskey(const unsigned char key[8], Mode md);
		void usekey(unsigned int *);
		void cookey(unsigned int *);

private:
		void scrunch(unsigned char *, register unsigned int *);
		 void unscrun(register unsigned int *, unsigned char *);
		 void desfunc(register unsigned int *, unsigned int *);

private:
		 static unsigned char Df_Key[24];
		 static unsigned short bytebit[8];
		 static unsigned int bigbyte[24];
		 static unsigned char pc1[56];
		 static unsigned char totrot[16];
		 static unsigned char pc2[48];
		 static unsigned int SP1[64];
		 static unsigned int SP2[64];
		 static unsigned int SP3[64];
		 static unsigned int SP4[64];
		 static unsigned int SP5[64];
		 static unsigned int SP6[64];
		 static unsigned int SP7[64];
		 static unsigned int SP8[64];
};


