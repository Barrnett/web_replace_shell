//////////////////////////////////////////////////////////////////////////
//URLDecoder.cpp

#include "cpf/URLDecoder.h"



/*

Unicode有二字节编码（UCS-2）和四字节编码（UCS-4）两种，目前常用的二字节编码。下面程序给出的是UTF-8转成Unicode（UCS-2）的函数。

 

#include <stdio.h>

#include <stdlib.h>
#include <memory.h>
#include <string.h>

 

/*************************************************************************************************

 * 将UTF8编码转换成Unicode（UCS-2）编码

 * 参数：

 *    char* pInput  指向输入字符串（以'\0'结尾）的指针 

 *    char** ppOutput 指向输出字符串指针的指针

 * 返回值：

 *    返回转换后的Unicode字符串的字节数，如果出错则返回-1

 * 注意：

 *     1. UTF8没有字节序问题，但是Unicode字符有字节序，

 *        字节序分为大端（Big Endian）和小端（Little Endian）两种，

 *        在Intel处理器中采用小端法表示，因此本例中采用小端法表示。（低地址存低位）

 *     2. 在调用本函数后需要手动释放 *ppOutput 指向的内存，否则将会造成内存泄漏。

 **************************************************************************************************/

//int utf8_to_unicode(char* pInput, char** ppOutput)
//{
//    int outputSize = 0; //记录转换后的Unicode字符串的字节数
//
//    *ppOutput = (char *)malloc(strlen(pInput) * 2);  //为输出字符串分配足够大的内存空间
//    memset(*ppOutput, 0, strlen(pInput) * 2);
//    char *tmp = *ppOutput; //临时变量，用于遍历输出字符串
// 
//    while (*pInput)
//    {
//        if (*pInput > 0x00 && *pInput <= 0x7F) //处理单字节UTF8字符（英文字母、数字）
//        {
//            *tmp = *pInput;
//            tmp++;
//            *tmp = 0; //小端法表示，在高地址填补0
//        }
//        else if (((*pInput) & 0xE0) == 0xC0) //处理双字节UTF8字符
//        {
//            char high = *pInput;
//            pInput++;
//            char low = *pInput;
//
//            if ((low & 0xC0) != 0x80)  //检查是否为合法的UTF8字符表示
//            {
//                return -1; //如果不是则报错
//            }
//   
//            *tmp = (high << 6) + (low & 0x3F);
//            tmp++;
//            *tmp = (high >> 2) & 0x07;
//        }
//        else if (((*pInput) & 0xF0) == 0xE0) //处理三字节UTF8字符
//        {
//            char high = *pInput;
//            pInput++;
//            char middle = *pInput;
//            pInput++;
//            char low = *pInput;
//
//            if (((middle & 0xC0) != 0x80) || ((low & 0xC0) != 0x80))
//            {
//                return -1;
//            }
//
//            *tmp = (middle << 6) + (low & 0x7F);
//            tmp++;
//            *tmp = (high << 4) + ((middle >> 2) & 0x0F); 
//        }
//        else //对于其他字节数的UTF8字符不进行处理
//        {
//            return -1;
//        }
//
//        pInput ++;
//        tmp ++;
//        outputSize += 2;
//    }
//
//    *tmp = 0;
//    tmp++;
//    *tmp = 0;
//
//    return outputSize;
//}
//
// 
//
////一个调用示例
//
//int main(int argc, char** argv)
//{
//
//    //汉字“我”的UTF8编码是 E6 88 91，Unicode编码是 62 11
//    char str[4] = {(char)0xE6, (char)0x88, (char)0x91, (char)0x00}; 
//    char* uni = NULL;
//
//    int num = utf8_to_unicode(str, &uni);
//
//    if (num == -1)
//    {
//        printf("Error!\n");
//    }
//    else
//    {
//        char* p = uni;
//        for (int i = 0; i < num; i++)
//        {
//            printf("%02X", *p);
//            p++;
//        }
//        printf("\n");
//    }
//
//    free(uni); //释放内存
//
//    return 0;
//}
//
//
//*/


