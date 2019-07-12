//////////////////////////////////////////////////////////////////////////
//CBlurKeyMatchSet.inl


#ifdef OS_WINDOWS
#include "cpf2/iconv.h"
#else
#include <iconv.h>		// in /usr/local
#endif

//#include "cpf2/libcharset.h"
#include "cpf2/localcharset.h"
#include "cpf2/cpf_charset.h"
#include "cpf/other_tools.h"


DEF_CCBlurKeyMatchSet_DECLEAR
DEF_CCBlurKeyMatchSet_CLASS::CBlurKeyMatchSet()
{
	memset(m_pTableArray,0x00,sizeof(m_pTableArray));

	m_attrType = 0;

	m_binary_type = CBlurKeyMatchSet_type_gbk;
}

DEF_CCBlurKeyMatchSet_DECLEAR
DEF_CCBlurKeyMatchSet_CLASS::~CBlurKeyMatchSet()
{

}


//用户输入的简单关键字中，有多少个1个字节的关键字singlbytesnums
//有多少个两个字节或两个字节以上的关键字othernums
DEF_CCBlurKeyMatchSet_DECLEAR
BOOL DEF_CCBlurKeyMatchSet_CLASS::Create(size_t singlbytesnums,size_t othernums,
										 size_t singleHashsize,size_t otherhashsize,
										 ACE_UINT32 attr_type )
{
	if( singlbytesnums == 0 && othernums == 0 )
		return false;

	if( attr_type ==  0 )
		return false;

	m_attrType = attr_type;

	if( m_attrType & attr_type_gbk )
	{
		m_pTableArray[CBlurKeyMatchSet_type_gbk] = new CBlurKeyMatch<ValueType,GroupType>;
		m_binary_type = CBlurKeyMatchSet_type_gbk;
	}

	if( m_attrType & attr_type_utf8 )
	{
		m_pTableArray[CBlurKeyMatchSet_type_utf8] = new CBlurKeyMatch<ValueType,GroupType>;
		m_binary_type = CBlurKeyMatchSet_type_utf8;
	}

	if( m_attrType & attr_type_uc2le )
	{
		m_pTableArray[CBlurKeyMatchSet_type_uc2le] = new CBlurKeyMatch<ValueType,GroupType>;
		m_binary_type = CBlurKeyMatchSet_type_uc2le;
	}

	for( size_t i = 0; i < sizeof(m_pTableArray)/sizeof(m_pTableArray[0]); ++i )
	{
		if( m_pTableArray[i] )
		{
			if( !m_pTableArray[i]->Create(singlbytesnums,othernums,
				singleHashsize,otherhashsize) )
			{
				return false;
			}
		}
	}

	return true;

}

//释放资源
DEF_CCBlurKeyMatchSet_DECLEAR
void DEF_CCBlurKeyMatchSet_CLASS::Destroy()
{
	for( size_t i = 0; i < sizeof(m_pTableArray)/sizeof(m_pTableArray[0]); ++i )
	{
		if( m_pTableArray[i] )
		{
			m_pTableArray[i]->Destroy();

			delete m_pTableArray[i];

			m_pTableArray[i] = NULL;			
		}
	}
}

DEF_CCBlurKeyMatchSet_DECLEAR
BOOL DEF_CCBlurKeyMatchSet_CLASS::AddBinaryKey(const BYTE * pKeyData,size_t keylen,const ValueType& value)
{
	if( !pKeyData || keylen == 0 )
	{
		ACE_ASSERT(FALSE);
		return false;
	}

	if( !AddOneCodeSetKey(CBlurKeyMatchSet_type_gbk,
		pKeyData,keylen,CBlurKeyMatchSet_type_gbk,value) )
	{
		return false;
	}

	return true;
}

DEF_CCBlurKeyMatchSet_DECLEAR
BOOL DEF_CCBlurKeyMatchSet_CLASS::DelBinaryKey(const BYTE * pKeyData,size_t keylen)
{
	if( !pKeyData || keylen == 0 )
	{
		ACE_ASSERT(FALSE);
		return false;
	}

	if( !DelOneCodeSetKey(CBlurKeyMatchSet_type_gbk,
		pKeyData,keylen,CBlurKeyMatchSet_type_gbk) )
	{
		return false;
	}

	return true;
}

DEF_CCBlurKeyMatchSet_DECLEAR
BOOL DEF_CCBlurKeyMatchSet_CLASS::DelKey(const BYTE * pKeyData,size_t keylen,
										 CBlurKeyMatchSet_type keysting_type)
{
	if( !pKeyData || keylen == 0 || m_attrType ==0)
	{
		ACE_ASSERT(FALSE);
		return false;
	}

	if( m_attrType & attr_type_gbk )
	{
		if( !DelOneCodeSetKey(CBlurKeyMatchSet_type_gbk,
			pKeyData,keylen,keysting_type) )
		{
			return false;
		}
	}

	if( m_attrType & attr_type_utf8 )
	{
		if( !DelOneCodeSetKey(CBlurKeyMatchSet_type_utf8,
			pKeyData,keylen,keysting_type) )
		{
			return false;
		}
	}

	if( m_attrType & attr_type_uc2le )
	{
		if( !DelOneCodeSetKey(CBlurKeyMatchSet_type_uc2le,
			pKeyData,keylen,keysting_type) )
		{
			return false;
		}	
	}

	return true;
}


