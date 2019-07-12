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

	//�û�����ļ򵥹ؼ����У��ж��ٸ�1���ֽڵĹؼ���singlbytesnums
	//�ж��ٸ������ֽڻ������ֽ����ϵĹؼ���othernums
	BOOL Create(size_t singlbytesnums,size_t othernums,
		size_t singleHashsize=256,size_t otherhashsize=65536,
		ACE_UINT32 attr_type =(attr_type_gbk|attr_type_utf8|attr_type_uc2le));

	//�ͷ���Դ
	void Destroy();

	BOOL AddBinaryKey(const BYTE * pKeyData,size_t keylen,const ValueType& value);

	BOOL DelBinaryKey(const BYTE * pKeyData,size_t keylen);

	//pKeyData�Ǹ��Ϲؼ���,pKeyData������unicode_le��ʽ�ģ������ǹؼ��ֵ��ڴ泤�ȣ�����������0
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

	//pKeyData�Ǹ��Ϲؼ���,pKeyData������unicode_le��ʽ�ģ������ǹؼ��ֵ��ڴ泤�ȣ�����������0
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


	//��p�ĵ�һ�ֽڿ�ʼ,������ַ����Ƿ��ڱ���.Ҫ���ַ�����ȫƥ��(��������)
	//����ؼ����� abc|ade,����ַ�����abc����ade���ܹ�ƥ����
	//����ַ�����abcd����abcade��û��ƥ����,��Ϊ���Ȳ�һ��.
	BOOL  FindExact(CBlurKeyMatchSet_type type,
		const  BYTE* p,size_t nLen, GroupType& group,
		bool bcasesensitive) const;

	//��FindFindExactҪģ��һЩ,Ҳ�ǴӴ�p�ĵ�һ�ֽڿ�ʼ,���ǲ�Ҫ�󳤶���ȫһ��
	//����ؼ����� abc|ade,����ַ�����abc����ade,��FindExact�ܹ�ƥ����,HeadMatchҲ�ܹ�ƥ����
	//��������ַ�����abcd����abcade,FindExact��û��ƥ����,��Ϊ���Ȳ�һ��.
	//��HeadMatch���ܹ�ƥ����,��ΪֻҪ��һ���ֽڿ�ʼ����������
	//�����������ַ�����123abc,�Ͳ��ܹ�ƥ����,��Ȼ������abc,���ǲ��Ǵ�ͷ��ʼ�ȵ�
	BOOL  HeadMatch(CBlurKeyMatchSet_type type,
		const BYTE* p,size_t nLen, GroupType& group,bool bcasesensitive) const;



	//����ƪ�����ж��ٸ��ؼ��ַ��ϡ��������Ҫ��ͷ��β����ƥ�����
	//����ؼ����� abc|ade,����ַ�����abc����ade,��FindExact�ܹ�ƥ����,HeadMatch��TextMatchҲ�ܹ�ƥ����
	//��������ַ�����abcd����abcade,FindExact��û��ƥ����,��Ϊ���Ȳ�һ��.
	//��HeadMatch,TextMatch���ܹ�ƥ����,��ΪֻҪ��һ���ֽڿ�ʼ����������

	//�����������ַ�����123abc,HeadMatch�Ͳ��ܹ�ƥ����,��Ȼ������abc,���ǲ��Ǵ�ͷ��ʼ�ȵ�
	//����TextMatch�Ϳ���ƥ����
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
	//pKeyData�Ǹ��Ϲؼ���,pKeyData������unicode_le��ʽ�ģ������ǹؼ��ֵ��ڴ泤�ȣ�����������0
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

	//���ڶ�������������ʹ�������ַ�������m_pTableArray[i]���洢
	CBlurKeyMatchSet_type					m_binary_type;

};

#include "CBlurKeyMatchSet.inl"


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
#endif//BLUR_KEY_MATCH_SET_H_2006_05_29