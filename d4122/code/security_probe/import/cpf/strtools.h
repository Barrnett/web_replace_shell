#ifndef __STR_TOOL_H_ZHUZP_2006_03_10
#define __STR_TOOL_H_ZHUZP_2006_03_10
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
#include "cpf/ostypedef.h"
#include "cpf/other_tools.h"
#include "cpf/cpf.h"
#include <string>


//#ifdef OS_WINDOWS

extern	CPF_CLASS		char g_item_sep;
extern	CPF_CLASS		char  g_subitem_sep;

extern	CPF_CLASS		wchar_t  g_w_item_sep;
extern	CPF_CLASS		wchar_t  g_w_subitem_sep;

//#endif//OS_WINDOWS

#define ESC_STRING_MAX_LENGTH			10240

namespace CPF
{

template<typename chartype>
int GetWordNums(const chartype* source,chartype sep =(chartype)(' '),BOOL bTrunctEmpty=true);
//{
//	return GetWordNums<chartype>((const chartype*)source, (size_t)ACE_OS::strlen(source),(chartype)sep, (BOOL)bTrunctEmpty);
//}


template<typename chartype>
int GetWordNums(const chartype* source,size_t numtchars,chartype sep =(chartype)(' '),BOOL bTrunctEmpty=true);

template<typename chartype>
BOOL GetIntWord(int pos,const chartype* source, int& data,chartype sep =(chartype)(' '),BOOL bTrunctEmpty=true);

template<typename chartype>
BOOL GetUintWord(int pos,const chartype* source, unsigned int& data,chartype sep =(chartype)(' '),BOOL bTrunctEmpty=true);

template<typename chartype>
BOOL GetUint64Word(int pos,const chartype* source, ACE_UINT64& data,chartype sep =(chartype)(' '),BOOL bTrunctEmpty=true);


/*
* 解析出第一个单词 -1终止
*/

template<typename chartype,typename DSTSTRTYPE>
BOOL  GetFirstWord(const chartype* source, size_t numtchars,DSTSTRTYPE& result,chartype sep =(chartype)(' '),BOOL bTrunctEmpty=true);

template<typename chartype,typename DSTSTRTYPE>
BOOL  GetFirstWord(const chartype* source, DSTSTRTYPE& result,chartype sep =(chartype)(' '),BOOL bTrunctEmpty=true)
{
	return GetFirstWord(source,ACE_OS::strlen(source),result,sep,bTrunctEmpty);
}

template<typename chartype>
int GetFirstWord(const chartype* source, chartype*& result,chartype sep =(chartype)(' '),BOOL bTrunctEmpty=true)
{
	return GetFirstWord(source,ACE_OS::strlen(source),result,sep,bTrunctEmpty);
}

template<typename chartype>
int GetFirstWord(const chartype* source, size_t numtchars,chartype*& result,chartype sep =(chartype)(' '),BOOL bTrunctEmpty=true);

//解析出最后一个单词 -1终止
template<typename chartype,typename DSTSTRTYPE>
BOOL  GetLastWord(const chartype* source, size_t numtchars,DSTSTRTYPE& result,chartype sep =(chartype)(' '),BOOL bTrunctEmpty=true);

template<typename chartype,typename DSTSTRTYPE>
BOOL  GetLastWord(const chartype* source, DSTSTRTYPE& result,chartype sep =(chartype)(' '),BOOL bTrunctEmpty=true)
{
	return GetLastWord(source,ACE_OS::strlen(source),result,sep,bTrunctEmpty);
}

template<typename chartype,typename DSTSTRTYPE>
int GetLastWord(const chartype* source, size_t numtchars,chartype*& result,chartype sep =(chartype)(' '),BOOL bTrunctEmpty=true);

template<typename chartype,typename DSTSTRTYPE>
int GetLastWord(const chartype* source, chartype*& result,chartype sep =(chartype)(' '),BOOL bTrunctEmpty=true)
{
	return GetLastWord(source,ACE_OS::strlen(source),result,sep,bTrunctEmpty);
}

/*
* pos:	GetWord 解析出第几个单词 -1终止
*/
template<typename chartype,typename DSTSTRTYPE>
BOOL GetWord(int pos,const chartype* source, size_t numtchars,DSTSTRTYPE& result,chartype sep = (chartype)(' '),BOOL bTrunctEmpty=true);

template<typename chartype,typename DSTSTRTYPE>
BOOL GetWord(int pos,const chartype* source, DSTSTRTYPE& result,chartype sep = (chartype)(' '),BOOL bTrunctEmpty=true)
{
	return GetWord(pos,source,ACE_OS::strlen(source),result,sep,bTrunctEmpty);
}


//返回字符的长度,如果==-1表示无效
//result是所在的字符指针
template<typename chartype>
int GetWord_Pointer(int pos,const chartype* source, size_t numtchars,const chartype*& result,chartype sep,BOOL bTrunctEmpty=true);
template<typename chartype>
int GetWord_Pointer(int pos,const chartype* source, const chartype*& result,chartype sep,BOOL bTrunctEmpty=true)
{
	return GetWord_Pointer(pos,source,ACE_OS::strlen(source),result,sep,bTrunctEmpty);
}

template<typename chartype,typename DSTSTRTYPE>
int GetWords(const chartype* source,size_t numtchars,
			 std::vector<DSTSTRTYPE>& result,
			 chartype sep =(chartype)(' '),
			 BOOL bTrunctEmpty=true);

template<typename chartype,typename DSTSTRTYPE>
int ESC_GetWords(const chartype* source,
					  std::vector<DSTSTRTYPE>& result,
					  chartype sep,
					  chartype esc_char,
					  BOOL bTrunctEmpty=true);


template<typename chartype,typename DSTSTRTYPE>
int GetWords(const chartype* source,
			 std::vector<DSTSTRTYPE>& result,
			 chartype sep =(chartype)(' '),
			 BOOL bTrunctEmpty=true)
{
	return GetWords(source,ACE_OS::strlen(source),result,sep,bTrunctEmpty);

}

template<typename chartype>
int GetWords_Pointer(const chartype* source,size_t numtchars,
					 std::vector< std::pair<const chartype*,int> >& result,
					 chartype sep =(chartype)(' '),
					 BOOL bTrunctEmpty=true);


template<typename chartype>
int GetWords_Pointer(const chartype* source,
					 std::vector< std::pair<const chartype*,int> >& result,
					 chartype sep =(chartype)(' '),
					 BOOL bTrunctEmpty=true)
{
	return GetWords_Pointer(source,ACE_OS::strlen(source),result,sep,bTrunctEmpty);
}



//返回间隔字符后的字符串和长度指示，不会修改source里面的内容
//每次调用后source变成下一个字符指针，sourlen表示剩余长度
//使用例子请参考test_stringtoo.cpp
template<typename chartype>
const chartype * separate_token_const(const chartype* &source,size_t& sourcelen,
										 chartype separetor, 
										 int& len,
										 BOOL bTrunctEmpty=true);


template<typename chartype,typename DSTSTRTYPE>
BOOL separate_token_const(const chartype* &source,size_t& sourcelen,
											   chartype separetor, 
											    DSTSTRTYPE& result,
											   BOOL bTrunctEmpty=true)
{
	int len = -1;

	const chartype * rtn = 
		separate_token_const(source,sourcelen,separetor,len,bTrunctEmpty);

	if( rtn )
	{
		result.assign(rtn,len);

		return true;
	}

	return false;
}



//返回间隔字符后的字符串和长度指示，不会修改source里面的内容.要求source必须以0结尾
//使用例子请参考test_stringtoo.cpp
template<typename chartype>
const chartype * separate_token_const(const chartype* &source,
								 chartype separetor, 
								 int& len,
								 BOOL bTrunctEmpty=true);

template<typename chartype,typename DSTSTRTYPE>
BOOL separate_token_const(const chartype* &source,
						  chartype separetor, 
						  DSTSTRTYPE& result,
						  BOOL bTrunctEmpty=true)
{
	int len = -1;

	const chartype * rtn = 
		separate_token_const(source,separetor,len,bTrunctEmpty);

	if( rtn )
	{
		result.assign(rtn,len);

		return true;
	}

	return false;
}

template<typename chartype,typename DSTSTRTYPE>
BOOL esc_separate_token_const(const chartype* &source,
						  chartype separetor, 
						  chartype esc_char,//转义字符
						  DSTSTRTYPE& result,						  
						  BOOL bTrunctEmpty=true);


//返回间隔字符后的字符串，会修改source里面的内容.间隔字符会变成0，
//要求source必须以0结尾
//使用例子请参考test_stringtoo.cpp
template<typename chartype>
chartype * separate_token(chartype* &source,
						chartype separetor, 
						BOOL bTrunctEmpty=true);

//对于例子为source = 1:235235235:usrname,hostname-msg

//const char * token = CPF::separate_token(source,':',false);
//if( !token )
//	return;
//
//int id1 = atoi(token);//1
//
//token = CPF::separate_token(source,':',false);
//if( !token )
//return;
//
//int id2 = atol(token);//235235235
//
//token = CPF::separate_token(source,',',false);
//if( !token )
//return;
//
//const char * usrname = token;
//
//token = CPF::separate_token(source,'-',false);
//if( !token )
//return;
//
//const char * hostname = token;
//
//token = CPF::separate_token(source,'\0',false);
//if( !token )
//return;
//
//const char * msg = token;

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

inline CPF_CLASS int cpf_isupper(char c);
inline CPF_CLASS int cpf_isupper(wchar_t c);

inline CPF_CLASS int cpf_islower(char c);
inline CPF_CLASS int cpf_islower(wchar_t c);

//判断一个字符串是否全部是数字radix表示进制(2,10,16)
inline CPF_CLASS int cpf_isdigitstring(const char *p,int radix);
inline CPF_CLASS int cpf_isdigitstring(const wchar_t *p,int radix);


//看这个字符串是否有大写的英文字母
template<typename chartype>
BOOL IsHaveUpperAlhpa(const chartype* source,size_t nlen)
{
	for( size_t i = 0; i < nlen; ++i )
	{
		if( CPF::cpf_isupper(*source++) )
			return true;
	}

	return false;	
}

//看这个字符串是否有小写的英文字母
template<typename chartype>
BOOL IsHaveLowerAlhpa(const chartype* source,size_t nlen)
{
	for( size_t i = 0; i < nlen; ++i )
	{
		if( CPF::cpf_islower(*source++) )
			return true;
	}

	return false;	
}

/*
16进制的一个可见字符转换成数字
*/
inline BYTE hexchar2byte(char ch)
{
	if (ch >= '0' && ch <= '9')
		return (BYTE)(ch - '0');

	ch = toupper(ch);

	if (ch >= 'A' && ch <= 'Z')
		return (BYTE)(ch - 'A' + 10);

	return 0;
}

//将16进制表示的字符串，转化成字节数组
inline BOOL hexstr2bin_littleendian(const char *buf, BYTE *bindata, int maxlen, int *len)
{
	for (*len=0; buf[0] && buf[1] && *len < maxlen; buf+=2, (*len)++)
	{
		bindata[*len] = (hexchar2byte(buf[0]) << 4 | hexchar2byte(buf[1]));
	}
	return	TRUE;
}

/* hexstr to little-endian binary */
inline BOOL hexstr2bin_bigendian(const char *buf, BYTE *bindata, int maxlen, int *len)
{
	*len = 0;
	for (int buflen = (int)strlen(buf); buflen >= 2 && *len < maxlen; buflen-=2, (*len)++)
	{
		bindata[*len] = hexchar2byte(buf[buflen-1]) | hexchar2byte(buf[buflen-2]) << 4;
	}
	return	TRUE;
}


inline void bin2hexstr_littleendian(const BYTE *bindata, int len, char *buf)
{
	static const char hexstr[] = "0123456789ABCDEF";

	for (int cnt=0; cnt < len; cnt++)
	{
		*buf++ = hexstr[bindata[cnt] >> 4];
		*buf++ = hexstr[bindata[cnt] & 0x0f];
	}
	*buf = 0;
}

inline void bin2hexstr_bigendian(const BYTE *bindata, int len, char *buf)
{
	static const char hexstr[] = "0123456789ABCDEF";

	while (len-- > 0)
	{
		*buf++ = hexstr[bindata[len] >> 4];
		*buf++ = hexstr[bindata[len] & 0x0f];
	}
	*buf = 0;
}

//////////////////////////////////////////////////////////////////////////


//转换所有字段，和子字段的间隔字符
CPF_CLASS const char * ESCItemAndSubItemString(const char * data);

//转换所有字段，和子字段的间隔字符
CPF_CLASS const wchar_t * ESCItemAndSubItemString(const wchar_t * data);

CPF_CLASS const char * ESCString(const char * data,char * output1,size_t output_len,char sep, char esc_char='\\');
CPF_CLASS const wchar_t * ESCString(const wchar_t * data,wchar_t * output1,size_t output_len,wchar_t sep, wchar_t esc_char=(L'\\'));

//项间隔符转义
CPF_CLASS const char * ESCStringItemSep(const char * data,char * output1,size_t output_len);

//子项间隔符转义
CPF_CLASS const char * ESCStringSubItemSep(const char * data,char * output1,size_t output_len);

//项间隔符转义
CPF_CLASS const wchar_t * ESCStringItemSep(const wchar_t * data,wchar_t * output1,size_t output_len);

//子项间隔符转义
CPF_CLASS const wchar_t * ESCStringSubItemSep(const wchar_t * data,wchar_t * output1,size_t output_len);





//功	  能：带通配符的字符串匹配
//参	  数：lpszSour是一个普通字符串；
//			  lpszMatch是一可以包含通配符的字符串；
//			  bMatchCase为0，不区分大小写，否则区分大小写。
//返  回  值：匹配，返回1；否则返回0。
//通配符意义：
//		‘*’	代表任意字符串，包括空字符串；
//		‘?’	代表任意一个字符，不能为空；
//时	  间：	2001.11.02	13:00
CPF_CLASS bool MatchingString(const char* lpszSour, const char* lpszMatch, bool bMatchCase = true);

//功  能：多重匹配，不同匹配字符串之间用‘,’隔开
//			如：“*.h,*.cpp”将依次匹配“*.h”和“*.cpp”
//参  数：nMatchLogic = 0, 不同匹配求或，else求与；bMatchCase, 是否大小敏感
//返回值：如果bRetReversed = 0, 匹配返回true；否则不匹配返回true
//时  间：2001.11.02  17:00
CPF_CLASS bool MultiMatching(const char* lpszSour, const char* lpszMatch, int nMatchLogic = 0, bool bRetReversed = 0, bool bMatchCase = true,char sep=',');

// 在字节流中s1中查找字节流s2，s1可以不是0结尾，s2需要0结尾
// 如果要使用字符串查找，可使用ACE_OS::strnstr
CPF_CLASS const char* memnstr(const char *s1, const char *s2, size_t len1);

// 在字节流中s1中查找字符串s2，可以均不是0结尾，
CPF_CLASS const char* memnmem(const char *s1, const char *s2, size_t len1, size_t len2);

}

#include "strtools.inl"
///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
#endif//__STR_TOOL_H_ZHUZP_2006_03_10