DEF_CCBlurKeyMatchSet_DECLEAR
BOOL DEF_CCBlurKeyMatchSet_CLASS::AddKey(const BYTE * pKeyData,size_t keylen,
										 CBlurKeyMatchSet_type keysting_type,
										 const ValueType& value)
{
	if( !pKeyData || keylen == 0 || m_attrType ==0)
	{
		ACE_ASSERT(FALSE);
		return false;
	}

	if( m_attrType & attr_type_gbk )
	{
		if( !AddOneCodeSetKey(CBlurKeyMatchSet_type_gbk,
			pKeyData,keylen,keysting_type,value) )
		{
			return false;
		}
	}

	if( m_attrType & attr_type_utf8 )
	{
		if( !AddOneCodeSetKey(CBlurKeyMatchSet_type_utf8,
			pKeyData,keylen,keysting_type,value) )
		{
			return false;
		}
	}

	if( m_attrType & attr_type_uc2le )
	{
		if( !AddOneCodeSetKey(CBlurKeyMatchSet_type_uc2le,
			pKeyData,keylen,keysting_type,value) )
		{
			return false;
		}	
	}
	
	return true;
}

DEF_CCBlurKeyMatchSet_DECLEAR
BOOL DEF_CCBlurKeyMatchSet_CLASS::AddOneCodeSetKey(CBlurKeyMatchSet_type type,
					  const BYTE * pKeyData,size_t keylen,
					  CBlurKeyMatchSet_type keysting_type,
					  const ValueType& value)
{
	char outputkey[2048];

	memset(outputkey,0x00,sizeof(outputkey));	

	size_t outkeylen = sizeof(outputkey);

	std::string dst_codesetstring = CPF::GetCodeSetString(type);

	if( type != keysting_type )
	{
		std::string key_codesetstring = CPF::GetCodeSetString(keysting_type);

		//将输入的unicode的关键字转化成制定的格式的关键字
		if( CPF::char_set_convert(dst_codesetstring.c_str(),key_codesetstring.c_str(),
			(const char *)pKeyData,keylen,(char *)outputkey,outkeylen ) == -1 )
		{
			return false;
		}
	}
	else
	{
		memcpy( outputkey,pKeyData, mymin(keylen,sizeof(outputkey)) );

		outkeylen = sizeof(outputkey) -  mymin(keylen,sizeof(outputkey));
	}

	COctetsArray octArray;

	//得到需要格式的拆分字符数组
	BLURTYPE blurtype = CPF::SplitFullKey(dst_codesetstring.c_str(),
		(BYTE *)outputkey,sizeof(outputkey)-outkeylen,octArray);

	if(  BLUR_INVALID == blurtype )
		return false;

	//添加关键字
	if( !m_pTableArray[type]->BeginAddKey(blurtype,value) )
		return false;

	for( size_t j = 0; j < octArray.size(); ++j )
	{
		if( !m_pTableArray[type]->AddKey(
			octArray[j].GetData(),octArray[j].GetDataLength()) )
		{
			return false;
		}
	}

	if( !m_pTableArray[type]->EndAddKey() )
		return false;

	return true;	
}

DEF_CCBlurKeyMatchSet_DECLEAR
BOOL DEF_CCBlurKeyMatchSet_CLASS::DelOneCodeSetKey(CBlurKeyMatchSet_type type,
												   const BYTE * pKeyData,size_t keylen,
												   CBlurKeyMatchSet_type keysting_type)
{
	char outputkey[2048];

	memset(outputkey,0x00,sizeof(outputkey));	

	size_t outkeylen = sizeof(outputkey);

	std::string dst_codesetstring = CPF::GetCodeSetString(type);

	if( type != keysting_type )
	{
		std::string key_codesetstring = CPF::GetCodeSetString(keysting_type);

		//将输入的unicode的关键字转化成制定的格式的关键字
		if( CPF::char_set_convert(dst_codesetstring.c_str(),key_codesetstring.c_str(),
			(const char *)pKeyData,keylen,(char *)outputkey,outkeylen ) == -1 )
		{
			return false;
		}
	}
	else
	{
		memcpy( outputkey,pKeyData, mymin(keylen,sizeof(outputkey)) );

		outkeylen = sizeof(outputkey) -  mymin(keylen,sizeof(outputkey));
	}

	COctetsArray octArray;

	//得到需要格式的拆分字符数组
	BLURTYPE blurtype = CPF::SplitFullKey(dst_codesetstring.c_str(),
		(BYTE *)outputkey,sizeof(outputkey)-outkeylen,octArray);

	if(  BLUR_INVALID == blurtype )
		return false;

	//添加关键字
	if( !m_pTableArray[type]->BeginDelKey(blurtype) )
		return false;

	for( size_t j = 0; j < octArray.size(); ++j )
	{
		if( !m_pTableArray[type]->DelKey(
			octArray[j].GetData(),octArray[j].GetDataLength()) )
		{
			return false;
		}
	}

	if( !m_pTableArray[type]->EndDelKey() )
		return false;

	return true;	
}