//
//char* __stdcall UnicodeToUtf8( const WCHAR* wstr )
//{
//	const WCHAR* w;
//	// Convert unicode to utf8
//	int len = 0;
//	for ( w = wstr; *w; w++ ) {
//
//		if ( *w < 0×0080 ) len++;
//		else if ( *w < 0×0800 ) len += 2;
//		else len += 3;
//	}
//
//	unsigned char* szOut = ( unsigned char* )malloc( len+1 );
//
//	if ( szOut == NULL )
//		return NULL;
//
//	int i = 0;
//	for ( w = wstr; *w; w++ ) {
//		if ( *w < 0×0080 )
//			szOut[i++] = ( unsigned char ) *w;
//		else if ( *w < 0×0800 ) {
//			szOut[i++] = 0xc0 | (( *w ) >> 6 );
//			szOut[i++] = 0×80 | (( *w ) &amp; 0×3f );
//		}
//		else {
//			szOut[i++] = 0xe0 | (( *w ) >> 12 );
//			szOut[i++] = 0×80 | (( ( *w ) >> 6 ) &amp; 0×3f );
//			szOut[i++] = 0×80 | (( *w ) &amp; 0×3f );
//		}    }
//
//	szOut[ i ] = ‘\0‘;
//	return ( char* )szOut;
//}
//

//#define putchar(a) buff[i++]=(BYTE)a;
//
//ConVertUnicodeToUTF8(const WCHAR *wstrIn, BYTE *buff,int vLen)
//{
//	ASSERT(wstrIn);
//	int wLen=wcslen(wstrIn);
//	int i=0;
//
//	for(int j=0;(DWORD)j<wcslen(wstrIn);j++)
//	{
//		ASSERT(i<vLen);
//		WCHAR c=wstrIn[j];
//		if (c < 0x80)
//		{
//			putchar (c);
//		}
//		else if (c < 0x800)
//		{
//			putchar (0xC0 | c>>6);
//			putchar (0x80 | c & 0x3F);
//		}
//		else if (c < 0x10000)
//		{
//			putchar (0xE0 | c>>12);
//			putchar (0x80 | c>>6 & 0x3F);
//			putchar (0x80 | c & 0x3F);
//		}
//		else if (c < 0x200000)
//		{
//			putchar (0xF0 | c>>18);
//			putchar (0x80 | c>>12 & 0x3F);
//			putchar (0x80 | c>>6 & 0x3F);
//			putchar (0x80 | c & 0x3F);
//		}
//	}
//}

