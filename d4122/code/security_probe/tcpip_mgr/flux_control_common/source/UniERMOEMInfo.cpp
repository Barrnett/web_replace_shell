//////////////////////////////////////////////////////////////////////////
//UniERMOEMInfo.cpp

#include "StdAfx.h"
#include "UniERMOEMInfo.h"
#include "cpf/TinyXmlEx.h"

CUniERMOEMInfo::CUniERMOEMInfo(void)
{
	m_str_oem_name = "unierm";

	m_str_product_name = "网络流量综合管理系统";

	m_str_brand = "UniERM";
	m_company_name_ch = "北京万任科技有限责任公司";
	m_company_name_en = "Beijing Wanren Technology Co.,Ltd.";
	m_company_home_url = "http://wwww.wanrentech.com";
	m_product_help_url = "http://www.wanrentech.com/user_guide.rar";
	m_product_brief_url = "http://www.wanrentech.com/unierm_brief.rar";

	m_str_desktop_name = "";
}

CUniERMOEMInfo::~CUniERMOEMInfo(void)
{
}


std::string	CUniERMOEMInfo::GetFullProductName() const
{
	if( -1 == m_str_product_name.find(m_str_brand.c_str()) )
	{
		return m_str_brand + m_str_product_name;
	}
	else
	{
		return m_str_product_name;
	}
}

BOOL CUniERMOEMInfo::ReadFromXml(const char * cur_oem_file_name)
{	
	CTinyXmlEx reader;

	if( 0 != reader.open_for_readonly(cur_oem_file_name) )
	{
		return false;
	}

	reader.GetAttr(reader.GetRootNode(),"oem_name",m_str_oem_name);

	reader.GetValueLikeIni(reader.GetRootNode(),"brand",m_str_brand);
	reader.GetValueLikeIni(reader.GetRootNode(),"product_name",m_str_product_name);
	reader.GetValueLikeIni(reader.GetRootNode(),"company_name_ch",m_company_name_ch);
	reader.GetValueLikeIni(reader.GetRootNode(),"company_name_en",m_company_name_en);
	reader.GetValueLikeIni(reader.GetRootNode(),"home_url",m_company_home_url);
	reader.GetValueLikeIni(reader.GetRootNode(),"product_help_url",m_product_help_url);
	reader.GetValueLikeIni(reader.GetRootNode(),"product_brief_url",m_product_brief_url);

	reader.GetValueLikeIni(reader.GetRootNode(),"desktop_name",m_str_desktop_name);

	if( m_str_desktop_name.empty() )
	{
		m_str_desktop_name = m_str_brand;

		if( m_str_desktop_name.empty() )
		{
			m_str_desktop_name = m_str_oem_name;
		}
	}

	return true;
}

std::string CUniERMOEMInfo::GetDesktopNameEx() const
{
	if( !m_str_desktop_name.empty() )
		return m_str_desktop_name;

	if( !m_str_brand.empty() )
		return m_str_brand;

	if( !m_str_oem_name.empty() )
		return m_str_oem_name;

	return "我的流量管理";
}
