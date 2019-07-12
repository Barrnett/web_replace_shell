#ifndef _INI_FILE_2006_05_09
#define _INI_FILE_2006_05_09
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

#include "cpf/cpf.h"
#include "ace/Configuration_Import_Export.h"
#include "ace/SString.h"
#include "cpf/strtools.h"
#include "cpf/other_tools.h"
#include <algorithm>

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//(1)大小写问题
//section以及item都是区分大小写的

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//(2)重复的item问题

//如果一个section内的多个item有相同的，在读取的时候会被合并。
//如果相同的item的value是不相同的，则会使用配置文件中在后面的那个item的value。
//例如：最后PortForRecvAlm＝14
//[Database1]
//IPAddress=192.168.1.5
//PortForRecvAlm=12
//PortForRecvAlm=13
//PortForRecvAlm=14
//IPAddress2=132

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//(3)重复的section问题

//如果有多个section是重复的，则在读取的时候，会将这些相同的section进行合并，
//如果section的值item是互相不相同的，则item会合并到一起.
//如果某些section中的item是相同的，但是value不相同，则会使用配置文件中在后面的那个section的value。

//例如:
//[Database1]
//IPAddress=192.168.1.5
//PortForRecvAlm=12
//IPAddress2=132.2
//aaa = "asdfafds"


//[Database1]
//IPAddress=192.168.1.6
//PortForRecvAlm=14
//IPAddress2=132.2
//kkk = "kkkk"

//最后的结果是:
//[Database1]
//IPAddress=192.168.1.6
//PortForRecvAlm=14
//IPAddress2=132.2
//kkk = "kkkk"
//aaa = "asdfafds"

class CPF_CLASS CIniFile
{
public:
	CIniFile();
	
	~CIniFile();
	
public:
	BOOL Open(LPACECTSTR lpszFileName, ACE_TCHAR cSep = ',', BOOL bForceWrite = TRUE);
	void Close();

	const ACE_TCHAR* GetFileName() const		  { return m_FileName.c_str(); }

	int RemoveAll();
	int RemoveSection(LPACECTSTR pSection);
	int RemoveItem(LPACECTSTR pSection, LPACECTSTR pItem);

	/*
	 *	进行参数的设置
	 */
	template<typename STRTYPE>
	BOOL SetValue(LPACECTSTR pSection, LPACECTSTR pItem, const std::vector<STRTYPE>& valueArray);
	
	BOOL SetValue(LPACECTSTR pSection, LPACECTSTR pItem,std::vector<int>& value);
	BOOL SetValue(LPACECTSTR pSection, LPACECTSTR pItem,std::vector<unsigned int>& value);

	BOOL SetValue(LPACECTSTR pSection, LPACECTSTR pItem, LPACECTSTR value);

	template<typename STRTYPE>
	BOOL SetValue(LPACECTSTR pSection, LPACECTSTR pItem, const STRTYPE& value)
	{	return 	this->SetValue(pSection,pItem,value.c_str());}
	
	int SetValue(LPACECTSTR pSection, LPACECTSTR pItem, bool value)
	{	return SetValue(pSection,pItem,(int)value);	}
	
	BOOL SetValue(LPACECTSTR pSection, LPACECTSTR pItem, double value);
	BOOL SetValue(LPACECTSTR pSection, LPACECTSTR pItem, int value);	
	BOOL SetValue(LPACECTSTR pSection, LPACECTSTR pItem, unsigned int value);

	BOOL SetValue(LPACECTSTR pSection, LPACECTSTR pItem, ACE_UINT64 value);
	BOOL SetBinaryValue(LPACECTSTR pSection, LPACECTSTR pItem,const BYTE * pBuffer,size_t bufferlen);
	
	/*
	*	进行参数的的设置,返回个数，如果=0，表示没有，如果小于0,表示出错，
	*   用于获取
	item_%d=value1
	item_%d=value2
	item_%d=value3,
	pre_Item是相同的item的前缀。比如例子中pre_Item就应该是"item"

	在set之前，会将所有以item为开始的item删除。
	*/
	template<typename STRTYPE>
	int SetGroupValue(LPACECTSTR pSection, LPACECTSTR pre_Item, const std::vector<STRTYPE>& value);

	template<typename STRTYPE>
	int GetSectionNames(std::vector<STRTYPE>& value);

	template<typename STRTYPE>
	int GetItemsInSection(LPACECTSTR pSection, std::vector<STRTYPE>& item, std::vector<STRTYPE>& value)
	{	return GetItemsInSection<STRTYPE>(pSection,&item,&value);}

	template<typename STRTYPE>
	int GetItemsInSection(LPACECTSTR pSection, std::vector< std::pair<STRTYPE,STRTYPE> >& item_and_value);


