#include <stdio.h>
#include "cpf/IniFile.h"
#include "cpf/strtools.h"
#include "ace/OS_NS_stdio.h"
#include "cpf/addr_tools.h"
#include "cpf/other_tools.h"

//////////////////////////////////////////////////////////////////////////
// CIniFile

/*
 *  
 */
CIniFile::CIniFile()
:m_config_importer (m_config)
{
	m_cSep = ACE_TCHAR(',');
	m_bForceWrite = TRUE;
}


CIniFile::~CIniFile()
{
	Close();
}

BOOL CIniFile::Open(LPACECTSTR lpszFileName, ACE_TCHAR cSep, BOOL bForceWrite)
{
	ACE_ASSERT(lpszFileName != NULL&&ACE_OS::strlen(lpszFileName)>0);

	if( lpszFileName == NULL || ACE_OS::strlen(lpszFileName)<=0 )
		return false;

	m_FileName = lpszFileName;
	m_cSep = cSep;

	m_bForceWrite = bForceWrite;
	
	if (m_config.open () == -1)
		return false;

	if (m_config_importer.import_config (m_FileName.c_str()) == -1)
		return false;

	return true;

}

void CIniFile::Close()
{
	if ( !m_bForceWrite )
	{
		if( m_FileName.length() > 0 )
		{
			m_config_importer.export_config(m_FileName.c_str());

		}
	}

}


BOOL CIniFile::RemoveAll()
{
	ACE_TString name;
	int index = 0;

	while (!m_config.enumerate_sections (m_config.root_section(),index,name))
	{
		m_config.remove_section (m_config.root_section(),name.c_str(),1);
		
		++index;
	}

	if( m_bForceWrite )
		m_config_importer.export_config(m_FileName.c_str());

	return TRUE;
}


BOOL CIniFile::RemoveSection(LPACECTSTR pSection)
{
	if (m_config.remove_section (m_config.root_section(),pSection,1) == -1 )
		return false;

	if( m_bForceWrite )
		m_config_importer.export_config(m_FileName.c_str());

	return true;

}

/*
 *	
 */
BOOL CIniFile::RemoveItem(LPACECTSTR pSection, LPACECTSTR pItem)
{
	ACE_Configuration_Section_Key section;

	if( m_config.open_section(m_config.root_section(),pSection,0,section) == -1 )
		return false;

	if( m_config.remove_value (section,pItem) == -1 )
		return false;

	if( m_bForceWrite )
		m_config_importer.export_config(m_FileName.c_str());

	return true;
}

int CIniFile::SetValue(LPACECTSTR pSection, LPACECTSTR pItem,std::vector<int>& value)
{
	std::string str_value = CPF::MakeUpIntSetString(value,',');

	return SetValue(pSection,pItem,str_value.c_str());
}

int CIniFile::SetValue(LPACECTSTR pSection, LPACECTSTR pItem,std::vector<unsigned int>& value)
{
	std::string str_value = CPF::MakeUpIntSetString(value,',');

	return SetValue(pSection,pItem,str_value.c_str());
}


BOOL CIniFile::SetValue(LPACECTSTR pSection, LPACECTSTR pItem, LPACECTSTR value)
{
	ACE_Configuration_Section_Key section;

	if( m_config.open_section(m_config.root_section(),pSection,1,section) == -1 )
	{
		return false;
	}

	ACE_TString aceValue(value);

	if( m_config.set_string_value(section,pItem,aceValue) == -1 )
		return false;

	if( m_bForceWrite )
		m_config_importer.export_config(m_FileName.c_str());

	return true;
}


/*
 *	
 */
BOOL CIniFile::SetValue(LPACECTSTR pSection, LPACECTSTR pItem, double value)
{
	ACE_TCHAR buf[256];

	ACE_OS::sprintf(buf,ACE_TEXT("%.3f"),value);

	return SetValue(pSection,pItem,(LPACECTSTR)buf);

}

/*
 *	
 */
