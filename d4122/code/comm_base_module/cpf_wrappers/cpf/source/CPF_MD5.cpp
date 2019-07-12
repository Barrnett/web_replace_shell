// CPF_MD5.cpp: implementation of the CMD5 class.
//
//////////////////////////////////////////////////////////////////////

#include <assert.h>
#include <stdlib.h>
#include <memory.h>

#include "cpf/CPF_MD5.h"

#define T01 0xd76aa478 /* 1 */
#define T02 0xe8c7b756 /* 2 */
#define T03 0x242070db /* 3 */
#define T04 0xc1bdceee /* 4 */
#define T05 0xf57c0faf /* 5 */
#define T06 0x4787c62a /* 6 */
#define T07 0xa8304613 /* 7 */
#define T08 0xfd469501 /* 8 */
#define T09 0x698098d8 /* 9 */
#define T10 0x8b44f7af /* 10 */
#define T11 0xffff5bb1 /* 11 */
#define T12 0x895cd7be /* 12 */
#define T13 0x6b901122 /* 13 */
#define T14 0xfd987193 /* 14 */
#define T15 0xa679438e /* 15 */
#define T16 0x49b40821 /* 16 */
#define T17 0xf61e2562 /* 17 */
#define T18 0xc040b340 /* 18 */
#define T19 0x265e5a51 /* 19 */
#define T20 0xe9b6c7aa /* 20 */
#define T21 0xd62f105d /* 21 */
#define T22 0x02441453 /* 22 */
#define T23 0xd8a1e681 /* 23 */
#define T24 0xe7d3fbc8 /* 24 */
#define T25 0x21e1cde6 /* 25 */
#define T26 0xc33707d6 /* 26 */
#define T27 0xf4d50d87 /* 27 */
#define T28 0x455a14ed /* 28 */
#define T29 0xa9e3e905 /* 29 */
#define T30 0xfcefa3f8 /* 30 */
#define T31 0x676f02d9 /* 31 */
#define T32 0x8d2a4c8a /* 32 */
#define T33 0xfffa3942 /* 33 */
#define T34 0x8771f681 /* 34 */
#define T35 0x6d9d6122 /* 35 */
#define T36 0xfde5380c /* 36 */
#define T37 0xa4beea44 /* 37 */
#define T38 0x4bdecfa9 /* 38 */
#define T39 0xf6bb4b60 /* 39 */
#define T40 0xbebfbc70 /* 40 */
#define T41 0x289b7ec6 /* 41 */
#define T42 0xeaa127fa /* 42 */
#define T43 0xd4ef3085 /* 43 */
#define T44 0x04881d05 /* 44 */
#define T45 0xd9d4d039 /* 45 */
#define T46 0xe6db99e5 /* 46 */
#define T47 0x1fa27cf8 /* 47 */
#define T48 0xc4ac5665 /* 48 */
#define T49 0xf4292244 /* 49 */
#define T50 0x432aff97 /* 50 */
#define T51 0xab9423a7 /* 51 */
#define T52 0xfc93a039 /* 52 */
#define T53 0x655b59c3 /* 53 */
#define T54 0x8f0ccc92 /* 54 */
#define T55 0xffeff47d /* 55 */
#define T56 0x85845dd1 /* 56 */
#define T57 0x6fa87e4f /* 57 */
#define T58 0xfe2ce6e0 /* 58 */
#define T59 0xa3014314 /* 59 */
#define T60 0x4e0811a1 /* 60 */
#define T61 0xf7537e82 /* 61 */
#define T62 0xbd3af235 /* 62 */
#define T63 0x2ad7d2bb /* 63 */
#define T64 0xeb86d391 /* 64 */


#define S11  7
#define S12 12
#define S13 17
#define S14 22
#define S21  5
#define S22  9
#define S23 14
#define S24 20
#define S31  4
#define S32 11
#define S33 16
#define S34 23
#define S41  6
#define S42 10
#define S43 15
#define S44 21

#define A 0x67452301
#define B 0xefcdab89
#define C 0x98badcfe
#define D 0x10325476

#define ROTATE_LEFT32(data,bits) (data)<<(bits)|(data)>>(32-(bits))
#define ROTATE_RIGHT32(data,bits) (data)>>(bits)|(data)<<(32-(bits))

#define F(x, y, z) (((x) & (y)) | ((~x) & (z)))
#define G(x, y, z) (((x) & (z)) | ((y) & (~z)))
#define H(x, y, z) ((x) ^ (y) ^ (z))
#define I(x, y, z) ((y) ^ ((x) | (~z)))