DEF_CCBlurKeyMatchSet_DECLEAR
BOOL DEF_CCBlurKeyMatchSet_CLASS::FindExact(CBlurKeyMatchSet_type type,
											const  BYTE* p,size_t nLen,
											GroupType& group,
											bool bcasesensitive) const
{
	if( !m_pTableArray[type] )
	{
		return false;
	}

	if( !bcasesensitive )
	{
		if( type == CBlurKeyMatchSet_type_gbk )
		{
			if( CPF::IsHaveUpperAlhpa((const char *)p,nLen/sizeof(char)) )
			{
				std::string tempKey((char *)p,nLen/sizeof(char));

				CPF::str_lwr(tempKey);

				return m_pTableArray[type]->FindExact((BYTE *)tempKey.c_str(),nLen,group);
			}
		}
		else if( type == CBlurKeyMatchSet_type_uc2le )
		{
			if( CPF::IsHaveUpperAlhpa((const wchar_t *)p,nLen/sizeof(wchar_t)) )
			{
				std::wstring tempKey((wchar_t *)p,nLen/sizeof(wchar_t));

				CPF::str_lwr(tempKey);

				return m_pTableArray[type]->FindExact((BYTE *)tempKey.c_str(),nLen,group);
			}
		}
		else
		{
			ACE_ASSERT(false);
			return false;
		}
	}

	return m_pTableArray[type]->FindExact(p,nLen,group);

}

DEF_CCBlurKeyMatchSet_DECLEAR
BOOL DEF_CCBlurKeyMatchSet_CLASS::HeadMatch(CBlurKeyMatchSet_type type,
											const BYTE* p,size_t nLen,
											GroupType& group,
											bool bcasesensitive) const
{
	if( !m_pTableArray[type] )
	{
		return false;
	}

	if( !bcasesensitive )
	{
		if( type == CBlurKeyMatchSet_type_gbk )
		{
			if( CPF::IsHaveUpperAlhpa((const char *)p,nLen/sizeof(char)) )
			{
				std::string tempKey((char *)p,nLen);

				CPF::str_lwr(tempKey);

				return m_pTableArray[type]->HeadMatch((BYTE *)tempKey.c_str(),nLen,group);
			}
		}
		else if( type == CBlurKeyMatchSet_type_uc2le )
		{
			if( CPF::IsHaveUpperAlhpa((const wchar_t *)p,nLen/sizeof(wchar_t)) )
			{
				std::wstring tempKey((wchar_t *)p,nLen/sizeof(wchar_t));

				CPF::str_lwr(tempKey);

				return m_pTableArray[type]->HeadMatch((BYTE *)tempKey.c_str(),nLen,group);
			}
		}
		else
		{
			ACE_ASSERT(false);
			return false;
		}
	}

	return m_pTableArray[type]->HeadMatch(p,nLen,group);
}


DEF_CCBlurKeyMatchSet_DECLEAR
BOOL DEF_CCBlurKeyMatchSet_CLASS::TextMatch(CBlurKeyMatchSet_type type,
											const BYTE* p,size_t nLen,
											GroupType & group,
											bool bcasesensitive) const
{
	if( !m_pTableArray[type] )
	{
		return false;
	}

	if( !bcasesensitive )
	{
		if( type == CBlurKeyMatchSet_type_gbk )
		{
			if( CPF::IsHaveUpperAlhpa((const char *)p,nLen/sizeof(char)) )
			{
				std::string tempKey((char *)p,nLen);

				CPF::str_lwr(tempKey);

				return m_pTableArray[type]->TextMatch((BYTE *)tempKey.c_str(),nLen,group);
			}
		}
		else if( type == CBlurKeyMatchSet_type_uc2le )
		{
			if( CPF::IsHaveUpperAlhpa((const wchar_t *)p,nLen/sizeof(wchar_t)) )
			{
				std::wstring tempKey((wchar_t *)p,nLen/sizeof(wchar_t));

				CPF::str_lwr(tempKey);

				return m_pTableArray[type]->TextMatch((BYTE *)tempKey.c_str(),nLen,group);
			}
		}
		else
		{
			ACE_ASSERT(false);
			return false;
		}
	}

	return m_pTableArray[type]->TextMatch(p,nLen,group);
}