BOOL CIniFile::SetValue(LPACECTSTR pSection, LPACECTSTR pItem, int value)
{
	ACE_TCHAR buf[256];

	ACE_OS::itoa(value,buf,10);

	return SetValue(pSection,pItem,(LPACECTSTR)buf);

}

/*
 *	
 */
BOOL CIniFile::SetValue(LPACECTSTR pSection, LPACECTSTR pItem, unsigned int value)
{
	ACE_TCHAR buf[256];

	ACE_OS::sprintf(buf, ACE_TEXT("%u"), value);

	return SetValue(pSection,pItem,(LPACECTSTR)buf);
}

BOOL CIniFile::SetValue(LPACECTSTR pSection, LPACECTSTR pItem, ACE_UINT64 value)
{
	ACE_TCHAR buf[256];
		
	ACE_OS::sprintf(buf,ACE_TEXT("lld"),value);

	return SetValue(pSection,pItem,(LPACECTSTR)buf);
}


/*
 *	
 */
BOOL CIniFile::GetValue(LPACECTSTR pSection, LPACECTSTR pItem, bool& value)
{
	int intvalue = 0;

	if( !GetValue(pSection,pItem,intvalue))
		return false;

	value = (bool)intvalue;

	return true;
}


/*
 *	
 */
BOOL CIniFile::GetValue(LPACECTSTR pSection, LPACECTSTR pItem, double& value)
{
	ACE_TString strvalue;

	if( !GetValue<ACE_TString>(pSection,pItem,strvalue) )
		return false;

	if( strvalue.length() == 0 )
		return false;

	ACE_TCHAR* pStop = NULL;
	value = ACE_OS::strtod(strvalue.c_str(), &pStop);

	return TRUE;
}

/*
 *	
 */
BOOL CIniFile::GetValue(LPACECTSTR pSection, LPACECTSTR pItem, int& value)
{
	ACE_TString strvalue;
	if( !(GetValue<ACE_TString>(pSection,pItem,strvalue)) )
		return false;

	if( strvalue.length() == 0 )
		return false;
	
	value = ACE_OS::atoi(strvalue.c_str());

	return true;
}


/*
 *	
 */
BOOL CIniFile::GetValue(LPACECTSTR pSection, LPACECTSTR pItem, unsigned int& value)
{
	ACE_TString strvalue;
	if( !(GetValue<ACE_TString>(pSection,pItem,strvalue)) )
		return false;

	if( strvalue.length() == 0 )
		return false;

	sscanf(strvalue.c_str(),"%u",&value);
	
	return TRUE;
}

/*
 *	
 */
BOOL CIniFile::GetValue(LPACECTSTR pSection, LPACECTSTR pItem, ACE_UINT64& value)
{
	ACE_TString strvalue;
	if( !GetValue<ACE_TString>(pSection,pItem,strvalue) )
		return false;

	if( strvalue.length() == 0 )
		return false;

	sscanf(strvalue.c_str(),"%lld",&value);

	return TRUE;
}

BOOL CIniFile::GetValue(LPACECTSTR pSection, LPACECTSTR pItem, ACE_UINT16& value)
{
	unsigned int valuetemp;

	if( GetValue(pSection,pItem,valuetemp) )
	{
		value = (ACE_UINT16)valuetemp;
		return true;
	}

	return false;
}

//用于item=192.168.0.1的IP地址类型或者mask的获取
BOOL CIniFile::GetValue_NetOrderIP(LPACECTSTR pSection, LPACECTSTR pItem, ACE_UINT32& value)
{
	std::string strIP;

	if( !GetValue(pSection,pItem,strIP) )
		return false;

	value = inet_addr(strIP.c_str());

	return true;
}

BOOL CIniFile::GetValue_HostOrderIP(LPACECTSTR pSection, LPACECTSTR pItem, ACE_UINT32& value)
{
	if( GetValue_NetOrderIP(pSection,pItem,value) )
	{
		value = ACE_NTOHL(value);
		return true;
	}

	return false;

}

