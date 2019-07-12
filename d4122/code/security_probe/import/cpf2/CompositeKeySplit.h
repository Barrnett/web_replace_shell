//////////////////////////////////////////////////////////////////////////
//CompositeKeySplit.h
//���Ϲؼ��ֵķ���

#ifndef COMPOSITE_KEY_SPLIT_H_2006_05_29
#define COMPOSITE_KEY_SPLIT_H_2006_05_29
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

#include "cpf/BlurKeyMatch.h"
#include "cpf2/cpf2.h"

typedef std::vector<COctets<std::allocator<BYTE> > >	COctetsArray;

namespace CPF{


//�������UNICODE-LE��ʽ���ַ������룬���ո��Ϲؼ��ֽ��зָ����õ�ÿ���ֶ�
//����ɹ������ظ��Ϲؼ���֮��ĸ��Ϲ�ϵ�����򷵻�BLUR_INVALID
CPF2_CLASS
BLURTYPE SplitFullKey(const wchar_t * fullKey_unicode_le,std::vector<std::wstring>& vtKey );

CPF2_CLASS
//��������ı������ͣ������ֱ���ĸ��Ϲؼ��ֽ��зָ����õ�ÿ���ֶε����ݺͳ��ȡ�ÿ���ֶβ�������β0
BLURTYPE SplitFullKey(const char * codeset,const BYTE * pFullKey,size_t keylen,COctetsArray& vtKey);

}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
#endif//COMPOSITE_KEY_SPLIT_H_2006_05_29