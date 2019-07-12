//////////////////////////////////////////////////////////////////////////
//CBlurKeyMatchSet.h

#ifndef BLUR_KEY_MATCH_SET_H_2006_05_29
#define BLUR_KEY_MATCH_SET_H_2006_05_29
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

#include "cpf/BlurKeyMatch.h"
#include "cpf/strtools.h"
#include "cpf2/cpf2.h"
#include "cpf2/CompositeKeySplit.h"

#ifndef DEF_CCBlurKeyMatchSet_DECLEAR
#define DEF_CCBlurKeyMatchSet_DECLEAR		template<class ValueType,class GroupType> 
#define DEF_CCBlurKeyMatchSet_CLASS			CBlurKeyMatchSet<ValueType,GroupType>
#endif // DEF_CCBlurKeyMatchSet_DECLEAR

enum CBlurKeyMatchSet_type 
{
	CBlurKeyMatchSet_type_gbk = 0,
	CBlurKeyMatchSet_type_utf8 = 1,
	CBlurKeyMatchSet_type_uc2le = 2,

};

enum{

	attr_type_gbk = (1<<CBlurKeyMatchSet_type_gbk),
	attr_type_utf8 = (1<<CBlurKeyMatchSet_type_utf8),
	attr_type_uc2le = (1<<CBlurKeyMatchSet_type_uc2le)
};

namespace CPF{

	CPF2_CLASS
		std::string GetCodeSetString(CBlurKeyMatchSet_type type);
}

DEF_CCBlurKeyMatchSet_DECLEAR
class CBlurKeyMatchSet
{
public:
	CBlurKeyMatchSet();
	~CBlurKeyMatchSet();

	//用户输入的简单关键字中，有多少个1个字节的关键字singlbytesnums
	//有多少个两个字节或两个字节以上的关键字othernums
	BOOL Create(size_t singlbytesnums,size_t othernums,
		size_t singleHashsize=256,size_t otherhashsize=65536,
		ACE_UINT32 attr_type =(attr_type_gbk|attr_type_utf8|attr_type_uc2le));

	//释放资源
	void Destroy();

	BOOL AddBinaryKey(const BYTE * pKeyData,size_t keylen,const ValueType& value);

	BOOL DelBinaryKey(const BYTE * pKeyData,size_t keylen);

	//pKeyData是复合关键字,pKeyData必须是unicode_le方式的，长度是关键字的内存长度，不包括最后的0
	inline BOOL AddKey(const BYTE * pKeyData,size_t keylen,const ValueType& value,bool bcasesensitive)
	{
		if( !bcasesensitive )
		{
			if( CPF::IsHaveUpperAlhpa((const wchar_t *)pKeyData,keylen/sizeof(wchar_t)) )
			{
				std::wstring ptempKey((wchar_t *)pKeyData,keylen/sizeof(wchar_t));

				CPF::str_lwr(ptempKey);

				return AddKey((BYTE *)ptempKey.c_str(),keylen,CBlurKeyMatchSet_type_uc2le,value);
			}
		}
		
		return AddKey(pKeyData,keylen,CBlurKeyMatchSet_type_uc2le,value);
	}

	//pKeyData是复合关键字,pKeyData必须是unicode_le方式的，长度是关键字的内存长度，不包括最后的0
	inline BOOL DelKey(const BYTE * pKeyData,size_t keylen,bool bcasesensitive)
	{
		if( !bcasesensitive )
		{
			if( CPF::IsHaveUpperAlhpa((const wchar_t *)pKeyData,keylen/sizeof(wchar_t)) )
			{
				std::wstring ptempKey((wchar_t *)pKeyData,keylen/sizeof(wchar_t));

				CPF::str_lwr(ptempKey);

				return DelKey((BYTE *)ptempKey.c_str(),keylen,CBlurKeyMatchSet_type_uc2le);
			}
		}

		return DelKey(pKeyData,keylen,CBlurKeyMatchSet_type_uc2le);
	}


	//从p的第一字节开始,看这个字符串是否在表中.要求字符串完全匹配(包括长度)
	//比如关键字是 abc|ade,如果字符串是abc或者ade则能够匹配上
	//如果字符串是abcd或者abcade就没有匹配上,因为长度不一样.
	BOOL  FindExact(CBlurKeyMatchSet_type type,
		const  BYTE* p,size_t nLen, GroupType& group,
		bool bcasesensitive) const;

	//比FindFindExact要模糊一些,也是从从p的第一字节开始,但是不要求长度完全一致
	//比如关键字是 abc|ade,如果字符串是abc或者ade,则FindExact能够匹配上,HeadMatch也能够匹配上
	//但是如果字符串是abcd或者abcade,FindExact就没有匹配上,因为长度不一样.
	//而HeadMatch就能够匹配上,因为只要从一个字节开始包含就行了
	//但是如果如果字符串是123abc,就不能够匹配上,虽然包含了abc,但是不是从头开始比的
	BOOL  HeadMatch(CBlurKeyMatchSet_type type,
		const BYTE* p,size_t nLen, GroupType& group,bool bcasesensitive) const;



	//看这篇文章有多少个关键字符合。这个文章要从头到尾进行匹配查找
	//比如关键字是 abc|ade,如果字符串是abc或者ade,则FindExact能够匹配上,HeadMatch和TextMatch也能够匹配上
	//但是如果字符串是abcd或者abcade,FindExact就没有匹配上,因为长度不一样.
	//而HeadMatch,TextMatch就能够匹配上,因为只要从一个字节开始包含就行了

	//但是如果如果字符串是123abc,HeadMatch就不能够匹配上,虽然包含了abc,但是不是从头开始比的
	//但是TextMatch就可以匹配上
	BOOL  TextMatch(CBlurKeyMatchSet_type type,
		const BYTE* p,size_t nLen,GroupType& group,bool bcasesensitive) const;

	BOOL  FindExact_binary(const  BYTE* p,size_t nLen, GroupType& group) const
	{
		return FindExact(m_binary_type,p,nLen,group,true);
	}

	BOOL  HeadMatch_binary(const BYTE* p,size_t nLen, GroupType& group) const
	{
		return HeadMatch(m_binary_type,p,nLen,group,true);
	}

	BOOL  TextMatch_binary(const BYTE* p,size_t nLen,GroupType& group) const
	{
		return TextMatch(m_binary_type,p,nLen,group,true);
	}


private:
	//pKeyData是复合关键字,pKeyData必须是unicode_le方式的，长度是关键字的内存长度，不包括最后的0
	BOOL AddKey(const BYTE * pKeyData,size_t keylen,
		CBlurKeyMatchSet_type keysting_type,
		const ValueType& value);

	BOOL DelKey(const BYTE * pKeyData,size_t keylen,
		CBlurKeyMatchSet_type keysting_type);

	BOOL AddOneCodeSetKey(CBlurKeyMatchSet_type type,
		const BYTE * pKeyData,size_t keylen,
		CBlurKeyMatchSet_type keysting_type,
		const ValueType& value);

	BOOL DelOneCodeSetKey(CBlurKeyMatchSet_type type,
		const BYTE * pKeyData,size_t keylen,
		CBlurKeyMatchSet_type keysting_type);

public:
	CBlurKeyMatch<ValueType,GroupType> *	m_pTableArray[3];
private:

	ACE_UINT32								m_attrType;

	//对于二进制数据我们使用哪种字符集类型m_pTableArray[i]来存储
	CBlurKeyMatchSet_type					m_binary_type;

};

#include "CBlurKeyMatchSet.inl"


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
#endif//BLUR_KEY_MATCH_SET_H_2006_05_29