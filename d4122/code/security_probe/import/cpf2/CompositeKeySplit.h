//////////////////////////////////////////////////////////////////////////
//CompositeKeySplit.h
//复合关键字的分离

#ifndef COMPOSITE_KEY_SPLIT_H_2006_05_29
#define COMPOSITE_KEY_SPLIT_H_2006_05_29
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

#include "cpf/BlurKeyMatch.h"
#include "cpf2/cpf2.h"

typedef std::vector<COctets<std::allocator<BYTE> > >	COctetsArray;

namespace CPF{


//将输入的UNICODE-LE格式的字符串输入，按照复合关键字进行分隔，得到每个字段
//如果成功，返回复合关键字之间的复合关系，否则返回BLUR_INVALID
CPF2_CLASS
BLURTYPE SplitFullKey(const wchar_t * fullKey_unicode_le,std::vector<std::wstring>& vtKey );

CPF2_CLASS
//根据输入的编码类型，将各种编码的复合关键字进行分隔。得到每个字段的数据和长度。每个字段不包括结尾0
BLURTYPE SplitFullKey(const char * codeset,const BYTE * pFullKey,size_t keylen,COctetsArray& vtKey);

}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
#endif//COMPOSITE_KEY_SPLIT_H_2006_05_29