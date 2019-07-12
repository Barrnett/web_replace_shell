//////////////////////////////////////////////////////////////////////////
//URLDecoder.cpp

#include "cpf/URLDecoder.h"



/*

Unicode�ж��ֽڱ��루UCS-2�������ֽڱ��루UCS-4�����֣�Ŀǰ���õĶ��ֽڱ��롣��������������UTF-8ת��Unicode��UCS-2���ĺ�����

 

#include <stdio.h>

#include <stdlib.h>
#include <memory.h>
#include <string.h>

 

/*************************************************************************************************

 * ��UTF8����ת����Unicode��UCS-2������

 * ������

 *    char* pInput  ָ�������ַ�������'\0'��β����ָ�� 

 *    char** ppOutput ָ������ַ���ָ���ָ��

 * ����ֵ��

 *    ����ת�����Unicode�ַ������ֽ�������������򷵻�-1

 * ע�⣺

 *     1. UTF8û���ֽ������⣬����Unicode�ַ����ֽ���

 *        �ֽ����Ϊ��ˣ�Big Endian����С�ˣ�Little Endian�����֣�

 *        ��Intel�������в���С�˷���ʾ����˱����в���С�˷���ʾ�����͵�ַ���λ��

 *     2. �ڵ��ñ���������Ҫ�ֶ��ͷ� *ppOutput ָ����ڴ棬���򽫻�����ڴ�й©��

 **************************************************************************************************/

//int utf8_to_unicode(char* pInput, char** ppOutput)
//{
//    int outputSize = 0; //��¼ת�����Unicode�ַ������ֽ���
//
//    *ppOutput = (char *)malloc(strlen(pInput) * 2);  //Ϊ����ַ��������㹻����ڴ�ռ�
//    memset(*ppOutput, 0, strlen(pInput) * 2);
//    char *tmp = *ppOutput; //��ʱ���������ڱ�������ַ���
// 
//    while (*pInput)
//    {
//        if (*pInput > 0x00 && *pInput <= 0x7F) //�����ֽ�UTF8�ַ���Ӣ����ĸ�����֣�
//        {
//            *tmp = *pInput;
//            tmp++;
//            *tmp = 0; //С�˷���ʾ���ڸߵ�ַ�0
//        }
//        else if (((*pInput) & 0xE0) == 0xC0) //����˫�ֽ�UTF8�ַ�
//        {
//            char high = *pInput;
//            pInput++;
//            char low = *pInput;
//
//            if ((low & 0xC0) != 0x80)  //����Ƿ�Ϊ�Ϸ���UTF8�ַ���ʾ
//            {
//                return -1; //��������򱨴�
//            }
//   
//            *tmp = (high << 6) + (low & 0x3F);
//            tmp++;
//            *tmp = (high >> 2) & 0x07;
//        }
//        else if (((*pInput) & 0xF0) == 0xE0) //�������ֽ�UTF8�ַ�
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
//        else //���������ֽ�����UTF8�ַ������д���
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
////һ������ʾ��
//
//int main(int argc, char** argv)
//{
//
//    //���֡��ҡ���UTF8������ E6 88 91��Unicode������ 62 11
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
//    free(uni); //�ͷ��ڴ�
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
//		if ( *w < 0��0080 ) len++;
//		else if ( *w < 0��0800 ) len += 2;
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
//		if ( *w < 0��0080 )
//			szOut[i++] = ( unsigned char ) *w;
//		else if ( *w < 0��0800 ) {
//			szOut[i++] = 0xc0 | (( *w ) >> 6 );
//			szOut[i++] = 0��80 | (( *w ) &amp; 0��3f );
//		}
//		else {
//			szOut[i++] = 0xe0 | (( *w ) >> 12 );
//			szOut[i++] = 0��80 | (( ( *w ) >> 6 ) &amp; 0��3f );
//			szOut[i++] = 0��80 | (( *w ) &amp; 0��3f );
//		}    }
//
//	szOut[ i ] = ��\0��;
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
		{ "&;",		1, '&' },		// ��֤һ�����ҵ�
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

	if (pSrc == pOri)	// û�ҵ�&��˵���ַ���û�仯
	{
		return slen;
	}

	int iLen = (int)slen-(int)(pSrc-pOri);

	ACE_ASSERT(strlen(pSrc) == iLen);

	// �ѽ�β���ȥ
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


