//////////////////////////////////////////////////////////////////////////
//cpf_des.h

#pragma once

#include "cpf/cpf.h"

class CPF_CLASS CPF_DES
{
public:
		//ʹ��key��56λ��Կ��/����data�е����ݡ�dataĬ��ΪΪ64λ���ݿ�
		//����ֵΪ1��ʾ�ɹ���/���ܣ�����ʧ��
		int encrypt (const unsigned char key[8],unsigned char* data, int blocks = 1 );
		int decrypt (const unsigned char key[8],unsigned char* data, int blocks = 1 );

		//ʹ��key��56λ��Կ��/����data�����ⳤ�����ݡ��ڵ��ü�/���ܺ���ǰ�����Խ�
		//���������ݸ��Ƶ��µĻ���������������С����extend��������
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