#define FF(a, b, c, d, x, s, ac) { \
	(a) += F ((b), (c), (d)) + (x) + (unsigned int)(ac); \
	(a) = ROTATE_LEFT32 ((a), (s)); \
	(a) += (b); \
}
#define GG(a, b, c, d, x, s, ac) { \
	(a) += G ((b), (c), (d)) + (x) + (unsigned int)(ac); \
	(a) = ROTATE_LEFT32 ((a), (s)); \
	(a) += (b); \
}
#define HH(a, b, c, d, x, s, ac) { \
	(a) += H ((b), (c), (d)) + (x) + (unsigned int)(ac); \
	(a) = ROTATE_LEFT32 ((a), (s)); \
	(a) += (b); \
}
#define II(a, b, c, d, x, s, ac) { \
	(a) += I ((b), (c), (d)) + (x) + (unsigned int)(ac); \
	(a) = ROTATE_LEFT32 ((a), (s)); \
	(a) += (b); \
}



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

namespace CPF
{


CMD5::CMD5()
{
  MD5Init();
}

void CMD5::MD5AddBlock(const unsigned char * message,unsigned int length)
{
	unsigned int  iblock;
	unsigned int  lastBlockSize;
	assert(message);

	messagelen+=length;

	if(lastBlockLen>0)
	{
		if(lastBlockLen+length<64)
		{
			memcpy(lastBlock+lastBlockLen,message,length);
			lastBlockLen+=length;
			return;
		}
		else
		{
			memcpy(lastBlock+lastBlockLen,message,64-lastBlockLen);
			block(lastBlock,state);
			message+=64-lastBlockLen;
			length-=64-lastBlockLen;
			lastBlockLen=0;
		}
	}

	for(iblock=0;iblock<length/64;iblock++)
	{
		block(message+iblock*64,state);
	}

	lastBlockSize=length-64*(length/64);
	if(lastBlockSize)
	{
		memcpy(lastBlock,message+length-lastBlockSize,lastBlockSize);
		lastBlockLen=lastBlockSize;
	}
}

void CMD5::MD5Finish(unsigned char * digest)
{
	int iRet;
	unsigned char padBuff[128];

	iRet=padding(lastBlock,lastBlockLen,messagelen,padBuff);

	block(padBuff,state);
	if(iRet==2) block(padBuff+64,state);

	encode(digest,state,16);
}

int CMD5::padding(const unsigned char * lastblock,unsigned int  lastblocklen,unsigned int messagelen,unsigned char * padBuff)
{
	unsigned int  bitLength[2];
	unsigned char temp[8];
	//assert(lastblocklen);
	assert(padBuff);

	memset(padBuff,0,128);
	memcpy(padBuff,lastblock,lastblocklen);
	memset(padBuff+lastblocklen,0x80,1);


	bitLength[0]=messagelen<<3;
	bitLength[1]=messagelen>>29;
	encode(temp,bitLength,8);

	if(lastblocklen<56){
		memcpy(padBuff+64-8,temp,8);
		return 1;
	}
	
	memcpy(padBuff+128-8,temp,8);
	return 2;
}


void CMD5::init(unsigned int  state[4]){
	state[0]=A;
	state[1]=B;
	state[2]=C;
	state[3]=D;
}


void CMD5::block(const unsigned char * Y,unsigned int  state[4]){
	unsigned int a,b,c,d,x[16];

	a=state[0];
	b=state[1];
	c=state[2];
	d=state[3];
	decode(x,Y,64);

	/* Round 1 */
	FF (a, b, c, d, x[ 0], S11,	T01); /* 1 */
	FF (d, a, b, c, x[ 1], S12, T02); /* 2 */
	FF (c, d, a, b, x[ 2], S13, T03); /* 3 */
	FF (b, c, d, a, x[ 3], S14, T04); /* 4 */
	FF (a, b, c, d, x[ 4], S11, T05); /* 5 */
	FF (d, a, b, c, x[ 5], S12, T06); /* 6 */
	FF (c, d, a, b, x[ 6], S13, T07); /* 7 */
	FF (b, c, d, a, x[ 7], S14, T08); /* 8 */
	FF (a, b, c, d, x[ 8], S11, T09); /* 9 */
	FF (d, a, b, c, x[ 9], S12, T10); /* 10 */
	FF (c, d, a, b, x[10], S13, T11); /* 11 */
	FF (b, c, d, a, x[11], S14, T12); /* 12 */
	FF (a, b, c, d, x[12], S11, T13); /* 13 */
	FF (d, a, b, c, x[13], S12, T14); /* 14 */
	FF (c, d, a, b, x[14], S13, T15); /* 15 */
	FF (b, c, d, a, x[15], S14, T16); /* 16 */

	/* Round 2 */
	GG (a, b, c, d, x[ 1], S21, T17); /* 17 */
	GG (d, a, b, c, x[ 6], S22, T18); /* 18 */
	GG (c, d, a, b, x[11], S23, T19); /* 19 */
	GG (b, c, d, a, x[ 0], S24, T20); /* 20 */
	GG (a, b, c, d, x[ 5], S21, T21); /* 21 */
	GG (d, a, b, c, x[10], S22, T22); /* 22 */
	GG (c, d, a, b, x[15], S23, T23); /* 23 */
	GG (b, c, d, a, x[ 4], S24, T24); /* 24 */
	GG (a, b, c, d, x[ 9], S21, T25); /* 25 */
	GG (d, a, b, c, x[14], S22, T26); /* 26 */
	GG (c, d, a, b, x[ 3], S23, T27); /* 27 */
	GG (b, c, d, a, x[ 8], S24, T28); /* 28 */
	GG (a, b, c, d, x[13], S21, T29); /* 29 */
	GG (d, a, b, c, x[ 2], S22, T30); /* 30 */
	GG (c, d, a, b, x[ 7], S23, T31); /* 31 */
	GG (b, c, d, a, x[12], S24, T32); /* 32 */
	/* Round 3 */
	HH (a, b, c, d, x[ 5], S31, T33); /* 33 */
	HH (d, a, b, c, x[ 8], S32, T34); /* 34 */
	HH (c, d, a, b, x[11], S33, T35); /* 35 */
	HH (b, c, d, a, x[14], S34, T36); /* 36 */
	HH (a, b, c, d, x[ 1], S31, T37); /* 37 */
	HH (d, a, b, c, x[ 4], S32, T38); /* 38 */
	HH (c, d, a, b, x[ 7], S33, T39); /* 39 */
	HH (b, c, d, a, x[10], S34, T40); /* 40 */
	HH (a, b, c, d, x[13], S31, T41); /* 41 */
	HH (d, a, b, c, x[ 0], S32, T42); /* 42 */
	HH (c, d, a, b, x[ 3], S33, T43); /* 43 */
	HH (b, c, d, a, x[ 6], S34, T44); /* 44 */
	HH (a, b, c, d, x[ 9], S31, T45); /* 45 */
	HH (d, a, b, c, x[12], S32, T46); /* 46 */
	HH (c, d, a, b, x[15], S33, T47); /* 47 */
	HH (b, c, d, a, x[ 2], S34, T48); /* 48 */
	/* Round 4 */
	II (a, b, c, d, x[ 0], S41, T49); /* 49 */
	II (d, a, b, c, x[ 7], S42, T50); /* 50 */
	II (c, d, a, b, x[14], S43, T51); /* 51 */
	II (b, c, d, a, x[ 5], S44, T52); /* 52 */
	II (a, b, c, d, x[12], S41, T53); /* 53 */
	II (d, a, b, c, x[ 3], S42, T54); /* 54 */
	II (c, d, a, b, x[10], S43, T55); /* 55 */
	II (b, c, d, a, x[ 1], S44, T56); /* 56 */
	II (a, b, c, d, x[ 8], S41, T57); /* 57 */
	II (d, a, b, c, x[15], S42, T58); /* 58 */
	II (c, d, a, b, x[ 6], S43, T59); /* 59 */
	II (b, c, d, a, x[13], S44, T60); /* 60 */
	II (a, b, c, d, x[ 4], S41, T61); /* 61 */
	II (d, a, b, c, x[11], S42, T62); /* 62 */
	II (c, d, a, b, x[ 2], S43, T63); /* 63 */
	II (b, c, d, a, x[ 9], S44, T64); /* 64 */
	state[0]+=a;
	state[1]+=b;
	state[2]+=c;
	state[3]+=d;
}

/* Encodes input (UINT4) into output (unsigned char). Assumes len is
  a multiple of 4.
 */
void CMD5::encode (unsigned char * output,const unsigned int * input,unsigned int len)
{
	unsigned int i, j;

	for (i = 0, j = 0; j < len; i++, j += 4) {
		output[j] = (unsigned char)(input[i] & 0xff);
		output[j+1] = (unsigned char)((input[i] >> 8) & 0xff);
		output[j+2] = (unsigned char)((input[i] >> 16) & 0xff);
		output[j+3] = (unsigned char)((input[i] >> 24) & 0xff);
	}
}

/* Decodes input (unsigned char) into output (UINT4). Assumes len is
  a multiple of 4.
 */
void CMD5::decode (unsigned int * output,const unsigned char * input,unsigned int len)
{
	unsigned int i, j;

	for (i = 0, j = 0; j < len; i++, j += 4)
		output[i] = ((unsigned int)input[j]) | (((unsigned int)input[j+1]) << 8) |
			(((unsigned int)input[j+2]) << 16) | (((unsigned int)input[j+3]) << 24);
}

}