size_t CURLDecoder::encode(const char * s,size_t slen,char *d)
{
	static const char *hex[]  = {
		"%00", "%01", "%02", "%03", "%04", "%05", "%06", "%07",
		"%08", "%09", "%0A", "%0B", "%0C", "%0D", "%0E", "%0F",
		"%10", "%11", "%12", "%13", "%14", "%15", "%16", "%17",
		"%18", "%19", "%1A", "%1B", "%1C", "%1D", "%1E", "%1F",
		"%20", "%21", "%22", "%23", "%24", "%25", "%26", "%27",
		"%28", "%29", "%2A", "%2B", "%2C", "%2D", "%2E", "%2F",
		"%30", "%31", "%32", "%33", "%34", "%35", "%36", "%37",
		"%38", "%39", "%3A", "%3B", "%3C", "%3D", "%3E", "%3F",
		"%40", "%41", "%42", "%43", "%44", "%45", "%46", "%47",
		"%48", "%49", "%4A", "%4B", "%4C", "%4D", "%4E", "%4F",
		"%50", "%51", "%52", "%53", "%54", "%55", "%56", "%57",
		"%58", "%59", "%5A", "%5B", "%5C", "%5D", "%5E", "%5F",
		"%60", "%61", "%62", "%63", "%64", "%65", "%66", "%67",
		"%68", "%69", "%6A", "%6B", "%6C", "%6D", "%6E", "%6F",
		"%70", "%71", "%72", "%73", "%74", "%75", "%76", "%77",
		"%78", "%79", "%7A", "%7B", "%7C", "%7D", "%7E", "%7F",
		"%80", "%81", "%82", "%83", "%84", "%85", "%86", "%87",
		"%88", "%89", "%8A", "%8B", "%8C", "%8D", "%8E", "%8F",
		"%90", "%91", "%92", "%93", "%94", "%95", "%96", "%97",
		"%98", "%99", "%9A", "%9B", "%9C", "%9D", "%9E", "%9F",
		"%A0", "%A1", "%A2", "%A3", "%A4", "%A5", "%A6", "%A7",
		"%A8", "%A9", "%AA", "%AB", "%AC", "%AD", "%AE", "%AF",
		"%B0", "%B1", "%B2", "%B3", "%B4", "%B5", "%B6", "%B7",
		"%B8", "%B9", "%BA", "%BB", "%BC", "%BD", "%BE", "%BF",
		"%C0", "%C1", "%C2", "%C3", "%C4", "%C5", "%C6", "%C7",
		"%C8", "%C9", "%CA", "%CB", "%CC", "%CD", "%CE", "%CF",
		"%D0", "%D1", "%D2", "%D3", "%D4", "%D5", "%D6", "%D7",
		"%D8", "%D9", "%DA", "%DB", "%DC", "%DD", "%DE", "%DF",
		"%E0", "%E1", "%E2", "%E3", "%E4", "%E5", "%E6", "%E7",
		"%E8", "%E9", "%EA", "%EB", "%EC", "%ED", "%EE", "%EF",
		"%F0", "%F1", "%F2", "%F3", "%F4", "%F5", "%F6", "%F7",
		"%F8", "%F9", "%FA", "%FB", "%FC", "%FD", "%FE", "%FF"
	};	

	char * dst_old = d;
	for (size_t i = 0; i < slen; ++i) 
	{
		char ch = s[i];

		if (('A' <= ch && ch <= 'Z') || ('a' <= ch && ch <= 'z') || ('0' <= ch && ch <= '9' ) )
		{		// 'A'..'Z'
			*d++ = ch;//sbuf.AddChar((char)ch);			
		} 
		//else if (ch == ' ') 
		//{			// space
		//	*d++ = '+';//sbuf.AddChar('+');
		//} 
		else if (ch == '-' || ch == '_'		// unreserved
			|| ch == '.' || ch == '!'
			|| ch == '~' || ch == '*'
			|| ch == '\'' || ch == '('
			|| ch == ')')
		{
			*d++ = ch;//sbuf.AddChar((char)ch);
		} 
		else
		{
			*d++ = hex[(BYTE)ch][0];
			*d++ = hex[(BYTE)ch][1];
			*d++ = hex[(BYTE)ch][2];
		}
	}

	*d = 0;

	return d - dst_old;
}

#define HEX_TO_BIN(ch) \
	(isdigit (ch) ? ((ch) - '0') : ((10+tolower(ch) - 'a') & 0x0F))


