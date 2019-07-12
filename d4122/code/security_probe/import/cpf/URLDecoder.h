//////////////////////////////////////////////////////////////////////////
//URLDecoder.h

#pragma once

#include "cpf/ostypedef.h"
#include "cpf/cpf.h"

class CPF_CLASS CURLDecoder
{
public:
/* Unencode URL编码函数 */
	/*
	在这里要注意，编译器在处理中文字符时，会自动根据字符的位7来读入一个
	或两个字符，这时可以强制采用unsigned char *来读入一个字符。可以看下
	面isT()的用法
	*/
	static int isT(char ch)
	{
		// unsigned char *p=(unsigned char*)&ch;
		// if(*p==' '||*p=='='||*p=='%'||*p=='.'||*p=='/'||*p>126) return 1;
		// else return 0;
		if(ch==' '||ch=='='||ch=='%'||ch=='.'||ch=='/'||ch&0x80) return 1;
		else return 0;
	}

	//d的最后会补充一个'\0'
	//返回编码d的长度,返回的长度不包括0的长度
	static size_t encode(const char * s,size_t slen,char *d);

	static size_t encode(const char *s,char *d)
	{
		return CURLDecoder::encode(s,strlen(s),d);
	}

	static void encode(std::string& data)
	{
		char * pdata = new char[(data.size()+1)*3];

		CURLDecoder::encode(data.c_str(),data.size(),pdata);

		data.assign(pdata);

		delete []pdata;
	}

	/* Unencode URL解码函数 */
	//d的最后会补充一个'\0'
	//返回解码后d的长度,返回的长度不包括0的长度
	static size_t unencode(const char *s,size_t slen,char *d);

	static size_t unencode(const char *s,char *d)
	{
		return CURLDecoder::unencode(s,strlen(s),d);
	}

	static void unencode(std::string& data)
	{
		size_t nlen = CURLDecoder::unencode(data.c_str(),data.size(),(char *)data.c_str());

		data.resize(nlen);
	}


	// 这部分是解码XML中的&声明
	//////////////////////////////////////////////////////////////////////////
	// add by LiangQ, 20130424 -->
	static size_t decode_xml_entity_ascii(const char *s, size_t slen, char *d);

	// true=找到了可替换的部分（data内容有变化），false=无变化
	static bool decode_xml_entity_ascii(std::string& data);

	// 剔除掉非URL的字符（>0x20、<0x7F
	static size_t erase_unurl_char(const char *s, size_t slen, char *d);

	// true=找到了可替换的部分（data内容有变化），false=无变化
	static bool erase_unurl_char(std::string& data);

	//////////////////////////////////////////////////////////////////////////
	// add by LiangQ, 20130424 <--
};
