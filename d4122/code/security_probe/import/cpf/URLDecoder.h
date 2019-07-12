//////////////////////////////////////////////////////////////////////////
//URLDecoder.h

#pragma once

#include "cpf/ostypedef.h"
#include "cpf/cpf.h"

class CPF_CLASS CURLDecoder
{
public:
/* Unencode URL���뺯�� */
	/*
	������Ҫע�⣬�������ڴ��������ַ�ʱ�����Զ������ַ���λ7������һ��
	�������ַ�����ʱ����ǿ�Ʋ���unsigned char *������һ���ַ������Կ���
	��isT()���÷�
	*/
	static int isT(char ch)
	{
		// unsigned char *p=(unsigned char*)&ch;
		// if(*p==' '||*p=='='||*p=='%'||*p=='.'||*p=='/'||*p>126) return 1;
		// else return 0;
		if(ch==' '||ch=='='||ch=='%'||ch=='.'||ch=='/'||ch&0x80) return 1;
		else return 0;
	}

	//d�����Ჹ��һ��'\0'
	//���ر���d�ĳ���,���صĳ��Ȳ�����0�ĳ���
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

	/* Unencode URL���뺯�� */
	//d�����Ჹ��һ��'\0'
	//���ؽ����d�ĳ���,���صĳ��Ȳ�����0�ĳ���
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


	// �ⲿ���ǽ���XML�е�&����
	//////////////////////////////////////////////////////////////////////////
	// add by LiangQ, 20130424 -->
	static size_t decode_xml_entity_ascii(const char *s, size_t slen, char *d);

	// true=�ҵ��˿��滻�Ĳ��֣�data�����б仯����false=�ޱ仯
	static bool decode_xml_entity_ascii(std::string& data);

	// �޳�����URL���ַ���>0x20��<0x7F
	static size_t erase_unurl_char(const char *s, size_t slen, char *d);

	// true=�ҵ��˿��滻�Ĳ��֣�data�����б仯����false=�ޱ仯
	static bool erase_unurl_char(std::string& data);

	//////////////////////////////////////////////////////////////////////////
	// add by LiangQ, 20130424 <--
};