size_t CURLDecoder::unencode(const char *s,size_t slen,char *d)
{
	char * dst_old = d;

	int ch = -1 ;

	for (size_t i = 0; i < slen ; ++i) 
	{
		switch (ch = (int)s[i]) 
		{
		case '%':
			{
				if(i+1 == slen)
					break;
				ch = (int)s[++i] ;

				if( ch=='u' || ch == 'U')
				{
					if( i + 3 >= slen )
						break;

					ACE_UINT16 u = ((HEX_TO_BIN(s[i+1])<<12)|(HEX_TO_BIN(s[i+2])<<8)|(HEX_TO_BIN(s[i+3])<<4)|(HEX_TO_BIN(s[i+4])<<0));

					//u = ACE_NTOHS(u);

					i+=4;

					if (u < 0x80)
					{
						*d++ = (char)u;
					}
					else if (u < 0x800)
					{
						*d++ = (0xC0 | u>>6);
						*d++ = (0x80 | u&0x3f);
					}
					else if( u < 0x10000 )
					{
						*d++ =  (0xe0|( u >> 12 ));
						*d++ =  (0x80|( (u >> 6) & 0x3f ));
						*d++ =  (0x80|( u & 0x3f ));
					}
					else if (u < 0x200000)
					{
						*d++ =  (0xf0|( u >> 18 ));
						*d++ =  (0x80|( (u >> 12) & 0x3f ));
						*d++ =  (0x80|( (u >> 6) & 0x3f ));
						*d++ =  (0x80|( u & 0x3f ));
					}
				}
				else
				{
					if( (unsigned)(ch + 1) > 256 )
						return 0;

					char hb = HEX_TO_BIN(ch);

					if(i+1 == slen)
						break;

					ch = (int)s[++i] ;

					if( (unsigned)(ch + 1) > 256 )
						return 0;

					char lb = HEX_TO_BIN(ch);

					*d++ = (hb << 4) | lb ;
				}
			}			
			break ;
		case '+':
			{
				*d++ = ' ' ;
			}				
			break ;

		default:
			{
				*d++ = ch ;
			}
			break;				
		}
	}

	*d = 0;

	return d - dst_old;
}



	
size_t CURLDecoder::decode_xml_entity_ascii(const char *s, size_t slen,char *d)
{
	typedef struct _ST_XML_ENTITY_
	{
		const char*     str;
		unsigned int	strLength;
		char		    chr;
	}ST_XML_ENTITY;
	static ST_XML_ENTITY g_xml_entity[] = 
	{
		{ "&amp;",  5, '&' },
		{ "&lt;",   4, '<' },
		{ "&gt;",   4, '>' },
		{ "&quot;", 6, '\"' },
		{ "&apos;", 6, '\'' },
		{ "&;",		1, '&' },		// 保证一定能找到
	};
	static int g_xml_entity_size = sizeof(g_xml_entity)/sizeof(g_xml_entity[0]);
	
	char* pOri = (char*)s;	//(char*)value.c_str();
	char* pSrc = pOri;
	char* pDst = pSrc;

	char* pFind = NULL;	//strchr(pSrc, '&');

	while(pFind = strchr(pSrc, '&'))
	{
		if (pFind - pSrc > 0)
		{
			if( pDst != pSrc )
			{
				memcpy(pDst,pSrc,(int)(pFind-pSrc));
				//strncpy(pDst, pSrc, pFind-pSrc);
			}

			pDst += (int)(pFind-pSrc);
		}

		// Now try to match it.
		int i = 0;
		do 
		{
			if ( strncmp( g_xml_entity[i].str, pFind, g_xml_entity[i].strLength ) == 0 )
			{
				*pDst = g_xml_entity[i].chr;
				pDst ++;
				pSrc = pFind + g_xml_entity[i].strLength;
				break;
			}
		} while ((++i)<g_xml_entity_size);
	}

	if (pSrc == pOri)	// 没找到&，说明字符串没变化
	{
		return slen;
	}

	int iLen = (int)slen-(int)(pSrc-pOri);

	ACE_ASSERT(strlen(pSrc) == iLen);

	// 把结尾搬过去
	if (iLen>0)
	{
		//	strcat(pDst, pSrc);
		memcpy(pDst,pSrc,iLen);
	}
	
	pDst[iLen] = '\0';

	int outlen = (int)(pDst-pOri)+iLen;

	ACE_ASSERT(strlen(pOri) == outlen);

	return outlen;
};

bool CURLDecoder::decode_xml_entity_ascii(std::string& data)
{
	size_t nOri = data.length();
	size_t nlen = decode_xml_entity_ascii(data.c_str(),data.size(),(char *)data.c_str());

	if (nOri != nlen)
	{
		data.resize(nlen);
		return true;
	}
	return false;
}





size_t CURLDecoder::erase_unurl_char(const char *s, size_t slen, char *d)
{
	char* pSrc = (char*)s;
	char* pDst = d;

	while( (pSrc-s) < (int)slen)
	{
		if ( (*pSrc > 0x20) && (*pSrc < 0x7F) )
		{
			*pDst ++ = *pSrc++;
		}
		else
			pSrc ++;

	}

	pDst[0] = '\0';

	return strlen(d);
};
bool CURLDecoder::erase_unurl_char(std::string& data)
{
	size_t nOri = data.length();
	size_t nlen = erase_unurl_char(data.c_str(),data.size(),(char *)data.c_str());

	if (nOri != nlen)
	{
		data.resize(nlen);
		return true;
	}
	return false;
}


