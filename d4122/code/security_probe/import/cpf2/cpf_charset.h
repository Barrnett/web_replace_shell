//////////////////////////////////////////////////////////////////////////
//cpf_charset.h

#pragma once

#include "cpf/cpf.h"
#include "cpf2/cpf2.h"

namespace CPF
{
	//如果完全成功转化，返回0，如果没有成功返回已经转化了多少字节
	//outlen以及inlen作为输入是两个空间的大小（不包括后面的0)
	//作为输出时，是剩下没有被转化的字节个数
	CPF2_CLASS
		int char_set_convert(const char *to_charset,const char *from_charset,
		const char *inbuf,size_t& inlen,char *outbuf,size_t& outlen);

	//////////////////////////////////////////////////////////////////////////
	//其他字符集转向HOME的字符集
	CPF2_CLASS
		inline int char_set_convert_to_home(const char *from_charset,
		const char *inbuf,size_t& inlen,char *outbuf,size_t& outlen)
	{
		return CPF::char_set_convert(CPF::get_home_charset(),from_charset,inbuf,inlen,outbuf,outlen);
	}

	CPF2_CLASS
		inline int char_set_convert_to_home(const char *from_charset,
		const char *inbuf,size_t inlen,std::string& outstring)
	{
		static char outbuf[102400];

		size_t outlen = sizeof(outbuf);

		int rtn = char_set_convert_to_home(from_charset,inbuf,inlen,outbuf,outlen);

		outstring.assign(outbuf,sizeof(outbuf)-outlen);

		return rtn;
	}

	CPF2_CLASS
		inline int UTF8_to_HOME(const char *inbuf,size_t& inlen,char *outbuf,size_t& outlen)
	{		
		static const char from_charset[] = "UTF-8";

		return char_set_convert_to_home(from_charset,inbuf,inlen,outbuf,outlen);
	}

	CPF2_CLASS
		inline int UTF8_to_HOME(const char *inbuf,size_t inlen,std::string& outstring)
	{
		static const char from_charset[] = "UTF-8";

		return char_set_convert_to_home(from_charset,inbuf,inlen,outstring);
	}

	CPF2_CLASS
		inline int UCS_2LE_to_HOME(const char *inbuf,size_t& inlen,char *outbuf,size_t& outlen)
	{
		static const char from_charset[] = "UCS-2LE";

		return char_set_convert_to_home(from_charset,inbuf,inlen,outbuf,outlen);
	}

	CPF2_CLASS
		inline int UCS_2LE_to_HOME(const std::wstring& instring,char *outbuf,size_t& outlen)
	{
		const char *inbuf = (const char *)instring.c_str();
		size_t inlen = instring.size()*sizeof(wchar_t);

		return UCS_2LE_to_HOME(inbuf,inlen,outbuf,outlen);
	}


	CPF2_CLASS
		inline int UCS_2LE_to_HOME(const char *inbuf,size_t inlen,std::string& outstring)
	{
		static const char from_charset[] = "UCS-2LE";

		return char_set_convert_to_home(from_charset,inbuf,inlen,outstring);
	}

	CPF2_CLASS
		inline int UCS_2LE_to_HOME(const std::wstring& instring,std::string& outstring)
	{
		const char *inbuf = (const char *)instring.c_str();
		size_t inlen = instring.size()*sizeof(wchar_t);

		return UCS_2LE_to_HOME(inbuf,inlen,outstring);
	}


	//////////////////////////////////////////////////////////////////////////
	//HOME字符集的转向字其他字符集
	CPF2_CLASS
		inline int home_convertto_char_set(const char *to_charset,
		const char *inbuf,size_t& inlen,char *outbuf,size_t& outlen)
	{
		return CPF::char_set_convert(to_charset,CPF::get_home_charset(),inbuf,inlen,outbuf,outlen);
	}

	CPF2_CLASS
		inline int HOME_to_UTF8(const char *inbuf,size_t& inlen,char *outbuf,size_t& outlen)
	{		
		static const char to_charset[] = "UTF-8";

		return home_convertto_char_set(to_charset,inbuf,inlen,outbuf,outlen);
	}

	CPF2_CLASS
		inline int HOME_to_UCS_2LE(const char *inbuf,size_t& inlen,char *outbuf,size_t& outlen)
	{
		static const char from_charset[] = "UCS-2LE";

		return home_convertto_char_set(from_charset,inbuf,inlen,outbuf,outlen);
	}

	CPF2_CLASS
		inline int HOME_to_UCS_2LE(const char *inbuf,size_t& inlen,std::wstring& outstring)
	{
		static wchar_t outbuf[102400];

		size_t outlen = sizeof(outbuf);

		int nrtn = HOME_to_UCS_2LE(inbuf,inlen,(char *)outbuf,outlen);

		outstring.assign(outbuf,(sizeof(outbuf)-outlen)/sizeof(wchar_t));

		return nrtn;
	}

	CPF2_CLASS
		inline int HOME_to_UTF8(const char *inbuf,char *outbuf,size_t& outlen)
	{		
		size_t inlen = strlen(inbuf);

		return HOME_to_UTF8(inbuf,inlen,outbuf,outlen);
	}

	CPF2_CLASS
		inline int HOME_to_UCS_2LE(const char *inbuf,char *outbuf,size_t& outlen)
	{
		size_t inlen = strlen(inbuf);

		return HOME_to_UCS_2LE(inbuf,inlen,outbuf,outlen);
	}

	CPF2_CLASS
		inline int HOME_to_UCS_2LE(const char *inbuf,std::wstring& outstring)
	{
		size_t inlen = strlen(inbuf);

		return HOME_to_UCS_2LE(inbuf,inlen,outstring);

	}

	CPF2_CLASS
		int isUTF8_16(const char* pBuf, size_t nLen);


	// 检测GBK、UTF-8等编码；
	enum UniMode {uni8Bit=0, uniUTF8=1, uni16BE=2, uni16LE=3, uniCookie=4, uniEnd};

	CPF2_CLASS
		UniMode determineEncoding(const unsigned char* pBuf, size_t nLen);
}