	//功能和GetItemsInSection(LPACECTSTR pSection, CStrArray& item, CStrArray& value);
	//用户可以使用参数pitem==NULL表示不关心这个参数，pvalue==NULL表示不关心值。这样来提高效率
	template<typename STRTYPE>
	int GetItemsInSection(LPACECTSTR pSection, std::vector<STRTYPE>* pitemarry, std::vector<STRTYPE>* pvaluearry);

	/*
	*	进行参数的提取,返回个数，如果=0，表示没有，如果小于0,表示出错，
	*   用于获取item=value1,value2,value3,一次性获取value1,value2,value3放入到vector中
	*/
	template<typename STRTYPE>
	int GetValue(LPACECTSTR pSection, LPACECTSTR pItem, std::vector<STRTYPE>& value);

	int GetValue(LPACECTSTR pSection, LPACECTSTR pItem,std::vector<int>& value);
	int GetValue(LPACECTSTR pSection, LPACECTSTR pItem,std::vector<unsigned int>& value);

	template<typename STRTYPE>
	BOOL GetValue(LPACECTSTR pSection, LPACECTSTR pItem, STRTYPE& value);

	BOOL GetValue(LPACECTSTR pSection, LPACECTSTR pItem, bool& value);
	BOOL GetValue(LPACECTSTR pSection, LPACECTSTR pItem, double& value);
	BOOL GetValue(LPACECTSTR pSection, LPACECTSTR pItem, int& value);
	
	BOOL GetValue(LPACECTSTR pSection, LPACECTSTR pItem, unsigned int& value);

	BOOL GetValue(LPACECTSTR pSection, LPACECTSTR pItem, ACE_UINT64& value);
	BOOL GetValue(LPACECTSTR pSection, LPACECTSTR pItem, ACE_UINT16& value);

	//用于item=192.168.0.1的IP地址类型或者mask的获取
	BOOL GetValue_NetOrderIP(LPACECTSTR pSection, LPACECTSTR pItem, ACE_UINT32& value);
	BOOL GetValue_HostOrderIP(LPACECTSTR pSection, LPACECTSTR pItem, ACE_UINT32& value);

	//用于item=01-02-03-04-05-06或者01:02:03:04:05:06的获取
	BOOL GetValue_Macaddr(LPACECTSTR pSection, LPACECTSTR pItem, BYTE * mac_addr);

	/*
	*	进行参数的提取,返回个数，如果=0，表示没有，如果小于0,表示出错，
	*   用于获取
	item_01=value1
	item_02=value2
	item_03=value3,
	一次性获取value1,value2,value3放入到vector中,并且按照字母顺序从小到大排序
	pre_Item是相同的item的前缀。比如例子中pre_Item就应该是"item"
	*/
	template<typename STRTYPE>
	int GetGroupValue(LPACECTSTR pSection, LPACECTSTR pre_Item, std::vector<STRTYPE>& value);

	//读一段数据，数据使用16进制来写,比如
	//keydata = 03051A8B
	//如果存在并且正确读取返回true,否则返回false.
	//datalen是数据的实际长度，如果用户提供的bufferlen<datalen,只会拷贝bufferlen的数据
	BOOL GetBinaryValue(LPACECTSTR pSection, LPACECTSTR pItem,BYTE * pBuffer,size_t bufferlen,size_t& datalen);

private:
	ACE_TString		m_FileName;		// 配置文件名
	ACE_TCHAR		m_cSep;			// 分隔字符，缺省是逗号
	BOOL			m_bForceWrite;	// 是否进行强制写入

	ACE_Configuration_Heap	m_config;
	ACE_Ini_ImpExp			m_config_importer;

};


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

template<typename STRTYPE>
BOOL CIniFile::SetValue(LPACECTSTR pSection, LPACECTSTR pItem, const std::vector<STRTYPE>& valueArray)
{
	STD_TString strvalue;

	if( valueArray.size() > 0 )
	{
		size_t i = 0;

		for(i = 0; i < valueArray.size()-1; ++i)
		{
			strvalue += valueArray[i].c_str();
			strvalue += m_cSep;
		}

		strvalue += valueArray[i].c_str();
	}

	return this->SetValue(pSection,pItem,strvalue.c_str());
}

template<typename STRTYPE>
int CIniFile::GetSectionNames(std::vector<STRTYPE>& valueArray)
{
	valueArray.clear();

	ACE_TString name;
	int index = 0;
	while (!m_config.enumerate_sections (m_config.root_section(),index,name))
	{
		valueArray.insert(valueArray.begin(),STRTYPE(name.c_str()));
		++index;
	}

	return (int)valueArray.size();

}