//用于item=01-02-03-04-05-06或者01:02:03:04:05:06的获取
BOOL CIniFile::GetValue_Macaddr(LPACECTSTR pSection, LPACECTSTR pItem, BYTE * mac_addr)
{
	std::string str_mac;

	if( GetValue(pSection,pItem,str_mac) )
	{
		CPF::mac_a_to_addr(str_mac.c_str(),mac_addr);

		return true;
	}

	return false;
	
}


int CIniFile::GetValue(LPACECTSTR pSection, LPACECTSTR pItem,std::vector<int>& value)
{
	value.clear();

	STD_TString strValue;

	if( !GetValue(pSection,pItem,strValue) )
		return 0;

	int size = CPF::GetWordNums<ACE_TCHAR>(strValue.c_str(),strValue.size(),m_cSep,true);

	if( size <= 0 )
		return 0;

	value.reserve(size);

	for( int i = 0; i < size; ++i )
	{
		int data = 0;

		if( CPF::GetIntWord<ACE_TCHAR>(i,strValue.c_str(),data,m_cSep) )
			value.push_back(data);
	}

	return (int)value.size();
}


int CIniFile::GetValue(LPACECTSTR pSection, LPACECTSTR pItem,std::vector<unsigned int>& value)
{
	value.clear();

	STD_TString strValue;

	if( !GetValue(pSection,pItem,strValue) )
		return 0;

	int size = CPF::GetWordNums<ACE_TCHAR>(strValue.c_str(),strValue.size(),m_cSep,true);

	if( size <= 0 )
		return 0;

	value.reserve(size);

	for( int i = 0; i < size; ++i )
	{
		unsigned int data = 0;

		if( CPF::GetUintWord<ACE_TCHAR>(i,strValue.c_str(),data,m_cSep) )
			value.push_back(data);
	}

	return (int)value.size();
}

BOOL CIniFile::SetBinaryValue(LPACECTSTR pSection, LPACECTSTR pItem,const BYTE * pBuffer,size_t bufferlen)
{
	char * buftemp = new char[bufferlen*2+2];

	memset(buftemp,0x00,bufferlen*2+2);

	char * ptemp = buftemp;
	
	for( size_t i = 0; i < bufferlen; ++i )
	{
		ACE_OS::sprintf(ptemp,"%02X",pBuffer[i]);

		ptemp += 2;
	}

	BOOL bRtn = SetValue(pSection,pItem,(LPACECTSTR)buftemp);

	delete []buftemp;

	return bRtn;
}

//读一段数据，数据使用16进制来写,比如
//keydata = 03051A8B
//如果存在并且正确读取返回true,否则返回false.
//datalen是数据的实际长度，如果用户提供的bufferlen<datalen,只会拷贝bufferlen的数据
BOOL CIniFile::GetBinaryValue(LPACECTSTR pSection, LPACECTSTR pItem,BYTE * pBuffer,size_t bufferlen,size_t& datalen)
{
	ACE_TString strvalue;
	if( !GetValue<ACE_TString>(pSection,pItem,strvalue) )
		return false;

	char * temp_buf = new char[strvalue.length()+1];
	char * temp_pos = temp_buf;

	//去掉空格
	for(int i = 0; i < (int)strvalue.length(); ++i)
	{
		if( strvalue[i] != ' ' )
		{
			*temp_pos = strvalue[i];
			++temp_pos;
		}
	}

	*temp_pos = 0;

	datalen = (temp_pos - temp_buf)/2;

	if( bufferlen < datalen )
	{
		delete []temp_buf;
		return false;
	}

	const char * pValue = temp_buf;

	for( size_t i = 0; i < datalen; i++ )
	{
		int tempdata = 0;

		if( sscanf(pValue,"%02x",&tempdata) != 1 )
		{
			delete []temp_buf;
			return false;
		}

		pValue += 2;

		pBuffer[i] = (BYTE)tempdata;
	}

	delete []temp_buf;
	return true;
}