template<typename STRTYPE>
int CIniFile::GetValue(LPACECTSTR pSection, LPACECTSTR pItem, std::vector<STRTYPE>& value)
{
	value.clear();

	ACE_Configuration_Section_Key section;

	if( m_config.open_section(m_config.root_section(),pSection,0,section) == -1 )
		return false;

	ACE_TString	strvalue;
	if( m_config.get_string_value(section,pItem,strvalue) == -1 )
		return false;

	CPF::GetWords<ACE_TCHAR,STRTYPE>(strvalue.c_str(),value,m_cSep);

	return (int)value.size();

}


template<typename STRTYPE>
BOOL CIniFile::GetValue(LPACECTSTR pSection, LPACECTSTR pItem, STRTYPE& value)
{
	ACE_Configuration_Section_Key section;

	if( m_config.open_section(m_config.root_section(),pSection,0,section) == -1 )
		return false;

	ACE_TString temp;

	if (m_config.get_string_value(section,pItem,temp)==-1)
		return false;

	value = (STRTYPE)(temp.c_str());

	return true;

}

template<typename STRTYPE>
int CIniFile::GetItemsInSection(LPACECTSTR pSection, std::vector<STRTYPE>* pitemarry, std::vector<STRTYPE>* pvaluearry)
{
	ACE_ASSERT( pitemarry || pvaluearry );

	if( !pvaluearry && !pitemarry )
		return 0;

	if( pitemarry )
	{
		pitemarry->clear();
	}

	if( pvaluearry )
	{
		pvaluearry->clear();
	}

	ACE_Configuration_Section_Key section;

	if( m_config.open_section(m_config.root_section(),pSection,0,section) == -1 )
		return false;

	ACE_Configuration::VALUETYPE type;
	ACE_TString value;
	ACE_TString name;
	int index = 0;

	while( !m_config.enumerate_values(section,index,name,type) )
	{
		if( m_config.get_string_value(section,name.c_str(),value) != -1 )
		{
			if(pitemarry)
			{
				pitemarry->push_back(STRTYPE(name.c_str()));
			}

			if( pvaluearry )
			{
				pvaluearry->push_back(STRTYPE(value.c_str()));
			}

		}

		++index;
	}

	if( pitemarry )
	{
		return (int)pitemarry->size();
	}
	else
	{
		if( pvaluearry )
		{
			return (int)pvaluearry->size();
		}
	}

	return 0;
}

template<typename STRTYPE>
int CIniFile::GetItemsInSection(LPACECTSTR pSection, std::vector< std::pair<STRTYPE,STRTYPE> >& item_and_value)
{
	ACE_Configuration_Section_Key section;

	if( m_config.open_section(m_config.root_section(),pSection,0,section) == -1 )
		return 0;

	ACE_Configuration::VALUETYPE type;
	ACE_TString value;
	ACE_TString name;
	int index = 0;

	while( !m_config.enumerate_values(section,index,name,type) )
	{
		if( m_config.get_string_value(section,name.c_str(),value) != -1 )
		{
			item_and_value.push_back(std::make_pair(STRTYPE(name.c_str()),STRTYPE(value.c_str())));

		}

		++index;
	}

	return (int)item_and_value.size();	

}


template<typename STRTYPE>
int CIniFile::GetGroupValue(LPACECTSTR pSection, LPACECTSTR pre_Item, std::vector<STRTYPE>& value)
{
	value.clear();

	std::vector< std::pair<STRTYPE,STRTYPE> >	item_and_value;

	GetItemsInSection(pSection,item_and_value);

	std::sort(item_and_value.begin(),item_and_value.end());

	for( size_t i = 0; i < item_and_value.size(); ++i)
	{
		if( ACE_OS::strstr(item_and_value[i].first.c_str(),pre_Item) == item_and_value[i].first.c_str() )
		{
			value.push_back(item_and_value[i].second);
		}
	}

	return value.size();
}

template<typename STRTYPE>
int CIniFile::SetGroupValue(LPACECTSTR pSection, LPACECTSTR pre_Item, const std::vector<STRTYPE>& value)
{
	//先删除所有相关的item.
	std::vector<STRTYPE>	items;

	std::vector<STRTYPE> *	pvalue = NULL;

	GetItemsInSection(pSection,&items,pvalue);

	for( size_t i = 0; i < items.size(); ++i)
	{
		if( ACE_OS::strstr(items[i].c_str(),pre_Item) == items[i].c_str() )
		{
			RemoveItem(pSection,items[i].c_str());
		}
	}

	char item[1024] = {0};

	for(size_t i = 0; i < value.size(); ++i)
	{
		ACE_OS::sprintf(item,"filename_%u",i);

		SetValue(pSection,item,value[i].c_str());
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
#endif//_INI_FILE_2006_05_09


